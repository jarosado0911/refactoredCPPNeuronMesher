
# SWC Neuron Viewer

A lightweight 3D OpenGL-based viewer to render `.swc` neuron morphology files with interactive features:
- Rotation (mouse drag)
- Zoom (scroll wheel)
- Render mode selection (keys `1` to `6`)
- Colored nodes and edges by type
- Variable thickness based on radius
- Optional lighting and bounding box

---

## ✅ Prerequisites

This project uses the following libraries:
- `GLFW` (window/context/input handling)
- `GL` (OpenGL core)
- `GLU` (utility rendering like spheres/cylinders)
- `X11` (if using WSL)

### Install dependencies (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install build-essential cmake libglfw3-dev libglu1-mesa-dev libgl1-mesa-dev x11-utils
```

### 🔍 For WSL Users

1. Install an X Server on Windows, like **VcXsrv** or **Xming**.
2. Launch your X server and allow access from 127.0.0.1.
3. Then in your WSL terminal:

```bash
export DISPLAY=$(grep -m 1 nameserver /etc/resolv.conf | awk '{print $2}'):0
export LIBGL_ALWAYS_INDIRECT=1
```

You should now be able to render OpenGL content in a popup window from WSL.

---

## 📦 Build Instructions

Clone your project and compile it using `g++`:

```bash
g++ -O3 -std=c++17 -o swc_viewer swc_viewer.cpp -lGL -lGLU -lglfw
```

If your source file is named differently, replace `swc_viewer.cpp` accordingly.

---

## 🧪 Running the Viewer

```bash
./swc_viewer path/to/neuron.swc
```

### Interactive Controls

- 🖱 **Left-click + drag**: rotate the neuron
- 🔍 **Scroll wheel**: zoom in/out
- 🔢 **Number keys (1–6)**:
    - `1`: edges only (uniform)
    - `2`: edges + uniform nodes
    - `3`: nodes only (uniform size)
    - `4`: nodes only (by radius)
    - `5`: edges by radius
    - `6`: full enhanced (edges by radius, lighting, etc.)

---

## 🛠 Development Tips

- For smoothest performance:
  - Compile with `-O3 -DNDEBUG`
  - Avoid `GLU` for high-detail models (use triangle strips/VBOs)
  - Profile with `nvidia-smi`, `glxgears`, or `apitrace`
- You can adjust default sphere/cylinder resolution for performance in `drawSphere()` and `drawCylinder()`.

---

## 📁 Example .swc Format

The `.swc` file format follows the standard 7-column structure:

```
# id type x y z radius parent
1 1 0.0 0.0 0.0 1.0 -1
2 3 5.0 0.0 0.0 0.5 1
3 3 10.0 0.0 0.0 0.4 2
...
```

---

## 📌 Troubleshooting

- **Window doesn't appear**: ensure `DISPLAY` is set and X server is running.
- **Blank window**: may need `LIBGL_ALWAYS_INDIRECT=1` in WSL.
- **Missing cylinders**: might be overlapping points with identical coordinates or zero-length segments.

---

## 📄 License

This project is released under the MIT License.
