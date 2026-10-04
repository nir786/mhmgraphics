#version 460 core
out vec4 FragColor; 

in vec3 normal;
in vec3 FragPos; 
in vec3 viewPos; 


struct Material{
	vec3 ambient; 
	vec3 diffuse;
	vec3 specular; 
	float shininess;
};

uniform Material material; 
uniform vec3 lightLoc;



float near = 0.1; 
float far = 10.0; 

float LinearizeDepth(float depth){
	float z = depth * 2.0 - 1.0; 
	return (2.0 * near * far )/(far + near - z * (far-near)); 
}



void main(){
	vec3 lightColor =vec3(1.0 , 1.0 , 1.0);
        vec3 ambient = lightColor * material.ambient;
	vec3 norm = normalize(normal);
	//vec3 lightDir =normalize(vec3(1.0 , 0.0 , 0.0) - FragPos);  
	vec3 lightDir =normalize(lightLoc - FragPos);  
	
	float diffusion = max(dot(norm,lightDir) , 0.0); 
	vec3 diffuse = lightColor *(diffusion * material.diffuse); 
	


	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir , norm ); 
	//float spec = pow(max(dot(viewDir , reflectDir) ,0.0), material.shininess);

	float spec = pow(max(dot(viewDir , reflectDir) ,0.0), 32.0);
	vec3 specular = lightColor *(spec * material.specular);
	vec3 result = ambient + diffuse + specular; 
	FragColor = vec4(result, 1.0 ); 

//	float depth = LinearizeDepth(gl_FragCoord.z)/ far; 
//FragColor = vec4(vec3(depth), 1.0);
















//	vec3 lightSource= vec3(1.0 , 1.0 , 0.0); 
//	float diffusion = max(dot(normal,lightSource) , 0.0); 
	
//	vec3 lightColor =vec3(1.0 , 1.0 , 1.0);
//	FragColor = vec4(lightColor, 1.0); 

}
