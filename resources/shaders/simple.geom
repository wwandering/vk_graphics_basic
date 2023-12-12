#version 450
layout (triangles) in;
layout (triangle_strip, max_vertices = 48) out;

layout(push_constant) uniform params_t
{
    mat4 mProjView;
    mat4 mModel;
    float time;
} params;

layout (location = 0) in GS_IN
{
    vec3 wPos;
    vec3 wNorm;
    vec3 wTangent;
    vec2 texCoord;

} gIn[];

layout (location = 0) out GS_OUT
{
    vec3 wPos;
    vec3 wNorm;
    vec3 wTangent;
    vec2 texCoord;

} gOut;

void main() {
    for (int i = 0; i < 3; i++) {
        gOut.wNorm = gIn[i].wNorm;
        gOut.wPos = gIn[i].wPos + gOut.wNorm * (20 * abs(sin(params.time / 3))) * 0.1;
        vec3 norm = normalize(gIn[i].wPos);
        vec3 add = norm * 0.1 * (1 - abs(sin(params.time / 2)));
        if (gIn[i].wPos.y < 0.3) {
            add *= 0.0;
            gOut.wPos = gIn[i].wPos;
        }
        gl_Position = params.mProjView * vec4(gOut.wPos + add, 1.0);
        EmitVertex();
    }
    EndPrimitive();
}