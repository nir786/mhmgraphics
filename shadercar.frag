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

uniform Material material; 
uniform vec3 lightLoc;

struct Light{
	vec4 position;
	vec4 color;
};
layout(std430 , binding = 0) buffer LightBuffer{
	Light lights[];
};

uniform int lightCount;
uniform sampler2D TEXTURE0 ; 
uniform int ignore; 



float near = 0.1; 
float far = 10.0; 

float LinearizeDepth(float depth){
	float z = depth * 2.0 - 1.0; 
	return (2.0 * near * far )/(far + near - z * (far-near)); 
}



void main(){
	vec3 lightLoc = vec3(0.0 , 0.0, 1.0);
if(ignore== 1) { 


	vec3 lightColor =vec3(1.0 , 1.0 , 1.0);
        vec3 ambient = lightColor * material.ambient;
	vec3 norm = normalize(normal);
	vec3 lightDir =normalize(lightLoc - FragPos);  
	
	float diffusion = max(dot(norm,lightDir) , 0.0); 
	vec3 diffuse = lightColor *(diffusion * material.diffuse); 
	


	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir , norm ); 

	float spec = pow(max(dot(viewDir , reflectDir) ,0.0), material.shininess);
	vec3 specular = lightColor *(spec * material.specular);
	vec3 result = ambient + diffuse + specular; 
	FragColor = vec4(result, 1.0 ); 
}else{
vec3 sum_light  = vec3(0.0); 
vec3 baseColor = texture(TEXTURE0, TexCoords).rgb; 

for(int i = 0 ; i < lightCount ; ++i){
	vec3 lightColor = lights[i].color.xyz * lights[i].color.w; 
	vec3 ambient = lightColor * material.ambient * baseColor;
	vec3 norm = normalize(normal);



	vec3 lightDir =normalize(lights[i].position.xyz - FragPos);  
	
	float diffusion = max(dot(norm,lightDir) , 0.0); 
	vec3 diffuse = lightColor *(diffusion * material.diffuse * baseColor); 
	


	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir , norm ); 

	float spec = pow(max(dot(viewDir , reflectDir) ,0.0), material.shininess);
	vec3 specular = lightColor *(spec * material.specular);
	vec3 result = ambient + diffuse + specular; 
        sum_light += result ;	
	}

FragColor = vec4(sum_light, 1.0 ); 
//FragColor = vec4(1.0 ,1.0 , 1.0, 1.0 ); 
}
//FragColor = vec4(sum_light , 1.0);

}
