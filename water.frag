#version 330

in vec3 frag_position;
in vec3 frag_normal;
in vec3 frag_diffuse_color;
in vec2 frag_texture_coord;

out vec4 frag_color;



void main () {
   vec3 light_pos_1 = vec3(1.0, 2.0, 3.0);
   vec3 light_pos_2 = vec3(-1.0, -2.0, -3.0);

   float kd = 0.5 * max(dot(normalize(light_pos_1), abs(frag_normal)), 0);
   kd += 0.5 * max(dot(normalize(light_pos_2), abs(frag_normal)), 0);

   kd = max(0.2, kd);

  

   frag_color = 0.7 * vec4(0.2148	,0.3281,	0.457, 1);
}

