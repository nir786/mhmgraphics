#version 460
out vec4 FragColor; 
in vec2 TexCoords; 
uniform sampler2D hdrBuffer; 
uniform sampler2D depthBuffer; 
uniform int mode;

uniform float exposure; 
void main(){
if(mode == 0){
	const float gamma = 2.2;
	vec3 hdrColor = texture(hdrBuffer , TexCoords).rgb ; 
vec3 mapped = vec3(1.0) -exp(-hdrColor * exposure);
mapped = pow(mapped , vec3(1.0/ gamma)); 

	FragColor = vec4(mapped  , 1.0); 
}else if(mode == 1){

	float depth = texture(depthBuffer , TexCoords).r; 
	vec3 col = vec3(depth); 
	FragColor= vec4(col , 1.0); 
}









}
