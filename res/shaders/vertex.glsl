#version 450 core

layout(location = 0) in vec4 position; // input attribute for vertex position
// in vec4 position;

//original Model View Projection matrix, used for rendering rectangle on the whole view area
uniform mat4 matrix_original;       

//inverted Model View Projection matrix used desired camera for rendering scene - for ray computation
uniform mat4 inverse_matrix;

//coordinates of the ray's start and end in homogeneous world coordinates
//for using further in fragment shader for computing origin and the direction of the ray
out vec4 near_4;   
out vec4 far_4;

//out vec4 uni_coord;
//uni_coord is a point inside cube [-1,1]x[-1,1]x[-1,1], z=-1,1 - near/far clip
//- its in a normalized device coordinates coordinates,
//see for details https://community.khronos.org/t/ray-origin-through-view-and-projection-matrices/72579/4

void main(){
    //gl_Position = modelViewProjectionMatrix * position;

    //compute final rectangle's vertex position
    // gl_Position = matrix_original * position;       
    gl_Position = vec4(position.xy, 0.0, 1.0);

    //get 2D projection of this vertex in normalized device coordinates
    vec2 pos = gl_Position.xy/gl_Position.w;
   
    //compute ray's start and end as inversion of this coordinates
    //in near and far clip planes
    near_4 = inverse_matrix * (vec4(pos, -1.0, 1.0));       
    far_4 = inverse_matrix * (vec4(pos, +1.0, 1.0));

}
