#include <iostream>
#include <iomanip>
#include <GLFW/glfw3.h>
#include <chrono>
#include <algorithm>
#include "Shader.hpp"
#include "Math.hpp"

// Global state
Camera camera;
bool mousePressed = false;
double lastMouseX = 0.0, lastMouseY = 0.0;

// Simulation parameters
struct SimParams {
    float mass = 1.0f;
    float diskOuter = 8.0f;
    bool starfieldOn = true;
    bool planetsOn = true;
    bool diskOn = true;
    bool lensingOn = true;
} params;

// FPS tracking
float fps = 0.0f;
int frameCount = 0;
auto lastFPSTime = std::chrono::high_resolution_clock::now();

void framebuffer_size_callback(GLFWwindow* /*window*/, const int width, const int height) {
    glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, const int button, const int action, int /*mods*/) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        mousePressed = (action == GLFW_PRESS);
        if (mousePressed) {
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
        }
    }
}

void cursor_position_callback(GLFWwindow* /*window*/, const double xpos, const double ypos) {
    if (mousePressed) {
        const double dx = xpos - lastMouseX;
        const double dy = ypos - lastMouseY;

        camera.azimuth -= static_cast<float>(dx) * 0.005f;
        camera.elevation -= static_cast<float>(dy) * 0.005f;
        camera.elevation = std::max(0.01f, std::min(3.13f, camera.elevation));

        lastMouseX = xpos;
        lastMouseY = ypos;
    }
}

void scroll_callback(GLFWwindow* /*window*/, double /*xoffset*/, const double yoffset) {
    camera.radius += static_cast<float>(yoffset) * 0.5f;
    camera.radius = std::max(camera.minRadius, std::min(camera.maxRadius, camera.radius));
}

void key_callback(GLFWwindow* window, const int key, int /*scancode*/, const int action, int /*mods*/) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true);
                break;
            case GLFW_KEY_1:
                params.starfieldOn = !params.starfieldOn;
                break;
            case GLFW_KEY_2:
                params.planetsOn = !params.planetsOn;
                break;
            case GLFW_KEY_3:
                params.diskOn = !params.diskOn;
                break;
            case GLFW_KEY_4:
                params.lensingOn = !params.lensingOn;
                break;
            case GLFW_KEY_EQUAL:
            case GLFW_KEY_KP_ADD:
                params.mass = std::min(5.0f, params.mass + 0.1f);
                break;
            case GLFW_KEY_MINUS:
            case GLFW_KEY_KP_SUBTRACT:
                params.mass = std::max(0.1f, params.mass - 0.1f);
                break;
            case GLFW_KEY_RIGHT_BRACKET:
                params.diskOuter = std::min(30.0f, params.diskOuter + 0.5f);
                break;
            case GLFW_KEY_LEFT_BRACKET:
                params.diskOuter = std::max(2.0f, params.diskOuter - 0.5f);
                break;
            default: ;
        }
    }
}

void updateFPS() {
    frameCount++;
    const auto currentTime = std::chrono::high_resolution_clock::now();

    if (const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFPSTime); duration.count() >= 1000) {
        fps = static_cast<float>(frameCount) / (static_cast<float>(duration.count()) / 1000.0f);
        frameCount = 0;
        lastFPSTime = currentTime;

        // Print status
        std::cout << "\rFPS: " << std::fixed << std::setprecision(1) << fps
                  << " | Mass: " << std::setprecision(2) << params.mass
                  << " | Disk: " << std::setprecision(1) << params.diskOuter
                  << " | Features: " << (params.starfieldOn ? "S" : "-")
                  << (params.planetsOn ? "P" : "-") << (params.diskOn ? "D" : "-")
                  << (params.lensingOn ? "L" : "-") << std::flush;
    }
}

int main(int /*argc*/, char* /*argv*/[]) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // MSAA

    // Create window
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Black Hole Simulator", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Load OpenGL functions after creating context
    if (!loadOpenGLFunctions()) {
        std::cerr << "Failed to load OpenGL functions" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    // Enable VSync for smooth rendering
    glfwSwapInterval(1);

    // Load shaders
    const Shader blackholeShader("shaders/blackhole.vert", "shaders/blackhole.frag");

    // Create fullscreen quad VAO
    unsigned int VAO, VBO;
    constexpr float vertices[] = {
        -1.0f,  1.0f,
         1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f, -1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);

    // Enable OpenGL features
    glEnable(GL_MULTISAMPLE);

    std::cout << "Black Hole Simulator Controls:" << std::endl;
    std::cout << "Mouse: Drag to rotate, scroll to zoom" << std::endl;
    std::cout << "Keys: 1-4 toggle features, +/- adjust mass, [/] adjust disk" << std::endl;
    std::cout << "ESC: Exit" << std::endl << std::endl;

    // Main render loop
    const auto startTime = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window)) {
        // Calculate time
        auto currentTime = std::chrono::high_resolution_clock::now();
        const float time = std::chrono::duration<float>(currentTime - startTime).count();

        // Get window size
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        // Update camera
        camera.updatePosition();
        const Mat4 invViewMatrix = camera.getInverseViewMatrix();

        // Clear
        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Use shader and set uniforms
        blackholeShader.use();
        blackholeShader.setVec2("u_resolution", static_cast<float>(width), static_cast<float>(height));
        blackholeShader.setFloat("u_time", time);
        blackholeShader.setMat4("u_invViewMatrix", invViewMatrix.m);
        blackholeShader.setVec3("u_cameraPosition", camera.position.x, camera.position.y, camera.position.z);

        // Simulation parameters
        blackholeShader.setFloat("u_mass", params.mass);
        blackholeShader.setFloat("u_schwarzschildRadius", params.mass);
        blackholeShader.setFloat("u_diskInnerRadius", 1.5f * params.mass);
        blackholeShader.setFloat("u_diskOuterRadius", params.diskOuter);
        blackholeShader.setInt("u_enableStarfield", params.starfieldOn ? 1 : 0);
        blackholeShader.setInt("u_enablePlanets", params.planetsOn ? 1 : 0);
        blackholeShader.setInt("u_enableDisk", params.diskOn ? 1 : 0);
        blackholeShader.setInt("u_enableLensing", params.lensingOn ? 1 : 0);

        // Adaptive marching parameters (optimized based on features enabled)
        float baseStep = params.lensingOn ? 0.25f : 0.45f;
        if (!params.diskOn) baseStep *= 1.15f;
        if (!params.planetsOn) baseStep *= 1.15f;

        // Dynamic FPS-based optimization
        if (fps > 0.0f) {
            if (fps < 30.0f) baseStep *= 1.2f;
            else if (fps > 55.0f) baseStep *= 0.9f;
        }
        baseStep = std::max(0.15f, std::min(0.8f, baseStep));

        const int maxSteps = params.lensingOn ? 220 : 120;
        constexpr float farDist = 100.0f;
        const float lensMaxRadius = params.lensingOn ? (params.diskOuter * 2.0f + 6.0f) : 0.0f;

        blackholeShader.setFloat("u_stepSize", baseStep);
        blackholeShader.setInt("u_maxSteps", maxSteps);
        blackholeShader.setFloat("u_farDist", farDist);
        blackholeShader.setFloat("u_lensMaxRadius", lensMaxRadius);

        // Render fullscreen quad
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();

        updateFPS();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}
