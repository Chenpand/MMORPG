#ifndef _SS_FIGURE_STATUE_H_
#define _SS_FIGURE_STATUE_H_

#include <CLFigureStatueDefine.h>

class SSFigureStatueMgr : public Avalon::Singleton<SSFigureStatueMgr>
{
public:
	/**
	*@brief ���û�ȡ�������
	*/
	void SetFigureStatue(const FigureStatueVec& statues);
	const FigureStatueVec& GetFigureStatues() { return m_FigureStatues; }

	bool IsFigureStatue(FigureStatueType type, UInt64 playerId);

	void GetFigureStatuesById(std::vector<FigureStatueType>& figureStatueList, UInt64 playerId);

private:
	// �������
	FigureStatueVec m_FigureStatues;
};

#endif