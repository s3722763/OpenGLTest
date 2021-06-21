from tkinter.constants import E
from opensimplex import OpenSimplex
from PIL import Image
import io
import random

import PySimpleGUI as sg

WIDTH = 2048
HEIGHT = 2048
FEATURE_SIZE = 512

def create_image(width, height, feature_size):
    gen = OpenSimplex(seed=random.randint(0, 2^64))

    im = Image.new('L', (WIDTH, HEIGHT))
    for y in range(0, HEIGHT):
        for x in range(0, WIDTH):
            value = (gen.noise2d(x / FEATURE_SIZE, y / FEATURE_SIZE))
            value += (0.5 * gen.noise2d(2 * x / FEATURE_SIZE, 2 * y / FEATURE_SIZE)) 
            value += (0.25 * gen.noise2d(4 * x / FEATURE_SIZE, 4 * y / FEATURE_SIZE))
            value += (0.125 * gen.noise2d(8 * x / FEATURE_SIZE, 8 * y / FEATURE_SIZE))

            value = value / (1 + 0.5 + 0.25 + 0.125)
            color = int((value + 1) * 128)
            im.putpixel((x, y), color)

    return im

layout = [
    [sg.Text("Height Map generator")],
    [sg.Image(key="-HEIGHTMAP-")],
    [[sg.Button("Create Height Map"), sg.Button("Save")]]
]

window = sg.Window("Height map generator", layout)
height_map = None

while True:
    event, values = window.read()

    if event == sg.WINDOW_CLOSED or event == 'Quit':
        break
    elif event == 'Create Height Map':
        height_map = create_image(WIDTH, HEIGHT, FEATURE_SIZE)
        bytes = io.BytesIO()
        height_map.save(bytes, format="PNG")
        height_map.save("heightmap.png")
    elif event == 'Save':
        height_map.save("heightmap.png")

    
window.close()