#pragma once

// Embedded GLSL shader sources

static auto BLACKHOLE_VERT_SRC = R"glsl(
#version 330 core

layout (location = 0) in vec2 a_position;

void main() {
    gl_Position = vec4(a_position, 0.0, 1.0);
}
)glsl";

static auto BLACKHOLE_FRAG_SRC = R"glsl(
#version 330 core

out vec4 FragColor;

uniform vec2 u_resolution;
uniform float u_time;
uniform mat4 u_invViewMatrix;
uniform vec3 u_cameraPosition;
uniform float u_mass;
uniform float u_schwarzschildRadius;
uniform float u_diskInnerRadius;
uniform float u_diskOuterRadius;
uniform int u_enableStarfield;
uniform int u_enablePlanets;
uniform int u_enableDisk;
uniform int u_enableLensing;
uniform float u_stepSize;
uniform int u_maxSteps;
uniform float u_farDist;
uniform float u_lensMaxRadius;

// Constants
const float PI = 3.14159265359;
const int MAX_STEPS = 300;
const float MAX_DIST = 100.0;
const float EPSILON = 0.001;

// Planet Properties
const float planet1_orbitRadius = 18.0;
const float planet1_radius = 0.4;
const float planet1_speed = 0.1;
const vec3 planet1_color = vec3(0.8, 0.3, 0.1);

const float planet2_orbitRadius = 30.0;
const float planet2_radius = 1.0;
const float planet2_speed = 0.05;
const vec3 planet2_color_base = vec3(0.3, 0.4, 0.7);

// Physics
const float G = 1.0;

// Utility Functions
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

// Procedural Starfield (background)
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
    float lon = atan(rd.z, rd.x);
    float lat = asin(clamp(rd.y, -1.0, 1.0));
    vec2 uv = vec2(lon / (2.0 * PI) + 0.5, lat / PI + 0.5);

    const vec2 GRID = vec2(520.0, 260.0);
    vec2 gUV = uv * GRID;
    vec2 baseCell = floor(gUV);
    vec2 f = fract(gUV);

    vec3 color = vec3(0.0);

    for (int j = -1; j <= 1; j++) {
        for (int i = -1; i <= 1; i++) {
            vec2 cid = baseCell + vec2(float(i), float(j));
            vec2 off = vec2(
                random(cid + vec2(13.1, 17.7)),
                random(cid + vec2(27.3, 39.5))
            );
            vec2 d = (vec2(float(i), float(j)) + off) - f;
            float dist = length(d);

            float sizeRnd = random(cid + vec2(3.7, 5.1));
            float size = 0.018 + 0.12 * sizeRnd * sizeRnd;

            float bRnd = random(cid + vec2(1.3, 2.1));
            float baseB = pow(bRnd, 10.0);
            float rare = step(0.985, random(cid + vec2(4.2, 7.9)));
            baseB += rare * 0.6;

            float twR = random(cid + vec2(9.2, 6.4));
            float tw = 0.88 + 0.22 * sin(t * (5.0 + 11.0 * twR) + twR * 6.28318);

            float core = smoothstep(size, 0.0, dist);
            core = core * core;
            float halo = smoothstep(2.5 * size, 0.0, dist) * 0.35;
            float starM = core + halo;
            float intensity = baseB * starM * tw;

            float temp = random(cid + vec2(2.7, 8.9));
            vec3 starCol = mix(vec3(1.0, 0.92, 0.86), vec3(0.75, 0.86, 1.0), temp);
            color += intensity * starCol;
        }
    }

    vec3 galN = normalize(vec3(0.0, 0.2, 1.0));
    float band = pow(1.0 - abs(dot(rd, galN)), 2.0);

    float nebBase = fbm(uv * vec2(8.0, 4.0));
    float nebDetail = fbm((uv + vec2(0.17, 0.03)) * vec2(16.0, 8.0));
    float nebMask = smoothstep(0.55, 0.9, band) * smoothstep(0.35, 0.85, nebBase);
    float neb = clamp(0.0 + 0.9 * nebBase + 0.4 * nebDetail, 0.0, 1.6) * nebMask;

    vec3 nebColA = vec3(0.12, 0.16, 0.22);
    vec3 nebColB = vec3(0.18, 0.12, 0.20);
    vec3 nebula = mix(nebColA, nebColB, nebDetail) * (0.06 * neb);
    color += nebula;

    color = color / (1.0 + color);
    return color;
}

vec4 getPlanetColor(vec3 p) {
    if (u_enablePlanets == 0) {
        return vec4(0.0);
    }

    // Planet 1 (Rocky Planet)
    float angle1 = u_time * planet1_speed;
    vec3 planet1_pos = vec3(cos(angle1) * planet1_orbitRadius, 0.0, sin(angle1) * planet1_orbitRadius);

    if (length(p - planet1_pos) < planet1_radius) {
        vec3 normal = normalize(p - planet1_pos);
        vec3 lightDir = normalize(-planet1_pos);
        float diffuse = max(0.0, dot(normal, lightDir)) * 0.7 + 0.3;
        return vec4(planet1_color * diffuse, 1.0);
    }

    // Planet 2 (Gas Giant)
    float angle2 = u_time * planet2_speed + 2.5;
    vec3 planet2_pos = vec3(cos(angle2) * planet2_orbitRadius, 0.0, sin(angle2) * planet2_orbitRadius);

    if (length(p - planet2_pos) < planet2_radius) {
        vec3 normal = normalize(p - planet2_pos);
        vec3 lightDir = normalize(-planet2_pos);
        float diffuse = max(0.0, dot(normal, lightDir)) * 0.6 + 0.4;

        float n = noise(p.xy * 3.0) * 0.5 + noise(p.yz * 6.0) * 0.5;
        vec3 texColor = mix(planet2_color_base, vec3(0.9), n);

        return vec4(texColor * diffuse, 1.0);
    }

    return vec4(0.0);
}

vec4 getDiskSample(vec3 p) {
    vec2 xz = p.xz;
    float r = length(xz);
    vec2 e_r = (r > 1e-4) ? xz / r : vec2(1.0, 0.0);

    float theta = atan(xz.y, xz.x);
    float omega = 1.6 * pow(max(r, 0.25), -1.5);
    float thetaFlow = theta - u_time * omega;

    float logr = log(max(r, 0.0007));
    float n = 0.0;
    n += 1.00 * noise(vec2(logr * 2.7,           sin(thetaFlow)));
    n += 0.50 * noise(vec2(logr * 5.11 + 17.0,   cos(thetaFlow)));
    n += 0.25 * noise(vec2(logr * 9.30 - 11.0,   sin(thetaFlow * 2.0)));
    n = clamp(n / 1.75, 0.0, 1.0);

    float arms = 3.0;
    float pitch = 4.0;
    float spiralPhase = arms * (thetaFlow + log(max(r, 0.0005)) * pitch);
    float armMask = pow(0.5 + 0.5 * cos(spiralPhase), 2.0);

    float radial = smoothstep(u_diskInnerRadius, u_diskOuterRadius, r);
    float armGain = mix(1.35, 1.1, radial);
    float intensity = pow(n, 1.5) * armGain * mix(0.7, 1.2, armMask) * (1.15 - 0.65 * radial);

    vec3 colInner = vec3(0.98, 0.98, 1.0);
    vec3 colMid   = vec3(1.0, 0.85, 0.55);
    vec3 colOuter = vec3(1.0, 0.55, 0.22);
    vec3 color = mix(colInner, colMid, smoothstep(0.0, 0.6, radial));
    color = mix(color, colOuter, smoothstep(0.4, 1.0, radial));

    float v = 0.8 * pow(max(r, 0.25), -0.5);
    vec3 velDir3 = normalize(vec3(-p.z, 0.0, p.x));
    vec3 viewDir = normalize(u_cameraPosition - p);
    float dop = clamp(dot(velDir3, viewDir) * v, -1.0, 1.0);
    color.r *= (1.0 - 0.35 * max(dop, 0.0));
    color.b *= (1.0 + 0.55 * max(-dop, 0.0));

    float falloff = 1.0 - smoothstep(u_diskInnerRadius, u_diskOuterRadius, r);
    vec3 emissive = color * intensity * (2.2 + 1.3 * (1.0 - radial)) * falloff;
    float baseAlpha = 0.33;
    float alpha = baseAlpha * falloff * clamp(intensity * (1.1 + 0.4 * (1.0 - radial)), 0.1, 1.0);
    return vec4(emissive, alpha);
}

vec3 rayMarch(vec3 rayOrigin, vec3 rayDir) {
    vec3 accColor = vec3(0.0);
    float transmittance = 1.0;
    vec3 p = rayOrigin;
    float farDist = (u_farDist > 0.0) ? u_farDist : MAX_DIST;

    for (int i = 0; i < MAX_STEPS; i++) {
        if (i >= u_maxSteps) break;
        vec3 p_prev = p;

        // Planet hit check
        vec4 planetHitColor = getPlanetColor(p);
        if (planetHitColor.w > 0.5) {
            accColor += transmittance * planetHitColor.rgb;
            return accColor;
        }

        // Event horizon check
        if (u_enableLensing == 1) {
            if (length(p) < u_schwarzschildRadius + EPSILON) {
                return accColor;
            }
        }

        // Apply gravitational lensing
        float r = length(p);
        float distToCenterSq = max(r * r, 1e-4);
        float stepSize = u_stepSize;
        stepSize += stepSize * smoothstep(u_diskOuterRadius + 2.0, farDist, r) * 2.5;
        stepSize *= 1.0 + 1.2 * smoothstep(0.5, 3.0, abs(p.y));

        if (u_enableLensing == 1 && r < u_lensMaxRadius) {
            vec3 gravityDir = (r > 1e-6) ? -p / r : vec3(0.0, 0.0, 0.0);
            vec3 acceleration = gravityDir * (G * u_mass) / distToCenterSq;
            rayDir = normalize(rayDir + acceleration * stepSize);
        }
        p += rayDir * stepSize;

        // Disk intersection
        if (u_enableDisk == 1 && p_prev.y * p.y < 0.0) {
            float t = -p_prev.y / (p.y - p_prev.y);
            vec3 hit = p_prev + t * (p - p_prev);
            float r_hit = length(hit.xz);
            if (r_hit > u_diskInnerRadius && r_hit < u_diskOuterRadius) {
                vec4 disk = getDiskSample(hit);
                accColor += transmittance * disk.rgb;
                transmittance *= (1.0 - disk.a);
                if (transmittance < 0.02) {
                    return accColor;
                }
            }
        }

        if (length(p) > farDist) {
            if (u_enableStarfield == 1) {
                accColor += transmittance * starField(rayDir, u_time);
            }
            return accColor;
        }
    }

    if (u_enableStarfield == 1) {
        accColor += transmittance * starField(rayDir, u_time);
    }
    return accColor;
}

void main() {
    vec2 uv = (gl_FragCoord.xy - 0.5 * u_resolution.xy) / u_resolution.y;
    vec3 rayDir = normalize(vec3(uv, -1.0));
    rayDir = (u_invViewMatrix * vec4(rayDir, 0.0)).xyz;

    // Early-out: starfield only, no marching needed
    if (u_enableStarfield == 1 && u_enablePlanets == 0 && u_enableDisk == 0 && u_enableLensing == 0) {
        vec3 bg = starField(rayDir, u_time);
        bg = pow(bg, vec3(0.4545));
        FragColor = vec4(bg, 1.0);
        return;
    }

    // If literally everything is off, return black
    if (u_enableStarfield == 0 && u_enablePlanets == 0 && u_enableDisk == 0 && u_enableLensing == 0) {
        FragColor = vec4(0.0);
        return;
    }

    vec3 color = rayMarch(u_cameraPosition, rayDir);
    float l = dot(color, vec3(0.21, 0.72, 0.07));
    color += color * l * 0.3;
    color = pow(color, vec3(0.4545));
    FragColor = vec4(color, 1.0);
}
)glsl";

