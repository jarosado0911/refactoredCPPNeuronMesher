import glfw
from OpenGL.GL import *
from OpenGL.GLU import *
import numpy as np
import tkinter as tk
from tkinter import filedialog
import os
import sys
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from neurongraph import NeuronGraph  # Assumes Python binding is installed

# Initialize GLFW
def init_window(width=800, height=600):
    if not glfw.init():
        raise Exception("GLFW can't be initialized")

    window = glfw.create_window(width, height, "Neuron Viewer", None, None)
    if not window:
        glfw.terminate()
        raise Exception("GLFW window can't be created")

    glfw.make_context_current(window)
    glEnable(GL_DEPTH_TEST)
    return window

# File dialog using Tkinter
def open_file_dialog():
    root = tk.Tk()
    root.withdraw()
    filetypes = [('SWC Files', '*.swc'), ('UGX Files', '*.ugx'), ('All Files', '*.*')]
    filename = filedialog.askopenfilename(title='Open Neuron File', filetypes=filetypes)
    return filename

# Load neuron from file
def load_neuron(path):
    graph = NeuronGraph()
    ext = os.path.splitext(path)[1].lower()
    if ext == ".swc":
        graph.readFromFile(path)
    elif ext == ".ugx":
        graph.readFromFileUGX(path)
    else:
        raise ValueError("Unsupported file format: " + ext)
    return graph

# Draw lines for neuron edges
def draw_graph(graph):
    nodes = graph.getNodes()
    glBegin(GL_LINES)
    for n in nodes.values():
        if n.pid == -1:
            continue
        parent = nodes.get(n.pid)
        if parent:
            glVertex3f(n.x, n.y, n.z)
            glVertex3f(parent.x, parent.y, parent.z)
    glEnd()

# Simple camera control variables
angle_x = 0.0
angle_y = 0.0
distance = 300.0
last_x, last_y = None, None
mouse_pressed = False

# GLFW mouse callbacks
def mouse_button_callback(window, button, action, mods):
    global mouse_pressed
    if button == glfw.MOUSE_BUTTON_LEFT:
        mouse_pressed = action == glfw.PRESS

def cursor_position_callback(window, xpos, ypos):
    global angle_x, angle_y, last_x, last_y
    if mouse_pressed:
        if last_x is not None and last_y is not None:
            dx = xpos - last_x
            dy = ypos - last_y
            angle_x += dy * 0.5
            angle_y += dx * 0.5
    last_x = xpos
    last_y = ypos

def scroll_callback(window, xoffset, yoffset):
    global distance
    distance -= yoffset * 5.0
    distance = max(10.0, distance)

# Main function
def main():
    window = init_window()
    glfw.set_mouse_button_callback(window, mouse_button_callback)
    glfw.set_cursor_pos_callback(window, cursor_position_callback)
    glfw.set_scroll_callback(window, scroll_callback)

    graph = None

    while not glfw.window_should_close(window):
        glfw.poll_events()

        width, height = glfw.get_framebuffer_size(window)
        glViewport(0, 0, width, height)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

        glMatrixMode(GL_PROJECTION)
        glLoadIdentity()
        gluPerspective(45.0, width / float(height), 0.1, 1000.0)

        glMatrixMode(GL_MODELVIEW)
        glLoadIdentity()
        gluLookAt(
            distance * np.sin(np.radians(angle_y)) * np.cos(np.radians(angle_x)),
            distance * np.sin(np.radians(angle_x)),
            distance * np.cos(np.radians(angle_y)) * np.cos(np.radians(angle_x)),
            0, 0, 0, 0, 1, 0
        )

        if graph:
            glColor3f(0.0, 1.0, 1.0)
            draw_graph(graph)

        glfw.swap_buffers(window)

        # Keyboard short-cut
        if glfw.get_key(window, glfw.KEY_O) == glfw.PRESS:
            filepath = open_file_dialog()
            if filepath:
                graph = load_neuron(filepath)

    glfw.terminate()

if __name__ == '__main__':
    main()
