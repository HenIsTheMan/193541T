#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint> indices){
    VAO = VBO = EBO = 0;
    this->vertices = vertices;
    this->indices = indices;
    int index = 0;
    float offset = 0.1f;
    for(float x = -10.f; x < 10.f; x += 2.f){
        for(float y = -10.f; y < 10.f; y += 2.f){
            translations[index++] = glm::vec2(x / 10.0f, y / 10.0f) + glm::vec2(offset);
        }
    }
}

void Mesh::Init(uint amt){
    glGenVertexArrays(1, &VAO); //Gen VAO and get ref ID of it
    glGenBuffers(1, &VBO); //A buffer manages a certain piece of GPU mem
    glGenBuffers(1, &EBO);

    ///Batch vertex vec data into large chunks per vertex attrib type since loading them from file(s) generally gives an arr per...
    std::vector<glm::vec3> pos;
    std::vector<glm::vec4> colour;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normal;
    for(uint i = 0; i < vertices.size(); ++i){
        pos.emplace_back(vertices[i].pos);
        colour.emplace_back(vertices[i].colour);
        texCoords.emplace_back(vertices[i].texCoords);
        normal.emplace_back(vertices[i].normal);
    }

    size_t size[6], biggerSize[6]{0, };
    size[0] = 0;
    size[1] = pos.size() * sizeof(glm::vec3);
    size[2] = colour.size() * sizeof(glm::vec4);
    size[3] = texCoords.size() * sizeof(glm::vec2);
    size[4] = normal.size() * sizeof(glm::vec3);
    size[5] = sizeof(translations);
    for(short i = 0; i < sizeof(biggerSize) / sizeof(biggerSize[0]); ++i){
        biggerSize[i] = (!i ? 0 : biggerSize[i - 1]) + size[i];
    }

    glm::mat4* modelMatrices = new glm::mat4[amt];
    float radius = 9.f, offset = 2.3f;
    for(uint i = 0; i < amt; ++i){
        float angle = (float)i / (float)amt * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset; //Multi??
        ///transform x and z along the circle and randomly displace along circle with 'radius' in range [-offset, offset]??
        glm::mat4 model = translate(glm::mat4(1.f), glm::vec3(sin(angle) * radius + displacement, 10.f + displacement * 0.4f, cos(angle) * radius + displacement));
        model = rotate(model, float(rand() % 360), glm::vec3(0.4f, 0.6f, 0.8f));
        model = scale(model, glm::vec3((rand() % 21) / 100.0f + 0.05f));
        modelMatrices[i] = model;
    }

    glBindVertexArray(VAO); { //Config VAO
        glBindBuffer(GL_ARRAY_BUFFER, VBO); //Makes VBO the buffer currently bound to the GL_ARRAY_BUFFER target, GL_ARRAY_BUFFER is VBO's type
        //glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW); //Copies vertex data stored in 'vertices' into VBO's mem //Mem layout of all structs in C++ is sequential so can put &vertices[0]
        
        ///Can combine vertex attrib data into 1 arr and fill vertex buffer with glBufferData
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex) + sizeof(translations) + amt * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, biggerSize[0], size[1], &pos[0]); //&pos??
        glBufferSubData(GL_ARRAY_BUFFER, biggerSize[1], size[2], &colour[0]);
        glBufferSubData(GL_ARRAY_BUFFER, biggerSize[2], size[3], &texCoords[0]);
        glBufferSubData(GL_ARRAY_BUFFER, biggerSize[3], size[4], &normal[0]);
        glBufferSubData(GL_ARRAY_BUFFER, biggerSize[4], size[5], &translations[0]);
        glBufferSubData(GL_ARRAY_BUFFER, biggerSize[5], amt * sizeof(glm::mat4), &modelMatrices[0]);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //Element index buffer
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint), &indices[0], GL_STATIC_DRAW); //Allocates a piece of GPU mem and adds data into it (reserve mem only if pass NULL as its data argument)

        //Specify vertex shader layout via vertex attrib pointers (attribs: pos, colour, normal, texture)
        int componentsAmt[]{3, 4, 2, 3, 2, 4, 4, 4, 4};
        for(int i = 0; i < sizeof(componentsAmt) / sizeof(componentsAmt[0]); ++i){
            glEnableVertexAttribArray(i);
            if(i < 5){
                glVertexAttribPointer(i, componentsAmt[i], GL_FLOAT, GL_FALSE, componentsAmt[i] * sizeof(float), (void*)(biggerSize[i])); //Specify a batch (112233) vertex attrib layout for the vertex arr's vertex buffer content
            } else{ //Max amt of data allowed for a vertex attrib is 4 floats so reserve 4 vertex attribs for mat4
                glVertexAttribPointer(i, componentsAmt[i], GL_FLOAT, GL_FALSE, componentsAmt[i] * sizeof(glm::vec4), (void*)(biggerSize[5] + ((size_t)i - 5) * sizeof(glm::vec4)));
            }
            //glVertexAttribPointer(i, componentsAmt[i], GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)((!i ? 0 : std::accumulate(componentsAmt, componentsAmt + i, 0)) * floatSize)); //Specify an interleaved (123123, diff vertex attrib data next to one another in mem per vertex) attrib layout for the vertex array buffer's content
        }
        for(short i = 4; i < 9; ++i){
            glVertexAttribDivisor(i, 1); //Params: vertex attrib location, attrib divisor (0 means update vertex attrib to the next element per vertex and non-0 means... every non-0 amt of instances) //Config vertex attrib as an instanced arr (allows for passing more data [limited by mem] to the vertex shader for instanced drawing/...)
        }

        //float data[] = {.5f, 1.f, -.35f}; //Can read data from file to save mem
        ////glBufferSubData(GL_ARRAY_BUFFER, 24, sizeof(data), &data); //Range of [24, 24 + sizeof(data)] //Fill certain regions of the buffer's mem by specifying an offset //Must allocate mem with glBufferData 1st before calling glBufferSubData
        //void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY); //Retrieve pointer to the currently bound buffer's mem
        //memcpy(ptr, data, sizeof(data)); //Copy data into the mem
        // make sure to tell OpenGL we're done with the pointer
        //glUnmapBuffer(GL_ARRAY_BUFFER); //The ptr becomes invalid and the function returns GL_TRUE if OpenGL was able to map your data successfully to the buffer??

        ///Copy data of a specified size from 1 buffer to another (from a specified read offset to a specified write offset)
        //glBindBuffer(GL_ARRAY_BUFFER, vbo1);
        //glBindBuffer(GL_COPY_WRITE_BUFFER, vbo2);
        //glCopyBufferSubData(GL_ARRAY_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, 8 * sizeof(float)); //Use GL_COPY_READ_BUFFER and/or GL_COPY_WRITE_BUFFER if 2 buffers of the same type

    } glBindVertexArray(0); //Break the existing vertex arr obj binding
}

void Mesh::LoadTexture(cstr fPath, str type){
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

void Mesh::Draw(bool indexed, bool tex){
    if(VAO + VBO + EBO == 0){
        Init();
    }
    if(tex){ //Make sure each shader sampler uniform corresponds to the correct texture unit
        for(uint i = 0; i < textures.size(); ++i){
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, textures[i].refID);
            ShaderChief::SetUni1i(("material." + textures[i].type + "Map").c_str(), i);
        }
    }
    glBindVertexArray(VAO); {
        if(indexed){
            glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0); //Render from index buffer rather than vertex buffer
        } else{
            glDrawArrays(GL_TRIANGLES, 0, 36); //Draw/Render call
        }
    } glBindVertexArray(0); //Break the existing vertex arr obj binding
    if(tex){
        for(uint i = 0; i < textures.size(); ++i){
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
}

void Mesh::DrawPts(uint amt){
    if(VAO + VBO + EBO == 0){
        Init(amt);
    }
    glBindVertexArray(VAO); {
        glDrawArrays(GL_POINTS, 0, amt); //GL_POINTS (each vertex is a primitive and rendered as a pt) is a render primitive //GL_POINT??
    } glBindVertexArray(0);
}

void Mesh::DrawInstanced(bool indexed, bool tex, uint amt){ //For instanced drawing/rendering (Draw/... objs with equal mesh data in 1 draw/... call so save on CPU-to-GPU comms [slow])
    if(VAO + VBO + EBO == 0){
        Init(amt);
    }
    if(tex){ //Make sure each shader sampler uniform corresponds to the correct texture unit
        for(uint i = 0; i < textures.size(); ++i){
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, textures[i].refID);
            ShaderChief::SetUni1i(("material." + textures[i].type + "Map").c_str(), i);
        }
    }
    glBindVertexArray(VAO); {
        if(indexed){
            glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0, amt);
        } else{
            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, amt);
        }
    } glBindVertexArray(0);
    if(tex){
        for(uint i = 0; i < textures.size(); ++i){
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }
}

//GLsizei(std::accumulate(componentsAmt, componentsAmt + arrSize, 0) * floatSize)
//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal)); //The macro returns the byte offset of that variable from the start of the struct