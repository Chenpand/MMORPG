#ifndef _CL_ACTIVITY_ATTRIBUTE_DEFINE_H_
#define _CL_ACTIVITY_ATTRIBUTE_DEFINE_H_

//神器罐子抽取次数
#define ACT_ATTR_ARTIFACT_JAR_EXTRACT_TIMES "af_jar_ex_ts"
// 神器罐子折扣率
#define ACT_ATTR_ARTIFACT_JAR_DISCOUNT_RATE "af_jar_disc_r"
// 神器罐子折扣可用次数
#define ACT_ATTR_ARTIFACT_JAR_DISCOUNT_EFFECT_TIMES "ar_jar_disc_et"
// 神器罐子折扣计算基值
#define ACT_ATTR_ARTIFACT_JAR_DISCOUNT_CALC_BASE_VALUE 10

// 神器罐子折扣抽取状态
enum ArtifactJarDiscountExtractStatus
{
	// 不可抽取
	AJDES_INVALID = 0,
	// 可抽取
	AJDES_IN,
	// 已抽取
	AJDES_OVER,
};

#endif