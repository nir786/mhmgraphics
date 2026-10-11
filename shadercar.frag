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
	vec4 direction;
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
vec3 ambient =vec3(0.0); 

float LinearizeDepth(float depth){
	float z = depth * 2.0 - 1.0; 
	return (2.0 * near * far )/(far + near - z * (far-near)); 
}
float attenuation(vec3 lightPos ,vec3 fragPos){

	float d = distance(lightPos , fragPos); 
	return  1.0 / (1.0  + 0.09  * d  + 0.032  * d * d); 
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
vec3 baseColor = texture(TEXTURE0, TexCoords).rgb; 
vec3 sum_light  = baseColor * vec3(0.50,0.50, 0.50); 
for(int i = 0 ; i < lightCount ; ++i){

	float atten = attenuation(lights[i].position.xyz , FragPos);



	if(lights[i].direction.w == 0.0){
	vec3 lightColor = lights[i].color.xyz * lights[i].color.w; 
	//vec3 ambient = lightColor * material.ambient * baseColor;
	vec3 norm = normalize(normal);



	vec3 lightDir =normalize(lights[i].position.xyz - FragPos);  
	
	float diffusion = max(dot(norm,lightDir) , 0.0); 
	vec3 diffuse = lightColor *(diffusion * material.diffuse * baseColor); 
	


	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir , norm ); 

	float spec = pow(max(dot(viewDir , reflectDir) ,0.0), material.shininess);
	vec3 specular = lightColor *(spec * material.specular);
	vec3 result =  (diffuse + specular) * atten; 
        sum_light += result ;	
	}else if (lights[i].direction.w == 2.0 ){

	vec3 lightColor = lights[i].color.xyz * lights[i].color.w; 
	//vec3 ambient = lightColor * material.ambient * baseColor;
	vec3 norm = normalize(normal);



	//vec3 lightDir =normalize(lights[i].direction.xyz - FragPos);  
	vec3 lightDir =normalize(FragPos - lights[i].position.xyz);  
	
	float theta = dot(lightDir , normalize(lights[i].direction.xyz));
	if(theta > lights[i].position.w){
	float innercutoff = 0.0 ; 
	float falloff = smoothstep(lights[i].position.w,1.0 , theta); 



	
	float diffusion = max(dot(norm,-lightDir) , 0.0); 
	vec3 diffuse = lightColor *(diffusion * material.diffuse * baseColor); 
	


	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(lightDir , norm ); 

	float spec = pow(max(dot(viewDir , reflectDir) ,0.0), material.shininess);
	vec3 specular = lightColor *(spec * material.specular) ;
	vec3 result = ((diffuse + specular)*falloff); 
        sum_light += result*atten ;
	}else {}
	}

}
FragColor = vec4(sum_light, 1.0 ); 
}
//FragColor = vec4(1.0 ,1.0 , 1.0, 1.0 ); 
}
//FragColor = vec4(sum_light , 1.0);

