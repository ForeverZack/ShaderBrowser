// shadow caster
// 顶点着色其中需要加上[ShadowCaster]来告诉引擎这个pass可以当做shadow caster使用
#ifdef OPEN_SHADOW_CASTER
	// 为了使同一个份着色器代码能作为shadow caster的着色器代码使用，这里直接用宏定义了PrePass的处理函数：直接返回
	// ，不再执行原始着色器代码复杂的计算（注意调用它们的位置，位置合理可以减少不必要的计算）
	#define TRANSFER_SHADOW_CASTER_NORMALOFFSET(v2f) \
				return;
	#define SHADOW_CASTER_FRAGMENT(o)	\
				return;
#else
	#define TRANSFER_SHADOW_CASTER_NORMALOFFSET() 
	#define SHADOW_CASTER_FRAGMENT() 
#endif


// shadow receiver
