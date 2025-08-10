#include <iostream>
#include <memory>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "BlackHoleSimulation.h"

// Window dimensions
const unsigned int WINDOW_WIDTH = 1280;
const unsigned int WINDOW_HEIGHT = 720;

// Global variables for mouse input
bool firstMouse = true;
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Forward declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);

// Global simulation instance
std::unique_ptr<BlackHoleSimulation> simulation;
bool mousePressed = false;

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4); // Enable multisampling

    // Create window
    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Black Hole Simulator", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Set callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Enable depth testing and blending
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);

    // Print OpenGL info
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl;

    // Create simulation
    simulation = std::make_unique<BlackHoleSimulation>(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!simulation->initialize()) {
        std::cerr << "Failed to initialize simulation" << std::endl;
        return -1;
    }

    // Print controls
    std::cout << "\n=== Black Hole Simulator Controls ===" << std::endl;
    std::cout << "Mouse: Drag to orbit camera" << std::endl;
    std::cout << "Scroll: Zoom in/out" << std::endl;
    std::cout << "Keys:" << std::endl;
    std::cout << "  1: Toggle starfield" << std::endl;
    std::cout << "  2: Toggle planets" << std::endl;
    std::cout << "  3: Toggle accretion disk" << std::endl;
    std::cout << "  4: Toggle gravitational lensing" << std::endl;
    std::cout << "  Q/A: Increase/decrease black hole mass" << std::endl;
    std::cout << "  W/S: Increase/decrease disk size" << std::endl;
    std::cout << "  R: Reset to defaults" << std::endl;
    std::cout << "  ESC: Exit" << std::endl;
    std::cout << "=====================================" << std::endl;

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process input
        processInput(window);

        // Update simulation
        simulation->update(deltaTime);

        // Render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        simulation->render();

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    simulation.reset();
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    if (simulation) {
        simulation->setWindowSize(width, height);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (!simulation) return;

    if (firstMouse) {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos) - lastX;
    // Do not invert y-axis
	float yoffset = static_cast<float>(ypos) - lastY;

    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    if (mousePressed) {
        simulation->rotateCamera(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (simulation) {
        simulation->zoomCamera(static_cast<float>(yoffset));
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        mousePressed = (action == GLFW_PRESS);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (!simulation) return;

    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;
        case GLFW_KEY_1:
            simulation->toggleStarfield();
            break;
        case GLFW_KEY_2:
            simulation->togglePlanets();
            break;
        case GLFW_KEY_3:
            simulation->toggleDisk();
            break;
        case GLFW_KEY_4:
            simulation->toggleLensing();
            break;
        case GLFW_KEY_Q:
            simulation->adjustMass(0.1f);
            break;
        case GLFW_KEY_A:
            simulation->adjustMass(-0.1f);
            break;
        case GLFW_KEY_W:
            simulation->adjustDiskSize(1.0f);
            break;
        case GLFW_KEY_S:
            simulation->adjustDiskSize(-1.0f);
            break;
        case GLFW_KEY_R:
            simulation->resetToDefaults();
            break;
        }
    }
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}