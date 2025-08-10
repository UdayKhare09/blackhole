#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <memory>
#include <vector>
#include <array>

// Simple 3D vector and matrix math structures
struct Vec3 {
    float x, y, z;
    
    Vec3() : x(0), y(0), z(0) {}
    Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    
    Vec3 operator+(const Vec3& other) const { return Vec3(x + other.x, y + other.y, z + other.z); }
    Vec3 operator-(const Vec3& other) const { return Vec3(x - other.x, y - other.y, z - other.z); }
    Vec3 operator*(float scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }
    Vec3 operator/(float scalar) const { return Vec3(x / scalar, y / scalar, z / scalar); }
    
    float length() const;
    Vec3 normalize() const;
    float dot(const Vec3& other) const;
    Vec3 cross(const Vec3& other) const;
};

struct Mat4 {
    std::array<float, 16> m;
    
    Mat4();
    static Mat4 identity();
    static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up);
    static Mat4 perspective(float fovy, float aspect, float near, float far);
    Mat4 inverse() const;
    Mat4 operator*(const Mat4& other) const;
    
    const float* data() const { return m.data(); }
};

class Camera {
public:
    Camera();
    
    void rotate(float deltaAzimuth, float deltaElevation);
    void zoom(float delta);
    void update();
    
    Mat4 getViewMatrix() const;
    Mat4 getInverseViewMatrix() const;
    Vec3 getPosition() const { return position; }
    
    void reset();

private:
    float azimuth;
    float elevation;
    float radius;
    float minRadius;
    float maxRadius;
    Vec3 target;
    Vec3 position;
    Mat4 viewMatrix;
    Mat4 invViewMatrix;
};

class Shader {
public:
    Shader() : programID(0) {}
    ~Shader();
    
    bool loadFromSource(const std::string& vertexSource, const std::string& fragmentSource);
    void use() const;
    
    void setFloat(const std::string& name, float value) const;
    void setInt(const std::string& name, int value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec3(const std::string& name, const Vec3& vec) const;
    void setMat4(const std::string& name, const Mat4& mat) const;
    
    GLuint getID() const { return programID; }

private:
    GLuint programID;
    
    GLuint compileShader(const std::string& source, GLenum type) const;
    void checkCompileErrors(GLuint shader, const std::string& type) const;
};

struct SimulationParams {
    float mass = 1.0f;
    float diskOuterRadius = 8.0f;
    bool starfieldEnabled = true;
    bool planetsEnabled = true;
    bool diskEnabled = true;
    bool lensingEnabled = true;
    
    // Computed values
    float schwarzschildRadius = 1.0f;
    float diskInnerRadius = 1.5f;
    
    void updateComputedValues() {
        schwarzschildRadius = mass;
        diskInnerRadius = 1.5f * schwarzschildRadius;
    }
};

class BlackHoleSimulation {
public:
    BlackHoleSimulation(int windowWidth, int windowHeight);
    ~BlackHoleSimulation();
    
    bool initialize();
    void update(float deltaTime);
    void render();
    
    // Input handling
    void rotateCamera(float deltaX, float deltaY);
    void zoomCamera(float delta);
    void setWindowSize(int width, int height);
    
    // Parameter controls
    void toggleStarfield() { params.starfieldEnabled = !params.starfieldEnabled; }
    void togglePlanets() { params.planetsEnabled = !params.planetsEnabled; }
    void toggleDisk() { params.diskEnabled = !params.diskEnabled; }
    void toggleLensing() { params.lensingEnabled = !params.lensingEnabled; }
    
    void adjustMass(float delta);
    void adjustDiskSize(float delta);
    void resetToDefaults();

private:
    int windowWidth, windowHeight;
    float currentTime;
    Camera camera;
    SimulationParams params;
    
    // OpenGL objects
    std::unique_ptr<Shader> rayMarchingShader;
    GLuint quadVAO, quadVBO;
    
    // Performance monitoring
    float frameTime;
    int frameCount;
    float fpsUpdateTimer;
    float currentFPS;
    
    bool initializeShaders();
    bool initializeGeometry();
    void updateUniforms();
    void printStatus() const;
    
    // Shader sources
    std::string getVertexShaderSource() const;
    std::string getFragmentShaderSource() const;
};