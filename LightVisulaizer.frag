#version 460 core

layout(points) in ; 
layout(line_strip , max_vertices = 195 ) out ; 



uniform int size;
float pie = 3.14; 

void main(){
	/*
	for(int i = 0 ; i < size ;i ++ ){
		EmitVertex();
		for(int c = 1 ; c < 65 ; c++ ){
		
		float theta = 2.0 * pie  * (float)c;
		theta = theta/64.0 ; 

		vec3 point  =  lights[i].position.xyz + vec3(cos(theta) , 0 , sin(theta)); 
		gl_Position = vec4(point , 1.0); 
		}
		EndPrimitive();
	}
	*/


}

