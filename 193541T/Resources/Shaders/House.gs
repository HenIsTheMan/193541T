#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 5) out; //The output primitive is rasterized and the fragment shader runs on the whole of it

/*in VS_OUT{
    vec3 color;
} gs_in[]; //Interface block arr*/

in vec3 myColour[]; //Declared as arr as most render primitives formed in Primitive/Shape Assembly have > 1 vertex
out vec3 ourColour; //Fragment shader expects only 1 interpolated colour //Emitted vertices have the last stored value in ourColour

void build_house(vec4 pos){ //Generates 5 vertices per pt primitive input to form 1 triangle strip??
    //ourColour = gs_in[0].color; //gs_in[0] since there's only one input vertex??
    ourColour = myColour[0];
    gl_Position = pos + vec4(-.2f, -.2f, 0.f, 0.f);
    EmitVertex();   
    gl_Position = pos + vec4(.2f, -.2f, 0.f, 0.f);
    EmitVertex();
    gl_Position = pos + vec4(-.2f, .2f, 0.f, 0.f);
    EmitVertex();
    gl_Position = pos + vec4(.2f, .2f, 0.f, 0.f);
    EmitVertex();
    gl_Position = pos + vec4(0.f, .4f, 0.f, 0.f);
    ourColour = vec3(1.f);
    EmitVertex();
    EndPrimitive();
}

void main(){
    build_house(gl_in[0].gl_Position); //Each house consists of 3 triangles
} 