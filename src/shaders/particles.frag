uniform sampler2D tex;

in vec4 outColor;

out vec4 vFragColor;

void main() 
{
	//vFragColor = texture(tex, gl_PointCoord) * outColor;
	float a =  texture(tex, gl_PointCoord);
	vFragColor = vec4(outColor.x, outColor.y, outColor.z, a);
	//vFragColor = outColor;
}