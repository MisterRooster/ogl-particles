out vec2 vCoord;

void main()
{
    vec4 quadInfo = vec4(1.0);
    
	switch(gl_VertexID)
	{
	case 0:
	    vCoord = vec2(0.0, 0.0);
		gl_Position = vec4(-quadInfo.zw, 0.0, 1.0);
		break;
	case 1:
	    vCoord = vec2(0.0, 1.0);
		gl_Position = vec4(-quadInfo.z, quadInfo.w, 0.0, 1.0);
		break;
	case 2:
	    vCoord = vec2(1.0, 1.0);
		gl_Position = vec4(quadInfo.zw, 0.0, 1.0);
		break;
	case 3:
	    vCoord = vec2(1.0, 0.0);
		gl_Position = vec4(quadInfo.z, -quadInfo.w, 0.0, 1.0);
		break;
	}
}