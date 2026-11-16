from pathlib import Path
import random


BASE_DIR = Path(__file__).resolve().parent
BUILD_DIR = BASE_DIR / "build_mac"
SOURCE_XML = BUILD_DIR / "sistema_solar_v2.xml"
TARGET_XML = BUILD_DIR / "sistema_solar_v3.xml"
INSERT_MARKER = "        <!-- ASTEROIDE -->"
LONE_ASTEROID_MARKER = (
    '                <model file="./output/asteroid_r008_s10_t10.3d">\n'
    '                    <texture file="texturas/2k_mercury.jpg" />'
)

MODEL_FILES = [
    "./output/asteroid_belt_1.3d",
    "./output/asteroid_belt_2.3d",
    "./output/asteroid_belt_3.3d",
]

ASTEROID_TEXTURE = "texturas/asteroid.png"

COLOR_PRESETS = [
    (220, 55, 8),
    (205, 50, 6),
    (190, 45, 4),
]

SPIN_AXES = [
    (1, 0, 0),
    (0, 1, 0),
    (0, 0, 1),
    (1, 1, 0),
    (0, 1, 1),
]


def build_asteroid_group(rng, radius_min, radius_max, y_span, scale_min, scale_max):
    angle = rng.uniform(0.0, 360.0)
    radius = rng.uniform(radius_min, radius_max)
    y = rng.uniform(-y_span, y_span)
    scale = rng.uniform(scale_min, scale_max)
    spin_time = rng.uniform(4.5, 11.0)

    axis_x, axis_y, axis_z = rng.choice(SPIN_AXES)
    model = rng.choice(MODEL_FILES)
    diffuse_value, ambient_value, emissive_value = rng.choice(COLOR_PRESETS)

    return f"""                <group>
                    <transform>
                        <rotate angle="{angle:.3f}" x="0" y="1" z="0" />
                        <translate x="{radius:.3f}" y="{y:.3f}" z="0" />
                        <scale x="{scale:.3f}" y="{scale:.3f}" z="{scale:.3f}" />
                        <rotate time="{spin_time:.3f}" x="{axis_x}" y="{axis_y}" z="{axis_z}" />
                    </transform>
                    <models>
                        <model file="{model}">
                            <texture file="{ASTEROID_TEXTURE}" />
                            <color>
                                <diffuse R="{diffuse_value}" G="{diffuse_value}" B="{diffuse_value}" />
                                <ambient R="{ambient_value}" G="{ambient_value}" B="{ambient_value}" />
                                <specular R="0" G="0" B="0" />
                                <emissive R="{emissive_value}" G="{emissive_value}" B="{emissive_value}" />
                                <shininess value="0" />
                            </color>
                        </model>
                    </models>
                </group>
"""


def build_belt_layer(rng, count, orbit_time, tilt_angle, radius_min, radius_max, y_span, scale_min, scale_max):
    groups = "".join(
        build_asteroid_group(rng, radius_min, radius_max, y_span, scale_min, scale_max)
        for _ in range(count)
    )

    return f"""            <group>
                <transform>
                    <rotate time="{orbit_time:.3f}" x="0" y="1" z="0" />
                    <rotate angle="{tilt_angle:.3f}" x="0" y="0" z="1" />
                </transform>
{groups}            </group>
"""


def build_belt_block():
    rng = random.Random(11)

    inner_layer = build_belt_layer(
        rng=rng,
        count=72,
        orbit_time=82.0,
        tilt_angle=2.5,
        radius_min=41.2,
        radius_max=43.9,
        y_span=0.6,
        scale_min=0.09,
        scale_max=0.18,
    )

    outer_layer = build_belt_layer(
        rng=rng,
        count=56,
        orbit_time=104.0,
        tilt_angle=-4.0,
        radius_min=43.5,
        radius_max=46.8,
        y_span=1.1,
        scale_min=0.08,
        scale_max=0.20,
    )

    return (
        "\n"
        "        <!-- CINTURA DE ASTEROIDES -->\n"
        "        <group>\n"
        f"{inner_layer}"
        f"{outer_layer}"
        "        </group>\n\n"
    )


def main():
    source_text = SOURCE_XML.read_text(encoding="utf-8")
    if INSERT_MARKER not in source_text:
        raise RuntimeError(f"Marker not found in {SOURCE_XML}")

    belt_block = build_belt_block()
    target_text = source_text.replace(INSERT_MARKER, belt_block + INSERT_MARKER, 1)
    target_text = target_text.replace(
        LONE_ASTEROID_MARKER,
        '                <model file="./output/asteroid_r008_s10_t10.3d">\n'
        f'                    <texture file="{ASTEROID_TEXTURE}" />',
        1,
    )
    TARGET_XML.write_text(target_text, encoding="utf-8")

    print(f"Created {TARGET_XML}")


if __name__ == "__main__":
    main()
