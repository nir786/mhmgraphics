#version 460 core
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal; 

uniform mat4 view ; 
uniform mat4 model ; 
uniform mat4 projection ; 


out vec3 normal; 
out vec3 FragPos; 
out vec3 viewPos; 




void main(){
	gl_Position =  projection * view * model * vec4(aPos, 1.0);


	FragPos = vec3(model * vec4(aPos*0.01 , 1.0)); 
	mat3 normalMatrix=transpose(inverse(mat3(model))); 
	normal = normalize(normalMatrix * aNormal); 
        viewPos = vec3(inverse(view)[3]);
	
}



