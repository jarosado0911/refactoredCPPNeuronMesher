void renderHelpWindow(GLFWwindow* window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Top-left origin projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);  // Y flipped for top-left origin

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    std::string help =
        "=== SWC Viewer Controls ===\n"
        "1 - 6         | Change render mode\n"
        "F             | Refine geometry (cubic spline)\n"
        "O             | Open SWC file\n"
        "R             | Reset camera view\n"
        "Mouse Drag    | Rotate scene (Left Button)\n"
        "Mouse Drag    | Pan scene (Right Button)\n"
        "Scroll Wheel  | Zoom in/out\n"
        "ESC           | Exit viewer";

    std::string fileInfo = "Current file: " + (currentLoadedFile.empty() ? "[none]" : currentLoadedFile);

    // Configurable float font size
    const float fontScale = 1.25f;  // Try 1.5f, 2.25f, etc.
    const int charHeight = 13;     // Pixel height of stb_easy_font chars
    const float lineHeight = charHeight * fontScale;
    const float padding = 20.0f;

    // Count lines in help text
    int numLines = std::count(help.begin(), help.end(), '\n') + 1;
    float textBlockHeight = numLines * lineHeight;
    float startY = (height - textBlockHeight) / 2.0f;

    static char buffer[500000];

    glPushMatrix();
    glScalef(fontScale, fontScale, 1.0f);

    glColor3f(1.0f, 1.0f, 1.0f);
    glEnableClientState(GL_VERTEX_ARRAY);

    float printX = padding / fontScale;
    float printY = startY / fontScale;
    int num_quads = stb_easy_font_print(printX, printY, (char*)help.c_str(), nullptr, buffer, sizeof(buffer));
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, num_quads * 4);

    float fileY = height - padding;
    int num_quads_file = stb_easy_font_print(printX, fileY / fontScale, (char*)fileInfo.c_str(), nullptr, buffer, sizeof(buffer));
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, num_quads_file * 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();

    glfwSwapBuffers(window);
}
