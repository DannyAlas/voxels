#version 450 core

uniform float time;

in vec4 near_4;    //for computing rays in fragment shader
in vec4 far_4;

out vec4 fragColor;

float sdSphere(vec3 p, float r) {
    return length(p) - r;
}

float sdBox(vec3 p, vec3 b) {
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float smin(float a, float b, float k) {
    float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
    return mix(b, a, h) - k * h * (1.0 - h);
}

float map(vec3 p) {
    vec3 spherePos = vec3(sin(time)*3.0, 0.0, 0.0);
    float sphere = sdSphere(p - spherePos, 1.0);
    
    float box = sdBox(p, vec3(0.75));

    float ground = p.y + 0.75;

    return smin(ground, smin(sphere, box, 2.), 1.);
}

// convert from screen space to world space

void main() {
    // init
    vec2 fragCoord = gl_FragCoord.xy;
    
    vec3 ro = near_4.xyz/near_4.w;
    vec3 far3 = far_4.xyz/far_4.w;
    vec3 dir = far3 - ro;
    vec3 rd = normalize(dir);

    vec3 col = vec3(0.0);

    float t = 0.0;

    // Raymarch
    for (int i = 0; i < 80; i++) {
        vec3 p = ro + rd * t; // position along the ray
        
        float d = map(p); // distance to the objects

        t+= d;

        if (d < 0.001 || t > 100.0) {
            break;
        }
    }

    col = vec3(t* 0.2);

    fragColor = vec4(col, 1.0);
}
