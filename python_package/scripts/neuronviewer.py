"""
Neuron Viewer - A 3D visualization tool for neuron morphologies.

This module provides a simple OpenGL-based viewer for visualizing neuron structures
loaded from SWC or UGX files. It supports interactive 3D navigation and multiple
rendering modes for examining neuron morphologies.

Key Features:
- Supports SWC and UGX file formats
- Interactive 3D camera controls
- Multiple rendering modes (wireframe, solid, etc.)
- Cross-platform compatibility

Dependencies:
- PyOpenGL
- GLFW
- NumPy
- Tkinter (for file dialogs)

Example:
    viewer = NeuronViewer()
    viewer.run()
"""
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
    """
    Initialize a GLFW window with the given width and height.
    
    This function initializes the GLFW library and creates a window
    with the given size. The window is set as the current OpenGL context
    and depth testing is enabled.
    
    Args:
        width (int): The width of the window in pixels.
        height (int): The height of the window in pixels.
    
    Returns:
        GLFWwindow: The initialized GLFW window.
    """
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
    """
    Open a file dialog to select a neuron file.

    This function uses a Tkinter file dialog to open a file selection window
    for choosing neuron morphology files. The supported file types are SWC 
    and UGX, but all file types can be selected.

    Returns
    -------
    str
        The path to the selected file or an empty string if no file was selected.
    """
    root = tk.Tk()
    root.withdraw()
    filetypes = [('SWC Files', '*.swc'), ('UGX Files', '*.ugx'), ('All Files', '*.*')]
    filename = filedialog.askopenfilename(title='Open Neuron File', filetypes=filetypes)
    return filename

# Load neuron from file
def load_neuron(path):
    """
    Load a neuron from a file.

    This function takes a file path as input and loads a neuron from the file.
    The neuron is loaded into a NeuronGraph object and returned.

    The supported file formats are SWC and UGX. If the file format is not
    recognized, a ValueError is raised.

    Parameters
    ----------
    path : str
        The path to the file containing the neuron data.

    Returns
    -------
    NeuronGraph
        The loaded neuron graph.

    Raises
    ------
    ValueError
        If the file format is not recognized.
    """
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
    """
    Draw a neuron as a set of lines connecting the nodes in the graph.

    This function takes a NeuronGraph object as input and draws it as a set of
    lines connecting the nodes in the graph. The lines are drawn by iterating
    over the nodes and connecting each node to its parent node (if it exists).

    Args:
        graph (NeuronGraph): The neuron graph to draw.
    """
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
    """
    Handles mouse button events for view manipulation.
    Tracks when the left mouse button is pressed or released to toggle the
    mouse_pressed state, which is used for initiating and controlling 3D view
    rotation in the neuron viewer.

    Args:
        window: The GLFW window that received the event.
        button: The mouse button that was pressed or released.
        action: The mouse button action (GLFW_PRESS or GLFW_RELEASE).
        mods: Bit field describing which modifier keys were held down.
    """
    global mouse_pressed
    if button == glfw.MOUSE_BUTTON_LEFT:
        mouse_pressed = action == glfw.PRESS

def cursor_position_callback(window, xpos, ypos):
    """
    Handles mouse cursor movement events for view rotation.
    Tracks the last known position of the mouse cursor and updates the angle_x and angle_y variables
    when the left mouse button is pressed.
    Rotation speed is scaled by a constant factor (0.5) to provide a good balance between precision and responsiveness.
    The actual view transformation is applied in the main rendering loop using the global state variables modified by this function.

    See also
    --------
    GLFW cursor position documentation: https://www.glfw.org/docs/latest/input_guide.html#cursor_pos
    """
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
    """
    Handles mouse scroll events for zooming the view.
    The xoffset parameter is currently unused but included for future compatibility.
    Zooming is centered on the current view (no focal point adjustment).
    The 1.1 base value provides a good balance between precision and responsiveness.
    See also
    --------
    GLFW scroll documentation: https://www.glfw.org/docs/latest/input_guide.html#scrolling
    """
    global distance
    distance -= yoffset * 5.0
    distance = max(10.0, distance)

# Main function
def main():
    """
    Main function to initialize the GLFW window and handle rendering loop.

    This function sets up the GLFW window, assigns necessary callbacks for mouse
    and keyboard interaction, and enters the main rendering loop. In each loop 
    iteration, it processes events, clears the screen, sets up the camera 
    perspective, and draws the neuron graph if loaded. It also handles the 
    keyboard shortcut for opening a file dialog to load neuron data from a file.
    """
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
