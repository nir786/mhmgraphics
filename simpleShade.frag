#version 460 core
out vec4 FragColor;  


in vec4 vertexColor;
in vec2 TexCoord;
in vec3 Normal; 

uniform sampler2D Texture1; 
uniform sampler2D Texture2;
uniform float blend; 
uniform int isLight; 
uniform vec3 LightSource; 
uniform vec3 ViewPos; 
uniform vec3 ObjectPos; 

float near = 0.1; 
float far = 10.0; 

float LinearizeDepth(float depth){
	float z = depth * 2.0 - 1.0; 
	return (2.0 * near * far )/(far + near - z * (far-near)); 
}


void main(){

//	vertexColor = vec4(aNormal*0.5 + 0.5,1.0); 

	float ambientStrength = 0.7; 

  
	float diffusion = max(dot(Normal,LightSource) , 0.0); 

	vec3 viewDir = normalize(ViewPos -ObjectPos );
	vec3 reflectDir = reflect(-LightSource , Normal);
	float spec = pow(max(dot(viewDir , reflectDir ), 0.0), 32); 
	vec3 specular = 0.5 * spec * vec3(1.0,0.85,0.65); 


	vec3 lightColor= diffusion +ambientStrength + specular;


	vec3 baseColor = vec3(0.5 , 0.5 , 0.5 ) * lightColor;







	if(isLight == 0){

		
	FragColor = vec4( baseColor, 1.0);
//	float depth = LinearizeDepth(gl_FragCoord.z)/ far; 
//	FragColor = vec4(vec3(depth),1.0);




	}else{

		FragColor = vec4(1.0f , 1.0f , 1.0f , 1.0f); 





	}

}
