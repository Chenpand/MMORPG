#include "ASExternalNetwork.h"

#include <CLLoginProtocol.h>
#include <CLErrorCode.h>

#include <AvalonSha1.h>
#include <AvalonStringUtil.h>
#include <AvalonMd5.h>

#include "ASApplication.h"

ASExternalConnection::ASExternalConnection(CLNetwork* network)
	:CLExternalConnection(network)
{
	m_VerifyTime = 0;
	m_AccId = 0;
	m_HashTimestamp = 0;
	m_GmAuthority = 0;
	m_PhoneBindRoleId = 0;
	m_IsIDAuth = 0;
	m_AddictSecTime = 0;
	m_Age = 0;
	m_LastLoginTime = 0;
}

ASExternalConnection::~ASExternalConnection()
{
}

bool ASExternalConnection::PreHandlePacket(Avalon::Packet *packet)
{
	if(CLExternalConnection::PreHandlePacket(packet)) return true;

	switch(packet->GetID())
	{
	case CLProtocol::ADMIN_LOGIN_VERIFY_REQ:
		{
			if(IsVerified()) return true;

			CLProtocol::AdminLoginVerifyReq req;
			if(!req.Decode(packet))
			{
				DebugLogStream << "conn(" << packet->GetConnID() << ") decode failed." << LogStream::eos;
				CLProtocol::AdminLoginVerifyRet ret;
				ret.result = ErrorCode::LOGIN_ERROR_VERSION;
				SendProtocol(ret);
				Close();
				return true;
			}

			if (ASApplication::Instance()->CheckTableMD5())
			{
				bool isMatch = false;
				do 
				{
					{
						std::string srcString = ASApplication::Instance()->GetTableMD5() + ASApplication::Instance()->GetSkillMD5() + req.params;
						UInt8 md5[16];
						Avalon::md5_encode((char*)md5, (void*)srcString.c_str(), srcString.length());

						bool matchMd5 = true;
						for (size_t i = 0; i < sizeof(md5); i++)
						{
							if (md5[i] != req.tableMd5[i])
							{
								matchMd5 = false;
							}
						}

						if (matchMd5 == true)
						{
							isMatch = true;
							break;
						}
					}
					
					const auto& md5Infoes = ASApplication::Instance()->GetClientMD5Infoes();
					for (const auto& info : md5Infoes)
					{
						std::string srcString = info.tableMd5 + info.skillMd5 + req.params;
						UInt8 md5[16];
						Avalon::md5_encode((char*)md5, (void*)srcString.c_str(), srcString.length());

						bool matchMd5 = true;
						for (size_t i = 0; i < sizeof(md5); i++)
						{
							if (md5[i] != req.tableMd5[i])
							{
								matchMd5 = false;
							}
						}

						if (matchMd5 == true)
						{
							isMatch = true;
							break;
						}
					}
				} while (0);

				if (isMatch == false)
				{
					DebugLogStream << "conn(" << packet->GetConnID() << ") invalid md5." << LogStream::eos;
					CLProtocol::AdminLoginVerifyRet ret;
					ret.result = ErrorCode::LOGIN_ERROR_VERSION;
					SendProtocol(ret);
					Close();
					return true;
				}

				
			}
			
#ifndef _DEBUG
            if (!ASApplication::Instance()->CheckVersion(req.version))
            {
				DebugLogStream << "conn(" << packet->GetConnID() << ") error version(" << req.version << ")." << LogStream::eos;
                CLProtocol::AdminLoginVerifyRet ret;
                ret.result = ErrorCode::LOGIN_ERROR_VERSION;
                SendProtocol(ret);
                Close();
                return true;
            }
#endif

			std::string key1 = ASApplication::Instance()->GetCryptKey1();  //此key均是从配置文件读取的
			std::string key2 = ASApplication::Instance()->GetCryptKey2();
			if(!VerifyAccount(req.hashValue, req.params, key1, key2)
				|| req.source1.length() > 64 || req.source2.length() > 64)
			{
				ErrorLogStream << "verified failed:" << req.params << "!" << LogStream::eos;
				
				CLProtocol::AdminLoginVerifyRet ret;
				ret.result = ErrorCode::LOGIN_UNKNOW_ACCOUNT;
				SendProtocol(ret);
				Close();
				return true;
			}

			// 看看是不是限制登录
			if (ASApplication::Instance()->IsForbidLogin(m_AccId))
			{
				DebugLogStream << "conn(" << packet->GetConnID() << ") account(" << m_AccId << ") forbid login." << LogStream::eos;
				CLProtocol::AdminLoginVerifyRet ret;
				ret.result = ErrorCode::LOGIN_SERVER_UNREADY;
				SendProtocol(ret);
				Close();
				return true;
			}
			
			if(!GetNetwork()->AddVerifiedConnection(m_AccId, this))
			{
				DebugLogStream << "conn(" << packet->GetConnID() << ") add verified connection failed." << LogStream::eos;

				CLProtocol::AdminLoginVerifyRet ret;
				ret.result = ErrorCode::LOGIN_REPEAT;
				SendProtocol(ret);
				Close();
				return true;
			}
			
			CLProtocol::AdminLoginNewPlayer notify;
			notify.accid = m_AccId;
			notify.gmauthority = m_GmAuthority;
			memmove(notify.hash, req.hashValue, 20);
			notify.openid = m_OpenId;
			notify.qqVipInfo = m_qqVipInfo;
			notify.source1 = req.source1;
			notify.source2 = req.source2;
			notify.pf = m_Pf;
			notify.did = m_DeviceId;
			notify.deviceVersion = m_DeviceVersion;
			notify.model = m_Model;
			notify.phoneBindRoleId = m_PhoneBindRoleId;
			notify.antiAddictInfo.Init(m_AddictSecTime, m_Age, m_IsIDAuth, m_LastLoginTime);
			Avalon::Packet* packet = Avalon::Packet::Create();
			if(!notify.Encode(packet))
			{
				DebugLogStream << "conn(" << packet->GetConnID() << ") encode packet failed." << LogStream::eos;
				Avalon::Packet::Destroy(packet);
				return true;
			}

			DebugLogStream << "account(" << m_AccId << "IsIDAuth:" << m_IsIDAuth << " Age:" << m_Age 
				<< " AddictSecTime:" << m_AddictSecTime << " LastLoginTime:" << m_LastLoginTime 
				<< " phoneBind:" << m_PhoneBindRoleId<< LogStream::eos;

			ASExternalNetwork::Instance()->DispatchPacket(packet);

			m_VerifyTime = UInt32(CURRENT_TIME.Sec());
		}
		return true;
	}

	if(CL_IS_SYS_PROTOCOL(packet->GetID())) return true;

	return false;
}

void ASExternalConnection::OnTick(const Avalon::Time& now)
{
	CLExternalConnection::OnTick(now);

	if(IsVerified() && now.Sec() > m_VerifyTime + 10)
	{
		Close();
	}
}

//noted by aprilliu, 根据服务器端的app key, app secrect (从配置文件读取的分节【Crypt】中crypt_key1， crypt_key2)
bool ASExternalConnection::VerifyAccount(const UInt8* _hashval, const std::string& uparams, const std::string& key1, const std::string& key2)
{
	if (uparams.length() > ASApplication::Instance()->GetAccVerifyParamLength())
	{
		ErrorLogStream << "uparams too long!" << uparams << LogStream::eos;
		return false;
	}

	std::vector<std::string> paramstrs;
	Avalon::StringUtil::Split(uparams, paramstrs, "&");

	std::string timestr;
	for(std::vector<std::string>::iterator iter = paramstrs.begin();
		iter != paramstrs.end(); ++iter)
	{
		std::string::size_type index = iter->find_first_of('=');
		if (index == std::string::npos)
		{
			ErrorLogStream << "uparams format error!" << uparams << LogStream::eos;
			return false;
		}
		if(index + 1 >= iter->length()) continue;

		std::string key = iter->substr(0, index);
		if(key.compare("openid") == 0)
		{
			m_OpenId = iter->substr(index + 1);
		}
		else if(key.compare("userid") == 0) //从客户端传来？怎么生成的？
		{
			m_AccId = Avalon::StringUtil::ConvertToValue<UInt32>(iter->substr(index+1).c_str());
		}
		else if(key.compare("isvip") == 0)
		{
			if(iter->at(index + 1) == '1') m_qqVipInfo.SetIsVip();
		}
		else if(key.compare("isyearvip") == 0)
		{
			if(iter->at(index + 1) == '1') m_qqVipInfo.SetYearVip();
		}
		else if(key.compare("viplv") == 0)
		{
			UInt8 vipLvl = Avalon::StringUtil::ConvertToValue<UInt8>(iter->substr(index+1).c_str());
			if (vipLvl > 8)
			{
				ErrorLogStream << "uparams vip error!" << uparams << LogStream::eos;
				return false;
			}
			m_qqVipInfo.level = vipLvl;
		}
		else if(key.compare("t") == 0) 
		{
			timestr = iter->substr(index+1);
			m_HashTimestamp = Avalon::StringUtil::ConvertToValue<UInt32>(timestr.c_str()); //含时间戳？
		}
		else if(key.compare("qq3366_level") == 0)
		{
			m_qqVipInfo.lv3366 = Avalon::StringUtil::ConvertToValue<UInt8>(iter->substr(index+1).c_str());
		}
		else if(key.compare("is_super_blue_vip") == 0)
		{
			if(iter->at(index + 1) == '1') m_qqVipInfo.SetSuperBlue();
		}
		else if(key.compare("is_yellow_high_vip") == 0)
		{
			if(iter->at(index + 1) == '1') m_qqVipInfo.SetSuperYellow();
		}
		else if(key.compare("isblue") == 0)
		{
			if(iter->at(index + 1) != '0') m_qqVipInfo.SetIsBlue();
		}
		else if(key.compare("pf") == 0)
		{
			m_Pf = iter->substr(index + 1);
		}
		else if (key.compare("did") == 0)
		{
			m_DeviceId = iter->substr(index + 1);
		}
		else if (key.compare("phone_bind_role") == 0)
		{
			m_PhoneBindRoleId = Avalon::StringUtil::ConvertToValue<ObjID_t>(iter->substr(index + 1));
		}
		else if (key.compare("is_id_auth") == 0)
		{
			m_IsIDAuth = Avalon::StringUtil::ConvertToValue<UInt32>(iter->substr(index + 1));
		}
		else if (key.compare("addict_time") == 0)
		{
			m_AddictSecTime = Avalon::StringUtil::ConvertToValue<UInt32>(iter->substr(index + 1));
		}
		else if (key.compare("age") == 0)
		{
			m_Age = Avalon::StringUtil::ConvertToValue<UInt8>(iter->substr(index + 1));
		}
		else if (key.compare("last_login_time") == 0)
		{
			m_LastLoginTime = Avalon::StringUtil::ConvertToValue<UInt32>(iter->substr(index + 1));
		}
		else if (key.compare("model") == 0)
		{
			m_Model = iter->substr(index + 1);
		}
		else if (key.compare("device_version") == 0)
		{
			m_DeviceVersion = iter->substr(index + 1);
		}
	}

	if (m_AccId == 0)
	{
		ErrorLogStream << "uparams accid error!" << uparams << LogStream::eos;
		return false;
	}

	if(key1.length() == 0 && key2.length() == 0)
	{
		m_GmAuthority = 30;
		return true;
	}

#ifndef DISABLE_CHECK_LOGIN
	if (m_OpenId.empty())
	{
		ErrorLogStream << "uparams openid error!" << uparams << LogStream::eos;
		return false;
	}
	UInt32 now = UInt32(Avalon::Time::CurrentTime().Sec());
	if (m_HashTimestamp < now - HASH_VALID_TIME || m_HashTimestamp > now + HASH_VALID_TIME)
	{
		ErrorLogStream << "uparams time out error!" << uparams << LogStream::eos;
		return false;
	}

	std::string keyseed = timestr + key1 + key2;
	char key[33];
	Avalon::md5_encode32(key, (void*)(keyseed.c_str()), keyseed.length());
	key[32] = 0;
	std::string sha1data = std::string(key) + uparams;

	Avalon::SHA1Engine sha1;

	sha1.update(sha1data);

	if(memcmp(&sha1.digest().front(), _hashval, 20) == 0)
		return true;
#else
	if (m_Pf == "")
	{
		m_Pf = "test";
	}
	
	if (m_OpenId == "")
	{
		m_OpenId = Avalon::StringUtil::ConvertToString(m_AccId);
	}
	
	return true;
#endif

	return false;
}

ASExternalNetwork::ASExternalNetwork()
{
}

ASExternalNetwork::~ASExternalNetwork()
{
}

Avalon::NetConnection* ASExternalNetwork::CreateConnection()
{
	return new ASExternalConnection(this);
}

void ASExternalNetwork::OnConnected(UInt32 id)
{
}

void ASExternalNetwork::OnDisconnected(UInt32 id)
{
}
