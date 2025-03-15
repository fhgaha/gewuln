#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in ivec4 aBoneIds; 
layout(location = 4) in vec4 aWeights;
	
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 finalBonesMatrices[MAX_BONES];
	
out vec2 TexCoords;
	
void main()
{
    vec4 totalPosition = vec4(0.0f);
    for(int i = 0 ; i <= MAX_BONE_INFLUENCE ; i++)
    {
        if(aBoneIds[i] == -1) 
            continue;
        if(aBoneIds[i] >= MAX_BONES) 
        {
            totalPosition = vec4(aPos, 1.0f);
            break;
        }
        vec4 localPosition = finalBonesMatrices[aBoneIds[i]] * vec4(aPos, 1.0f);
        totalPosition += localPosition * aWeights[i];
        // vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * aNormal;
    }
		
    gl_Position = projection * view * model * totalPosition;
    TexCoords = aTexCoords;
}

// void main()
// {
//     mat4 BoneTransform  = finalBonesMatrices[aBoneIds[0]] * aWeights[0];
//          BoneTransform += finalBonesMatrices[aBoneIds[1]] * aWeights[1];
//          BoneTransform += finalBonesMatrices[aBoneIds[2]] * aWeights[2];
//          BoneTransform += finalBonesMatrices[aBoneIds[3]] * aWeights[3];
    
//     vec4 PosL = BoneTransform * vec4(aPos, 1.0f);
//     gl_Position = projection * view * model * PosL;
//     TexCoords = aTexCoords;
// }