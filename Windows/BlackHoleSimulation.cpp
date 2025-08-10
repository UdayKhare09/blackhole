#include "BlackHoleSimulation.h"
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>

// Constants
const float PI = 3.14159265359f;
const float DEG_TO_RAD = PI / 180.0f;

// Vec3 implementation
float Vec3::length() const {
    return std::sqrt(x * x + y * y + z * z);
}

Vec3 Vec3::normalize() const {
    float len = length();
    if (len > 1e-6f) {
        return *this / len;
    }
    return Vec3(0, 0, 0);
}

float Vec3::dot(const Vec3& other) const {
    return x * other.x + y * other.y + z * other.z;
}

Vec3 Vec3::cross(const Vec3& other) const {
    return Vec3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}

// Mat4 implementation
Mat4::Mat4() {
    m.fill(0.0f);
}

Mat4 Mat4::identity() {
    Mat4 result;
    result.m[0] = result.m[5] = result.m[10] = result.m[15] = 1.0f;
    return result;
}

Mat4 Mat4::lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
    Vec3 f = (center - eye).normalize();
    Vec3 s = f.cross(up).normalize();
    Vec3 u = s.cross(f);

    Mat4 result = identity();
    result.m[0] = s.x;
    result.m[4] = s.y;
    result.m[8] = s.z;
    result.m[1] = u.x;
    result.m[5] = u.y;
    result.m[9] = u.z;
    result.m[2] = -f.x;
    result.m[6] = -f.y;
    result.m[10] = -f.z;
    result.m[12] = -s.dot(eye);
    result.m[13] = -u.dot(eye);
    result.m[14] = f.dot(eye);
    return result;
}

Mat4 Mat4::perspective(float fovy, float aspect, float near, float far) {
    float tanHalfFovy = std::tan(fovy / 2.0f);
    
    Mat4 result;
    result.m[0] = 1.0f / (aspect * tanHalfFovy);
    result.m[5] = 1.0f / tanHalfFovy;
    result.m[10] = -(far + near) / (far - near);
    result.m[11] = -1.0f;
    result.m[14] = -(2.0f * far * near) / (far - near);
    return result;
}

Mat4 Mat4::inverse() const {
    Mat4 inv;
    float* invOut = inv.m.data();
    const float* m_ptr = m.data();

    invOut[0] = m_ptr[5] * m_ptr[10] * m_ptr[15] -
                m_ptr[5] * m_ptr[11] * m_ptr[14] -
                m_ptr[9] * m_ptr[6] * m_ptr[15] +
                m_ptr[9] * m_ptr[7] * m_ptr[14] +
                m_ptr[13] * m_ptr[6] * m_ptr[11] -
                m_ptr[13] * m_ptr[7] * m_ptr[10];

    invOut[4] = -m_ptr[4] * m_ptr[10] * m_ptr[15] +
                m_ptr[4] * m_ptr[11] * m_ptr[14] +
                m_ptr[8] * m_ptr[6] * m_ptr[15] -
                m_ptr[8] * m_ptr[7] * m_ptr[14] -
                m_ptr[12] * m_ptr[6] * m_ptr[11] +
                m_ptr[12] * m_ptr[7] * m_ptr[10];

    invOut[8] = m_ptr[4] * m_ptr[9] * m_ptr[15] -
               m_ptr[4] * m_ptr[11] * m_ptr[13] -
               m_ptr[8] * m_ptr[5] * m_ptr[15] +
               m_ptr[8] * m_ptr[7] * m_ptr[13] +
               m_ptr[12] * m_ptr[5] * m_ptr[11] -
               m_ptr[12] * m_ptr[7] * m_ptr[9];

    invOut[12] = -m_ptr[4] * m_ptr[9] * m_ptr[14] +
                 m_ptr[4] * m_ptr[10] * m_ptr[13] +
                 m_ptr[8] * m_ptr[5] * m_ptr[14] -
                 m_ptr[8] * m_ptr[6] * m_ptr[13] -
                 m_ptr[12] * m_ptr[5] * m_ptr[10] +
                 m_ptr[12] * m_ptr[6] * m_ptr[9];

    float det = m_ptr[0] * invOut[0] + m_ptr[1] * invOut[4] + m_ptr[2] * invOut[8] + m_ptr[3] * invOut[12];

    if (std::abs(det) < 1e-6f) {
        return identity(); // Return identity if matrix is not invertible
    }

    det = 1.0f / det;

    // Calculate remaining elements...
    invOut[1] = (-m_ptr[1] * m_ptr[10] * m_ptr[15] +
                 m_ptr[1] * m_ptr[11] * m_ptr[14] +
                 m_ptr[9] * m_ptr[2] * m_ptr[15] -
                 m_ptr[9] * m_ptr[3] * m_ptr[14] -
                 m_ptr[13] * m_ptr[2] * m_ptr[11] +
                 m_ptr[13] * m_ptr[3] * m_ptr[10]) * det;

    invOut[5] = (m_ptr[0] * m_ptr[10] * m_ptr[15] -
                 m_ptr[0] * m_ptr[11] * m_ptr[14] -
                 m_ptr[8] * m_ptr[2] * m_ptr[15] +
                 m_ptr[8] * m_ptr[3] * m_ptr[14] +
                 m_ptr[12] * m_ptr[2] * m_ptr[11] -
                 m_ptr[12] * m_ptr[3] * m_ptr[10]) * det;

    invOut[9] = (-m_ptr[0] * m_ptr[9] * m_ptr[15] +
                 m_ptr[0] * m_ptr[11] * m_ptr[13] +
                 m_ptr[8] * m_ptr[1] * m_ptr[15] -
                 m_ptr[8] * m_ptr[3] * m_ptr[13] -
                 m_ptr[12] * m_ptr[1] * m_ptr[11] +
                 m_ptr[12] * m_ptr[3] * m_ptr[9]) * det;

    invOut[13] = (m_ptr[0] * m_ptr[9] * m_ptr[14] -
                  m_ptr[0] * m_ptr[10] * m_ptr[13] -
                  m_ptr[8] * m_ptr[1] * m_ptr[14] +
                  m_ptr[8] * m_ptr[2] * m_ptr[13] +
                  m_ptr[12] * m_ptr[1] * m_ptr[10] -
                  m_ptr[12] * m_ptr[2] * m_ptr[9]) * det;

    invOut[2] = (m_ptr[1] * m_ptr[6] * m_ptr[15] -
                 m_ptr[1] * m_ptr[7] * m_ptr[14] -
                 m_ptr[5] * m_ptr[2] * m_ptr[15] +
                 m_ptr[5] * m_ptr[3] * m_ptr[14] +
                 m_ptr[13] * m_ptr[2] * m_ptr[7] -
                 m_ptr[13] * m_ptr[3] * m_ptr[6]) * det;

    invOut[6] = (-m_ptr[0] * m_ptr[6] * m_ptr[15] +
                 m_ptr[0] * m_ptr[7] * m_ptr[14] +
                 m_ptr[4] * m_ptr[2] * m_ptr[15] -
                 m_ptr[4] * m_ptr[3] * m_ptr[14] -
                 m_ptr[12] * m_ptr[2] * m_ptr[7] +
                 m_ptr[12] * m_ptr[3] * m_ptr[6]) * det;

    invOut[10] = (m_ptr[0] * m_ptr[5] * m_ptr[15] -
                  m_ptr[0] * m_ptr[7] * m_ptr[13] -
                  m_ptr[4] * m_ptr[1] * m_ptr[15] +
                  m_ptr[4] * m_ptr[3] * m_ptr[13] +
                  m_ptr[12] * m_ptr[1] * m_ptr[7] -
                  m_ptr[12] * m_ptr[3] * m_ptr[5]) * det;

    invOut[14] = (-m_ptr[0] * m_ptr[5] * m_ptr[14] +
                  m_ptr[0] * m_ptr[6] * m_ptr[13] +
                  m_ptr[4] * m_ptr[1] * m_ptr[14] -
                  m_ptr[4] * m_ptr[2] * m_ptr[13] -
                  m_ptr[12] * m_ptr[1] * m_ptr[6] +
                  m_ptr[12] * m_ptr[2] * m_ptr[5]) * det;

    invOut[3] = (-m_ptr[1] * m_ptr[6] * m_ptr[11] +
                 m_ptr[1] * m_ptr[7] * m_ptr[10] +
                 m_ptr[5] * m_ptr[2] * m_ptr[11] -
                 m_ptr[5] * m_ptr[3] * m_ptr[10] -
                 m_ptr[9] * m_ptr[2] * m_ptr[7] +
                 m_ptr[9] * m_ptr[3] * m_ptr[6]) * det;

    invOut[7] = (m_ptr[0] * m_ptr[6] * m_ptr[11] -
                 m_ptr[0] * m_ptr[7] * m_ptr[10] -
                 m_ptr[4] * m_ptr[2] * m_ptr[11] +
                 m_ptr[4] * m_ptr[3] * m_ptr[10] +
                 m_ptr[8] * m_ptr[2] * m_ptr[7] -
                 m_ptr[8] * m_ptr[3] * m_ptr[6]) * det;

    invOut[11] = (-m_ptr[0] * m_ptr[5] * m_ptr[11] +
                  m_ptr[0] * m_ptr[7] * m_ptr[9] +
                  m_ptr[4] * m_ptr[1] * m_ptr[11] -
                  m_ptr[4] * m_ptr[3] * m_ptr[9] -
                  m_ptr[8] * m_ptr[1] * m_ptr[7] +
                  m_ptr[8] * m_ptr[3] * m_ptr[5]) * det;

    invOut[15] = (m_ptr[0] * m_ptr[5] * m_ptr[10] -
                  m_ptr[0] * m_ptr[6] * m_ptr[9] -
                  m_ptr[4] * m_ptr[1] * m_ptr[10] +
                  m_ptr[4] * m_ptr[2] * m_ptr[9] +
                  m_ptr[8] * m_ptr[1] * m_ptr[6] -
                  m_ptr[8] * m_ptr[2] * m_ptr[5]) * det;

    for (int i = 0; i < 16; i++) {
        invOut[i] *= det;
    }

    return inv;
}

Mat4 Mat4::operator*(const Mat4& other) const {
    Mat4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i * 4 + j] = 0.0f;
            for (int k = 0; k < 4; k++) {
                result.m[i * 4 + j] += m[i * 4 + k] * other.m[k * 4 + j];
            }
        }
    }
    return result;
}

// Camera implementation
Camera::Camera() {
    reset();
}

void Camera::reset() {
    azimuth = 0.5f;
    elevation = 1.5f;
    radius = 15.0f;
    minRadius = 4.0f;
    maxRadius = 50.0f;
    target = Vec3(0, 0, 0);
    update();
}

void Camera::rotate(float deltaAzimuth, float deltaElevation) {
    azimuth -= deltaAzimuth * 0.005f;
    elevation -= deltaElevation * 0.005f;
    elevation = std::max(0.01f, std::min(PI - 0.01f, elevation));
    update();
}

void Camera::zoom(float delta) {
    radius += delta * 0.5f;
    radius = std::max(minRadius, std::min(maxRadius, radius));
    update();
}

void Camera::update() {
    position.x = radius * std::sin(elevation) * std::cos(azimuth);
    position.y = radius * std::cos(elevation);
    position.z = radius * std::sin(elevation) * std::sin(azimuth);
    
    viewMatrix = Mat4::lookAt(position, target, Vec3(0, 1, 0));
    invViewMatrix = viewMatrix.inverse();
}

Mat4 Camera::getViewMatrix() const {
    return viewMatrix;
}

Mat4 Camera::getInverseViewMatrix() const {
    return invViewMatrix;
}

// Shader implementation
Shader::~Shader() {
    if (programID != 0) {
        glDeleteProgram(programID);
    }
}

bool Shader::loadFromSource(const std::string& vertexSource, const std::string& fragmentSource) {
    GLuint vertexShader = compileShader(vertexSource, GL_VERTEX_SHADER);
    if (vertexShader == 0) return false;

    GLuint fragmentShader = compileShader(fragmentSource, GL_FRAGMENT_SHADER);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return false;
    }

    programID = glCreateProgram();
    glAttachShader(programID, vertexShader);
    glAttachShader(programID, fragmentShader);
    glLinkProgram(programID);

    GLint success;
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(programID, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(programID);
        programID = 0;
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return true;
}

GLuint Shader::compileShader(const std::string& source, GLenum type) const {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::string typeStr = (type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT";
        std::cerr << "Shader compilation failed (" << typeStr << "): " << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

void Shader::use() const {
    glUseProgram(programID);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, float x, float y) const {
    glUniform2f(glGetUniformLocation(programID, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z);
}

void Shader::setVec3(const std::string& name, const Vec3& vec) const {
    glUniform3f(glGetUniformLocation(programID, name.c_str()), vec.x, vec.y, vec.z);
}

void Shader::setMat4(const std::string& name, const Mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, mat.data());
}

// BlackHoleSimulation implementation
BlackHoleSimulation::BlackHoleSimulation(int windowWidth, int windowHeight) 
    : windowWidth(windowWidth), windowHeight(windowHeight), currentTime(0.0f),
      quadVAO(0), quadVBO(0), frameTime(0.0f), frameCount(0), 
      fpsUpdateTimer(0.0f), currentFPS(0.0f) {
    params.updateComputedValues();
}

BlackHoleSimulation::~BlackHoleSimulation() {
    if (quadVAO != 0) {
        glDeleteVertexArrays(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);
    }
}

bool BlackHoleSimulation::initialize() {
    std::cout << "Initializing Black Hole Simulation..." << std::endl;
    
    if (!initializeShaders()) {
        std::cerr << "Failed to initialize shaders" << std::endl;
        return false;
    }
    
    if (!initializeGeometry()) {
        std::cerr << "Failed to initialize geometry" << std::endl;
        return false;
    }
    
    std::cout << "Simulation initialized successfully!" << std::endl;
    printStatus();
    return true;
}

void BlackHoleSimulation::update(float deltaTime) {
    currentTime += deltaTime;
    frameTime = deltaTime;
    frameCount++;
    fpsUpdateTimer += deltaTime;
    
    // Update FPS every second
    if (fpsUpdateTimer >= 1.0f) {
        currentFPS = frameCount / fpsUpdateTimer;
        frameCount = 0;
        fpsUpdateTimer = 0.0f;
    }
    
    camera.update();
    params.updateComputedValues();
}

void BlackHoleSimulation::render() {
    rayMarchingShader->use();
    updateUniforms();
    
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void BlackHoleSimulation::rotateCamera(float deltaX, float deltaY) {
    camera.rotate(deltaX, deltaY);
}

void BlackHoleSimulation::zoomCamera(float delta) {
    camera.zoom(delta);
}

void BlackHoleSimulation::setWindowSize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
}

void BlackHoleSimulation::adjustMass(float delta) {
    params.mass = std::max(0.1f, std::min(5.0f, params.mass + delta));
    std::cout << "Black hole mass: " << params.mass << std::endl;
}

void BlackHoleSimulation::adjustDiskSize(float delta) {
    params.diskOuterRadius = std::max(2.0f, std::min(30.0f, params.diskOuterRadius + delta));
    std::cout << "Disk outer radius: " << params.diskOuterRadius << std::endl;
}

void BlackHoleSimulation::resetToDefaults() {
    params = SimulationParams();
    camera.reset();
    std::cout << "Reset to default parameters" << std::endl;
    printStatus();
}

bool BlackHoleSimulation::initializeShaders() {
    rayMarchingShader = std::make_unique<Shader>();
    return rayMarchingShader->loadFromSource(getVertexShaderSource(), getFragmentShaderSource());
}

bool BlackHoleSimulation::initializeGeometry() {
    // Create a full-screen quad
    float quadVertices[] = {
        -1.0f,  1.0f,  // top-left
         1.0f,  1.0f,  // top-right
        -1.0f, -1.0f,  // bottom-left
         1.0f, -1.0f   // bottom-right
    };

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    return true;
}

void BlackHoleSimulation::updateUniforms() {
    rayMarchingShader->setVec2("u_resolution", static_cast<float>(windowWidth), static_cast<float>(windowHeight));
    rayMarchingShader->setFloat("u_time", currentTime);
    rayMarchingShader->setMat4("u_invViewMatrix", camera.getInverseViewMatrix());
    rayMarchingShader->setVec3("u_cameraPosition", camera.getPosition());
    
    rayMarchingShader->setFloat("u_mass", params.mass);
    rayMarchingShader->setFloat("u_schwarzschildRadius", params.schwarzschildRadius);
    rayMarchingShader->setFloat("u_diskInnerRadius", params.diskInnerRadius);
    rayMarchingShader->setFloat("u_diskOuterRadius", params.diskOuterRadius);
    
    rayMarchingShader->setInt("u_enableStarfield", params.starfieldEnabled ? 1 : 0);
    rayMarchingShader->setInt("u_enablePlanets", params.planetsEnabled ? 1 : 0);
    rayMarchingShader->setInt("u_enableDisk", params.diskEnabled ? 1 : 0);
    rayMarchingShader->setInt("u_enableLensing", params.lensingEnabled ? 1 : 0);
    
    // Adaptive parameters based on performance and settings
    float baseStep = params.lensingEnabled ? 0.25f : 0.45f;
    if (!params.diskEnabled) baseStep *= 1.15f;
    if (!params.planetsEnabled) baseStep *= 1.15f;
    
    // Adjust based on FPS
    if (currentFPS > 0.0f) {
        if (currentFPS < 30.0f) baseStep *= 1.2f;
        else if (currentFPS > 55.0f) baseStep *= 0.9f;
    }
    
    baseStep = std::max(0.15f, std::min(0.8f, baseStep));
    int maxSteps = params.lensingEnabled ? 220 : 120;
    float farDist = 100.0f;
    float lensMaxRadius = params.lensingEnabled ? (params.diskOuterRadius * 2.0f + 6.0f) : 0.0f;
    
    rayMarchingShader->setFloat("u_stepSize", baseStep);
    rayMarchingShader->setInt("u_maxSteps", maxSteps);
    rayMarchingShader->setFloat("u_farDist", farDist);
    rayMarchingShader->setFloat("u_lensMaxRadius", lensMaxRadius);
}

void BlackHoleSimulation::printStatus() const {
    std::cout << "\n=== Current Settings ===" << std::endl;
    std::cout << "Mass: " << params.mass << std::endl;
    std::cout << "Disk radius: " << params.diskOuterRadius << std::endl;
    std::cout << "Starfield: " << (params.starfieldEnabled ? "ON" : "OFF") << std::endl;
    std::cout << "Planets: " << (params.planetsEnabled ? "ON" : "OFF") << std::endl;
    std::cout << "Disk: " << (params.diskEnabled ? "ON" : "OFF") << std::endl;
    std::cout << "Lensing: " << (params.lensingEnabled ? "ON" : "OFF") << std::endl;
    if (currentFPS > 0.0f) {
        std::cout << "FPS: " << currentFPS << std::endl;
    }
    std::cout << "=======================" << std::endl;
}

std::string BlackHoleSimulation::getVertexShaderSource() const {
    return R"(
#version 330 core
layout (location = 0) in vec2 a_position;

void main() {
    gl_Position = vec4(a_position, 0.0, 1.0);
}
)";
}

std::string BlackHoleSimulation::getFragmentShaderSource() const {
    return R"(
#version 330 core
precision highp float;

uniform vec2 u_resolution;
uniform float u_time;
uniform mat4 u_invViewMatrix; 
uniform vec3 u_cameraPosition;
uniform float u_mass;
uniform float u_schwarzschildRadius;
uniform float u_diskInnerRadius;
uniform float u_diskOuterRadius;
uniform int u_enableStarfield; // 1=on, 0=off
uniform int u_enablePlanets;   // 1=on, 0=off
uniform int u_enableDisk;      // 1=on, 0=off
uniform int u_enableLensing;   // 1=on, 0=off
// Adaptive marching controls
uniform float u_stepSize;       // base step size (scene units)
uniform int   u_maxSteps;       // dynamic max steps (<= MAX_STEPS)
uniform float u_farDist;        // escape distance
uniform float u_lensMaxRadius;  // radius beyond which lensing is negligible

// --- Constants ---
const float PI = 3.14159265359;
const int MAX_STEPS = 300; // Compile-time cap for WebGL; runtime uses u_maxSteps
const float MAX_DIST = 100.0; // fallback
const float EPSILON = 0.001;

// --- Planet Properties ---
const float planet1_orbitRadius = 18.0;
const float planet1_radius = 0.4;
const float planet1_speed = 0.1;
const vec3  planet1_color = vec3(0.8, 0.3, 0.1);

const float planet2_orbitRadius = 30.0;
const float planet2_radius = 1.0;
const float planet2_speed = 0.05;
const vec3  planet2_color_base = vec3(0.3, 0.4, 0.7);

// --- Physics ---
const float G = 1.0; // scaled units

// --- Utility Functions ---
float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

float noise(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));
    vec2 u = f * f * (3.0 - 2.0 * f);
    return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

// --- Procedural Starfield (background) ---
float fbm(vec2 p) {
    float a = 0.5;
    float f = 0.0;
    float sum = 0.0;
    for (int k = 0; k < 4; k++) {
        f += a * noise(p);
        sum += a;
        p = p * 2.03 + vec2(17.7, 11.3);
        a *= 0.5;
    }
    return (sum > 0.0) ? (f / sum) : 0.0;
}

vec3 starField(vec3 rd, float t) {
    rd = normalize(rd);
    // Spherical mapping (longitude/latitude)
    float lon = atan(rd.z, rd.x); // -PI..PI
    float lat = asin(clamp(rd.y, -1.0, 1.0)); // -PI/2..PI/2
    vec2 uv = vec2(lon / (2.0 * PI) + 0.5, lat / PI + 0.5);

    // Star grid resolution controls density (more cells -> more potential stars)
    const vec2 GRID = vec2(520.0, 260.0);
    vec2 gUV = uv * GRID;
    vec2 baseCell = floor(gUV);
    vec2 f = fract(gUV);

    vec3 color = vec3(0.0);

    // Sample 3x3 neighbor cells for stars near borders
    for (int j = -1; j <= 1; j++) {
        for (int i = -1; i <= 1; i++) {
            vec2 cid = baseCell + vec2(float(i), float(j));
            // Random star position within the cell
            vec2 off = vec2(
                random(cid + vec2(13.1, 17.7)),
                random(cid + vec2(27.3, 39.5))
            );
            vec2 d = (vec2(float(i), float(j)) + off) - f;
            float dist = length(d);

            // Size distribution: many tiny, few larger (smaller overall for sharper look)
            float sizeRnd = random(cid + vec2(3.7, 5.1));
            float size = 0.018 + 0.12 * sizeRnd * sizeRnd;

            // Brightness distribution: heavy-tailed, with rare bright stars
            float bRnd = random(cid + vec2(1.3, 2.1));
            float baseB = pow(bRnd, 10.0);
            float rare = step(0.985, random(cid + vec2(4.2, 7.9)));
            baseB += rare * 0.6; // occasional bright pop

            // Twinkle: subtle temporal modulation per star
            float twR = random(cid + vec2(9.2, 6.4));
            float tw = 0.88 + 0.22 * sin(t * (5.0 + 11.0 * twR) + twR * 6.28318);

            // Star profile: sharp core + soft halo
            float core = smoothstep(size, 0.0, dist);
            core = core * core; // sharpen
            float halo = smoothstep(2.5 * size, 0.0, dist) * 0.35;
            float starM = core + halo;
            float intensity = baseB * starM * tw;

            // Temperature-based coloration (blue-white to warm)
            float temp = random(cid + vec2(2.7, 8.9));
            vec3 starCol = mix(vec3(1.0, 0.92, 0.86), vec3(0.75, 0.86, 1.0), temp);
            color += intensity * starCol;
        }
    }

    // Subtle Milky Way-like band and wispy nebula
    vec3 galN = normalize(vec3(0.0, 0.2, 1.0));
    float band = pow(1.0 - abs(dot(rd, galN)), 2.0);

    // Nebula noise with gentle variation, concentrated near the band
    // Align noise to sky UV to avoid stretching at poles
    float nebBase = fbm(uv * vec2(8.0, 4.0));
    float nebDetail = fbm((uv + vec2(0.17, 0.03)) * vec2(16.0, 8.0));
    float nebMask = smoothstep(0.55, 0.9, band) * smoothstep(0.35, 0.85, nebBase);
    float neb = clamp(0.0 + 0.9 * nebBase + 0.4 * nebDetail, 0.0, 1.6) * nebMask;

    // Soft color blend for nebula (very subtle)
    vec3 nebColA = vec3(0.12, 0.16, 0.22);
    vec3 nebColB = vec3(0.18, 0.12, 0.20);
    vec3 nebula = mix(nebColA, nebColB, nebDetail) * (0.06 * neb);
    color += nebula;

    // Gentle tone mapping for the background
    color = color / (1.0 + color);
    return color;
}

/**
 * Calculates the color of a planet if the ray hits one.
 * Returns vec4(color, 1.0) on hit, vec4(0.0) on miss.
 * The 'w' component is used as a hit flag.
 */
vec4 getPlanetColor(vec3 p) {
    if (u_enablePlanets == 0) {
        return vec4(0.0);
    }
    // Planet 1 (Rocky Planet)
    float angle1 = u_time * planet1_speed;
    vec3 planet1_pos = vec3(cos(angle1) * planet1_orbitRadius, 0.0, sin(angle1) * planet1_orbitRadius);
    
    if (length(p - planet1_pos) < planet1_radius) {
        // Simple lighting: brighter on the side facing the center (accretion disk)
        vec3 normal = normalize(p - planet1_pos);
        vec3 lightDir = normalize(-planet1_pos); // Light from the center
        float diffuse = max(0.0, dot(normal, lightDir)) * 0.7 + 0.3;
        return vec4(planet1_color * diffuse, 1.0);
    }

    // Planet 2 (Gas Giant)
    float angle2 = u_time * planet2_speed + 2.5; // Offset start angle
    vec3 planet2_pos = vec3(cos(angle2) * planet2_orbitRadius, 0.0, sin(angle2) * planet2_orbitRadius);

    if (length(p - planet2_pos) < planet2_radius) {
        vec3 normal = normalize(p - planet2_pos);
        vec3 lightDir = normalize(-planet2_pos);
        float diffuse = max(0.0, dot(normal, lightDir)) * 0.6 + 0.4;

        // Add some noise for texture/bands
        float n = noise(p.xy * 3.0) * 0.5 + noise(p.yz * 6.0) * 0.5;
        vec3 texColor = mix(planet2_color_base, vec3(0.9), n);

        return vec4(texColor * diffuse, 1.0);
    }

    return vec4(0.0); // No hit
}

/**
 * Sample the accretion disk at a point: returns emissive color and alpha.
 * The disk is modeled as a thin, semi-transparent, glowing plane at y=0 between inner/outer radii.
 */
vec4 getDiskSample(vec3 p) {
    // Polar setup (no direct use of angle to avoid seam)
    vec2 xz = p.xz;
    float r = length(xz);
    vec2 e_r = (r > 1e-4) ? xz / r : vec2(1.0, 0.0);
    vec2 e_t = vec2(-e_r.y, e_r.x);

    // Keplerian angular rotation: use thetaFlow to rotate pattern rigidly per-radius
    float theta = atan(xz.y, xz.x);
    float omega = 1.6 * pow(max(r, 0.25), -1.5); // angular velocity ~ r^-1.5
    float thetaFlow = theta - u_time * omega;

    // Seam-free polar noise: use log(r) and sin/cos(thetaFlow)
    float logr = log(max(r, 0.0007));
    float n = 0.0;
    n += 1.00 * noise(vec2(logr * 2.7,           sin(thetaFlow)));
    n += 0.50 * noise(vec2(logr * 5.11 + 17.0,   cos(thetaFlow)));
    n += 0.25 * noise(vec2(logr * 9.30 - 11.0,   sin(thetaFlow * 2.0)));
    n = clamp(n / 1.75, 0.0, 1.0);

    // Spiral arm modulation (rotate with thetaFlow to remain stable)
    float arms = 3.0; // number of arms
    float pitch = 4.0; // spiral tightness
    float spiralPhase = arms * (thetaFlow + log(max(r, 0.0005)) * pitch);
    float armMask = pow(0.5 + 0.5 * cos(spiralPhase), 2.0);
    // Blend arm contrast stronger inside, softer outside
    float radial = smoothstep(u_diskInnerRadius, u_diskOuterRadius, r);
    float armGain = mix(1.35, 1.1, radial);
    float intensity = pow(n, 1.5) * armGain * mix(0.7, 1.2, armMask) * (1.15 - 0.65 * radial);

    // Temperature/brightness gradient (hot inner -> warm outer)
    vec3 colInner = vec3(0.98, 0.98, 1.0);   // blue-white hot
    vec3 colMid   = vec3(1.0, 0.85, 0.55);   // golden
    vec3 colOuter = vec3(1.0, 0.55, 0.22);   // warm orange
    vec3 color = mix(colInner, colMid, smoothstep(0.0, 0.6, radial));
    color = mix(color, colOuter, smoothstep(0.4, 1.0, radial));

    // Doppler tint from local orbital velocity relative to view
    float v = 0.8 * pow(max(r, 0.25), -0.5); // orbital speed ~ r^-0.5
    vec3 velDir3 = normalize(vec3(-p.z, 0.0, p.x));
    vec3 viewDir = normalize(u_cameraPosition - p);
    float dop = clamp(dot(velDir3, viewDir) * v, -1.0, 1.0);
    color.r *= (1.0 - 0.35 * max(dop, 0.0));
    color.b *= (1.0 + 0.55 * max(-dop, 0.0));

    // Emissive and alpha with radial falloff
    float falloff = 1.0 - smoothstep(u_diskInnerRadius, u_diskOuterRadius, r);
    vec3 emissive = color * intensity * (2.2 + 1.3 * (1.0 - radial)) * falloff;
    float baseAlpha = 0.33;
    float alpha = baseAlpha * falloff * clamp(intensity * (1.1 + 0.4 * (1.0 - radial)), 0.1, 1.0);
    return vec4(emissive, alpha);
}

/**
 * Main ray marching function with planet detection.
 */
vec3 rayMarch(vec3 rayOrigin, vec3 rayDir) {
    vec3 accColor = vec3(0.0); // accumulated color along the ray
    float transmittance = 1.0; // remaining transparency (1 means fully visible background)
    vec3 p = rayOrigin;
    float farDist = (u_farDist > 0.0) ? u_farDist : MAX_DIST;

    for (int i = 0; i < MAX_STEPS; i++) {
        if (i >= u_maxSteps) break;
        vec3 p_prev = p;

        // Opaque planet hit: composite and stop
        vec4 planetHitColor = getPlanetColor(p);
        if (planetHitColor.w > 0.5) {
            accColor += transmittance * planetHitColor.rgb;
            return accColor;
        }

        // Event horizon absorbs remaining light: stop
        if (u_enableLensing == 1) {
            if (length(p) < u_schwarzschildRadius + EPSILON) {
                return accColor;
            }
        }

        // Apply gravitational lensing (curved geodesic approximation)
        float r = length(p);
        float distToCenterSq = max(r * r, 1e-4);
        // Adaptive step: take larger steps far from disk and outside lensing region
        float step = u_stepSize;
        // Increase step with distance from center
        step += step * smoothstep(u_diskOuterRadius + 2.0, farDist, r) * 2.5;
        // Increase step when far from the disk plane (|y| large)
        step *= 1.0 + 1.2 * smoothstep(0.5, 3.0, abs(p.y));

        if (u_enableLensing == 1 && r < u_lensMaxRadius) {
            vec3 gravityDir = (r > 1e-6) ? -p / r : vec3(0.0, 0.0, 0.0);
            vec3 acceleration = gravityDir * (G * u_mass) / distToCenterSq;
            rayDir = normalize(rayDir + acceleration * step);
        }
        p += rayDir * step;

        // Thin-disk intersection via sign change on y
        if (u_enableDisk == 1 && p_prev.y * p.y < 0.0) {
            float t = -p_prev.y / (p.y - p_prev.y);
            vec3 hit = p_prev + t * (p - p_prev);
            float r = length(hit.xz);
            if (r > u_diskInnerRadius && r < u_diskOuterRadius) {
                vec4 disk = getDiskSample(hit);
                // Front-to-back compositing
                accColor += transmittance * disk.rgb;
                transmittance *= (1.0 - disk.a);
                // Early out if nearly opaque after accumulation
                if (transmittance < 0.02) {
                    return accColor;
                }
            }
        }

        if (length(p) > farDist) {
            // Ray escaped scene bounds: composite starfield using bent direction
            if (u_enableStarfield == 1) {
                accColor += transmittance * starField(rayDir, u_time);
            }
            return accColor;
        }
    }

    // If we reached max steps without escaping, add background as well
    if (u_enableStarfield == 1) {
        accColor += transmittance * starField(rayDir, u_time);
    }
    return accColor;
}

/**
 * Main function executed for every pixel.
 */
void main() {
    vec2 uv = (gl_FragCoord.xy - 0.5 * u_resolution.xy) / u_resolution.y;
    vec3 rayDir = normalize(vec3(uv, -1.0));
    rayDir = (u_invViewMatrix * vec4(rayDir, 0.0)).xyz;
    
    // Early-out: starfield only, no marching needed
    if (u_enableStarfield == 1 && u_enablePlanets == 0 && u_enableDisk == 0 && u_enableLensing == 0) {
        vec3 bg = starField(rayDir, u_time);
        bg = pow(bg, vec3(0.4545));
        gl_FragColor = vec4(bg, 1.0);
        return;
    }
    
    // If literally everything is off, return black
    if (u_enableStarfield == 0 && u_enablePlanets == 0 && u_enableDisk == 0 && u_enableLensing == 0) {
        gl_FragColor = vec4(0.0);
        return;
    }
    
    vec3 color = rayMarch(u_cameraPosition, rayDir);
    float l = dot(color, vec3(0.21, 0.72, 0.07));
    color += color * l * 0.3; // Bloom/glow effect
    color = pow(color, vec3(0.4545)); // Gamma correction
    gl_FragColor = vec4(color, 1.0);
}
)";
}