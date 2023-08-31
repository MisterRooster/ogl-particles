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
	
	float dist = length(eyePos.xyz);
	float att = inversesqrt(0.5f*dist);
	gl_PointSize = 2.0f * att;
}