uniform sampler2D tex;

in vec4 outColor;

out vec4 vFragColor;

void main() 
{
	//vFragColor = texture(tex, gl_PointCoord) * outColor;
	//vFragColor = texture(tex, gl_PointCoord) * vec4(1.0);
	vFragColor = outColor;
}