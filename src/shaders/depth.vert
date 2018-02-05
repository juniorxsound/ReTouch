#version 150 core

in vec3 position;
in vec2 uvs;

out vec3 vPos;
out vec2 vUv;

void main(){
    //Interpolate and send to the frag shader
    vUv = uvs;
    vPos = position;

    gl_Position = vec4(position, 1.0);
}
