#include "Mesh.h"

Vertex::Vertex(glm::vec3 pos, glm::vec4 colour, glm::vec2 texCoords, glm::vec3 normal){
    this->pos = pos;
    this->colour = colour;
    this->texCoords = texCoords;
    this->normal = normal;
}

Texture::Texture(uint newRefID, str newType): refID(newRefID), type(newType){}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint> indices){
    this->vertices = vertices;
    this->indices = indices;
    SetupMesh();
}

void Mesh::SetupMesh(){ //Create and config a VAO
    glGenVertexArrays(1, &VAO); //Gen VAO and get ref ID of it
    glGenBuffers(1, &VBO); //A buffer manages a certain piece of GPU mem
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO); {
        glBindBuffer(GL_ARRAY_BUFFER, VBO); //Makes VBO the buffer currently bound to the GL_ARRAY_BUFFER target, GL_ARRAY_BUFFER is VBO's type
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW); //Copies vertex data stored in 'vertices' into VBO's mem //Mem layout of all structs in C++ is sequential so can put &vertices[0]

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //Give meaning to buffer by binding it to a buffer target
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW); //Allocates a piece of GPU mem and adds data into it (reserve mem only if pass NULL as its data argument)

        //Specify vertex shader layout via vertex attrib pointers (attribs: pos, colour, normal, texture)
        int componentsAmt[]{3, 4, 2, 3}, arrSize = sizeof(componentsAmt) / sizeof(componentsAmt[0]);
        for(int i = 0; i < arrSize; ++i){
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, componentsAmt[i], GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)((!i ? 0 : std::accumulate(componentsAmt, componentsAmt + i, 0)) * sizeof(float))); //Specify an interleaved (123123, diff vertex attrib data next to one another in mem per vertex) attrib layout for the vertex array buffer's content
        }

        //float data[] = {.5f, 1.f, -.35f}; //Can read data from file to save mem
        ////glBufferSubData(GL_ARRAY_BUFFER, 24, sizeof(data), &data); //Range of [24, 24 + sizeof(data)] //Fill certain regions of the buffer's mem by specifying an offset //Must allocate mem with glBufferData 1st before calling glBufferSubData
        //void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY); //Retrieve pointer to the currently bound buffer's mem
        //memcpy(ptr, data, sizeof(data)); //Copy data into the mem
        // make sure to tell OpenGL we're done with the pointer
        //glUnmapBuffer(GL_ARRAY_BUFFER); //The ptr becomes invalid and the function returns GL_TRUE if OpenGL was able to map your data successfully to the buffer??

        ////Batch vertex vec data into large chunks per attrib type since loading them from file(s) generally gives an arr per attrib type
        //float positions[] = {...};
        //float normals[] = {...};
        //float tex[] = {...};
        //glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(positions), &positions); //Need 0??
        //glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions), sizeof(normals), &normals);
        //glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions) + sizeof(normals), sizeof(tex), &tex);
        ////Can combine vertex attrib data into 1 arr and fill the buffer with glBufferData

        /////Specify a batch (112233) attrib layout for the vertex array buffer's content
        //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(positions)));
        //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(sizeof(positions) + sizeof(normals)));

        ///Copy data of a specified size from 1 buffer to another (from a specified read offset to a specified write offset)
        //glBindBuffer(GL_ARRAY_BUFFER, vbo1);
        //glBindBuffer(GL_COPY_WRITE_BUFFER, vbo2);
        //glCopyBufferSubData(GL_ARRAY_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, 8 * sizeof(float)); //Use GL_COPY_READ_BUFFER and/or GL_COPY_WRITE_BUFFER if 2 buffers of the same type
    } glBindVertexArray(0); //Break the existing vertex arr obj binding
}

void Mesh::LoadTexture(cstr fPath, str type){ //bool gamma = 0??
    uint refID;
    int width, height, colourChannelsAmt;
    unsigned char* data = stbi_load(fPath, &width, &height, &colourChannelsAmt, 0);
    glGenTextures(1, &refID);
    glBindTexture(GL_TEXTURE_2D, refID); { //Make tex referenced by 'refID' the tex currently bound to the currently active tex unit (most likely default) so subsequent texture commands will config it
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
            textures.emplace_back(Texture(refID, type));
        } else{
            printf("Failed to load texture at: %s\n", fPath);
        }
        stbi_image_free(data); //Free the img mem
    } glBindTexture(GL_TEXTURE_2D, 0); //Unbind currently bound tex from currently active tex unit
}

void Mesh::Draw(bool indexed, bool tex) const{
    if(tex){ //Make sure each shader sampler uniform corresponds to the correct texture unit
        for(uint i = 0; i < textures.size(); ++i){
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, textures[i].refID);
            ShaderChief::SetUni1i(("material." + textures[i].type + "Map").c_str(), i);
        }
    }
    glBindVertexArray(VAO); {
        if(indexed){
            glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0); //Render call //Render from index buffer rather than vertex buffer
        } else{
            glDrawArrays(GL_TRIANGLES, 0, 36); //Render call
        }
    } glBindVertexArray(0); //Break the existing vertex arr obj binding
}

void Mesh::DrawPts(uint amt = 1) const{
    glBindVertexArray(VAO); {
        glDrawArrays(GL_POINTS, 0, amt); //Render call //GL_POINTS (each vertex is a primitive and rendered as a pt) is a render primitive //GL_POINT??
    } glBindVertexArray(0); //Break the existing vertex arr obj binding
}

//size_t floatSize = sizeof(float);
//glVertexAttribPointer(i, componentsAmt[i], GL_FLOAT, GL_FALSE, GLsizei(std::accumulate(componentsAmt, componentsAmt + arrSize, 0) * floatSize),
//(void*)((!i ? 0 : std::accumulate(componentsAmt, componentsAmt + i, 0)) * floatSize));

//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
//glEnableVertexAttribArray(3);
//The macro returns the byte offset of that variable from the start of the struct