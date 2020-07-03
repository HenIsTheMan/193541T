#version 330 core
out vec4 FragColor;

in myInterface{ //Input interface block
    vec2 TexCoords;
} fsIn; //Instance

uniform sampler2D screenTexture;

const float offset = 1.f / 300.f; //Customisable

void main(){
    FragColor = texture(screenTexture, fsIn.TexCoords);

    ////Post-processing effects
    //FragColor = vec4(vec3(1.f) - vec3(texture(screenTexture, fsIn.TexCoords)), 1.f); //Colour Inversion

    ///Grayscale
    /*FragColor = texture(screenTexture, fsIn.TexCoords);
    //float avg = (FragColor.r + FragColor.g + FragColor.b) / 3.f;
    float avg = 0.2126f * FragColor.r + 0.7152f * FragColor.g + 0.0722f * FragColor.b; //Weighted grayscale (weighted colour channels used, most physically accurate)
    FragColor = vec4(vec3(avg), 1.f);*/

    ///Kernel
    vec2 offsets[9] = vec2[]( //For each surrounding texCoord
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    ///3x3 kernels
    /*float kernel[9] = float[]( //Sharpen kernel (sharpens each colour value by sampling all surrounding pixels)
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );*/

    /*float kernel[9] = float[]( //Blur kernel (vary blur amt over time for drunk effect, can use blur for smoothing colour values)
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16
    );*/
    //Because all values add up to 16, directly returning the combined sampled colors would result in an extremely bright color so we have to divide each value of the kernel by 16??
    
    /*float kernel[9] = float[]( //Edge-detection kernel (highlights all edges and darkens the rest)
        1, 1, 1,
        1, -8, 1,
        1, 1, 1
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; ++i){ //Tex sampling
        sampleTex[i] = vec3(texture(screenTexture, fsIn.TexCoords.st + offsets[i]));
    }
    vec3 col = vec3(0.f);
    for(int i = 0; i < 9; ++i){
        col += sampleTex[i] * kernel[i]; //Multiply sampled tex values with weighted kernel values and add the products
    }
    FragColor = vec4(col, 1.f);*/
}