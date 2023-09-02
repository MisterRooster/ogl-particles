uniform sampler2D tex;

in vec4 outColor;
out vec4 vFragColor;

void main() 
{
	vec4 mask =  texture(tex, gl_PointCoord);
	vFragColor = vec4(outColor.rgb * mask.r, outColor.a);
}