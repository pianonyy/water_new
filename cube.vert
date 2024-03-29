#version 330

in vec3 vertex_position;
in vec3 vertex_normal;

out vec3 frag_position;
out vec3 frag_normal;
out vec3 frag_diffuse_color;
out vec2 frag_texture_coord;

uniform mat4 model_mat;
uniform mat4 view_mat;
uniform mat4 proj_mat;
uniform vec3 color;

void main () {
   frag_position = (model_mat * vec4(vertex_position, 1.0)).xyz;
   frag_normal = normalize((inverse(transpose(model_mat)) * vec4(vertex_normal, 1.0)).xyz);
   frag_diffuse_color = color;
   frag_texture_coord = (vertex_position.xy + vec2(1.0, 1.0)) / 2.0;

   gl_Position = proj_mat * view_mat * model_mat * vec4(vertex_position, 1.0);
}
