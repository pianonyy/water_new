#version 330

in vec3 vertex_position;
in vec3 vertex_normal;

out vec3 frag_position;
out vec3 frag_normal;
out vec4 vert_color;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;
uniform vec3 color;

void main () {
   frag_position = (model_mat * vec4(vertex_position, 1.0)).xyz;
   frag_normal = (inverse(transpose(model_mat)) * vec4(vertex_normal, 1.0)).xyz;
   frag_normal = normalize(frag_normal);

   vec3 light_pos_1 = vec3(1.0, 2.0, 3.0);
   vec3 light_pos_2 = vec3(-1.0, -2.0, -3.0);

   float kd = 0.5 * max(dot(normalize(light_pos_1), abs(frag_normal)), 0);
   kd += 0.5 * max(dot(normalize(light_pos_2), abs(frag_normal)), 0);

   vert_color = vec4(kd * color + vec3(0.1, 0.1, 0.1), 0.8);

   gl_Position = proj_mat * view_mat * model_mat * vec4(vertex_position, 1.0);
}
