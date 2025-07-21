#ifndef _CL_ACTIVITY_ATTRIBUTE_DEFINE_H_
#define _CL_ACTIVITY_ATTRIBUTE_DEFINE_H_

//�������ӳ�ȡ����
#define ACT_ATTR_ARTIFACT_JAR_EXTRACT_TIMES "af_jar_ex_ts"
// ���������ۿ���
#define ACT_ATTR_ARTIFACT_JAR_DISCOUNT_RATE "af_jar_disc_r"
// ���������ۿۿ��ô���
#define ACT_ATTR_ARTIFACT_JAR_DISCOUNT_EFFECT_TIMES "ar_jar_disc_et"
// ���������ۿۼ����ֵ
#define ACT_ATTR_ARTIFACT_JAR_DISCOUNT_CALC_BASE_VALUE 10

// ���������ۿ۳�ȡ״̬
enum ArtifactJarDiscountExtractStatus
{
	// ���ɳ�ȡ
	AJDES_INVALID = 0,
	// �ɳ�ȡ
	AJDES_IN,
	// �ѳ�ȡ
	AJDES_OVER,
};

#endif