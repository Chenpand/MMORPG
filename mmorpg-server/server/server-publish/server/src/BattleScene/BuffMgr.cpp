#include "BuffMgr.h"

#include <CLDungeonDataEntry.h>
#include "Creature.h"
#include "Player.h"

Buff::Buff()
{
	m_uid = 0;
	m_id = 0;
	m_overlayNums = 1;
	m_dataEntry = NULL;
	m_duration = 0;;
	m_dirty = false;
	m_OpActBuffFlag = false;
}

Buff::~Buff()
{
}


void Buff::SetOverlay(UInt32 num)
{
	m_dirty = true;  
	if (num > BUFF_OVERLAY_MAX)
	{
		m_overlayNums = BUFF_OVERLAY_MAX;
	}
	else
	{
		m_overlayNums = num;
	}
}

bool Buff::DecreaseEffectTimes(UInt32 num)
{
	if (num == 0 || m_EffectTimes < num)
	{
		return false;
	}

	m_EffectTimes -= num;
	m_dirty = true;

	return true;
}

void Buff::DecodeString(std::istringstream& is)
{
	char split;
	is >> m_uid;
	if (m_uid != 0)
	{
		UInt64 startTime;
		is >> split >> m_id >> split >> m_overlayNums >> split >> startTime >> split >> m_duration >> split >> m_EffectTimes >> split;
		m_startTime = Avalon::Time(startTime);
	}
}

void Buff::EncodeString(std::ostringstream& os)
{
	os << m_uid << "," << m_id << "," << m_overlayNums << "," << m_startTime.MSec() << "," << m_duration << "," << m_EffectTimes << ";";

	SetDirty(false);
}

const std::string Buff::LogStr() const
{
	std::ostringstream oss;
	oss << "Buff(" << m_uid << ", " << m_id << ")'s sT=" << m_startTime.MSec() << ", duration=" << m_duration << ", effectTimes=" << m_EffectTimes;
	return oss.str();
}



BuffMgr::BuffMgr()
{
	m_pOwner = NULL;
	m_Buffs.clear();
	m_Timer.SetInterval(1000);	//1秒定时
}

BuffMgr::~BuffMgr()
{
	for (auto buffer : m_Buffs)
	{
		CL_SAFE_DELETE(buffer);
	}
	m_Buffs.clear();
}

void BuffMgr::SetOwner(Creature *owner)
{
	m_pOwner = owner;
}

Buff* BuffMgr::FindBuff(UInt32 buffId)
{
	for (int i = 0; i < (int)m_Buffs.size(); ++i)
	{
		if (buffId == m_Buffs[i]->GetDataEntry()->id)
		{
			return m_Buffs[i];
		}
	}

	return NULL;
}

Buff* BuffMgr::CreateBuff(UInt32 buffid)
{
	BuffDataEntry* buffData = BuffDataEntryMgr::Instance()->FindEntry(buffid);
	if (!buffData)
		return NULL;

	Buff* buff = new Buff;
	buff->SetId(buffData->id);
	buff->SetDataEntry(buffData);
	buff->SetUid(CLRecordClient::Instance()->GenGuid());
	buff->SetStartTime();
	buff->SetDuration(buffData->duration);
	buff->SetEffectTimes(buffData->effectTimes);

	InfoLogStream << PLAYERINFO(GetOwner()) << " create " << buff->LogStr() << LogStream::eos;

	SetDirty();

	return buff;
}

bool BuffMgr::AddBuff(UInt32 buffid)
{
	if ((UInt32)m_Buffs.size() >= MAX_UUFF_NUM)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " buff size reach max(" << MAX_UUFF_NUM << ")!" << LogStream::eos;
		return false;
	}

	Buff* buff = FindBuff(buffid);
	if (!buff)
	{
		buff = CreateBuff(buffid);
		if (!buff)
			return false;

		m_Buffs.push_back(buff);
		SetDirty();
		SyncClient();
	}
	else
	{
		switch (buff->GetDataEntry()->overLay)
		{
		case BOT_NEW_ONE:
		{
			Buff* buff = CreateBuff(buffid);
			if (!buff)
				return false;

			m_Buffs.push_back(buff);
			SetDirty();
			SyncClient();
			break;
		}
		case BOT_VALUE_OL:
		{
			if (buff->GetDataEntry()->overLayLimit > 0 && buff->GetOverlay() >= buff->GetDataEntry()->overLayLimit)
				buff->SetOverlay(buff->GetDataEntry()->overLayLimit);
			else
				buff->SetOverlay(buff->GetOverlay() + 1);

			SyncClient();
			SetDirty();
			break;
		}
		case BOT_TIME_OL:
		{
			buff->SetDuration(buff->GetDuration() + buff->GetDataEntry()->duration);
			SetDirty();
			SyncClient();
			break;
		}
		case BOT_CORVER:
		{
			buff->SetStartTime();
			buff->SetDuration(buff->GetDataEntry()->duration);

			SetDirty();
			SyncClient();
			break;
		}
		case BOT_CAN_NOT_OL:
		{
			return false;
		}
		default:return false;
		}
	}

	// 增加buff效果
	_AddBuffEffect(buffid);

	return true;
}

void BuffMgr::VisitBuffs(BuffVisitor& visitor)
{
	for (size_t i = 0; i < m_Buffs.size(); ++i)
	{
		if (m_Buffs[i] != NULL)
		{
			if (!visitor(m_Buffs[i])) return;
		}
	}
}

void BuffMgr::GetBuffs(std::vector<Buff*>& buffs, BuffType buffType)
{
	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	for (auto buff : m_Buffs)
	{
		if (!buff || !buff->GetDataEntry()) continue;

		UInt32 buffStartTime = (UInt32)buff->GetStartTime().Sec();

		if (buffStartTime + buff->GetDuration() < curTime)
		{
			continue;
		}

		if (buff->GetDataEntry()->buffType == buffType)
		{
			buffs.push_back(buff);
		}
	}
}

void BuffMgr::OnTick(const Avalon::Time& now)
{
	//1秒间隔到
	if(m_Timer.IsTimeout(now))
	{
		UInt32 curTime = (UInt32)now.Sec();

		std::vector<Buff*>::iterator itr = m_Buffs.begin();
		while ( itr != m_Buffs.end() )
		{
			Buff* buff = *itr;
			if (buff->GetDataEntry()->durationType == BDT_TIME)
			{
				if (0 == buff->GetDuration())
				{
					_NotifyRemoveBuff(buff->GetId());

					DebugLogStream << PLAYERINFO(GetOwner()) << "delete tick " << buff->LogStr() << LogStream::eos;

					//buff到时清除
					delete buff;
					itr = m_Buffs.erase(itr);
					SetDirty();
					_RemoveBuffEffect(buff->GetId());
					SyncClient();
					continue;
				}

				buff->SetDuration(buff->GetDuration() - 1);
			}
			else if (buff->GetDataEntry()->durationType == BDT_DEADLINE_CALC)
			{
				// 该持续时长类型需必须与生效次数同时配置
				UInt32 deadline = buff->GetStartTimeSec() + buff->GetDuration();
				if (curTime >= deadline)
				{
					buff->SetDuration(0);

					_NotifyRemoveBuff(buff->GetId());

					DebugLogStream << PLAYERINFO(GetOwner()) << "delete tick2 " << buff->LogStr() << LogStream::eos;

					delete buff;
					itr = m_Buffs.erase(itr);
					SetDirty();
					_RemoveBuffEffect(buff->GetId());
					SyncClient();
					continue;
				}
			}
			else if (buff->GetDataEntry()->durationType == BDT_DEADLINE_TIME_CONTINUE_PVP)
			{
				UInt32 deadline = buff->GetStartTimeSec() + buff->GetDuration();
				if (curTime >= deadline && ((Player*)GetOwner())->GetStatus() != CREATURE_STATUS_PK)
				{
					buff->SetDuration(0);

					_NotifyRemoveBuff(buff->GetId());

					DebugLogStream << PLAYERINFO(GetOwner()) << "delete tick3 " << buff->LogStr() << LogStream::eos;

					delete buff;
					itr = m_Buffs.erase(itr);
					SetDirty();
					_RemoveBuffEffect(buff->GetId());
					SyncClient();
					continue;
				}
			}

			if (buff->GetDataEntry()->effectTimes > 0 && buff->GetEffectTimes() == 0)
			{
				_NotifyRemoveBuff(buff->GetId());

				DebugLogStream << PLAYERINFO(GetOwner()) << "delete tick4 " << buff->LogStr() << LogStream::eos;

				delete buff;
				itr = m_Buffs.erase(itr);
				SetDirty();
				_RemoveBuffEffect(buff->GetId());
				SyncClient();
				continue;
			}

			++itr;
		}
	}
}
#if 0
bool BuffMgr::GetDungeonBufferAddition(UInt8& expRate, UInt8& goldRate, UInt8& taskDropNumRate, UInt16& pinkDropRate)
{
	for (size_t i = 0; i < m_Buffs.size(); ++i)
	{
		Buff* buff = m_Buffs[i];
		BuffDataEntry* buffData = buff->GetDataEntry();
		UInt8 durationType = buffData->durationType;
		UInt32 rate = 0;
		switch (buffData->buffType)
		{
			case BT_EXP_ADD:
			{
				rate = buffData->expAddRate.GetValue() * buff->GetOverlay();
				if (rate >= 100)	expRate = 100;
				else				expRate = rate;
				
                break;
			}
			case BT_GOLD_ADD:
			{
				rate = buffData->goldAddRate.GetValue() * buff->GetOverlay();
				if (rate >= 100)	goldRate = 100;
				else				goldRate = rate;

                break;
			}
			case BT_TASK_DROP_NUM:
			{
				rate = buffData->taskDropNumRate * buff->GetOverlay();
				if (rate >= 100)	taskDropNumRate = 100;
				else				taskDropNumRate = rate;

                break;
			}
			case BT_PINK_DROP:
			{		
				rate = buffData->pinkDropRate * buff->GetOverlay();
				if (rate >= 100)	pinkDropRate = 100;
				else				pinkDropRate = rate;

                break;
			}
			default:
            {
                continue;
            }
		}
	}


	

	return false;
}
#endif
bool BuffMgr::GetDungeonBufferAddition(DungeonAddition& addition)
{
	for (size_t i = 0; i < m_Buffs.size(); ++i)
	{
		Buff* buff = m_Buffs[i];
		BuffDataEntry* buffData = buff->GetDataEntry();
		//UInt8 durationType = buffData->durationType;
		UInt32 rate = 0;
		switch (buffData->buffType)
		{
		case BT_EXP_ADD:
		{
			rate = buffData->expAddRate.GetValue() * buff->GetOverlay();
			if (rate >= 100)	addition.additionRate[DART_BUFF_EXP_RATE] = 100;
			else				addition.additionRate[DART_BUFF_EXP_RATE] = rate;

			break;
		}
		case BT_GOLD_ADD:
		{
							rate = buffData->goldAddRate.GetValue() * buff->GetOverlay();
			if (rate >= 100)	addition.additionRate[DART_GOLD_RATE] = 100;
			else				addition.additionRate[DART_GOLD_RATE] = rate;

			break;
		}
		case BT_TASK_DROP_NUM:
		{
								 rate = buffData->taskDropNumRate * buff->GetOverlay();
			if (rate >= 100)	addition.additionRate[DART_TASK_DROP_NUM_RATE] = 100;
			else				addition.additionRate[DART_TASK_DROP_NUM_RATE] = rate;

			break;
		}
		case BT_PINK_DROP:
		{
							 rate = buffData->pinkDropRate * buff->GetOverlay();
			if (rate >= 100)	addition.additionRate[DART_PINK_DROP_RATE] = 100;
			else				addition.additionRate[DART_PINK_DROP_RATE] = rate;

			break;
		}
		default: continue;
		}
	}

	return false;
}

void BuffMgr::OnUseDungeonBuffers(const DungeonDataEntry* dungeonData)
{
	if (!dungeonData) return;

	for (std::vector<Buff*>::iterator itr = m_Buffs.begin();
		itr != m_Buffs.end();)
	{
		Buff* buff = *itr;
		std::vector<Buff*>::iterator tmpItr = itr;
		++itr;
		if (!buff)	continue;

		auto buffData = buff->GetDataEntry();
		if (!buffData)
		{
			continue;
		}
		
		UInt8 durationType = buff->GetDataEntry()->durationType;

		if (_IsApplyDungeonForBuff(dungeonData, buffData))
		{
			if (buff->DecreaseEffectTimes(1))
			{
				InfoLogStream << PLAYERINFO(GetOwner()) << " decrease " << buff->LogStr() << " effect times in dungeon(" << dungeonData->id << ")." << LogStream::eos;

				SetDirty();
			}
		}

		DebugLogStream << PLAYERINFO(GetOwner()) << "OnUseDungeonBuffers() buffid = " << buff->GetId() << " durationType = " <<
			durationType << " duration = " << buff->GetDuration() << ", effect times=" << buff->GetEffectTimes() << LogStream::eos;

		if (durationType == BDT_DUNGEON)
		{
			buff->SetDuration(buff->GetDuration() - 1);

			if (0 == buff->GetDuration())
			{
				_NotifyRemoveBuff(buff->GetId());

				delete buff;
				itr = m_Buffs.erase(tmpItr);

				continue;
			}

			SyncClient();
		}
	}
}

void BuffMgr::GetDungeonRaceBuffInfo(DungeonRacePlayerInfo& dungeonRaceInfo, const DungeonDataEntry* dungeonData)
{
	if (!dungeonData) return;

	dungeonRaceInfo.raceInfo.buffs.clear();

	for (auto buff : m_Buffs)
	{
		if (!buff) continue;

		auto buffData = buff->GetDataEntry();
		if (!buffData)
		{
			continue;
		}

		if (!_IsApplyDungeonForBuff(dungeonData, buffData))
		{
			continue;
		}

		// 添加战斗buff
		RaceBuffInfo raceBuff;
		raceBuff.m_id = buff->GetId();
		raceBuff.m_overlayNums = buff->GetOverlay();
		raceBuff.m_startTime = buff->GetStartTime().MSec();
		raceBuff.m_duration = buff->GetDuration();
		dungeonRaceInfo.raceInfo.buffs.push_back(raceBuff);
	}
}

void BuffMgr::AddOpActBuff(UInt32 buffid, UInt32 duration)
{
	if ((UInt32)m_Buffs.size() >= MAX_UUFF_NUM)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << "Buff size reach max while add buff(" << buffid << ")!" << LogStream::eos;
		return;
	}

	Buff* buff = FindBuff(buffid);
	if (!buff)
	{
		buff = CreateBuff(buffid);
		if (!buff)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << "Create buff(" << buffid << ") failed!" << LogStream::eos;
			return;
		}

		m_Buffs.push_back(buff);
	}

	buff->SetOpActBuffFlag(true);
	buff->SetDuration(duration);

	SyncClient();

	InfoLogStream << PLAYERINFO(GetOwner()) << "Add OpActBuff(" << buffid << "), duration(" << duration << ")." << LogStream::eos;
}

bool BuffMgr::RemoveBuff(UInt32 buffid)
{
	for (auto iter = m_Buffs.begin(); iter != m_Buffs.end(); ++iter)
	{
		auto buff = *iter;
		if (!buff) continue;

		if (buff->GetId() == buffid)
		{
			_NotifyRemoveBuff(buff->GetId());

			InfoLogStream << PLAYERINFO(GetOwner()) << "Remove OpActBuff(" << buffid << ")." << LogStream::eos;

			CL_SAFE_DELETE(buff);
			m_Buffs.erase(iter);
			
			SetDirty();
			SyncClient();
			return true;
		}
	}

	return false;
}

void BuffMgr::ClearBuff()
{
	for (auto& iter : m_Buffs)
	{
		if (NULL == iter) 
			continue;

		CL_SAFE_DELETE(iter);
	}

	m_Buffs.clear();
	SetDirty();
}

void BuffMgr::GetExpBuffAddition(DungeonAddition& addition)
{
	for (auto buff : m_Buffs)
	{
		if (!buff) continue;

		BuffDataEntry* buffData = buff->GetDataEntry();
		if (!buffData)
		{
			ErrorLogStream << PLAYERINFO(GetOwner()) << " buff(" << buff->GetId() << ")'s data entry is null!" << LogStream::eos;
			continue;
		}

		if (buffData->buffType != BT_EXP_ADD_ACTIVITY_AND_COMMON)
		{
			continue;
		}

		UInt32 expAdd = buffData->expAddRate.GetValue();

		addition.additionRate[DART_EXP_RATE] += expAdd;
		addition.additionRate[DART_BUFF_EXP_RATE] += expAdd;

		InfoLogStream << PLAYERINFO(GetOwner()) << " add buff(" << buff->GetId() << ")'s addition, exp add(" << expAdd << ")." << LogStream::eos;
	}
}

Int32 BuffMgr::GetBuffAddition(BuffType buffType)
{
	Int32 val = 0;
	for (auto& iter : m_Buffs)
	{
		if (iter == NULL)
			continue;

		if (buffType != iter->GetDataEntry()->buffType)
			continue;

		switch (buffType)
		{
		case BT_SPEED_ADD:
		{
			val += iter->GetDataEntry()->param1;
			break;
		}
		default:
			break;
		}
	}

	return val;
}

bool BuffMgr::_IsApplyDungeonForBuff(const DungeonDataEntry* dungeonData, const BuffDataEntry* buffData)
{
	if (!dungeonData || !buffData)
	{
		return false;
	}

	if (buffData->applyDungeons.empty())
	{
		return true;
	}

	if (std::find(buffData->applyDungeons.begin(), buffData->applyDungeons.end(), (UInt32)dungeonData->subType) != buffData->applyDungeons.end())
	{
		return true;
	}

	return false;
}

void BuffMgr::_NotifyRemoveBuff(UInt32 buffId)
{
	CLProtocol::SceneNotifyRemoveBuff protocol;
	protocol.buffId = buffId;
	((Player*)m_pOwner)->SendProtocol(protocol);
}

void BuffMgr::_AddBuffEffect(UInt32 buffId)
{
	BuffDataEntry* buffData = BuffDataEntryMgr::Instance()->FindEntry(buffId);
	if (NULL == buffData)
	{
		ErrorLogStream << "Not Find Buff Data BuffId:" << buffId << LogStream::eos;
		return;
	}

	switch (buffData->buffType)
	{
	case BT_SPEED_ADD:
	{
		Player* player = dynamic_cast<Player*>(m_pOwner);
		if (NULL != player)
			player->CountMoveSpeed();

		break;
	}
	case BT_PROPERTY_OP:
	{
		Player* player = dynamic_cast<Player*>(m_pOwner);
		if (NULL != player)
		{
			if (buffData->param1 != 0)
			{
				// param1>0说明是加血，加血是加绝对值
				if (buffData->param1 > 0)
				{
					player->IncChijiHp(buffData->param1);
					player->IncLostDungeonHp(buffData->param1);
				}
				else
				{
					// 扣血是按当前百分比扣血
					Int32 val = player->GetChijiHp() * (float(buffData->param1) / HP_MP_FULL_VALUE);
					player->DecChijiHp(abs(val));
					player->DecLostDungeonHp(val);
				}

				player->SyncProperty();
			}

			if (buffData->param2 != 0)
			{
				player->IncChijiMp(buffData->param2);
				player->IncLostDungeonMp(buffData->param2);
				player->SyncProperty();
			}
		}

		break;
	}
	default:
		break;
	}
}

void BuffMgr::_RemoveBuffEffect(UInt32 buffId)
{
	BuffDataEntry* buffData = BuffDataEntryMgr::Instance()->FindEntry(buffId);
	if (NULL == buffData)
	{
		ErrorLogStream << "Not Find Buff Data BuffId:" << buffId << LogStream::eos;
		return;
	}

	switch (buffData->buffType)
	{
	case BT_SPEED_ADD:
	{
		Player* player = (Player*)m_pOwner;
		if (NULL != player)
			player->CountMoveSpeed();

		break;
	}
	default:
		break;
	}
}

void BuffMgr::Input(Avalon::NetInputStream& stream)
{
	Decode(stream);
}

void BuffMgr::Output(Avalon::NetOutputStream& stream)
{
	Encode(stream, 0, false);
}

void BuffMgr::Encode(Avalon::NetOutputStream& stream, UInt32 type, bool bDirty)
{
	for (size_t i = 0; i < m_Buffs.size(); ++i)
	{
		Buff* buff = m_Buffs[i];
		if (buff)
		{
			buff->Output(stream);
		}
	}
	stream & UInt32(0);
}

void BuffMgr::Decode(Avalon::NetInputStream& stream)
{
	bool bEnd = false;
	do
	{
		Buff* buff = new Buff;
		buff->Input(stream);

		BuffDataEntry* data = BuffDataEntryMgr::Instance()->FindEntry(buff->GetId());
		if (!data)
		{
			delete buff;
			continue;
		}

		buff->SetDataEntry(data);

		if (0 == buff->GetId())
		{
			delete buff;
			bEnd = true;
		}
		else
		{
			m_Buffs.push_back(buff);
		}
	} while (false == bEnd);
}

void BuffMgr::DecodeString(std::istringstream& is)
{
	UInt32 curTime = (UInt32)CURRENT_TIME.Sec();

	bool bEnd = false;
	do 
	{
		Buff* buff = new Buff;
		buff->DecodeString(is);

		if (0 == buff->GetUid())
		{
			delete buff;
			bEnd = true;
		}
		else
		{
			BuffDataEntry* data = BuffDataEntryMgr::Instance()->FindEntry(buff->GetId());
			if (!data)
			{
				delete buff;
				continue;
			}

			buff->SetDataEntry(data);

			if (buff->GetDataEntry()->durationType == BDT_DEADLINE_CALC)
			{
				UInt32 deadline = buff->GetStartTimeSec() + buff->GetDuration();;
				if (deadline >= curTime)
				{
					UInt32 restTime = deadline - curTime;
					buff->SetDuration(restTime);
				}
				else
				{
					buff->SetDuration(0);
				}

				buff->SetStartTime();
			}

			m_Buffs.push_back(buff);
		}
	} while (false == bEnd);
}

void BuffMgr::EncodeString(std::ostringstream& os)
{
	for (size_t i = 0; i < m_Buffs.size(); ++i)
	{
		Buff* buff = m_Buffs[i];
		if (!buff)
		{
			continue;
		}

		auto buffData = buff->GetDataEntry();
		if (!buffData)
		{
			continue;
		}

		// 只存有生效次数限制的buff(由于现有buff机制存储在player上，有存量限制)
		if (buffData->effectTimes == 0)
		{
			continue;
		}

		if (buff->GetOpActBuffFlag())
		{
			// 活动buff不存储
			buff->SetDirty(false);
			continue;
		}

		buff->EncodeString(os);
	}
	os << 0;

	ClearDirty(DB_SYNC_TYPE);

	UInt32 buffSize = os.str().size();

	static UInt32 BuffMaxSize = 512;

	if (buffSize >= BuffMaxSize)
	{
		ErrorLogStream << PLAYERINFO(GetOwner()) << " buff size reach max limit(" << BuffMaxSize << ")!" << LogStream::eos;
		os.str("");
	}
}

void BuffMgr::SyncClient()
{
	m_pOwner->SyncProperty();
}

