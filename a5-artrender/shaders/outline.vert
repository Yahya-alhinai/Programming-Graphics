#version 330

// CSci-4611 Assignment 5:  Art Render


uniform mat4 model_view_matrix;
uniform mat4 normal_matrix;
uniform mat4 proj_matrix;
uniform float thickness;

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 left_normal;
layout(location = 3) in vec3 right_normal;

void main() {
    vec3 v = (model_view_matrix * vec4(vertex, 1)).xyz;
    vec3 e = normalize(-v);
    vec3 n_right = normalize((normal_matrix * vec4(left_normal, 0)).xyz);
    vec3 n_left = normalize((normal_matrix * vec4(right_normal, 0)).xyz);

    if ( (dot(e, n_right) < 0 && dot(e, n_left) > 0) || (dot(e, n_right) > 0 && dot(e, n_left) < 0) ) {
		gl_Position = proj_matrix * model_view_matrix * vec4(vertex + thickness * normal,1);
    }
	else {
		gl_Position = proj_matrix * model_view_matrix * vec4(vertex,1);
	}
}