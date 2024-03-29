#include "Buffer.h"

Framebuffer::Framebuffer(const int&& texTarget, const int&& depthStencil, const int&& width, const int&& height, const int&& minTexFilter, const int&& magTexFilter, const int&& texWrapper):
    tex(texTarget, depthStencil, width, height, minTexFilter, magTexFilter, texWrapper),
    extractedBrightnessTex(texTarget, depthStencil, width, height, minTexFilter, magTexFilter, texWrapper),
    RBO(depthStencil & 1 ? new Renderbuffer(texTarget, width, height) : 0)
{
    glGenFramebuffers(1, &refID);
    glBindFramebuffer(GL_FRAMEBUFFER, refID); //GL_READ_FRAMEBUFFER (read target) is for operations like glReadPixels and GL_DRAW_FRAMEBUFFER (write target)
        if(texTarget == GL_TEXTURE_2D_MULTISAMPLE){
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, tex.GetRefID(), 0); //Attach colour buffer tex obj to currently bound framebuffer obj //++param info?? //++more colour attachments?? //Colour values are stored once per pixel so colour buffer size unaffected by...
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, extractedBrightnessTex.GetRefID(), 0);
        } else{
            glFramebufferTexture2D(GL_FRAMEBUFFER, depthStencil == 2 ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.GetRefID(), 0);
            glFramebufferTexture2D(GL_FRAMEBUFFER, depthStencil == 2 ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, extractedBrightnessTex.GetRefID(), 0);
        }
        if(depthStencil == 2){ //No need colour buffer so specify that no colour data will be rendered
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }
        if(RBO){ //Attach renderbuffer obj to the depth and stencil attachment of the framebuffer //A vertex's depth value is interpolated to each subsample before depth testing while stencil values are stored per subsample before... so size of depth and stencil buffers rises by qty of subsamples per pixel
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, depthStencil == 1 ? GL_DEPTH_ATTACHMENT : GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO->GetRefID());
        }
        uint colAttachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glDrawBuffers(2, colAttachments);
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){ //Verify currently bound framebuffer //++more possibilities?? //++GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT and more
            puts("Created framebuffer is incomplete.\n");
        }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Del() const{
    tex.Del();
    if(RBO){
        RBO->Del();
    }
    glDeleteFramebuffers(1, &refID);
    delete this;
}

const uint& Framebuffer::GetRefID() const{
    return refID;
}

const Tex& Framebuffer::GetTex() const{
    return tex;
}

const Tex& Framebuffer::GetExtractedBrightnessTex() const{
    return extractedBrightnessTex;
}

const Renderbuffer& Framebuffer::GetRenderbuffer() const{
    return *RBO;
}

Tex::Tex(): activeOnMesh(0), refID(0), type("unknown"){}

void Tex::Del() const noexcept{
    glDeleteBuffers(1, &refID);
}

Tex::Tex(const int& texTarget, const int& depthStencil, const int& width, const int& height, const int& minTexFilter, const int& magTexFilter, const int& texWrapper): activeOnMesh(0){
    Create(texTarget, depthStencil, width, height, minTexFilter, magTexFilter, texWrapper, "render buffer");
}

void Tex::Create(const int& texTarget, const int& depthStencil, const int& width, const int& height, const int& minTexFilter, const int& magTexFilter, const int& texWrapper, const str& texType, const std::vector<cstr>* const& fPaths, const bool& flipTex){ //Colour buffer (stores all the frag colours: the visual output)
    //Colour values are clamped to Low Dynamic Range (LDR, [0.f, 1.f]) after each fs run before they are stored in FBO (use 8 bits of mem per colour component by default) with a normalised fixed-pt FB colour format like GL_RGB as its texColourBuffer's internal format (LDR rendering)
    //Capture more colour values through HDR rendering with floating pt FBO (...specified as GL_RGB16F [use 16...], GL_RGBA32F [use..., high precision]..., can store floating pt values outside LDR) then do tone mapping (final step of HDR rendering, use diff eqns and/or curves to transform... back to LDR with minimal loss in HDR details, often comes with a specific stylistic colour balance)
    //See details in bright and dark areas using diff exposure lvls (low for bright, high for dark), can specify light intensity of light sources accurately and effects like bloom more feasible and realistic with HDR rendering
    //Some tone mapping algs favour certain colours or light intensities while others show low and high exposure colours simultaneously

    //Combine pics of same scene with diff exposure lvls to form HDR img (details visible based on combined exposure lvls or exposure it is viewed with)
    //Clamping... results in loss of details in bright and dark areas and hence an unrealistic look in bright regions (all have same white colour)??
    //add a gamma correction filter for good measure (including the use of sRGB textures)??
    //This operation holds for most types of FB colour formats?? Floating pt texColour buffer and FB colour formats??
    //Lighting eqns not limited by LDR so can tone map at the end of a lighting fs without using a floating pt FBO but likely need to store intermediate HDR results as floating pt buffers in complex scenes??
    //Simply re-transforming the colors back with a simple average wouldn't do us much good as brighter areas then become a lot more dominant??

    //unsigned int colorBuffers[2];
    //glGenTextures(2, colorBuffers);

    stbi_set_flip_vertically_on_load(flipTex); //OpenGL reads y/v tex coord in reverse so must flip tex vertically
    glGenTextures(1, &refID);
    glBindTexture(texTarget, refID); { //Make tex referenced by 'refID' the tex currently bound to the currently active tex unit so subsequent tex commands will config it
        type = texType;
        if(fPaths){ //TexCoords exactly between 2 adj faces of the cubemap might not hit an exact face due to hardware limitations so use GL_CLAMP_TO_EDGE to tell OpenGL to return their edge values when tex sampling between faces
            int width, height, colourChannelsAmt;
            unsigned char* data;
            size_t size = (*fPaths).size();
            for(GLuint i = 0; i < size; ++i){ //Gen a tex for each face of the currently bound cubemap
                data = stbi_load((*fPaths)[i], &width, &height, &colourChannelsAmt, 0);
                if(data){
                    //GLenum format = (colourChannelsAmt == 1 ? GL_GREEN : GL_RGB + colourChannelsAmt - 3); //++GL_SRGB and GL_SRGB_ALPHA with bool gamma = 0??
                    GLenum format1 = colourChannelsAmt == 3 ? GL_RGB16F : GL_RGBA16F;
                    GLenum format2 = colourChannelsAmt == 3 ? GL_RGB : GL_RGBA;
                    if(size == 6){
                        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format1, width, height, 0, format2, GL_UNSIGNED_BYTE, data); //All tex targets are looped through this way as the BTS int value of the enums is linearly incremented
                    } else{
                        glTexImage2D(GL_TEXTURE_2D, 0, format1, width, height, 0, format2, GL_UNSIGNED_BYTE, data);
                        glGenerateMipmap(GL_TEXTURE_2D); //Gen required mipmap lvls for the currently bound texture
                    }
                    stbi_image_free(data); //Free the img mem
                } else{
                    printf("Failed to load tex at: %s\n", (*fPaths)[i]);
                }
            }
        } else{
            if(texTarget == GL_TEXTURE_2D_MULTISAMPLE){ //MSAA uses a much larger depth or stencil buffer to determine subsample coverage after the frag shader is run once per pixel for each primitive with vertex data interpolated to the center of each pixel //Amt of subsamples covered affects how much pixel colour mixes with curr framebuffer colour (latest clear colour if 1st draw/...)
                glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 16, GL_RGBA16F, width, height, GL_TRUE); //Multisampled tex attachment //If the last argument is set to GL_TRUE, the image will use identical sample locations and the same number of subsamples for each texel??
            } else{
                const int&& component1 = depthStencil == 2 ? GL_DEPTH_COMPONENT : GL_RGBA16F;
                const int&& component2 = depthStencil == 2 ? GL_DEPTH_COMPONENT : GL_RGBA;
                glTexImage2D(GL_TEXTURE_2D, 0, component1, width, height, 0, component2, depthStencil == 2 ? GL_FLOAT : GL_UNSIGNED_BYTE, NULL); //Set tex's dimensions to screen size (not required) and NULL to allocate mem (data uninitialised) //Render to framebuffer to fill tex //Call glViewport again before rendering to your framebuffer if render the screen to tex of smaller or larger size??
            }
            if(texTarget == GL_TEXTURE_CUBE_MAP){
                for(GLuint i = 0; i < 6; ++i){
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
                }
            }
        }

        ///For Texture Wrapping
        glTexParameteri(texTarget, GL_TEXTURE_WRAP_S, texWrapper);
        glTexParameteri(texTarget, GL_TEXTURE_WRAP_T, texWrapper);
        if(texTarget == GL_TEXTURE_CUBE_MAP){ //??
            glTexParameteri(texTarget, GL_TEXTURE_WRAP_R, texWrapper);
        }
        if(texWrapper == GL_CLAMP_TO_BORDER){
            float borderColor[] = {1.f, 1.f, 1.f, 1.f}; //Use red instead??
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); //So texture(shadowMap, projectedCoords.xy).r returns 1.f if sample outside depth/... map's [0.f, 1.f] coord range
        }

        ///For Texture Filtering
        glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, minTexFilter); //Nearest neighbour/Point filtering/interpolation when textures are scaled downwards
        glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, magTexFilter); //Linear filtering/interpolation for upscaled textures
        //++more??
        //++ Mipmaps for downscaled textures
    } glBindTexture(texTarget, 0); //Unbind currently bound tex from currently active tex unit
}

const bool& Tex::GetActiveOnMesh() const noexcept{
    return activeOnMesh;
}

const uint& Tex::GetRefID() const noexcept{
    return refID;
}

const str& Tex::GetType() const noexcept{
    return type;
}

void Tex::SetActiveOnMesh(const bool& flag) noexcept{
    activeOnMesh = flag;
}

Renderbuffer::Renderbuffer(const int& texTarget, const int& width, const int& height){
    Create(texTarget, width, height);
}

void Renderbuffer::Del() const noexcept{
    glDeleteBuffers(1, &refID);
    delete this;
}

void Renderbuffer::Create(const int& texTarget, const int& width, const int& height){ //RBOs store render data directly (so data in native format) in their buffer (an arr of stuff) without conversions to texture-specific formats so fast as a writeable storage medium (fast when writing or copying data to other buffers and with operations like switching buffers) //The glfwSwapBuffers function may as well be implemented with RBOs (simply write to a renderbuffer img, and swap to the other one at the end)??
    glGenRenderbuffers(1, &refID); //Can read from RBOs via the slow glReadPixels which returns a specified area of pixels from the currently bound framebuffer but not directly from the RBO attachments themselves
    glBindRenderbuffer(GL_RENDERBUFFER, refID); { //RBOs (only as a framebuffer attachment [mem location that can act as a render buffer for the framebuffer] while texs are general purpose data buffers) often used as depth and stencil attachments as no need to sample data values in depth and stencil buffer for depth and stencil testing respectively
        if(texTarget == GL_TEXTURE_2D_MULTISAMPLE){
            glRenderbufferStorageMultisample(GL_RENDERBUFFER, 16, GL_DEPTH24_STENCIL8, width, height); //Config RBO's mem storage //Create a depth and stencil attachment renderbuffer obj //GL_DEPTH24_STENCIL8 is the internal format (determines precision) which holds a depth buffer with 24 bits and...
        } else{
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
        }
    } glBindRenderbuffer(GL_RENDERBUFFER, 0); //Unbind RBO after allocating enuf mem for it
}

const uint& Renderbuffer::GetRefID() const noexcept{
    return refID;
}

UniBuffer::~UniBuffer() noexcept{
    glDeleteBuffers(1, &refID);
}

void UniBuffer::Init(const size_t& bytes){ //Using a UBO increases limit of uni data OpenGL can handle (can query with GL_MAX_VERTEX_UNIFORM_COMPONENTS, good for stuff needing a lot of unis like skeletal animation)
    glGenBuffers(1, &refID);
    glBindBuffer(GL_UNIFORM_BUFFER, refID); {
        glBufferData(GL_UNIFORM_BUFFER, bytes, NULL, GL_STATIC_DRAW); //Alloc bytes of mem
    } glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniBuffer::Bind(const uint& bindingPtIndex, const uint& offset, const size_t& size){ //Bind UBO to binding pt
    glBindBufferRange(GL_UNIFORM_BUFFER, bindingPtIndex, refID, offset, size); //Extra offset and size param as compared to glBindBufferBase
}