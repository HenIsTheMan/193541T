#version 330 core
layout (points) in; //Input primitive type as layout qualifier/...
layout (line_strip, max_vertices = 2) out; //Output primitive type and... as layout qualifiers/... //Line strip binds tgt a set of >= 2 pts to form 1 continuous line

in vec3 myColour[];
out vec3 ourColour;

void main(){
    ourColour = myColour[0];

    gl_Position = gl_in[0].gl_Position + vec4(-.5f, 0.f, 0.f, 0.f); 
    EmitVertex(); //Adds vec currently set to gl_Position to the output primitive??

    gl_Position = gl_in[0].gl_Position + vec4(.5f, 0.f, 0.f, 0.f);
    EmitVertex();
    
    EndPrimitive(); //All emitted vertices for the output primitive are combined into the specified output render primitive //Call EndPrimitive() repeatedly after >= 1 EmitVertex() calls to gen multiple primitives
}