#include "Buffer.h"

Framebuffer::Framebuffer(const int&& texTarget, const int&& depthStencil, const int&& width, const int&& height, const int&& minTexFilter, const int&& magTexFilter, const int&& texWrapper):
    texColourBuffer(texTarget, depthStencil, width, height, minTexFilter, magTexFilter, texWrapper),
    RBO(depthStencil & 1 ? new Renderbuffer(texTarget, width, height) : 0)
{
    glGenFramebuffers(1, &refID);
    glBindFramebuffer(GL_FRAMEBUFFER, refID); { //GL_READ_FRAMEBUFFER (read target) is for operations like glReadPixels and GL_DRAW_FRAMEBUFFER (write target)
        if(texTarget == GL_TEXTURE_2D_MULTISAMPLE){
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texColourBuffer.GetRefID(), 0); //Attach colour buffer tex obj to currently bound framebuffer obj //++param info?? //++more colour attachments?? //Colour values are stored once per pixel so colour buffer size unaffected by...
        } else{
            glFramebufferTexture2D(GL_FRAMEBUFFER, depthStencil == 2 ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColourBuffer.GetRefID(), 0);
        }
        if(depthStencil == 2){ //No need colour buffer so specify that no colour data will be rendered
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }
        if(RBO){ //Attach renderbuffer obj to the depth and stencil attachment of the framebuffer //A vertex's depth value is interpolated to each subsample before depth testing while stencil values are stored per subsample before... so size of depth and stencil buffers rises by qty of subsamples per pixel
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, depthStencil == 1 ? GL_DEPTH_ATTACHMENT : GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO->GetRefID());
        }
        if(texTarget == GL_TEXTURE_CUBE_MAP){
            for(GLuint i = 0; i < 6; ++i){
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, texColourBuffer.GetRefID(), 0);
            }
        }
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){ //Verify currently bound framebuffer //++more possibilities?? //++GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT and more
            printf("Created framebuffer is incomplete.\n");
        }
    } glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Del(){
    glDeleteFramebuffers(1, &refID);
}

const uint& Framebuffer::GetRefID() const noexcept{
    return refID;
}

const TexColourBuffer& Framebuffer::GetTexColourBuffer() const noexcept{
    return texColourBuffer;
}

const Renderbuffer& Framebuffer::GetRenderbuffer() const noexcept{
    return *RBO;
}

TexColourBuffer::TexColourBuffer(const int& texTarget, const int& depthStencil, const int& width, const int& height, const int& minTexFilter, const int& magTexFilter, const int& texWrapper){
    Create(texTarget, depthStencil, width, height, minTexFilter, magTexFilter, texWrapper);
}

void TexColourBuffer::Create(const int& texTarget, const int& depthStencil, const int& width, const int& height, const int& minTexFilter, const int& magTexFilter, const int& texWrapper){ //Colour buffer (stores all the frag colours: the visual output)
    glGenTextures(1, &refID);
    glBindTexture(texTarget, refID); {
        if(texTarget == GL_TEXTURE_2D_MULTISAMPLE){ //MSAA uses a much larger depth or stencil buffer to determine subsample coverage after the frag shader is run once per pixel for each primitive with vertex data interpolated to the center of each pixel //Amt of subsamples covered affects how much pixel colour mixes with curr framebuffer colour (latest clear colour if 1st draw/...)
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 16, GL_RGB, width, height, GL_TRUE); //Multisampled tex attachment //If the last argument is set to GL_TRUE, the image will use identical sample locations and the same number of subsamples for each texel??
        } else{
            int component = (depthStencil == 2 ? GL_DEPTH_COMPONENT : GL_RGB);
            glTexImage2D(GL_TEXTURE_2D, 0, component, width, height, 0, component, depthStencil == 2 ? GL_FLOAT : GL_UNSIGNED_BYTE, NULL); //Set tex's dimensions to screen size (not required) and NULL to allocate mem (data uninitialised) //Render to framebuffer to fill tex //Call glViewport again before rendering to your framebuffer if render the screen to tex of smaller or larger size??
        }
        glTexParameteri(texTarget, GL_TEXTURE_MIN_FILTER, minTexFilter);
        glTexParameteri(texTarget, GL_TEXTURE_MAG_FILTER, magTexFilter);
        glTexParameteri(texTarget, GL_TEXTURE_WRAP_S, texWrapper);
        glTexParameteri(texTarget, GL_TEXTURE_WRAP_T, texWrapper);
        if(texTarget == GL_TEXTURE_CUBE_MAP){
            glTexParameteri(texTarget, GL_TEXTURE_WRAP_R, texWrapper);
            for(GLuint i = 0; i < 6; ++i){
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
            }
        }
    } glBindTexture(texTarget, 0);
}

const uint& TexColourBuffer::GetRefID() const noexcept{
    return refID;
}

Renderbuffer::Renderbuffer(const int& texTarget, const int& width, const int& height){
    Create(texTarget, width, height);
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