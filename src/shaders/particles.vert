uniform mat4x4 modelViewMat;
uniform mat4x4 projectionMat;

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec4 vColor;

out vec4 outColor;

void main() 
{
    vec4 eyePos = modelViewMat * gl_Vertex;

    gl_Position = projectionMat * eyePos;
	outColor = vColor;
	gl_PointSize = 5.0f / gl_Position.w;
}