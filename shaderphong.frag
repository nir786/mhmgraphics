#version 460 core
out vec4 FragColor; 

in vec3 normal;
in vec3 FragPos; 
in vec3 viewPos; 
in vec2 TexCoords;


struct Material{
	vec3 ambient; 
	vec3 diffuse;
	vec3 specular; 
	float shininess;
};
struct Light{
	vec4 position;
	vec4 color;
};
layout(std430 , binding = 0) buffer LightBuffer{
	Light lights[];
};

uniform int lightCount;


uniform Material material; 
//uniform vec3 lightLoc;
uniform sampler2D tex ; 



float near = 0.1; 
float far = 10.0; 

float LinearizeDepth(float depth){
	float z = depth * 2.0 - 1.0; 
	return (2.0 * near * far )/(far + near - z * (far-near)); 
}



void main(){


vec3 sum_light  = vec3(0.0); 
for(int i = 0 ; i < lightCount ; ++i){
	vec3 lightColor = lights[i].color.xyz * lights[i].color.w; 
	vec3 ambient = lightColor * material.ambient;
	vec3 norm = normalize(normal);



	vec3 lightDir =normalize(lights[i].position.xyz - FragPos);  
	
	float diffusion = max(dot(norm,lightDir) , 0.0); 
	vec3 diffuse = lightColor *(diffusion * material.diffuse); 
	


	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir , norm ); 

	float spec = pow(max(dot(viewDir , reflectDir) ,0.0), 32.0);
	vec3 specular = lightColor *(spec * material.specular);
	vec3 result = ambient + diffuse + specular; 
        sum_light += result ;	
}
FragColor = vec4(sum_light * texture(tex, TexCoords).rgb, 1.0 ); 
//FragColor = vec4(lights[0].color.xyz, 1.0); 




/*
	vec3 lightColor =vec3(1.0 , 1.0 , 1.0);
        vec3 ambient = lightColor * material.ambient;
	vec3 norm = normalize(normal);



	vec3 lightDir =normalize(lightLoc - FragPos);  
	
	float diffusion = max(dot(norm,lightDir) , 0.0); 
	vec3 diffuse = lightColor *(diffusion * material.diffuse); 
	


	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir , norm ); 

	float spec = pow(max(dot(viewDir , reflectDir) ,0.0), 32.0);
	vec3 specular = lightColor *(spec * material.specular);
	vec3 result = ambient + diffuse + specular; 
	FragColor = vec4(result * texture(tex, TexCoords).rgb, 1.0 ); 

*/



































//	vec3 lightSource= vec3(1.0 , 1.0 , 0.0); 
//	float diffusion = max(dot(normal,lightSource) , 0.0); 
	
//	vec3 lightColor =vec3(1.0 , 1.0 , 1.0);
//	FragColor = vec4(lightColor, 1.0); 

}
