#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;
layout(location = 3) in vec3 texCoord;

// varying variables
out vec3 vColor;
out vec3 vTexCoord;
out vec3 vNormal;
out vec3 vPosition;

// uniform variables
uniform int mode;	// 1 -- color / 2 -- texture
uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;

uniform int shadowState;	// 1 -- normal / 2 -- shadow
uniform mat4 light_mvpMatrix;
uniform vec3 lightDir;

void main(){
	vColor = color;
	vTexCoord = texCoord;
	vPosition = position;

	// SHADOW: From light
	if (shadowState == 2) {
		gl_Position = light_mvpMatrix * vec4(position, 1.0);
		return;
	}
	
	//varNormal = normalize(normal).xyz;
	vNormal = normalize(normal).xyz;

	gl_Position = mvpMatrix * vec4(position, 1.0);
}