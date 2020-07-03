#include "Mesh.h"

short Mesh::nextTexUnit = 0;

Vertex::Vertex(glm::vec3 pos, glm::vec4 colour, glm::vec2 texCoords, glm::vec3 normal){
    this->pos = pos;
    this->colour = colour;
    this->texCoords = texCoords;
    this->normal = normal;
}

Texture::Texture(short newTexUnit, str newType): texUnit(newTexUnit), type(newType){}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint> indices){
    this->vertices = vertices;
    this->indices = indices;
    SetupMesh();
}

void Mesh::SetupMesh(){ //Create and config a VAO
    glGenVertexArrays(1, &VAO); //Gen VAO and get ref ID of it
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO); {
        glBindBuffer(GL_ARRAY_BUFFER, VBO); //Makes VBO the buffer currently bound to the GL_ARRAY_BUFFER target, GL_ARRAY_BUFFER is VBO's type
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW); //Copies vertex data stored in 'vertices' into VBO's mem

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //GL_ELEMENT_ARRAY_BUFFER is the buffer target
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW);

        //Specify vertex shader layout via vertex attrib pointers (attribs: pos, colour, normal, texture)
        int componentsAmt[]{3, 4, 2, 3}, arrSize = sizeof(componentsAmt) / sizeof(componentsAmt[0]);
        for(int i = 0; i < arrSize; ++i){
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, componentsAmt[i], GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)((!i ? 0 : std::accumulate(componentsAmt, componentsAmt + i, 0)) * sizeof(float)));
        }
    } glBindVertexArray(0); //Break the existing vertex arr obj binding
}

void Mesh::LoadTexture(cstr fPath, str type){ //bool gamma = 0??
    uint refID;
    int width, height, colourChannelsAmt;
    unsigned char* data = stbi_load(fPath, &width, &height, &colourChannelsAmt, 0);
    glGenTextures(1, &refID);
    glActiveTexture(GL_TEXTURE0 + nextTexUnit++);
    glBindTexture(GL_TEXTURE_2D, refID); //Make 'tex' the texture currently bound to the currently active texture unit so subsequent texture commands will configure it

    ///For Texture Wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //GL_TEXTURE_2D is the texture target
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); //more options??
    //float borderColor[] = {1.0f, 0.0f, 0.0f, 1.0f};
    //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    ///For Texture Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //Nearest neighbour/Point filtering/interpolation when textures are scaled downwards
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); //Linear filtering/interpolation for upscaled textures
    //++more??
    //++ Mipmaps for downscaled textures

    if(data){
        GLenum format = (colourChannelsAmt == 1 ? GL_RED : GL_RGB + colourChannelsAmt - 3);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D); //Gen required mipmap lvls for the currently bound texture
        textures.emplace_back(Texture(nextTexUnit - 1, type));
    } else{
        printf("Failed to load texture at: %s\n", fPath);
    }
    stbi_image_free(data); //Free the img mem
}

void Mesh::Draw(bool indexed, bool tex) const{
    if(tex){ //Make sure each shader sampler uniform corresponds to the correct texture unit
        int dMapCount = 0, sMapCount = 0, nMapCount = 0, rMapCount = 0; //Per mesh
        for(const auto& tex: textures){
            int& count = dMapCount;
            if(tex.type == "s"){
                count = sMapCount;
            } else if(tex.type == "n"){
                count = nMapCount;
            } else{
                count = rMapCount;
            }
            ShaderChief::SetUni1i(("material." + tex.type + "Map[" + std::to_string(count++) + ']').c_str(), tex.texUnit);
        }
        ShaderChief::SetUni1i("dMapCount", dMapCount);
        ShaderChief::SetUni1i("sMapCount", sMapCount);
        ShaderChief::SetUni1i("eMapCount", 0);
        ShaderChief::SetUni1i("nMapCount", nMapCount);
        ShaderChief::SetUni1i("rMapCount", rMapCount);
    }

    glBindVertexArray(VAO);
    if(indexed){
        glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0); //Render from index buffer rather than vertex buffer
    } else{
        glDrawArrays(GL_TRIANGLES, 0, 36); //GL_TRIANGLES??
    }
    glBindVertexArray(0); //Break the existing vertex arr obj binding
}

void Mesh::Draw2() const{
    int mapCount = 0; //Per mesh
    for(const auto& tex: textures){
        ShaderChief::SetUni1i(("map[" + std::to_string(mapCount++) + ']').c_str(), tex.texUnit);
    }
    ShaderChief::SetUni1i("mapCount", mapCount);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0); //Render from index buffer rather than vertex buffer
    glBindVertexArray(0); //Break the existing vertex arr obj binding
}

//size_t floatSize = sizeof(float);
//glVertexAttribPointer(i, componentsAmt[i], GL_FLOAT, GL_FALSE, GLsizei(std::accumulate(componentsAmt, componentsAmt + arrSize, 0) * floatSize),
//(void*)((!i ? 0 : std::accumulate(componentsAmt, componentsAmt + i, 0)) * floatSize));

//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
//glEnableVertexAttribArray(3);
//The macro returns the byte offset of that variable from the start of the struct