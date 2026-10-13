#version 460 core
layout (location = 0) in vec3 aPos; 
layout (location = 1) in vec3 aNormal; 
layout (location = 2) in vec2 aUV; 


///////////////////////////////////////////////////////////////////////////////////
out VS_OUT{
	out vec3 normal; 
	out vec3 FragPos; 
	out vec3 viewPos; 
	out vec2 TexCoords; 
	out vec4 FragPosLightSpace;
}vs_out;
///////////////////////////////////////////////////////////////////////////////////
uniform mat4 view ; 
uniform mat4 model ; 
uniform mat4 projection ; 
uniform mat4 lightSpaceMatrix;
///////////////////////////////////////////////////////////////////////////////////



void main(){


	vs_out.FragPos = vec3(model * vec4(aPos , 1.0)); 
	mat3 normalMatrix=transpose(inverse(mat3(model))); 
	vs_out.normal = normalize(normalMatrix * aNormal); 
        vs_out.viewPos = vec3(inverse(view)[3]);
	vs_out.TexCoords = aUV;
	vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

	
	gl_Position =  projection * view * model * vec4(aPos, 1.0);
}



