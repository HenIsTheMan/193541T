#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aNormal; //Vertex normals supplied by the model (how??)
layout (location = 4) in vec2 aOffset;
layout (location = 5) in mat4 instanceMatrix;
layout (location = 6) in vec3 aTangent;
layout (location = 7) in vec3 aBitangent; //???????????????????????????????

out myInterface{ //Output interface block
    vec4 Colour;
    vec2 TexCoords;
    vec3 Normal;
    vec3 FragPos;
} vsOut; //Instance

out mat3 TBN;

uniform mat4 MVP;
uniform mat4 model, view, projection;
uniform bool useOffset; //From Instancing.vs
uniform bool useMat; //From Instancing2.vs
uniform bool screenQuad; //From ScreenQuad.vs
uniform bool cubemap; //From Cubemap.vs
uniform bool explosion;
uniform bool drawNormals; //From Normals.vs

void main(){
    vsOut.Colour = aColor;
    vsOut.TexCoords = aTexCoords;
    vsOut.Normal = mat3(transpose(inverse(model))) * aNormal; //Multiplication with normal matrix
    vsOut.FragPos = vec3(model * vec4(aPos, 1.f));


    vec3 T = normalize(mat3(transpose(inverse(model))) * aTangent);
    //vec3 B = normalize(mat3(transpose(inverse(model))) * aBitangent); //More precise to multiply with normal matrix as only orientation of vecs matters
    vec3 N = normalize(mat3(transpose(inverse(model))) * aNormal);

    ///Gram–Schmidt process (Tangent vecs of larger meshes that share many vertices are generally avged to give smooth results which causes TBN vectors to become non-orthogonal)
    T = normalize(T - dot(T, N) * N); //Re-orthogonalise T with respect to N
    vec3 B = cross(N, T);

    TBN = mat3(T, B, N);
    //TBN = transpose(mat3(T, B, N)); //Transpose of orthogonal matrix (each axis is a perpendicular unit vec) == its inverse)


    if(useOffset){
        vec3 downscaledPos = aPos * (gl_InstanceID / 100.f); //gl_InstanceID is incremented for each instanced draw/... call
        gl_Position = MVP * vec4(downscaledPos + vec3(aOffset, 0.f), 1.f);
        return;
    }
    if(useMat){
        vsOut.Normal = mat3(transpose(inverse(instanceMatrix))) * aNormal;
        vsOut.FragPos = vec3(instanceMatrix * vec4(aPos, 1.f));
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
        return;
    }
    if(explosion){
        gl_Position = vec4(aPos, 1.f);
        return;
    }
    if(drawNormals){
        mat3 normalMtx = mat3(transpose(inverse(view * model)));
        vsOut.Normal = normalize(vec3(projection * vec4(normalMtx * aNormal, 0.f))); //Transformed clip-space normal vec //Multiply by normalMatrix to acct for scaling and rotations due to view and model matrix??
    }
    gl_Position = MVP * vec4(aPos, 1.f);
}