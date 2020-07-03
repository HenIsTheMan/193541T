#include "Mesh.h"
#include <random> //??

int Mesh::primitiveRestartIndex = 0;

Mesh::Mesh(const std::vector<Vertex>* const& vertices, const std::vector<uint>* const& indices) noexcept{
    VAO = VBO = EBO = 0;
    if(vertices){
        this->vertices = *vertices;
    }
    this->indices = const_cast<std::vector<uint>* const&>(indices);
}

Mesh::Mesh(const Mesh& other) noexcept{
    VAO = other.VAO;
    VBO = other.VBO;
    EBO = other.EBO;
    vertices = other.vertices;
    if(this != &other){
        indices = new std::vector<uint>(*(other.indices));
    }
    textures = other.textures;
}

Mesh::~Mesh() noexcept{
    delete indices;
    glDeleteBuffers(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Mesh::Init(const uint& instanceAmt){
    glGenVertexArrays(1, &VAO); //Gen VAO and get ref ID of it
    glGenBuffers(1, &VBO); //A buffer manages a certain piece of GPU mem
    glGenBuffers(1, &EBO);

    ///Batch vertex vec data into large chunks per vertex attrib type since loading them from file(s) generally gives an arr per...
    std::vector<glm::vec3> pos;
    std::vector<glm::vec4> colour;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normal;
    std::vector<glm::vec3> tangent;
    std::vector<glm::vec3> bitangent;
    for(uint i = 0; i < vertices.size(); ++i){
        pos.emplace_back(vertices[i].pos);
        colour.emplace_back(vertices[i].colour);
        texCoords.emplace_back(vertices[i].texCoords);
        normal.emplace_back(vertices[i].normal);
        tangent.emplace_back(vertices[i].tangent);
        bitangent.emplace_back(vertices[i].bitangent);
    }

    ///Can read data from file to save mem
    size_t size[8]{0, }, biggerSize[8]{0, };
    size[1] = pos.size() * sizeof(glm::vec3);
    size[2] = colour.size() * sizeof(glm::vec4);
    size[3] = texCoords.size() * sizeof(glm::vec2);
    size[4] = normal.size() * sizeof(glm::vec3);
    size[5] = sizeof(translations);
    size[6] = instanceAmt * sizeof(glm::mat4);
    size[7] = tangent.size() * sizeof(glm::vec3);
    for(short i = 1; i < sizeof(biggerSize) / sizeof(biggerSize[0]); ++i){
        biggerSize[i] = biggerSize[i - 1] + size[i];
    }

    int index = 0;
    for(float x = -25.f; x < 25.f; x += 5.f){
        for(float y = -25.f; y < 25.f; y += 5.f){
            translations[index++] = glm::vec2(x, y);
        }
    }

    glm::mat4* modelMatrices = new glm::mat4[instanceAmt];
    float radius = 9.f;
    float offset = 2.3f;
    for(uint i = 0; i < instanceAmt; ++i){
        float angle = (float)i / (float)instanceAmt * 360.f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.f - offset; //Multi??
        ///transform x and z along the circle and randomly displace along circle with 'radius' in range [-offset, offset]??
        glm::mat4 model = translate(glm::mat4(1.f), glm::vec3(sin(angle) * radius + displacement, 20.f + displacement * 0.4f, cos(angle) * radius + displacement));
        model = rotate(model, float(rand() % 360), glm::vec3(0.4f, 0.6f, 0.8f));
        model = scale(model, glm::vec3((rand() % 21) / 100.f + 0.05f));
        modelMatrices[i] = model;
    }

    glBindVertexArray(VAO); {
        glBindBuffer(GL_ARRAY_BUFFER, VBO); //Makes VBO the buffer currently bound to the GL_ARRAY_BUFFER target, GL_ARRAY_BUFFER is VBO's type
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex) + size[5] + size[6], NULL, GL_STATIC_DRAW); //Alloc mem with glBufferData 1st //Can combine vertex attrib data into 1 arr or vec and fill VBO's mem with glBufferData
        glBufferSubData(GL_ARRAY_BUFFER, biggerSize[0], size[1], &pos[0]); //Mem layout of all structs in C++ is sequential so can put &pos[0]
        glBufferSubData(GL_ARRAY_BUFFER, biggerSize[1], size[2], &colour[0]); //Range of [biggerSize[1], size[2]]
        glBufferSubData(GL_ARRAY_BUFFER, biggerSize[2], size[3], &texCoords[0]);
        glBufferSubData(GL_ARRAY_BUFFER, biggerSize[3], size[4], &normal[0]);
        uint tempBO;
        glGenBuffers(1, &tempBO);
        glBindBuffer(GL_COPY_READ_BUFFER, tempBO);
        glBufferData(GL_COPY_READ_BUFFER, size[5], translations, GL_STATIC_DRAW);
        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_ARRAY_BUFFER, 0, biggerSize[4], size[5]); //Copy data of a specified size from 1 buffer to another (from a specified read offset to a specified write offset) //Use GL_COPY_READ_BUFFER and/or... if 2 buffers of the same type
        glBufferSubData(GL_ARRAY_BUFFER, biggerSize[5], size[6], &modelMatrices[0]);
        glBufferSubData(GL_ARRAY_BUFFER, biggerSize[6], size[7], &tangent[0]);
        glBufferSubData(GL_ARRAY_BUFFER, biggerSize[7], bitangent.size() * sizeof(glm::vec3), &bitangent[0]);

        if(indices){
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); //Element index buffer
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, (*indices).size() * sizeof(uint), &((*indices)[0]), GL_STATIC_DRAW); //Allocates a piece of GPU mem and adds data into it (reserve mem only if pass NULL as its data argument)
        }

        //Specify vertex shader layout via vertex attrib pointers (attribs: pos, colour, normal, texture)
        int componentsAmt[]{3, 4, 2, 3, 2, 4, 4, 4, 4, 3, 3};
        for(int i = 0; i < sizeof(componentsAmt) / sizeof(componentsAmt[0]); ++i){
            glEnableVertexAttribArray(i);
            if(i < 5 || i > 8){
                glVertexAttribPointer(i, componentsAmt[i], GL_FLOAT, GL_FALSE, componentsAmt[i] * sizeof(float), (void*)(biggerSize[i])); //Specify a batch (112233) vertex attrib layout for the vertex arr's vertex buffer content
            } else{ //Max amt of data allowed for a vertex attrib is 4 floats so reserve 4 vertex attribs for mat4
                glVertexAttribPointer(i, componentsAmt[i], GL_FLOAT, GL_FALSE, componentsAmt[i] * sizeof(glm::vec4), (void*)(biggerSize[5] + ((size_t)i - 5) * sizeof(glm::vec4)));
            }
        }
        for(short i = 4; i < 9; ++i){
            glVertexAttribDivisor(i, 1); //Params: vertex attrib location, attrib divisor (0 means update vertex attrib to the next element per vertex and non-0 means... every non-0 amt of instances) //Config vertex attrib as an instanced arr (allows for passing more data [limited by mem] to the vertex shader for instanced drawing/...)
        }
    } glBindVertexArray(0); //Break the existing vertex arr obj binding
    delete[] modelMatrices;
}

Mesh* const Mesh::CreatePts(){
    Mesh* mesh = new Mesh;
    mesh->vertices.emplace_back(Vertex(glm::vec3(-1.f, 1.f, 0.f), glm::vec4(1.f, 0.f, 0.f, 1.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, 0.f)));
    mesh->vertices.emplace_back(Vertex(glm::vec3(1.f, 1.f, 0.f), glm::vec4(0.f, 1.f, 0.f, 1.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, 0.f)));
    mesh->vertices.emplace_back(Vertex(glm::vec3(1.f, -1.f, 0.f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, 0.f)));
    mesh->vertices.emplace_back(Vertex(glm::vec3(-1.f, -1.f, 0.f), glm::vec4(1.f, 1.f, 0.f, 1.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, 0.f)));
    mesh->vertices.emplace_back(Vertex(glm::vec3(0.f, 0.f, 0.f), glm::vec4(1.f, 0.f, 1.f, 1.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, 0.f)));
    return mesh;
}

Mesh* const Mesh::CreateQuad(){
    Mesh* mesh = new Mesh;
    glm::vec3 pos0(-1.f, -1.f, 0.f);
    glm::vec3 pos1(1.f, 1.f, 0.f);
    glm::vec3 pos2(-1.f, 1.f, 0.f);
    glm::vec3 pos3(1.f, -1.f, 0.f);
    glm::vec2 uv0(0.f, 0.f);
    glm::vec2 uv1(1.f, 1.f);
    glm::vec2 uv2(0.f, 1.f);
    glm::vec2 uv3(1.f, 0.f);

    glm::vec3 edge1 = pos1 - pos0;
    glm::vec3 edge2 = pos2 - pos0;
    glm::vec2 deltaUV1 = uv1 - uv0;
    glm::vec2 deltaUV2 = uv2 - uv0;
    float f = 1.f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    glm::vec3 tangent1, bitangent1; //For each vertex //T and B lie on the same plane as normal map surface and align with tex axes U and V so calc them with vertices (to get edges of...) and texCoords (since in tangent space) of primitives
    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent1 = glm::normalize(tangent1);

    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent1 = glm::normalize(bitangent1);

    mesh->vertices.emplace_back(Vertex(glm::vec3(-1.f, -1.f, 0.f), glm::vec4(0.f, 0.f, 1.f, 1.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, 1.f), tangent1, bitangent1));
    mesh->vertices.emplace_back(Vertex(glm::vec3(1.f, 1.f, 0.f), glm::vec4(0.f, 1.f, 1.f, 1.f), glm::vec2(1.f, 1.f), glm::vec3(0.f, 0.f, 1.f), tangent1, bitangent1));
    mesh->vertices.emplace_back(Vertex(glm::vec3(-1.f, 1.f, 0.f), glm::vec4(1.f, 0.f, 0.f, 1.f), glm::vec2(0.f, 1.f), glm::vec3(0.f, 0.f, 1.f), tangent1, bitangent1));
    mesh->vertices.emplace_back(Vertex(glm::vec3(1.f, -1.f, 0.f), glm::vec4(0.f, 1.f, 0.f, 1.f), glm::vec2(1.f, 0.f), glm::vec3(0.f, 0.f, 1.f), tangent1, bitangent1));
    mesh->indices = new std::vector<uint>{0, 1, 2, 0, 3, 1};
    return mesh;
}

Mesh* const Mesh::CreateCube(){
    Mesh* mesh = new Mesh;

    mesh->vertices.emplace_back(Vertex(glm::vec3(1.f, 1.f, 1.f), glm::vec4(1.f), glm::vec2(1.f, 0.f), glm::vec3(0.f, 1.f, 0.f)));
    mesh->vertices.emplace_back(Vertex(glm::vec3(1.f, 1.f, -1.f), glm::vec4(1.f), glm::vec2(1.f, 1.f), glm::vec3(0.f, 1.f, 0.f)));
    mesh->vertices.emplace_back(Vertex(glm::vec3(-1.f, 1.f, -1.f), glm::vec4(1.f), glm::vec2(0.f, 1.f), glm::vec3(0.f, 1.f, 0.f)));
    mesh->vertices.emplace_back(Vertex(glm::vec3(-1.f, 1.f, 1.f), glm::vec4(1.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 1.f, 0.f)));

    mesh->vertices.emplace_back(Vertex(glm::vec3(1.f, -1.f, 1.f), glm::vec4(1.f), glm::vec2(0.f, 0.f), glm::vec3(1.f, 0.f, 0.f)));
    mesh->vertices.emplace_back(Vertex(glm::vec3(1.f, -1.f, -1.f), glm::vec4(1.f), glm::vec2(1.f, 0.f), glm::vec3(1.f, 0.f, 0.f)));
    mesh->vertices.emplace_back(Vertex(glm::vec3(1.f, 1.f, -1.f), glm::vec4(1.f), glm::vec2(1.f, 1.f), glm::vec3(1.f, 0.f, 0.f)));
    mesh->vertices.emplace_back(Vertex(glm::vec3(1.f, 1.f, 1.f), glm::vec4(1.f), glm::vec2(0.f, 1.f), glm::vec3(1.f, 0.f, 0.f)));

    mesh->vertices.emplace_back(Vertex(glm::vec3(1.f, 1.f, 1.f), glm::vec4(1.f), glm::vec2(1.f, 1.f), glm::vec3(0.f, 0.f, 1.f)));
    mesh->vertices.emplace_back(Vertex(glm::vec3(-1.f, 1.f, 1.f), glm::vec4(1.f), glm::vec2(0.f, 1.f), glm::vec3(0.f, 0.f, 1.f)));
    mesh->vertices.emplace_back(Vertex(glm::vec3(-1.f, -1.f, 1.f), glm::vec4(1.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, 1.f)));
    mesh->vertices.emplace_back(Vertex(glm::vec3(1.f, -1.f, 1.f), glm::vec4(1.f), glm::vec2(1.f, 0.f), glm::vec3(0.f, 0.f, 1.f)));

    mesh->vertices.emplace_back(Vertex(glm::vec3(1.f, -1.f, -1.f), glm::vec4(1.f), glm::vec2(1.f, 0.f), glm::vec3(0.f, 0.f, -1.f)));
    mesh->vertices.emplace_back(Vertex(glm::vec3(-1.f, -1.f, -1.f), glm::vec4(1.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, 0.f, -1.f)));
    mesh->vertices.emplace_back(Vertex(glm::vec3(-1.f, 1.f, -1.f), glm::vec4(1.f), glm::vec2(0.f, 1.f), glm::vec3(0.f, 0.f, -1.f)));
    mesh->vertices.emplace_back(Vertex(glm::vec3(1.f, 1.f, -1.f), glm::vec4(1.f), glm::vec2(1.f, 1.f), glm::vec3(0.f, 0.f, -1.f)));

    mesh->vertices.emplace_back(Vertex(glm::vec3(-1.f, -1.f, -1.f), glm::vec4(1.f), glm::vec2(1.f, 0.f), glm::vec3(-1.f, 0.f, 0.f)));
    mesh->vertices.emplace_back(Vertex(glm::vec3(-1.f, -1.f, 1.f), glm::vec4(1.f), glm::vec2(0.f, 0.f), glm::vec3(-1.f, 0.f, 0.f)));
    mesh->vertices.emplace_back(Vertex(glm::vec3(-1.f, 1.f, 1.f), glm::vec4(1.f), glm::vec2(0.f, 1.f), glm::vec3(-1.f, 0.f, 0.f)));
    mesh->vertices.emplace_back(Vertex(glm::vec3(-1.f, 1.f, -1.f), glm::vec4(1.f), glm::vec2(1.f, 1.f), glm::vec3(-1.f, 0.f, 0.f)));

    mesh->vertices.emplace_back(Vertex(glm::vec3(-1.f, -1.f, -1.f), glm::vec4(1.f), glm::vec2(0.f, 1.f), glm::vec3(0.f, -1.f, 0.f)));
    mesh->vertices.emplace_back(Vertex(glm::vec3(1.f, -1.f, -1.f), glm::vec4(1.f), glm::vec2(1.f, 1.f), glm::vec3(0.f, -1.f, 0.f)));
    mesh->vertices.emplace_back(Vertex(glm::vec3(1.f, -1.f, 1.f), glm::vec4(1.f), glm::vec2(1.f, 0.f), glm::vec3(0.f, -1.f, 0.f)));
    mesh->vertices.emplace_back(Vertex(glm::vec3(-1.f, -1.f, 1.f), glm::vec4(1.f), glm::vec2(0.f, 0.f), glm::vec3(0.f, -1.f, 0.f)));

    mesh->indices = new std::vector<uint>;
    short myArr[6]{0, 1, 2, 2, 3, 0};
    for(short i = 0; i < 6; ++i){
        for(short j = 0; j < 6; ++j){
            (*(mesh->indices)).emplace_back(i * 4 + myArr[j]);
        }
    }

    return mesh;
}

void Mesh::Draw(const int& primitive, const uint& instanceAmt){
    try{
        if(instanceAmt == 0){
            throw std::runtime_error("Invalid amt of instances!\n");
        }
    } catch(std::runtime_error err){
        printf(err.what());
    }
    if(!VAO && !VBO && !EBO){ //Init on 1st draw/...
        Init(instanceAmt);
    }
    for(uint i = 0; i < textures.size(); ++i){
        if(textures[i].GetActiveOnMesh()){
            ShaderProg::UseTex(GL_TEXTURE_2D, textures[i], ("material." + textures[i].GetType() + "Map").c_str());
        }
    }
    if(primitive == GL_TRIANGLE_STRIP){
        glEnable(GL_PRIMITIVE_RESTART);
        glPrimitiveRestartIndex(primitiveRestartIndex);
    }
    glBindVertexArray(VAO); {
        if(instanceAmt == 1){
            indices ? glDrawElements(primitive, GLsizei((*indices).size()), GL_UNSIGNED_INT, 0) : glDrawArrays(primitive, 0, (GLsizei)vertices.size()); //Draw/Render call/command
        } else{
            indices ? glDrawElementsInstanced(primitive, GLsizei((*indices).size()), GL_UNSIGNED_INT, 0, instanceAmt) : glDrawArraysInstanced(primitive, 0, (GLsizei)vertices.size(), instanceAmt); //For instanced drawing/rendering (Draw/... objs with equal mesh data in 1 draw/... call so save on CPU-to-GPU comms [slow])
        }
    } glBindVertexArray(0); //Break the existing VAO binding
    if(primitive == GL_TRIANGLE_STRIP){
        glDisable(GL_PRIMITIVE_RESTART);
    }
    for(uint i = 0; i < textures.size(); ++i){
        if(textures[i].GetActiveOnMesh()){
            ShaderProg::StopUsingTex(GL_TEXTURE_2D, textures[i]);
            textures[i].SetActiveOnMesh(0);
        }
    }
}

void Mesh::LoadTex(const cstr& fPath, const str& type, const bool&& flipTex){
    Tex tex;
    const std::vector<cstr>* const fPaths = new std::vector<cstr>{fPath};
    tex.Create(GL_TEXTURE_2D, 999, 0, 0, GL_NEAREST, GL_LINEAR, GL_REPEAT, type, fPaths, flipTex);
    delete fPaths;
    textures.emplace_back(tex);
}

std::vector<std::vector<float>> Mesh::GenRandHeightData(const HillAlgParams& params){
    std::vector<std::vector<float>> heightData(params.rows, std::vector<float>(params.columns, 0.f));

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> hillRadiusDistribution(params.hillRadiusMin, params.hillRadiusMax);
    std::uniform_real_distribution<float> hillHeightDistribution(params.hillMinHeight, params.hillMaxHeight);
    std::uniform_int_distribution<int> hillCenterRowIntDistribution(0, params.rows - 1);
    std::uniform_int_distribution<int> hillCenterColIntDistribution(0, params.columns - 1);

    for(int i = 0; i < params.numHills; ++i){
        const auto hillCenterRow = hillCenterRowIntDistribution(generator);
        const auto hillCenterCol = hillCenterColIntDistribution(generator);
        const auto hillRadius = hillRadiusDistribution(generator);
        const auto hillHeight = hillHeightDistribution(generator);
        for(auto r = hillCenterRow - hillRadius; r < hillCenterRow + hillRadius; ++r){
            for(auto c = hillCenterCol - hillRadius; c < hillCenterCol + hillRadius; ++c){
                if(r < 0 || r >= params.rows || c < 0 || c >= params.columns){
                    continue;
                }
                const auto r2 = hillRadius * hillRadius; //r*r ter??
                const auto x2x1 = hillCenterCol - c; //(x2-x1) ter??
                const auto y2y1 = hillCenterRow - r; //(y2-y1) ter??
                const auto height = float(r2 - x2x1 * x2x1 - y2y1 * y2y1);
                if(height < 0.f){ //should not happen, but checks like this won't hurt anyway
                    continue;
                }

                //we calculate a factor, how far is that number on scale from 0.0 to r2 and we add the random hill height multiplied by this factor. We also make sure however, that we don't exceed 1.0, which is the maximum possible height. This way we keep the heightmap data between 0.0 and 1.0 for sure!
                const auto factor = height / r2;
                heightData[r][c] += hillHeight * factor;
                if(heightData[r][c] > 1.f) {
                    heightData[r][c] = 1.f;
                }
            }
        }
    }
    return heightData;
}

const std::vector<std::vector<float>> Mesh::ExtractHeightData(cstr const& fPath){
    stbi_set_flip_vertically_on_load(1);
    int w, h, bytesPerPixel;
    const unsigned char* imgData = stbi_load(fPath, &w, &h, &bytesPerPixel, 0);
    if(!imgData){
        exit(1);
    }

    std::vector<std::vector<float>> result(h, std::vector<float>(w));
    const unsigned char* pixelPtr = &imgData[0]; //Also can take G or B component
    for(short i = 0; i < h; ++i){
        for(short j = 0; j < w; ++j){
            result[i][j] = float(*pixelPtr) / 255.f;
            pixelPtr += bytesPerPixel;
        }
    } //At the end of the loop, the ptr is increased by number of bytes per pixel, which means we're jumping to the next pixel
    stbi_image_free(const_cast<unsigned char*>(imgData));
    return result;
}

Mesh* const Mesh::CreateRandHeightMap(const HillAlgParams& params){
    std::vector<std::vector<float>> heightData = GenRandHeightData(params);
    int rows = int(heightData.size()), columns = int(heightData[0].size());

    Mesh* mesh = new Mesh;
    GenHeightMapVertices(mesh, heightData, rows, columns);
    GenHeightMapIndices(mesh, rows, columns);
    return mesh;

    ////Clear the data, we won't need it anymore
    //_vertices.clear();
    //_textureCoordinates.clear();
    //_normals.clear();
}

Mesh* const Mesh::CreateHeightMap(cstr const& fPath){
    std::vector<std::vector<float>> heightData = ExtractHeightData(fPath);
    int rows = int(heightData.size()), columns = int(heightData[0].size());

    Mesh* mesh = new Mesh;
    GenHeightMapVertices(mesh, heightData, rows, columns);
    GenHeightMapIndices(mesh, rows, columns);
    return mesh;
}

void Mesh::GenHeightMapVertices(Mesh* const& mesh, const std::vector<std::vector<float>>& heightData, const int& rows, const int& columns){
    auto pos = std::vector<std::vector<glm::vec3>>(rows, std::vector<glm::vec3>(columns));
    auto texCoords = std::vector<std::vector<glm::vec2>>(rows, std::vector<glm::vec2>(columns));
    const auto textureStepU = .001f, textureStepV = .001f; //Smaller means tex repeats lesser
    for(auto i = 0; i < rows; ++i){
        for(auto j = 0; j < columns; ++j){
            const auto factorRow = float(i) / float(rows - 1);
            const auto factorColumn = float(j) / float(columns - 1);
            const auto& vertexHeight = heightData[i][j];
            pos[i][j] = glm::vec3(-0.5f + factorColumn, vertexHeight, -0.5f + factorRow);

            texCoords[i][j] = glm::vec2(textureStepU * j, textureStepV * i);
        }
    }

    auto normals = std::vector<std::vector<glm::vec3>>(rows, std::vector<glm::vec3>(columns));
    std::vector<std::vector<glm::vec3>> tempNormals[2];
    for(auto i = 0; i < 2; ++i){
        tempNormals[i] = std::vector<std::vector<glm::vec3>>(rows - 1, std::vector<glm::vec3>(columns - 1));
    }
    for(auto i = 0; i < rows - 1; ++i){
        for(auto j = 0; j < columns - 1; ++j){
            const auto& vertexA = pos[i][j];
            const auto& vertexB = pos[i][j + 1];
            const auto& vertexC = pos[i + 1][j + 1];
            const auto& vertexD = pos[i + 1][j];

            const auto triangleNormalA = glm::cross(vertexB - vertexA, vertexA - vertexD);
            const auto triangleNormalB = glm::cross(vertexD - vertexC, vertexC - vertexB);

            tempNormals[0][i][j] = triangleNormalA.length() ? glm::normalize(triangleNormalA) : triangleNormalA;
            tempNormals[1][i][j] = triangleNormalB.length() ? glm::normalize(triangleNormalB) : triangleNormalB;
        }
    }
    for(auto i = 0; i < rows; ++i){
        for(auto j = 0; j < columns; ++j){
            const auto isFirstRow = i == 0;
            const auto isFirstColumn = j == 0;
            const auto isLastRow = i == rows - 1;
            const auto isLastColumn = j == columns - 1;
            auto finalVertexNormal = glm::vec3(0.f);
            if(!isFirstRow && !isFirstColumn){ //Look for triangle to the upper-left
                finalVertexNormal += tempNormals[0][i - 1][j - 1];
            }
            if(!isFirstRow && !isLastColumn){ //Look for triangles to the upper-right
                for(auto k = 0; k < 2; ++k){
                    finalVertexNormal += tempNormals[k][i - 1][j];
                }
            }
            if(!isLastRow && !isLastColumn){ //Look for triangle to the bottom-right
                finalVertexNormal += tempNormals[0][i][j];
            }
            if(!isLastRow && !isFirstColumn){ //Look for triangles to the bottom-right
                for(auto k = 0; k < 2; ++k){
                    finalVertexNormal += tempNormals[k][i][j - 1];
                }
            }
            normals[i][j] = finalVertexNormal.length() ? glm::normalize(finalVertexNormal) : finalVertexNormal; //Store final normal of j-th vertex in i-th row //Normalize to give avg of 4 normals

            mesh->vertices.emplace_back(pos[i][j], glm::vec4(1.f), texCoords[i][j], normals[i][j]);
        }
    }
}

void Mesh::GenHeightMapIndices(Mesh* const& mesh, const int& rows, const int& columns){ //set primitive restart index, which is number of vertices, just like in case of torus and in every row we add vertices one by one and finalize the row with primitive restart index
    primitiveRestartIndex = rows * columns; //numVertices = rows * columns
    mesh->indices = new std::vector<uint>;

    for(auto i = 0; i < rows - 1; ++i){
        for(auto j = 0; j < columns; ++j){
            for(auto k = 0; k < 2; ++k){
                const auto row = i + k;
                const auto index = row * columns + j;
                mesh->indices->emplace_back(index);
            }
        }
        mesh->indices->emplace_back(primitiveRestartIndex); //Restart triangle strips
    }
    //// Calculate total count of indices
    //_numIndices = (rows - 1) * columns * 2 + rows - 1;
}

Mesh* const Mesh::CreateSlicedTexQuad(const float& quadSize, const float& hTile, const float& vTile){
    Mesh* mesh = new Mesh;
    for(uint z = 0; z < (uint)quadSize; ++z){
        for(uint x = 0; x < (uint)quadSize; ++x){
            mesh->vertices.emplace_back(Vertex({float(x) / quadSize - .5f, 0.f, float(z) / quadSize - .5f}, glm::vec4(1.f), {float(x) / quadSize * hTile, 1.f - float(z) / quadSize * vTile}, {0.f, 1.f, 0.f}));
        }
    }
    mesh->indices = new std::vector<uint>;
    for(uint z = 0; z < uint(quadSize - 1.f); ++z){
        for(uint x = 0; x < uint(quadSize - 1.f); ++x){
            ///Triangle 1
            (*(mesh->indices)).emplace_back(uint(quadSize * z + x + 0));
            (*(mesh->indices)).emplace_back(uint(quadSize * (z + 1) + x + 0));
            (*(mesh->indices)).emplace_back(uint(quadSize * z + x + 1));

            ///Triangle 2
            (*(mesh->indices)).emplace_back(uint(quadSize * (z + 1) + x + 1));
            (*(mesh->indices)).emplace_back(uint(quadSize * z + x + 1));
            (*(mesh->indices)).emplace_back(uint(quadSize * (z + 1) + x + 0));
        }
    }
    return mesh;
}

//uint* indicesPtr = new uint[indices.size()];
//std::copy(indices.begin(), indices.end(), indicesPtr);
//void* ptr = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY); //Get ptr to currently bound buffer's mem
//memcpy(ptr, (void*)indicesPtr, indices.size() * sizeof(uint)); //Copy data into the mem
//glUnmapNamedBuffer(EBO); //The ptr becomes invalid and the function returns GL_TRUE if OpenGL was able to map your data successfully to the buffer??

//glVertexAttribPointer(i, componentsAmt[i], GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)((!i ? 0 : std::accumulate(componentsAmt, componentsAmt + i, 0)) * floatSize)); //Specify an interleaved (123123, diff vertex attrib data next to one another in mem per vertex) attrib layout for the vertex array buffer's content
//GLsizei(std::accumulate(componentsAmt, componentsAmt + arrSize, 0) * floatSize)
//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal)); //The macro returns the byte offset of that variable from the start of the struct