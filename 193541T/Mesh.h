#pragma once
#include "Buffer.h"
#include "Src.h"
#include "Utility.h"

class Mesh final{ //Single drawable entity
    glm::vec2 translations[100];
    uint VAO, VBO, EBO;
    void Init(const uint&);
public:
    Mesh(const std::vector<Vertex>* const& = nullptr, const std::vector<uint>* const& = nullptr) noexcept;
    Mesh(const Mesh&) noexcept; //Avoid shallow copy so won't delete a wild ptr
    ~Mesh() noexcept;

    ///In local space (local to mesh)
    static Mesh* const CreatePts();
    static Mesh* const CreateQuad();
    static Mesh* const CreateCube();

    std::vector<Vertex> vertices;
    std::vector<uint>* indices;
    std::vector<Tex> textures;
    void Draw(const int&, const uint&); //Multi-texturing??
    //++ DrawQuads(...) and DrawCubes(...)??

    void LoadTex(const cstr&, const str&, const bool&& = 1);

    ///Place all within class??
    struct HillAlgParams{ //const struct??
        HillAlgParams(int rows, int columns, int numHills, int hillRadiusMin, int hillRadiusMax, float hillMinHeight, float hillMaxHeight){
            this->rows = rows;
            this->columns = columns;
            this->numHills = numHills;
            this->hillRadiusMin = hillRadiusMin;
            this->hillRadiusMax = hillRadiusMax;
            this->hillMinHeight = hillMinHeight;
            this->hillMaxHeight = hillMaxHeight;
        }

        int rows; //number of rows of the 2D array to generate
        int columns; //number of columns of the 2D array to generate
        int numHills; //how many hills should be raised
        int hillRadiusMin; //minimal random radius of generated hill(in terms of rows and columns)
        int hillRadiusMax; //maximal random radius of generated hill(in terms of rows and columns)
        float hillMinHeight; //minimal random height of generated hill(float value less than 1.0)
        float hillMaxHeight; //maximal random height of generated hill(float value less than 1.0)
    };

    static int primitiveRestartIndex;
    static std::vector<std::vector<float>> GenRandHeightData(const HillAlgParams&);
    static const std::vector<std::vector<float>> ExtractHeightData(cstr const&);
    static Mesh* const CreateRandHeightMap(const HillAlgParams&);
    static Mesh* const CreateHeightMap(cstr const&);
    static void GenHeightMapVertices(Mesh* const&, const std::vector<std::vector<float>>&, const int&, const int&);
    static void GenHeightMapIndices(Mesh* const&, const int&, const int&);
};

//Specify vertices in CCW winding order so front faces are rendered in CW order while...
//Actual winding order is calculated at the rasterization stage after the vertex shader has run //Vertices are then seen from the viewer's POV