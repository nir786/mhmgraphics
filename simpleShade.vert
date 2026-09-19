#version 460 core
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal; 





out vec4 vertexColor; 
out vec2 TexCoord;
out vec3 Normal; 

uniform mat4 transform;





uniform mat4 view ; 
uniform mat4 model ; 
uniform mat4 projection ; 





uniform float aAlpha ;   

void main(){
	gl_Position =  projection * view * model * vec4(aPos, 1.0);
	vertexColor = vec4(0.5 , 0.5, aAlpha, 1.0);
	TexCoord = aTexCoord; 




	Normal = normalize(transpose(inverse(mat3(model)))*aNormal);
}
