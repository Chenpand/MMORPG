#ifndef _ESCORT_SCRIPT_H_
#define _ESCORT_SCRIPT_H_

#include <AvalonScriptFile.h>
#include <CLGameDefine.h>

/**
 *@brief ���ͽű�����
 */
class EscortScript : public Avalon::ScriptFile
{
public:
	EscortScript(Avalon::ScriptState* state);
	~EscortScript();

public:
	/**
	 *@brief ��ʼ��
	 */
	void Init();

public:

	/**
	 *@brief ���ø�Ʒ���³�ʼˢ�¸���
	 */
	void SetRefreshRate(int quality, UInt16 greenRate, UInt16 blueRate, UInt16 purpleRate, UInt16 goldRate);

	/**
	 *@brief ���ø�Ʒ���µ���ˢ�¸���
	 */
	void SetIncRefreshRate(int quality, UInt16 greenRate, UInt16 blueRate, UInt16 purpleRate, UInt16 goldRate);

public:
	/**
	 *@brief ��ȡ��ǰƷ����ˢ�¼���
	 */
	void GetRefreshRate(int quality, UInt16 rates[5]);

	/**
	 *@brief ��ȡ��ǰƷ���µ�������
	 */
	void GetIncRefreshRate(int quality, UInt16 rates[5]);

private:
	//��Ʒ���³�ʼˢ�¸���(��ֱ�)
	UInt16		m_RefreshRates[5][5];
	//��Ʒ����ÿ��ˢ�����Ӹ���(��ֱ�)
	UInt16		m_IncRates[5][5];
};

#endif
