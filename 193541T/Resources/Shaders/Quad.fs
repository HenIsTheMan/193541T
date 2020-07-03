#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D texture1;

float near = .1f;
float far = 100.f;

float LineariseDepth(float depth){ //Reverse the process of projection for depth values
    float NDC = depth * 2.f - 1.f; //Re-transform depth values in the range [0, 1] to NDC (range [-1, 1])
    return (2.f * near * far) / (far + near - NDC * (far - near));	//Reverse non-linear eqn from the projection matrix and apply this inversed transformation eqn
}

void main(){
    float depthLinearised = LineariseDepth(gl_FragCoord.z); //gl_FragCoord.xy is fragment's window-space/screen-space coords with (0, 0) being the bottom-left corner //gl_FragCoord.z is depth value of fragment (compared to the depth buffer's content)
    FragColor = texture(texture1, TexCoords);
    /*if(FragColor.a < .1f){
        discard; //Fragment is discarded and not stored in the color buffer
    }*/
    if(FragColor.a > .1f && FragColor.a < 1.f){
        FragColor.a += (depthLinearised / far) * 1.5f;
        FragColor.a = min(FragColor.a, 1.f);
    }
    //FragColor = vec4(vec3(1.f) - vec3(depthLinearised / far), 1.f); //For visualising the depth buffer //Divide by far to make range [0, 1] as linearised depth values range from near to far
    //FragColor = vec4(vec3(gl_FragCoord.z), 1.f);
}

//Depth buffer contains depth values between 0.0 and 1.0 and it compares its contents with the z-values (values range from near to far) of all objs
//Linearly or non-linearly transform z-values in view space to depth values between 0.f and 1.f
//Non-linear depth buffer utilising a non-linear depth eqn proportional to 1/z (large depth value precision when z is small so Z-fighting is more noticeable when objs are further away) is used instead of a linear one due to projection properties
//Depth values in the depth buffer are linear in view space and are either linear or not linear (most likely non-linear due to projection properties) in clip space
//Eqn to transform z-values (returns depth values between near and far) is embedded within the orthographic or perspective projection matrix