import random
import math

N_STARS = 2000
OUTPUT_FILE = "estrelas.xml"

SPHERE_FILE = "./output/sphere.3d"

MIN_RADIUS = 180
MAX_RADIUS = 900

MIN_SCALE = 0.025
MAX_SCALE = 0.085

random.seed(11)

def random_star_position():
    # Distribuição numa esfera à volta do sistema solar
    theta = random.uniform(0, 2 * math.pi)
    phi = math.acos(random.uniform(-1, 1))
    r = random.uniform(MIN_RADIUS, MAX_RADIUS)

    x = r * math.sin(phi) * math.cos(theta)
    y = r * math.cos(phi)
    z = r * math.sin(phi) * math.sin(theta)

    return x, y, z

def random_star_color():
    # Pequena variação para não ficarem todas iguais
    colors = [
        (255, 255, 255),  # branca
        (220, 230, 255),  # azulada
        (255, 240, 210),  # amarelada
        (255, 220, 180),  # alaranjada
    ]
    return random.choice(colors)

with open(OUTPUT_FILE, "w", encoding="utf-8") as f:
    f.write("<!-- ESTRELAS GERADAS AUTOMATICAMENTE -->\n")
    f.write("<group>\n")

    for i in range(N_STARS):
        x, y, z = random_star_position()
        scale = random.uniform(MIN_SCALE, MAX_SCALE)
        r, g, b = random_star_color()

        f.write("    <group>\n")
        f.write("        <transform>\n")
        f.write(f'            <translate x="{x:.3f}" y="{y:.3f}" z="{z:.3f}" />\n')
        f.write(f'            <scale x="{scale:.3f}" y="{scale:.3f}" z="{scale:.3f}" />\n')
        f.write("        </transform>\n")
        f.write("        <models>\n")
        f.write(f'            <model file="{SPHERE_FILE}">\n')
        f.write("                <color>\n")
        f.write(f'                    <diffuse R="{r}" G="{g}" B="{b}" />\n')
        f.write(f'                    <ambient R="{r}" G="{g}" B="{b}" />\n')
        f.write('                    <specular R="0" G="0" B="0" />\n')
        f.write(f'                    <emissive R="{r}" G="{g}" B="{b}" />\n')
        f.write('                    <shininess value="0" />\n')
        f.write("                </color>\n")
        f.write("            </model>\n")
        f.write("        </models>\n")
        f.write("    </group>\n")

    f.write("</group>\n")

print(f"Geradas {N_STARS} estrelas em {OUTPUT_FILE}")