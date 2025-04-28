// #version 330 core
#version 460

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;
layout(location = 3) in ivec4 aBoneIds; 
layout(location = 4) in vec4 aWeights;
	
const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
const float EPSILON = 0.0001;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 finalBonesMatrices[MAX_BONES];

out vec2 TexCoords;
out vec3 Normal;
	
    
bool approx_v2(vec2 lhs, vec2 rhs)
{
   return dot(lhs - rhs, lhs - rhs) <= EPSILON * EPSILON;
}
 
bool approx_v4(vec4 lhs, vec4 rhs)
{
   return all(lessThan(abs(lhs - rhs), vec4(EPSILON)));
}

bool approx_f(float lhs, float rhs) {
    return (lhs - rhs) * (lhs - rhs) < EPSILON * EPSILON;
}

void main()
{
    vec4 totalPosition = vec4(0.0f);
    for(int i = 0 ; i < MAX_BONE_INFLUENCE ; i++)
    {
        // this check doesnt change anything
        if(aBoneIds[i] == -1) {
            continue;
        }
        
        // without this not animatied models are not displayed
        if(aBoneIds[i] >= MAX_BONES) {
            totalPosition = vec4(aPos, 1.0f);
            break;
        }
        
        vec4 localPosition = finalBonesMatrices[aBoneIds[i]] * vec4(aPos, 1.0f);
        totalPosition += localPosition * aWeights[i];
        // vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * aNormal;
    }
	
    gl_Position = projection * view * model * totalPosition;
    TexCoords = aTexCoords;
    Normal = aNormal;
    
    // this err happens
    // bool err = approx_v4(gl_Position, vec4(0.0f));
    // if (err) {
    //     gl_Position = vec4(-0.5f, 0.5f, -10.0f, 1.0f);
    // }
}

// void main()
// {
//     mat4 mfinal = view * model;
    
//     vec4 weights = vec4(aWeights[0], aWeights[1], aWeights[2], aWeights[3]);
    
//     float weight_sum = weights.x + weights.y + weights.z + weights.w;
//     if (!approx_f(weight_sum, 1.0f)) {
//         weights = vec4(1.0f, 0.0f, 0.0f, 0.0f);
//     } 
//     else 
//     {
//         bool too_many = aBoneIds[3] > MAX_BONES; // why this works?
//         if (!too_many) {
//             mat4 mskin  = finalBonesMatrices[aBoneIds[0]] * weights[0]  //aWeights[0]
//                         + finalBonesMatrices[aBoneIds[1]] * weights[1]  //aWeights[1]
//                         + finalBonesMatrices[aBoneIds[2]] * weights[2]  //aWeights[2]
//                         + finalBonesMatrices[aBoneIds[3]] * weights[3]; //aWeights[3];
//             mfinal *= mskin;
//         }
//     }
    
//     gl_Position = projection * mfinal * vec4(aPos, 1.0f);
//     TexCoords = aTexCoords;
//     Normal = aNormal;
// }
