#version 450

// vec2 positions[3] = vec2[]
// (
//   vec2(0.0, -0.5),
//   vec2(0.5, 0.5),
//   vec2(-0.5, 0.5)
// );

layout(set = 0, binding = 0) uniform GlobalUBO
{
  mat4 projectionViewMatrix;
  vec3 directionToLight;
}ubo;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push {
  mat4 modelMatrix; 
  mat4 normalMatrix;
} push;

const float AMBIENT = 0.02;

void main() 
{
  gl_Position = ubo.projectionViewMatrix * push.modelMatrix * vec4(position, 1.0);
  //gl_Position = vec4(inPosition, 0.0, 1.0);//vec4(positions[gl_VertexIndex], 0.0, 1.0);
  vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);

  float lightIntensity = AMBIENT + max(dot(normalWorldSpace, ubo.directionToLight), 0);

  fragColor = lightIntensity * color;
}