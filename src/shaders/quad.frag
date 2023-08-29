uniform sampler2D D;
uniform vec2 mousePos;

in vec2 vCoord;
out vec4 FragColor0;

void main()
{
	float intensity = clamp(1.0f / (0.8f + 20.0f*length(vCoord.xy - mousePos)), 0.0f, 1.0f);
	vec4 noise = texture(D, vCoord.xy);
	FragColor0 = noise + 0.3*vec4(0.0f, 0.6f, 1.0f, 1.0f) * intensity;
}