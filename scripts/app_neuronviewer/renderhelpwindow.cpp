#include "globals.h"
#include "opgl_utils.h"

void renderHelpWindow(GLFWwindow* window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Set orthographic projection with origin at top-left
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    std::string fileInfo = "Current file: " + (currentLoadedFile.empty() ? "[none]" : currentLoadedFile) +
                           "\nFile has " + std::to_string(currentNodes.size()) + " nodes.";

    const float fontScale = 1.25f;
    const int charHeight = 13;
    const float lineHeight = charHeight * fontScale;
    const float padding = 20.0f;

    int helpLines = std::count(help.begin(), help.end(), '\n') + 1;
    int fileLines = std::count(fileInfo.begin(), fileInfo.end(), '\n') + 1;
    int totalLines = helpLines + fileLines;

    float totalHeight = totalLines * lineHeight;
    float maxHeight = height - 2 * padding;

    if (totalHeight > maxHeight) {
        std::cerr << "Warning: help text too tall for window. Consider reducing fontScale or increasing window height.\n";
    }

    static char buffer[500000];
    glPushMatrix();
    glScalef(fontScale, fontScale, 1.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnableClientState(GL_VERTEX_ARRAY);

    float printX = padding / fontScale;
    float printY = padding / fontScale;

    // Draw help text
    int num_quads = stb_easy_font_print(printX, printY, (char*)help.c_str(), nullptr, buffer, sizeof(buffer));
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, num_quads * 4);

    // Draw file info below help
    printY += helpLines * charHeight;  // No extra line gap
    int num_quads_file = stb_easy_font_print(printX, printY, (char*)fileInfo.c_str(), nullptr, buffer, sizeof(buffer));
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, num_quads_file * 4);

    glDisableClientState(GL_VERTEX_ARRAY);
    glPopMatrix();

    glfwSwapBuffers(window);
}
