// 骨骼信息 boneInfo: 0.当前骨骼id  1.父节点骨骼id（-1表示没有父节点）
layout (location=0) in ivec2 boneInfo;
out vec4 result_position;
out vec4 result_rotation;
out vec4 result_scale;


// 最大关键帧帧数
const int MAX_KEY_FRAME_COUNT = 200;
// identity matrix
const mat4 mat4_identity = mat4(1, 0, 0, 0,
                                0, 1, 0, 0,
                                0, 0, 1, 0,
                                0, 0, 0, 1);

// 以下为该模型所有动画共用数据(该类数据只要设置一次即可)：
// 骨骼数据（0.当前骨骼id  1.父节点骨骼）
uniform ivec2 bonesInfo[MAX_BONES];

// 非共用数据(切换动画需要重新设置)：
// 骨骼变换关键帧信息 （注意！！这里的信息，一定要按骨骼id来排序！！！！！！！！！！！！！！！！！！！！！！！）
uniform samplerBuffer position_keys; //0~2:position 3:time
uniform samplerBuffer rotation_keys; // quaternion
uniform float rotation_times[MAX_KEY_FRAME_COUNT]; // 关键帧时间 长度和rotation_keys相同
uniform samplerBuffer scale_keys; //0~2:scale 3:time
// 上面的骨骼变换信息包含哪些骨骼 (id：boneId ; value: -1.不包含，即该骨骼节点没有发生变换 1~MAX_BONES.在所有发生变换的骨骼信息里排第几个(用来定位采样关键帧))
uniform int contains_bones[MAX_BONES];  // 例如：[-1, 0, -1, -1, 1] 表示该动画只有boneId为1，4的骨骼发生了变换，采样位置为0，1。
// 每个发生变换的骨骼有多少关键帧 (用上面contains_bones的value定位，0.position关键帧数量 1.rotation关键帧数量 2.scale关键帧数量)
uniform ivec3 trans_bone_keyframe_count[MAX_BONES];
// 动画数据(0:第几帧(小数), 1:)
uniform vec4 animation_info;
// 是否进行线性插值(0:不使用插值，直接用前一帧的数据 1:插值)
uniform int interpolate = 1;



// 函数声明(跟c一样，glsl的函数声明需要放在最前面，才不会因为定义的先后顺序影响使用)
int convertSamplerStartUV(int boneId, int trans_type);
void samplerKeyPairs(int boneId, float time, out vec4 leftFrame, out vec4 rightFrame, int trans_type, samplerBuffer buffer);
void samplerKeyPairs(int boneId, float time, out vec4 leftFrame, out vec4 rightFrame, out float leftTime, out float rightTime);
void computeBoneMatrix(int boneId, out vec4 position_out, out vec4 rotation_out, out vec4 scale_out);
void getLocalBoneMatrix(int boneId, out vec4 position_out, out vec4 rotation_out, out vec4 scale_out);



void main() 
{
    // test
    getLocalBoneMatrix(boneInfo[0], result_position, result_rotation, result_scale);
//    result_position = vec4(boneInfo[0], boneInfo[1], 0, 0);
//    result_rotation = vec4(0, 0, 0, 0);
//    result_scale = vec4(0, 0, 0, 0);
}

// 插值
vec4 lerp(vec4 a, vec4 b, float percent)
{
    return a + (b-a)*percent;
}

// 计算采样起始位置  trans_type: 0.position 1.rotation 2.scale
int convertSamplerStartUV(int boneId, int trans_type)
{
    int uv = 0;
    int index = 0;
    // 计算当前骨骼id之前的骨骼变换数据量的大小
    for (int i=0; i<boneId; ++i)
    {
        index = contains_bones[i];
        if (index != -1)
        {
            uv = uv + trans_bone_keyframe_count[trans_type][trans_type];
        }
    }
    return uv;
}

// 采样数据
void samplerKeyPairs(int boneId, float time, out vec4 leftFrame, out vec4 rightFrame, int trans_type, samplerBuffer buffer)
{
    int startUV = convertSamplerStartUV(boneId, trans_type);
    int index = contains_bones[boneId];
    int count = trans_bone_keyframe_count[index][trans_type];
    
    // 只有一个关键帧
    if (count == 1)
    {
        leftFrame = rightFrame = texelFetch(buffer, startUV);
        return;
    }
    
    // 时间超过最后一个关键帧时间，则停止在最后一个关键帧
    vec4 keyValue = texelFetch(buffer, startUV+count-1);
    if (keyValue[3] < time)
    {
        leftFrame = rightFrame = keyValue;
        return;
    }
    
    // 检测关键帧位置
    for (int i=0; i<count; ++i)
    {
        keyValue = texelFetch(buffer, startUV+i);
        if(keyValue[3] >= time)
        {
            leftFrame = texelFetch(buffer, startUV+i-1);
            rightFrame = keyValue;
            break;
        }
    }
}
void samplerKeyPairs(int boneId, float time, out vec4 leftFrame, out vec4 rightFrame, out float leftTime, out float rightTime)
{
    int startUV = convertSamplerStartUV(boneId, 0);
    int index = contains_bones[boneId];
    int count = trans_bone_keyframe_count[index][1];
    
    // 只有一个关键帧
    if (count == 1)
    {
        leftFrame = rightFrame = texelFetch(rotation_keys, startUV);
        leftTime = rightTime = rotation_times[startUV];
        return;
    }
    
    // 时间超过最后一个关键帧时间，则停止在最后一个关键帧
    vec4 keyValue = texelFetch(rotation_keys, startUV+count-1);
    float keyTime = rotation_times[startUV+count-1];
    if (keyTime < time)
    {
        leftFrame = rightFrame = keyValue;
        leftTime = rightTime = rotation_times[startUV+count-1];
        return;
    }
    
    // 检测关键帧位置
    for (int i=0; i<count; ++i)
    {
        keyValue = texelFetch(rotation_keys, startUV+i);
        keyTime = rotation_times[startUV+i];
        if(keyTime >= time)
        {
            leftFrame = texelFetch(rotation_keys, startUV+i-1);
            leftTime = rotation_times[startUV+i-1];
            rightFrame = keyValue;
            rightTime = rotation_times[startUV+i];
            break;
        }
    }
}

// 计算骨骼矩阵(这个方法默认boneId的骨骼发生了变换)
void computeBoneMatrix(int boneId, out vec4 position_out, out vec4 rotation_out, out vec4 scale_out)
{
    float time = animation_info[0];
    
    vec4 leftFrame[3];
    vec4 rightFrame[3];
    float leftTime, rightTime;
    
    // position
    samplerKeyPairs(boneId, time, leftFrame[0], rightFrame[0], 0, position_keys);
    // rotation
    samplerKeyPairs(boneId, time, leftFrame[1], rightFrame[1], leftTime, rightTime);
    // scale
    samplerKeyPairs(boneId, time, leftFrame[2], rightFrame[2], 2, scale_keys);
    
    
    vec4 position, rotation, scale;
    float percent;
    // position
    if (leftFrame[0]==rightFrame[0] || interpolate!=1)
    {
        position = leftFrame[0];
    }
    else
    {
        percent = (time-leftFrame[0][3]) / (rightFrame[0][3] - leftFrame[0][3]);
        position = lerp(leftFrame[0], rightFrame[0], percent);
    }
    // rotation
    if (leftFrame[1]==rightFrame[1] || interpolate!=1)
    {
        rotation = leftFrame[1];
    }
    else
    {
        percent = (time-leftTime) / (rightTime - leftTime);
        rotation = lerp(leftFrame[1], rightFrame[1], percent);
    }
    // scale
    if (leftFrame[2]==rightFrame[2] || interpolate!=1)
    {
        scale = leftFrame[2];
    }
    else
    {
        percent = (time-leftFrame[2][3]) / (rightFrame[2][3] - leftFrame[2][3]);
        scale = lerp(leftFrame[2], rightFrame[2], percent);
    }
   
    
    // test
    position_out = position;
    rotation_out = rotation;
    scale_out = scale;
}

// 根据骨骼id得到骨骼自身的变换矩阵
void getLocalBoneMatrix(int boneId, out vec4 position_out, out vec4 rotation_out, out vec4 scale_out)
{
    if (contains_bones[boneId] != -1)
    {
        computeBoneMatrix(boneId, position_out, rotation_out, scale_out);
        
        position_out = vec4(1, 2, 3, 4);
        rotation_out = vec4(0, 0, 0, 0);
        scale_out = vec4(1, 1, 1, 0);
    }
    else
    {
        // wrong!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 应该传入node->mTransformation(应该是bindpose)
        position_out = vec4(0, 0, 0, 0);
        rotation_out = vec4(0, 0, 0, 0);
        scale_out = vec4(2, 2, 2, 0);
    }
}
