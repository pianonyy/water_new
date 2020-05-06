#version 330

in vec3 frag_position;
in vec3 frag_normal;
in vec3 frag_diffuse_color;
in vec2 frag_texture_coord;

out vec4 frag_color;

uniform sampler2D flag_texture;

void main () {
   vec3 light_pos_1 = vec3(1.0, 2.0, 3.0);
   vec3 light_pos_2 = vec3(-1.0, -2.0, -3.0);

   float kd = 0.5 * max(dot(normalize(light_pos_1), abs(frag_normal)), 0);
   kd += 0.5 * max(dot(normalize(light_pos_2), abs(frag_normal)), 0);

   kd = max(0.2, kd);

   vec3 texel = texture(flag_texture, frag_texture_coord).xyz;

   frag_color =0.1 * vec4(0.923, 0.923, 0.923, 1.0) + kd * vec4(texel, 30.5);
}

