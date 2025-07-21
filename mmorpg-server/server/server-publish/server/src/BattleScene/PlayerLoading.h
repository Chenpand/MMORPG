#pragma once

#include "Player.h"
#include "PlayerMgr.h"
#include "SSApplication.h"
#include <CLRecordCallback.h>
#include <AsyncMsgCallback.h>
#include <CLEnterGameProtocol.h>

/**
*@brief �����������ݻص�
*/
class SelectRoleInfoCallback : public CLRecordCallback
{
public:
    SelectRoleInfoCallback(Player* player)
        :m_PlayerId(player->GetID()), m_LoadSerial(player->IncLoadDataCount()) {
    }
    ~SelectRoleInfoCallback() {}

public:
    virtual void OnFinished(Player* player) = 0;
    virtual void OnFailed(Player* player) = 0;

private:

    void OnFinished()
    {
        Player* player = GetPlayer();
        if (player == NULL) return;

        OnFinished(player);
        player->SetLoadDataFinished(m_LoadSerial);
    }

    void OnLoadFailed()
    {
        Player* player = GetPlayer();
        if (player == NULL) return;

        OnFailed(player);
		player->LoadingFailed();
    }

    Player* GetPlayer()
    {
        Player* player = PlayerMgr::Instance()->FindPlayer(m_PlayerId);
        if (player == NULL || player->GetGameStatus() != PS_INIT
            || !player->CheckLoadDataSerial(m_LoadSerial)) return NULL;
        return player;
    }

private:
    //���id
    ObjID_t m_PlayerId;
    //�������
    UInt8	m_LoadSerial;
};

//��ѯ�̵�ص�
class SelectShopCallback : public SelectRoleInfoCallback
{
    AVALON_DEFINE_OBJECTPOOL(SelectShopCallback, Avalon::Mutex)
public:
    SelectShopCallback(Player* player)
        :SelectRoleInfoCallback(player) {}
    ~SelectShopCallback() {}

public:
    void OnFinished(Player* player)
    {
        while (NextRow())
        {
            Shop* shop = Shop::Create(GetData("shopid"));
            if (shop == NULL) continue;
            shop->SetDBData(this);
            shop->SetID(GetKey());
            player->GetShopMgr().LoadShop(shop);
        }
    }

    void OnFailed(Player* player)
    {
        GameErrorLogStream("Player") << "player(" << player->GetAccID() << "," << player->GetID()
            << "," << player->GetName() << ") select shop info failed." << LogStream::eos;
    }

};

//��ѯ��Ʒ�ص�
class SelectShopItemCallback : public SelectRoleInfoCallback
{
    AVALON_DEFINE_OBJECTPOOL(SelectShopItemCallback, Avalon::Mutex)
public:
    SelectShopItemCallback(Player* player)
        :SelectRoleInfoCallback(player) {}
    ~SelectShopItemCallback() {}

public:
    void OnFinished(Player* player)
    {
        while (NextRow())
        {
            ShopItem* shopItem = ShopItem::Create(GetData("shopitemid"));
            if (shopItem == NULL) continue;
            shopItem->SetDBData(this);
            shopItem->SetID(GetKey());
            player->GetShopMgr().LoadShopItem(shopItem);
        }
    }

    void OnFailed(Player* player)
    {
        GameErrorLogStream("Player") << "player(" << player->GetAccID() << "," << player->GetID()
            << "," << player->GetName() << ") select shopitem info failed." << LogStream::eos;
    }

};

/**
*@brief ��ѯ���³ǻص�
*/
class SelectDungeonsCallback : public SelectRoleInfoCallback
{
    AVALON_DEFINE_OBJECTPOOL(SelectDungeonsCallback, Avalon::Mutex)
public:
    SelectDungeonsCallback(Player* player)
        :SelectRoleInfoCallback(player) {}
    ~SelectDungeonsCallback() {}

public:
    void OnFinished(Player* player)
    {
        while (NextRow())
        {
            Dungeon* dungeon = Dungeon::CreateDungeon(GetData("dungeon_id"));
            if (dungeon == NULL) continue;
            dungeon->SetDBData(this);
            dungeon->SetID(GetKey());
            player->GetDungeonMgr().LoadDungeon(dungeon);
        }
    }

    void OnFailed(Player* player)
    {
        GameErrorLogStream("Player") << "player(" << player->GetAccID() << "," << player->GetID()
            << "," << player->GetName() << ") select dungeon info failed." << LogStream::eos;
    }
};

/**
*@brief ��ѯ���³��Ѷ���Ϣ�ص�
*/
class SelectDungeonsHardInfoCallback : public SelectRoleInfoCallback
{
    AVALON_DEFINE_OBJECTPOOL(SelectDungeonsCallback, Avalon::Mutex)
public:
    SelectDungeonsHardInfoCallback(Player* player)
        :SelectRoleInfoCallback(player) {}
    ~SelectDungeonsHardInfoCallback() {}

public:
    void OnFinished(Player* player)
    {
        while (NextRow())
        {
            DungeonHardInfo* hard = DungeonHardInfo::CreateDungeonHardInfo(GetData("dungeon_id"));
            if (hard == NULL) continue;
            hard->SetDBData(this);
            hard->SetID(GetKey());
            player->GetDungeonMgr().LoadDungeonHardInfo(hard);
        }
    }

    void OnFailed(Player* player)
    {
        GameErrorLogStream("Player") << "player(" << player->GetAccID() << "," << player->GetID()
            << "," << player->GetName() << ") select dungeon hard info failed." << LogStream::eos;
    }
};

/**
*@brief ��ѯ���߻ص�
*/
class SelectItemsCallback : public SelectRoleInfoCallback
{
    AVALON_DEFINE_OBJECTPOOL(SelectItemsCallback, Avalon::Mutex)
public:
    SelectItemsCallback(Player* player)
        :SelectRoleInfoCallback(player) {}
    ~SelectItemsCallback() {}

public:
    void OnFinished(Player* player)
    {
        std::vector<Item*> items;

		UInt32 num = 0;
        while (NextRow())
        {
			num++;
            Item* item = Item::CreateItem(GetData("itemid"));
            if (item == NULL) continue;
            item->SetDBData(this);
            item->SetID(GetKey());
            if (!player->LoadItem(item, false))
            {
				ErrorLogStream << PLAYERINFO(player) << " loadItem error! dataId:" << item->GetDataID() \
					<< " uid:" << item->GetID() << " pack:" << item->GetPos().pack << " grid:" << item->GetPos().gridIndex << LogStream::eos;
                items.push_back(item);
                continue;
            }
        }

        for (std::vector<Item*>::iterator iter = items.begin();
            iter != items.end(); ++iter)
        {
            Item* item = *iter;
			if (!player->LoadItem(item, true))
            {
                GameErrorLogStream("Player") << "player(" << player->GetAccID() << "," << player->GetID()
                    << "," << player->GetName() << ") load item(" << item->GetDataID() << "," << item->GetNum() << ") failed!";
                Item::DestroyItem(item);
            }
        }
    }

    void OnFailed(Player* player)
    {
        GameErrorLogStream("Player") << "player(" << player->GetAccID() << "," << player->GetID()
            << "," << player->GetName() << ") select item info failed." << LogStream::eos;
    }
};

/**
*@brief ��ѯ���ӽ��ػص�
*/
class SelectJarPoolCallback : public SelectRoleInfoCallback
{
    AVALON_DEFINE_OBJECTPOOL(SelectJarPoolCallback, Avalon::Mutex)
public:
    SelectJarPoolCallback(Player* player)
        :SelectRoleInfoCallback(player) {}
    ~SelectJarPoolCallback() {}

public:
    void OnFinished(Player* player)
    {
        std::vector<JarItem*> jarItems;

        while (NextRow())
        {
            JarItem* jarItem = new JarItem();
            jarItem->SetDBData(this);
            jarItem->SetID(GetKey());

            if (!player->LoadJarItem(jarItem))
            {
                jarItems.push_back(jarItem);
                continue;
            }
        }

        for (std::vector<JarItem*>::iterator iter = jarItems.begin();
            iter != jarItems.end(); ++iter)
        {
            JarItem* jarItem = *iter;

           // ErrorLogStream << PLAYERINFO(player) << "load jaritem err! uid = " << jarItem->GetID() << LogStream::eos;
            CL_SAFE_DELETE(jarItem);
        }
    }

    void OnFailed(Player* player)
    {
        ErrorLogStream << PLAYERINFO(player) << "select jaritem info failed!" << LogStream::eos;
    }
};

/**
*@brief ������Ӫ�����ص�
*/
class SelectOpTaskCallback : public SelectRoleInfoCallback
{
	AVALON_DEFINE_OBJECTPOOL(SelectOpTaskCallback, Avalon::Mutex)
public:
	SelectOpTaskCallback(Player* player)
		:SelectRoleInfoCallback(player) {}
	~SelectOpTaskCallback() {}

public:
	void OnFinished(Player* player)
	{
		while (NextRow())
		{

		}
	}

	void OnFailed(Player* player)
	{
		GameErrorLogStream("Player") << "player(" << player->GetAccID() << "," << player->GetID()
			<< "," << player->GetName() << ") select operate task failed." << LogStream::eos;
	}
};

/**
*@brief ��������Ӫ�����ص�
*/
class SelectNewOpTaskCallback : public SelectRoleInfoCallback
{
	AVALON_DEFINE_OBJECTPOOL(SelectNewOpTaskCallback, Avalon::Mutex)
public:
	SelectNewOpTaskCallback(Player* player)
		:SelectRoleInfoCallback(player) {}
	~SelectNewOpTaskCallback() {}

public:
	void OnFinished(Player* player)
	{

	}

	void OnFailed(Player* player)
	{
		GameErrorLogStream("Player") << "player(" << player->GetAccID() << "," << player->GetID()
			<< "," << player->GetName() << ") select new operate task failed." << LogStream::eos;
	}
};

/**
*@brief ��ѯPkͳ�ƻص�
*/
class SelectPkStatisticCallback : public SelectRoleInfoCallback
{
    AVALON_DEFINE_OBJECTPOOL(SelectPkStatisticCallback, Avalon::Mutex)
public:
    SelectPkStatisticCallback(Player* player)
        :SelectRoleInfoCallback(player) {}
    ~SelectPkStatisticCallback() {}

public:
    void OnFinished(Player* player)
    {
        while (NextRow())
        {
            PkStatistic *info = PkStatistic::CreatePkStatistic((PkType)(UInt8)GetData("type"));
            info->SetDBData(this);
            info->SetID(GetKey());

            player->GetPkStatisticMgr().LoadPkStatistic(info);
        }
    }

    void OnFailed(Player* player)
    {
        GameErrorLogStream("Player") << "player(" << player->GetAccID() << "," << player->GetID()
            << "," << player->GetName() << ") select pk statistic failed." << LogStream::eos;
    }
};

class SelectTaskCallback : public SelectRoleInfoCallback
{
    AVALON_DEFINE_OBJECTPOOL(SelectTaskCallback, Avalon::Mutex)
public:
    SelectTaskCallback(Player* player) : SelectRoleInfoCallback(player) {}
    ~SelectTaskCallback() {}

public:
    void OnFinished(Player* player)
    {
        while (NextRow())
        {
            UInt64 guid = GetKey();
            UInt32 dataId = GetData("dataid");
            TaskStatus status = (TaskStatus)(UInt32)GetData("status");
			std::string parameter = GetData("parameter").GetString();
			UInt32 finishedTime = GetData("finished_time");
			UInt8 submitCount = GetData("submit_count");
            player->LoadTask(guid, dataId, status, parameter, finishedTime, submitCount);
        }

		//player->OnLoadTaskFinished();

    }

    void OnFailed(Player* player)
    {
        GameErrorLogStream("Player") << "player(" << player->GetAccID() << "," << player->GetID()
            << "," << player->GetName() << ") select task failed." << LogStream::eos;
    }

};

//��ѯ�����
class SelectActiveTaskCallback : public SelectRoleInfoCallback
{
    AVALON_DEFINE_OBJECTPOOL(SelectActiveTaskCallback, Avalon::Mutex)
public:
    SelectActiveTaskCallback(Player* player) : SelectRoleInfoCallback(player) {}
    ~SelectActiveTaskCallback() {}

public:
    void OnFinished(Player* player)
    {
    }

    void OnFailed(Player* player)
    {
    }
};

/**
*@brief ��ѯ��ӻص�
*/
class SelectRetinueCallback : public SelectRoleInfoCallback
{
    AVALON_DEFINE_OBJECTPOOL(SelectRetinueCallback, Avalon::Mutex)
public:
    SelectRetinueCallback(Player* player) :SelectRoleInfoCallback(player) {}
    ~SelectRetinueCallback() {}

public:
    void OnFinished(Player* player)
    {
        player->GetRetinueMgr().OnSelectDataRet(this);
    }

    void OnFailed(Player* player)
    {
        GameErrorLogStream("Player") << "player(" << player->GetAccID() << "," << player->GetID()
            << "," << player->GetName() << ") select retinue failed." << LogStream::eos;
    }
};

/**
*@brief ��ѯ����ص�
*/
class SelectPetCallback : public SelectRoleInfoCallback
{
	AVALON_DEFINE_OBJECTPOOL(SelectPetCallback, Avalon::Mutex)
public:
	SelectPetCallback(Player* player) :SelectRoleInfoCallback(player) {}
	~SelectPetCallback(){}

public:
	void OnFinished(Player* player)
	{
		player->GetPetMgr().OnSelectDataRet(this);
	}

	void OnFailed(Player* player)
	{
		GameErrorLogStream("Player") << "player(" << player->GetAccID() << "," << player->GetID()
			<< "," << player->GetName() << ") select pet failed." << LogStream::eos;
	}
};

// ��ѯ�ֿ�ص�
class SelectStorageCallback : public SelectRoleInfoCallback
{
    AVALON_DEFINE_OBJECTPOOL(SelectStorageCallback, Avalon::Mutex)
public:
    SelectStorageCallback(Player* player)
        :SelectRoleInfoCallback(player) {
		
	}
    ~SelectStorageCallback() {}

public:
    void OnFinished(Player* player)
    {
        std::vector<Item*> items;

		player->GetItemMgr().ClearMailGroup();

        while (NextRow())
        {
			
            Item* item = Item::CreateItem(GetData("itemid"));
            if (item == NULL) continue;
            item->SetDBData(this);
            item->SetID(GetKey());

			if (!player->GetItemMgr().LoadStorageItem(item, false))
            {
				ErrorLogStream << PLAYERINFO(player) << "load storage item fail, push to ReAddList! uid:" << item->GetID() << " dataid:" << item->GetDataID()
					<< " num:" << item->GetNum() << LogStream::eos;

				items.push_back(item);
                continue;
            }
        }

		//����ʧ��,������λ��
		for (std::vector<Item*>::iterator iter = items.begin();
			iter != items.end(); ++iter)
		{
			Item* item = *iter;


			if (!player->GetItemMgr().LoadStorageItem(item, true))
			{
				ErrorLogStream << PLAYERINFO(player) << "ReAdd load storage item fail, uid:" << item->GetID() << " dataid:" << item->GetDataID()
					<< " num:" << item->GetNum() << LogStream::eos;

				Item::DestroyItem(item);
			}
		}
    }

    void OnFailed(Player* player)
    {
        ErrorLogStream << "player(" << player->GetAccID() << "," << player->GetID()
            << "," << player->GetName() << ") select storage item info failed." << LogStream::eos;
    }

};

/**
*@brief ��ѯ�����ص�
*/
class SelectCountersCallback : public SelectRoleInfoCallback
{
    AVALON_DEFINE_OBJECTPOOL(SelectCountersCallback, Avalon::Mutex)
public:
    SelectCountersCallback(Player* player)
        :SelectRoleInfoCallback(player) {}
    ~SelectCountersCallback() {}

public:
    void OnFinished(Player* player)
    {
        player->GetCounterMgr().OnSelectDataRet(this);
    }

    void OnFailed(Player* player)
    {
        GameErrorLogStream("Player") << "player(" << player->GetAccID() << "," << player->GetID()
            << "," << player->GetName() << ") select counters failed." << LogStream::eos;
    }
};

/**
*@brief ��ѯ��һ������ݻص�
*/
class SelectActGiftBagCallback : public SelectRoleInfoCallback
{
    AVALON_DEFINE_OBJECTPOOL(SelectActGiftBagCallback, Avalon::Mutex)
public:
    SelectActGiftBagCallback(Player *pPlayer)
        : SelectRoleInfoCallback(pPlayer)
    {
    }
    ~SelectActGiftBagCallback()
    {
    }

public:
    void OnFinished(Player *pPlayer)
    {

    }

    void OnFailed(Player *pPlayer)
    {
        GameErrorLogStream("Player") << "player(" << pPlayer->GetAccID() << "," << pPlayer->GetID()
            << ")(" << pPlayer->GetName() << ") select activity giftbag info failed." << LogStream::eos;
    }
};

/**
*@brief ��ѯ��һ������ݻص�
*/
class SelectMainDataCallback : public SelectRoleInfoCallback
{
    AVALON_DEFINE_OBJECTPOOL(SelectMainDataCallback, Avalon::Mutex)
public:
    SelectMainDataCallback(Player* player)
        :SelectRoleInfoCallback(player) {}
    ~SelectMainDataCallback() {}

public:
    void OnFinished(Player* player)
    {
        if (!NextRow())
        {
            OnFailed(player);
            return;
        }

		player->SetID(GetKey());
        UInt64 saveTime = GetData("savetime");
		std::string platform = player->GetPf();
		std::string openid = player->GetOpenId();
        if (saveTime > CURRENT_TIME.MSec()) saveTime = CURRENT_TIME.MSec();
        player->SetSavebackTime(saveTime);	//����ʱ�ϴδ浵ʱ�伴����ʱ��
		player->SetTotalOnlineTime((UInt32)GetData("totleonlinetime"));

        UInt64 localSaveTime = GetData("localsavetime");
        if (localSaveTime > CURRENT_TIME.MSec()) localSaveTime = CURRENT_TIME.MSec();
        player->SetLocalSavebackTime(localSaveTime);

        UInt32 sceneId = player->GetSceneID();
        CLPosition pos = player->GetPos();
        player->SetDBData(this);
        player->SetSceneID(sceneId);
        player->SetPos(pos);
		//player->InitPackage();
		player->ParseGameSet();

		if (player->GetPf() != platform && platform != "")
		{
			player->SetPf(platform);

			auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", player->GetID(), true);
			cmd->PutData("platform", player->GetPf());
			CLRecordClient::Instance()->SendCommand(cmd);
		}

		if (player->GetOpenId() != openid && openid != "")
		{
			player->SetOpenId(openid);

			auto cmd = CLRecordClient::Instance()->PrepareUpdateCommand("t_player_info", player->GetID(),true);
			cmd->PutData("openid", player->GetOpenId());
			CLRecordClient::Instance()->SendCommand(cmd);
		}

		player->ClearPfDirtyFlag();
		player->ClearOpenIdDirtyFlag();
    }

    void OnFailed(Player* player)
    {
        GameErrorLogStream("Player") << "player(" << player->GetAccID() << "," << player->GetID() << ","
            << player->GetName() << ") select main data failed." << LogStream::eos;
    }
};

template<class WaitMessage>
class SelectWSDataCallback : public AsyncMsgCallback
{
	AVALON_DEFINE_OBJECTPOOL(SelectWSDataCallback, Avalon::NullMutex)
public:
	SelectWSDataCallback(Player* player, UInt32 timeout = 60)
		: AsyncMsgCallback(new WaitMessage()) 
	{
		SetTimeOut(timeout);
		m_PlayerId = player->GetID();
		m_LoadSerial = player->IncLoadDataCount();
	}

	virtual ~SelectWSDataCallback() {}

public:
	void OnSuccess(Avalon::Protocol* msg)
	{
		WaitMessage* waitMsg = (WaitMessage*)msg;
		if (!waitMsg)
		{
			OnFailed(MCE_DECODE_MSG_FAILED);
			return;
		}

		auto player = PlayerMgr::Instance()->FindPlayer(m_PlayerId);
		if (!player)
		{
			ErrorLogStream << "query account return, player(" << m_PlayerId << ") offline." << LogStream::eos;
			return;
		}

		DebugLogStream << "player(" << m_PlayerId << ") load(" << m_LoadSerial << ") success." << LogStream::eos;

		OnFinish(player, *waitMsg);

		player->SetLoadDataFinished(m_LoadSerial);
	}

	void OnFailed(MsgCallbackError errorCode)
	{
		auto player = PlayerMgr::Instance()->FindPlayer(m_PlayerId);
		if (!player)
		{
			ErrorLogStream << "query account failed(" << (UInt8)errorCode << "), player(" << m_PlayerId << ") offline." << LogStream::eos;
			return;
		}

		GameErrorLogStream("Player") << "player(" << player->GetAccID() << "," << player->GetID() << ","
			<< player->GetName() << ") load data(" << m_LoadSerial << ") failed." << LogStream::eos;

		OnFailed(player, errorCode);

		player->LoadingFailed();
	}

public:
	virtual void OnFinish(Player* player, const WaitMessage& msg) = 0;
	virtual void OnFailed(Player* player, MsgCallbackError errorCode) {}

protected:
	ObjID_t			m_PlayerId;
	UInt8			m_LoadSerial;
};

/**
*@brief ��ѯ�˺����ݻص�
*/
class SelectAccountDataCallback : public SelectWSDataCallback<CLProtocol::WorldQueryAccountRet>
{
    AVALON_DEFINE_OBJECTPOOL(SelectMainDataCallback, Avalon::NullMutex)
public:
	SelectAccountDataCallback(Player* player)
		: SelectWSDataCallback(player) {
		
	}

    ~SelectAccountDataCallback() {}

public:
	void OnFinish(Player* player, const CLProtocol::WorldQueryAccountRet& res)
    {
		if (res.result)
		{
			OnFailed((MsgCallbackError)res.result);
			return;
		}
		player->SetAccGuid(res.accountGuid);
		player->SetAccID(res.accountId);
		player->InitVip(res.vipLevel, res.vipExp);
		player->InitPoint(res.point);
		player->InitTotalChargeNum(res.totalChargeNum);
		player->InitMoneyManage(res.moneyManageStatus);
		player->InitWeaponLeaseTickets(res.weaponLeaseTickets);
		//player->InitMonthCardExpireTime(res.monthCardExpireTime);
		player->SetAdventureTeamLevel(res.adventureTeamLevel);
		player->SetAdventureTeamGrade(res.adventureTeamGrade);

		// ���³���Ϣ
		player->GetDungeonMgr().ClearDungeonHardInfo();
		for (auto& info : res.dungeonHardInfoes)
		{
			DungeonHardInfo* hard = DungeonHardInfo::CreateDungeonHardInfo(info.id);
			if (hard == NULL) continue;
			hard->SetDungeonID(info.id);
			hard->SetUnlockedHardType((DungeonHardType)info.unlockedHardType);
			player->GetDungeonMgr().LoadDungeonHardInfo(hard);
		}

		player->InitStoragePack(res.storageSize);

		// ���»�ȡ�˺Ųֿ�����
		player->LoadAccountStorage();
		
		DebugLogStream << "player(" << player->GetID() << ", " << player->GetAccID() << ") vip(" << player->GetVipLvl() << ", "
			<< player->GetVipExp() << ") point(" << player->GetPoint() << ") totalCharge(" << res.totalChargeNum << ") storageSize(" << res.storageSize << ")." << LogStream::eos;
    }

};

/**
*@brief ��ѯ�������쳣��¼�ص�
*/
class SelectFrozenAndNeverBackAbnormalTransactionCallback : public SelectRoleInfoCallback
{
	AVALON_DEFINE_OBJECTPOOL(SelectFrozenAndNeverBackAbnormalTransactionCallback, Avalon::Mutex)
public:
	SelectFrozenAndNeverBackAbnormalTransactionCallback(Player *pPlayer)
		: SelectRoleInfoCallback(pPlayer)
	{
	}
	~SelectFrozenAndNeverBackAbnormalTransactionCallback()
	{
	}

public:
	void OnFinished(Player *pPlayer)
	{
		if (pPlayer == NULL) return;
		
		pPlayer->GetAbnormalTransactionMgr().LoadDataFromDBCallback(this);
		pPlayer->GetAbnormalTransactionMgr().OnQueryFrozenAndNeverBackAbnormalTransactionRecordRet();
	}

	void OnFailed(Player *pPlayer)
	{
		if (!pPlayer) return;
		GameErrorLogStream("Player") << "player(" << pPlayer->GetAccID() << "," << pPlayer->GetID()
			<< ")(" << pPlayer->GetName() << ") select t_abnormal_transaction failed." << LogStream::eos;
	}
};

/**
*@brief ��ѯ�������쳣��¼�ص�
*/
class SelectBuyerFrozenAndNeverBackAbnormalTransactionCallback : public SelectRoleInfoCallback
{
	AVALON_DEFINE_OBJECTPOOL(SelectBuyerFrozenAndNeverBackAbnormalTransactionCallback, Avalon::Mutex)
public:
	SelectBuyerFrozenAndNeverBackAbnormalTransactionCallback(Player *pPlayer)
		: SelectRoleInfoCallback(pPlayer)
	{
	}
	~SelectBuyerFrozenAndNeverBackAbnormalTransactionCallback()
	{
	}

public:
	void OnFinished(Player *pPlayer)
	{
		if (pPlayer == NULL) return;

		pPlayer->GetAbnormalTransactionMgr().LoadBuyerDataFromDB(this);
		pPlayer->GetAbnormalTransactionMgr().OnQueryFrozenAndNeverBackAbnormalTransactionRecordRet();
	}

	void OnFailed(Player *pPlayer)
	{
		if (!pPlayer) return;
		GameErrorLogStream("Player") << "player(" << pPlayer->GetAccID() << "," << pPlayer->GetID()
			<< ")(" << pPlayer->GetName() << ") select t_abnormal_transaction failed." << LogStream::eos;
	}
};

/**
*@brief ��ѯ�ʾ����ص�
*/
class SelectQuestionnaireCallback : public SelectRoleInfoCallback
{
	AVALON_DEFINE_OBJECTPOOL(SelectQuestionnaireCallback, Avalon::Mutex)
public:
	SelectQuestionnaireCallback(Player* player)
		:SelectRoleInfoCallback(player) {}
	~SelectQuestionnaireCallback() {}

public:
	void OnFinished(Player* player)
	{
		if (NextRow())
		{
			player->SetActiveTimeType(GetData("activeTimeType"));
			player->SetMasterType(GetData("masterType"));
			player->SetRegionId(GetData("regionId"));
			player->SetDeclaration(GetData("declaration").GetString());
		}
	}

	void OnFailed(Player* player)
	{
		GameErrorLogStream("Player") << "player(" << player->GetAccID() << "," << player->GetID()
			<< "," << player->GetName() << ") select Questionnaire info failed." << LogStream::eos;
	}
};

/**
*@brief ��ѯ��Ӫ����Իص�
*/
class SelectOpActivityAttibuteCallback : public SelectRoleInfoCallback
{
	AVALON_DEFINE_OBJECTPOOL(SelectOpActivityAttibuteCallback, Avalon::Mutex)
public:
	SelectOpActivityAttibuteCallback(Player* player)
		:SelectRoleInfoCallback(player) {}
	~SelectOpActivityAttibuteCallback() {}

public:
	virtual void OnFinished(Player* player)
	{
		if (!player) return;

		player->GetActivityAttributeMgr().LoadOpActivityAttributeFromDB(this);
	}

	virtual void OnFailed(Player* player)
	{
		if (!player) return;

		ErrorLogStream << PLAYERINFO(player) << " select t_activity_op_attribute failed!" << LogStream::eos;
	}
};

/**
*@brief ��ѯ��Ӫ��˺ż�¼
*/
class SelectOpActivityAccCallback : public SelectRoleInfoCallback
{
	AVALON_DEFINE_OBJECTPOOL(SelectOpActivityAccCallback, Avalon::Mutex)
public:
	SelectOpActivityAccCallback(Player* player)
		:SelectRoleInfoCallback(player) {}
	~SelectOpActivityAccCallback() {}

public:
	virtual void OnFinished(Player* player)
	{
		if (!player) return;

		while(NextRow())
		{

		}
	}

	virtual void OnFailed(Player* player)
	{
		if (!player) return;

		ErrorLogStream << PLAYERINFO(player) << " select t_activity_account_record failed!" << LogStream::eos;
	}
};

/**
*@brief ��ѯͷ���
*/
class SelectHeadFrameCallback : public SelectRoleInfoCallback
{
	AVALON_DEFINE_OBJECTPOOL(SelectHeadFrameCallback, Avalon::Mutex)
public:
	SelectHeadFrameCallback(Player* player)
		:SelectRoleInfoCallback(player) {}
	~SelectHeadFrameCallback() {}

public:
	virtual void OnFinished(Player* player)
	{
		if (!player) return;
		player->GetHeadFrameMgr().LoadHeadFrame(this);
	}

	virtual void OnFailed(Player* player)
	{
		if (!player) return;

		ErrorLogStream << PLAYERINFO(player) << " select t_head_frame failed!" << LogStream::eos;
	}
};
