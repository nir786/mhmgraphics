#version 460 core

layout(points) in; 
layout(line_strip , max_vertices = 195 ) out ; 


//layout (triangles) in;
//layout (triangle_strip , max_vertices=18) out ; 


struct Light {
  vec4 position;
  vec4 direction;
  vec4 color;
};
uniform int size;
uniform mat4 projection; 
uniform mat4 view; 
float pie = 3.14; 
float height = 25.0f ; 
//float x  = 1920.0/10.0 ; 
//float y  = 1080.0/10.0 ; 
float x = 30.0;
float y = 20.0;
layout(std430, binding = 0) buffer LightBuffer { Light lights[]; };

void debugDraw_Point(int i ){
	for(int c =0 ; c < 65 ; c++ ){
		float theta = 2.0 * pie  * float(c);
		theta = theta/64.0 ; 

		vec3 point  =  lights[i].position.xyz + 25.0 *vec3(0,cos(theta)  , sin(theta)); 
		gl_Position = projection * view  *vec4(point , 1.0); 

		EmitVertex();
		}
	EndPrimitive();
	for(int c =0 ; c < 65 ; c++ ){
		float theta = 2.0 * pie  * float(c);
		theta = theta/64.0 ; 

		vec3 point  =  lights[i].position.xyz + 25.0 *vec3(cos(theta) , 0  , sin(theta)); 
		gl_Position = projection * view  *vec4(point , 1.0); 

		EmitVertex();
		}
	EndPrimitive();


}
void debugDraw_Project(int i ){


	vec3 origen = lights[i].position.xyz;  
	vec3 endPoint = origen + normalize(lights[i].direction.xyz)*25.0f ; 
	
	gl_Position = projection * view * vec4(origen , 1.0);  
	EmitVertex();
	gl_Position = projection * view * vec4(endPoint , 1.0);  
	EmitVertex();
	EndPrimitive();
	vec4 norm_dir = vec4(normalize(lights[i].direction.xyz),0.0); 

	
	vec3  defualt_dir = vec3(0.0 , 0.0 , 1.0); 

	float theta = acos(clamp(dot(defualt_dir,lights[i].direction.xyz) , -1.0 , 1.0)); 
	vec3 k = normalize( cross(defualt_dir,lights[i].direction.xyz)); 


	vec3 dir = lights[i].direction.xyz  * 25.0f ; 
	vec3 point_mx = vec3(lights[i].position.x+ x/2  , lights[i].position.y+ y/2  , lights[i].position.z+25.0)  ; 
	vec3 point_px = vec3(lights[i].position.x- x/2  , lights[i].position.y+ y/2  , lights[i].position.z + 25.0) ; 
	
	vec3 point_my = vec3(lights[i].position.x+ x/2 , lights[i].position.y - y/2 , lights[i].position.z + 25.0) ; 
	vec3 point_py = vec3(lights[i].position.x- x/2 , lights[i].position.y - y/2 , lights[i].position.z + 25.0) ; 

	point_mx -= origen;
	point_px -= origen;
	point_my -= origen;
	point_py -= origen;

//rodrigezz 
	point_mx = point_mx*cos(theta) + cross(k , point_mx)*sin(theta) + k*(k*point_mx)*(1-cos(theta)); 
	point_px = point_px*cos(theta) + cross(k , point_px)*sin(theta) + k*(k*point_px)*(1-cos(theta)); 
	point_my = point_my*cos(theta) + cross(k , point_my)*sin(theta) + k*(k*point_my)*(1-cos(theta)); 
	point_py = point_py*cos(theta) + cross(k , point_py)*sin(theta) + k*(k*point_py)*(1-cos(theta)); 

	point_mx += origen;
	point_px += origen;
	point_my += origen;
	point_py += origen;






	gl_Position = projection * view * vec4(point_px , 1.0);  
	EmitVertex();
	gl_Position = projection * view* vec4(point_mx , 1.0);  
	EmitVertex();
	EndPrimitive();

	gl_Position = projection * view *vec4(point_my , 1.0);  
	EmitVertex();
	gl_Position = projection * view *vec4(point_py , 1.0);  
	EmitVertex();
	EndPrimitive();

	gl_Position = projection * view * vec4(point_mx , 1.0);  
	EmitVertex();
	gl_Position = projection * view * vec4(point_my , 1.0);  
	EmitVertex();
	EndPrimitive();

	gl_Position = projection * view * vec4(point_px , 1.0);  
	EmitVertex();
	gl_Position = projection * view * vec4(point_py , 1.0);  
	EmitVertex();
	EndPrimitive();


	
	gl_Position = projection * view * vec4(lights[i].position.xyz , 1.0);  
	EmitVertex();
	gl_Position = projection * view * vec4(point_mx , 1.0);  
	EmitVertex();
	EndPrimitive();


	gl_Position = projection * view * vec4(lights[i].position.xyz , 1.0);  
	EmitVertex();
	gl_Position = projection * view * vec4(point_px , 1.0);  
	EmitVertex();
	EndPrimitive();



	gl_Position = projection * view * vec4(lights[i].position.xyz , 1.0);  
	EmitVertex();
	gl_Position = projection * view * vec4(point_my , 1.0);  
	EmitVertex();
	EndPrimitive();


	gl_Position = projection * view * vec4(lights[i].position.xyz , 1.0);  
	EmitVertex();
	gl_Position = projection * view * vec4(point_py , 1.0);  
	EmitVertex();

	EndPrimitive();











}







void main(){
	for(int i = 0 ; i < size ;i ++ ){
		if(lights[i].direction.w ==2.0){
			debugDraw_Project(i); 
		}
		else{
			debugDraw_Point(i);
		} 

	}


}

