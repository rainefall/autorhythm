// shader that blends a base texture with a colour from a gradient
// the position at which the gradient is sampled from depends on an input
// value "position"

shader_type spatial;
render_mode unshaded, cull_disabled; // disable lighting and backface culling

 // position along the x of the LUT to sample from
uniform float position;

 // base texture
uniform sampler2D tex : hint_white;
// look up texture, in this case a colour gradient
uniform sampler2D lut : hint_white;

// values for scaling and translating UV coordinates
// no longer need these because level geometry along with UV coordinates is
// all generated properly, but for legacy reasons i can't remove this
uniform vec2 uv_multiplier = vec2(1,1);
uniform vec2 uv_offset;

// vertex program
void vertex() {
	// scale and translate uv coordinates
	UV = UV * uv_multiplier + uv_offset;
}

// fragment program (this is known as a pixel shader in HLSL)
void fragment() {
	// sample from the base texture
	vec4 base = texture(tex, UV);
	// set output alpha to alpha of base texture
	ALPHA = base.a;
	// set the fragment output colour to the base texture multiplied by the
	// value at the given position on the LUT
	ALBEDO = base.rgb * texture(lut, vec2(position, 0)).rgb;
}