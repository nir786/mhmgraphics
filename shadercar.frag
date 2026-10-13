#version 460 core
out vec4 FragColor; 
in VS_OUT{
	in vec3 normal; 
	in vec3 FragPos; 
	in vec3 viewPos; 
	in vec2 TexCoords; 
	in vec4 FragPosLightSpace;
}fs_in;

struct Material{
	vec3 ambient; 
	vec3 diffuse;
	vec3 specular; 
	float shininess;
};


struct Light{
	vec4 position;
	vec4 direction;
	vec4 color;
};
layout(std430 , binding = 0) buffer LightBuffer{
	Light lights[];
};

uniform Material material; 
uniform vec3 lightLoc;
uniform int lightCount;
uniform sampler2D TEXTURE0 ; 
uniform sampler2D shadowMap;
uniform int ignore; 



float bias = 0.005;
float near = 0.1; 
float far = 10.0; 
vec3 ambient =vec3(0.0); 

float LinearizeDepth(float depth){
	float z = depth * 2.0 - 1.0; 
	return (2.0 * near * far )/(far + near - z * (far-near)); 
}
float attenuation(vec3 lightPos ,vec3 fragPos){

	float d = distance(lightPos , fragPos); 
	return  1.0 / (1.0  + 0.08  * d  + 0.032  * d * d); 
}
float ShadowCalculation(vec4 fragPosLightSpace){
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 +0.5;
	float closestDepth = texture(shadowMap , projCoords.xy).r;
	float currentDepth = projCoords.z;
	return currentDepth - bias > closestDepth ? 0.0 : 1.0;

}

vec3 phongCalculation(int i , vec3 baseColor ){
	vec3 lightColor = lights[i].color.xyz * lights[i].color.w; 
	vec3 norm = normalize(fs_in.normal);
	//vec3 lightDir =normalize(lights[i].position.xyz - fs_in.FragPos);  
	vec3 lightDir =normalize(fs_in.FragPos-lights[i].position.xyz );  
	float diffusion = max(dot(norm,-lightDir) , 0.0); 
	vec3  diffuse = lightColor *(diffusion * material.diffuse * baseColor); 
//////////////////////////diffusion//////////////////////////////////

	vec3  viewDir = normalize(fs_in.viewPos - fs_in.FragPos);
	vec3  reflectDir = reflect(lightDir , norm ); 

	float spec = pow(max(dot(viewDir , reflectDir) ,0.0), material.shininess);
	vec3 specular = lightColor *(spec * material.specular) ;
/////////////////////////specular////////////////////////////////////
return specular + diffuse ; 


}


void main(){
	vec3 lightLoc = vec3(0.0 , 0.0, 1.0);

if(ignore== 1) { 


	vec3 lightColor =vec3(1.0 , 1.0 , 1.0);
        vec3 ambient = lightColor * material.ambient;
	vec3 norm = normalize(fs_in.normal);
	vec3 lightDir =normalize(lightLoc - fs_in.FragPos);  
	
	float diffusion = max(dot(norm,lightDir) , 0.0); 
	vec3 diffuse = lightColor *(diffusion * material.diffuse); 
	


	vec3 viewDir = normalize(fs_in.viewPos - fs_in.FragPos);
	vec3 reflectDir = reflect(-lightDir , norm ); 

	float spec = pow(max(dot(viewDir , reflectDir) ,0.0), material.shininess);
	vec3 specular = lightColor *(spec * material.specular);
	vec3 result = ambient + diffuse + specular; 
	FragColor = vec4(result, 1.0 ); 
}else{
vec3 baseColor = texture(TEXTURE0, fs_in.TexCoords).rgb; 
vec3 sum_light  = baseColor * material.ambient ; 
for(int i = 0 ; i < lightCount ; ++i){

float atten = attenuation(lights[i].position.xyz , fs_in.FragPos);



	if(lights[i].direction.w == 0.0){
//	vec3 lightColor = lights[i].color.xyz * lights[i].color.w; 
//	vec3 norm = normalize(fs_in.normal);
	//vec3 lightDir =normalize(lights[i].position.xyz - fs_in.FragPos);  
//	vec3 lightDir =normalize(fs_in.FragPos-lights[i].position.xyz );  




	//float diffusion = max(dot(norm,lightDir) , 0.0); 
/*
	float diffusion = max(dot(norm,-lightDir) , 0.0); 
	vec3 diffuse = lightColor *(diffusion * material.diffuse * baseColor); 
	


//	vec3 viewDir = normalize(fs_in.viewPos - fs_in.FragPos);
//	vec3 reflectDir = reflect(-lightDir , norm ); 

//	float spec = pow(max(dot(viewDir , reflectDir) ,0.0), material.shininess);
//	vec3 specular = lightColor *(spec * material.specular);
	//vec3 result =  (diffuse + specular) * atten; 




	vec3  viewDir = normalize(fs_in.viewPos - fs_in.FragPos);
	vec3  reflectDir = reflect(lightDir , norm ); 

	float spec = pow(max(dot(viewDir , reflectDir) ,0.0), material.shininess);
	vec3 specular = lightColor *(spec * material.specular) ;

*/
	//vec3 result =  (diffuse + specular) * atten; /*atten*/

	vec3 result =  phongCalculation(i , baseColor) * atten; /*atten*/
        sum_light += result ;	







	}else if (lights[i].direction.w == 2.0 ){

	vec3 lightColor = lights[i].color.xyz * lights[i].color.w; 
	vec3 norm = normalize(fs_in.normal);
	vec3 lightDir =normalize(fs_in.FragPos - lights[i].position.xyz);  
	








	float theta = dot(lightDir , normalize(lights[i].direction.xyz));

	if(theta > lights[i].position.w){
	float innercutoff = 0.0 ; 
	float falloff = smoothstep(lights[i].position.w,1.0 , theta); 


/*



	float diffusion = max(dot(norm,-lightDir) , 0.0); 
	vec3  diffuse = lightColor *(diffusion * material.diffuse * baseColor); 





	vec3  viewDir = normalize(fs_in.viewPos - fs_in.FragPos);
	vec3  reflectDir = reflect(lightDir , norm ); 

	float spec = pow(max(dot(viewDir , reflectDir) ,0.0), material.shininess);
	vec3 specular = lightColor *(spec * material.specular) ;
*/




	bias = max(0.05 * (1.0 - dot(fs_in.normal , lightDir)) , 0.005);
float shadow = ShadowCalculation(fs_in.FragPosLightSpace); 
	vec3 result = ((phongCalculation(i,baseColor))*falloff); 
        sum_light += result * shadow * atten ; /** attun*/
		}else {

		}
	}

	}
	FragColor = vec4(sum_light, 1.0 ); 
}
}

