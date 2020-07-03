#version 330 core
out vec4 FragColor;

uniform vec3 myRGB;
//uniform float myAlpha;

void main(){
	FragColor = vec4(gl_FragCoord.y > 300 ? myRGB : vec3(1.f) - myRGB, 1.f);
}

//++ post-processing filters like Gaussian Blur??
/*#version 420 core
out vec4 FragColor;

layout (depth_greater) out float gl_FragDepth; //Redeclare gl_FragDepth with depth condition to preserve some early depth testing //++ more depth conditions??

void main(){
    FragColor = vec4(1.f);
    gl_FragDepth = gl_FragCoord.z + .1f;
}*/