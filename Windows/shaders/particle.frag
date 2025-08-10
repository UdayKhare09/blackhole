// Fragment Shader for future enhancement
// This is for reference - current implementation uses legacy OpenGL

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
const int MAX_STEPS = 300; // Compile-time cap; runtime uses u_maxSteps
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

// Main ray marching implementation continues here...
// (This is a simplified version - the full implementation is in BlackHoleSimulation.cpp)

void main() {
    vec2 uv = (gl_FragCoord.xy - 0.5 * u_resolution.xy) / u_resolution.y;
    vec3 rayDir = normalize(vec3(uv, -1.0));
    rayDir = (u_invViewMatrix * vec4(rayDir, 0.0)).xyz;
    
    // Simplified black hole visualization
    vec3 color = vec3(0.0);
    
    // Simple distance-based coloring for demonstration
    float dist = length(rayDir);
    color = mix(vec3(1.0, 0.5, 0.0), vec3(0.0), smoothstep(0.5, 1.0, dist));
    
    gl_FragColor = vec4(color, 1.0);
}