shader_type spatial;
render_mode unshaded;

uniform float position;

uniform sampler2D tex : hint_white;
uniform sampler2D lut : hint_white;

uniform vec2 uv_multiplier = vec2(1,1);
uniform vec2 uv_offset;

void vertex() {
	UV = UV * uv_multiplier + uv_offset;
}

void fragment() {
	vec3 lum = texture(tex, UV).rgb;
	ALPHA = lum.r;
	ALBEDO = lum * texture(lut, vec2(position, 0)).rgb;
}