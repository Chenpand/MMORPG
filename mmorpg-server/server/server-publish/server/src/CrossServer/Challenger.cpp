#include "Challenger.h"
#include "CSApplication.h"

#include <sstream>

Challenger::Challenger()
{
	m_GateId = 0;

	m_Level = 0;
	m_Occu = 0;
	m_Score = 0;
	m_ZoneId = 0;
	m_Power = 0;
	m_Group = 0;
	m_AwardFlag = 0;
	
	m_pPkData = NULL;
	m_PkDataLen = 0;
}

Challenger::~Challenger()
{
	delete[] m_pPkData;
	m_pPkData = NULL;
}

void Challenger::InsertToDB()
{
	Avalon::RetryDBConnection* conn = CSApplication::Instance()->GetDBConnection();

	std::ostringstream oss;
	oss << "INSERT INTO `t_worldchallenger`(`guid`,`name`,`level`,`occu`,`score`,`zoneId`,`power`,`group`,`awardFlag`) VALUES("
		<< GetID() << ",'" << GetName() << "'," << m_Level << "," << int(m_Occu) << "," << m_Score << "," << m_ZoneId << "," << m_Power << "," << int(m_Group)
		<< "," << m_AwardFlag << ")";
	if(conn->Update(oss.str()) < 0)
	{
		ErrorLogStream << "sql error:" << conn->GetError() << LogStream::eos;
	}
}

void Challenger::SaveMe()
{
	Avalon::RetryDBConnection* conn = CSApplication::Instance()->GetDBConnection();
	
	std::ostringstream oss;
	oss << "UPDATE `t_worldchallenger` SET ";
	oss << "`level`=" << m_Level << ",";
	oss << "`occu`=" << int(m_Occu) << ",";
	oss << "`score`=" << m_Score << ",";
	oss << "`zoneId`=" << m_ZoneId << ",";
	oss << "`power`=" << m_Power << "," ;
	oss << "`awardFlag`=" << m_AwardFlag;
	oss << " WHERE `guid`=" << GetID();
	if(conn->Update(oss.str()) < 0)
	{
		ErrorLogStream << "sql error:" << conn->GetError() << LogStream::eos;
	}
}

void Challenger::SetPkData(const UInt8* pkdata, size_t pkdatalen)
{
	if(pkdata == NULL || pkdatalen == 0) return;

	delete[] m_pPkData;
	m_pPkData = new UInt8[pkdatalen];
	memmove(m_pPkData, pkdata, pkdatalen);
	m_PkDataLen = pkdatalen;
}

