#version 330
attribute vec3 position;
attribute vec2 texCoord;
uniform mat4 pvm;

varying vec2 uv;

void main() {
	gl_Position = pvm * vec4(position, 1.0);
	uv = texCoord;
}