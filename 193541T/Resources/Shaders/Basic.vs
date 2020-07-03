#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aNormal; //Vertex normals supplied by the model (how??)
layout (location = 4) in vec2 aOffset;
layout (location = 5) in mat4 instanceMatrix;

out myInterface{ //Output interface block
    vec4 Colour;
    vec2 TexCoords;
    vec3 Normal;
    vec3 FragPos;
    vec3 TexDir; //Dir vec representing a 3D texCoord for indexing/sampling the cubemap (similar to the pos vec of an interpolated local vertex pos of a cube) //Can be a non-unit vec
} vsOut; //Instance

uniform mat4 MVP;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
/*layout (std140) uniform Matrices{ //std140 is the uni block layout (mem layout used by a uni block to store its content)
    mat4 projection;
    mat4 view;
};
uniform mat4 model;*/
//layout(std140, binding = 2) uniform example{}; //Another layout qualifier/specifier to set binding pt explicitly

uniform bool useOffset; //From Instancing.vs
uniform bool useMat; //From Instancing2.vs
uniform bool screenQuad; //From ScreenQuad.vs
uniform bool cubemap; //From Cubemap.vs
uniform bool drawNormals; //From Normals.vs

void main(){
    vsOut.Colour = aColor;
    vsOut.TexCoords = aTexCoords;
    vsOut.Normal = mat3(transpose(inverse(model))) * aNormal; //Multiplication with normal matrix
    vsOut.FragPos = vec3(model * vec4(aPos, 1.f)); //Vertex pos in world space

    if(useOffset){
        vec3 pos = aPos * (gl_InstanceID / 100.f); //Downscaled pos //gl_InstanceID is incremented for each instanced draw/... call
        gl_Position = MVP * vec4(pos + vec3(aOffset, 0.f), 1.f);
        return;
    }
    if(useMat){
        gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.f);
        return;
    }
    if(screenQuad){
        gl_Position = model * vec4(aPos.x, aPos.y, 0.f, 1.f); //What if z != 0.f??
        return;
    }
    if(cubemap){
        gl_Position = projection * view * vec4(aPos, 1.f);
        gl_Position = gl_Position.xyww; //Resulting NDC after perspective division will have a z value (gl_FragCoord.z) equal to 1.f (max depth value) so skybox appears behind all other objs
        vsOut.TexDir = aPos;
        return;
    }
    if(drawNormals){
        mat3 normalMtx = mat3(transpose(inverse(view * model)));
        vsOut.Normal = normalize(vec3(projection * vec4(normalMtx * aNormal, 0.f))); //Transformed clip-space normal vec //Multiply by normalMatrix to acct for scaling and rotations due to view and model matrix??
    }
    gl_Position = MVP * vec4(aPos, 1.f); //Clip-space output pos vec
}