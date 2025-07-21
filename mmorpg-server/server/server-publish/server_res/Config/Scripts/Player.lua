--初始化
function Init(script)
	
	--配置道具兑换相关
	script:SetExchangeItem(1, 65, 5, "32001_168");--(类别(同类显示一个地方，别处配置类别)，道具id，品质，"原材料id1_数量1，id2_数量2");
	script:SetExchangeItem(1, 20022, 4, "32016_9,32001_35");
	script:SetExchangeItem(1, 21104, 3, "32017_5,32001_35");
	script:SetExchangeItem(1, 43601, 0, "32001_10");
	script:SetExchangeItem(1, 43602, 1, "32001_40");
	script:SetExchangeItem(1, 43603, 2, "32001_100");
	script:SetExchangeItem(1, 32053, 4, "32001_10");
	script:SetExchangeItem(1, 31828, 4, "32017_10");
	script:SetExchangeItem(1, 31828, 4, "32016_10");
	script:SetExchangeItem(1, 32000, 4, "32016_1");
	script:SetExchangeItem(1, 32000, 4, "32017_1");
	script:SetExchangeItem(2, 42535, 4, "661_20,32058_120");
	script:SetExchangeItem(2, 42542, 5, "660_25,32058_140");
	script:SetExchangeItem(2, 31828, 4, "660_20");
	script:SetExchangeItem(2, 31828, 4, "661_30");
	script:SetExchangeItem(2, 44039, 5, "32058_120");
	script:SetExchangeItem(2, 65, 5, "32058_168");
	script:SetExchangeItem(2, 65, 5, "661_40");
	script:SetExchangeItem(2, 65, 5, "660_30");
	script:SetExchangeItem(2, 659, 4, "32058_15");
	script:SetExchangeItem(2, 659, 4, "660_2");
	script:SetExchangeItem(2, 659, 4, "661_3");
	script:SetExchangeItem(3, 20022, 4, "663_5000");
	script:SetExchangeItem(3, 815, 4, "663_5000");
	script:SetExchangeItem(3, 816, 4, "663_5000");
	script:SetExchangeItem(3, 818, 4, "663_2500");
	script:SetExchangeItem(3, 817, 4, "663_1500");
	script:SetExchangeItem(3, 21104, 3, "663_2500");
	script:SetExchangeItem(3, 45, 4, "663_1500");
	script:SetExchangeItem(3, 32032, 3, "663_40");
	script:SetExchangeItem(3, 32043, 3, "663_40");
	script:SetExchangeItem(3, 32046, 3, "663_40");
	script:SetExchangeItem(3, 32049, 3, "663_40");
	script:SetExchangeItem(3, 44039, 5, "663_6000");
	script:SetExchangeItem(3, 31828, 4, "663_6000");
	script:SetExchangeItem(3, 65, 5, "663_10000");
	script:SetExchangeItem(3, 42542, 5, "663_15000");
	script:SetExchangeItem(3, 20026, 4, "663_5000");
	script:SetExchangeItem(3, 21112, 4, "663_3500");
	script:SetExchangeItem(3, 21114, 4, "663_3500");
	script:SetExchangeItem(4, 32074, 5, "32073_1");
	script:SetExchangeItem(4, 32075, 5, "32073_1");
	script:SetExchangeItem(4, 32076, 5, "32073_1");
	script:SetExchangeItem(4, 32077, 5, "32073_1");
	script:SetExchangeItem(4, 32078, 5, "32073_1");
	script:SetExchangeItem(5, 21115, 3, "32130_5,32131_25");
	script:SetExchangeItem(5, 32074, 5, "32129_15,32131_90");
	script:SetExchangeItem(5, 32075, 5, "32129_15,32131_90");
	script:SetExchangeItem(5, 32076, 5, "32129_15,32131_90");
	script:SetExchangeItem(5, 32077, 5, "32129_15,32131_90");
	script:SetExchangeItem(5, 32078, 5, "32129_15,32131_90");
	script:SetExchangeItem(5, 31828, 4, "32130_6,32129_6,32131_50");
	script:SetExchangeItem(5, 65, 5, "32130_9,32129_9,32131_80");
	script:SetExchangeItem(5, 44039, 5, "32130_6,32129_6,32131_50");
	script:SetExchangeItem(5, 42542, 5, "32130_13,32129_13,32131_120");
	script:SetExchangeItem(5, 32129, 4, "32130_3");
	script:SetExchangeItem(5, 32130, 4, "32129_2");
	script:SetExchangeItem(5, 32132, 3, "32131_5");
	script:SetExchangeItem(5, 32133, 3, "32131_5");
	script:SetExchangeItem(5, 32134, 3, "32131_5");
	script:SetExchangeItem(5, 32135, 3, "32131_5");
	script:SetExchangeItem(6, 32166,3,"32163_1,32164_1,32165_1");
	script:SetExchangeItem(6, 36003,2,"36002_3,32166_2");
	script:SetExchangeItem(6, 36004,3,"36003_3,32166_6");
	script:SetExchangeItem(6, 43601,0,"32166_10");
	script:SetExchangeItem(6, 43602,1,"32166_20");
	script:SetExchangeItem(6, 43603,2,"32166_60");
	script:SetExchangeItem(6, 43604,3,"32166_110");
	
	--随机装备设置
	--参数：等级，品质，价格
	script:AddEquipShopPrice(30, 2, 10000);
	script:AddEquipShopPrice(30, 3, 20000);
	script:AddEquipShopPrice(40, 2, 10000);
	script:AddEquipShopPrice(40, 3, 20000);
	script:AddEquipShopPrice(50, 2, 10000);
	script:AddEquipShopPrice(50, 3, 20000);
	script:AddEquipShopPrice(60, 2, 10000);
	script:AddEquipShopPrice(60, 3, 20000);
	script:AddEquipShopPrice(70, 2, 10000);
	script:AddEquipShopPrice(70, 3, 20000);
	script:AddEquipShopPrice(80, 2, 10000);
	script:AddEquipShopPrice(80, 3, 20000);
	script:AddEquipShopPrice(90, 2, 10000);
	script:AddEquipShopPrice(90, 3, 20000);
	script:AddEquipShopPrice(100, 2, 10000);
	script:AddEquipShopPrice(100, 3, 20000);
	--参数：小时
	script:AddEquipShopRefreshTime(10);
	script:AddEquipShopRefreshTime(14);
	script:AddEquipShopRefreshTime(18);
	script:AddEquipShopRefreshTime(22);

end

--玩家出生
function OnBirth(player)
	--if player:AcceptTask(1001) then
	--	player:ShowUnfinishedTaskDialog(1001);
	--end
	
	local reason = "103;0;0";
	player:AddItem(reason, 200001001, 1);
    player:AddItem(reason, 600000007, 1000);
    player:AddItem(reason, 600000006, 3);

	local occ = player:GetMainOccu();
	if occ == 1 then
		player:EquipItem(110311001)
	elseif occ == 2 then
		player:EquipItem(110315001)
	elseif occ == 5 then
		player:EquipItem(110315001)
	elseif occ == 3 then
		player:EquipItem(110320001)
	end
	
end

--发送第一次上线奖励
function SendFirstLoginReward(player)
	--等级30级以上的玩家每天登录游戏还可以获得我们送出的“新年大礼盒”，在邮件附件中收取
	if Sys:IsInActivity("y_login") == true then
		if player:GetCounter("y_login") == 0 and player:GetLevel() >= 30 then

			local group = player:GetRewardGroup();
			group:AddReward(32013, 1, 1, 0);
			Sys:SendSysMail(player:GetID(), "新年登录大礼","炎黄传奇感祝您新年快乐、万事如意！现特发放新年奖励：新年大礼盒*1，请注意查收。祝您游戏愉快！", 0, group);
			player:IncCounter("y_login", 1);
		end
		--活动期间所有老玩家(2月6号之创建)登录游戏都能获得新年回归大礼包，（只能获得一次）在邮件附件中收去。
		if player:GetCounter("y_oldplayerlogin") == 0 then
			if player:GetCreateTime() < 1360080000 and player:GetLevel() >= 50 then
				local group = player:GetRewardGroup();
				group:AddReward(32014, 1, 1, 0);
				Sys:SendSysMail(player:GetID(), "新年回归大礼","炎黄传奇感祝您新年快乐、万事如意！现特发放老玩家回归奖励：新年回归大礼包*1，请注意查收。祝您游戏愉快！", 0, group);
				player:IncCounter("y_oldplayerlogin", 1);
			end
		end
	end
	--三月 登录回馈礼包
	if Sys:IsInActivity("m_huikui") == true then
		if player:GetCounter("m_huikui") == 0 then
			local group = player:GetRewardGroup();
			group:AddReward(658, 1, 1, 0);
			Sys:SendSysMail(player:GetID(), "3月回馈大礼包", "感谢您一直对炎黄传奇的支持。特此奉送3月回馈大礼包，请及时领取。祝您游戏愉快", 0, group);
			player:IncCounter("m_huikui", 1);
		end
	end
	--三月 登录礼包
	if Sys:IsInActivity("m_login") == true and player:GetLevel() >=30 then
		if player:GetCounter("m_login") == 0 then
			local group = player:GetRewardGroup();
			group:AddReward(662, 1, 1, 0);
			Sys:SendSysMail(player:GetID(), "活动登录礼包", "感谢您登录炎黄传奇，以下为活动登录礼包，请及时领取。祝您游戏愉快！", 0, group);
			player:IncCounter("m_login", 1);
		end
	end

	-- 三月女生
	if Sys:IsInActivity("m_girl") == true then
		if player:GetCounter("m_girl") == 0 and player:GetSex() == 1 and player:GetLevel() >= 35 then
			local group = player:GetRewardGroup();
			group:AddReward(657, 1, 1, 0);
			Sys:SendSysMail(player:GetID(), "女生特权礼盒", "亲爱的玩家，感谢您对炎黄传奇的支持。特此奉上女生特权礼盒，请及时领取。祝您游戏愉快", 0, group);
			player:IncCounter("m_girl", 1);
		end
	end

	if Sys:IsInActivity("m_girl") == true then
		if player:GetCounter("m_boy") == 0 and player:GetSex() == 0 and player:GetLevel() >= 40 then
			local group = player:GetRewardGroup();
			group:AddReward(424, 9, 1, 0);
			Sys:SendSysMail(player:GetID(), "玫瑰赠佳人", "亲爱的玩家，正值38妇女节，给炎黄的美女们献上一朵玫瑰吧！请及时领取附件。祝您游戏愉快", 0, group);
			player:IncCounter("m_boy", 1);
		end
	end
	
	--愚人节卡片
	if Sys:IsInActivity("fools_joy") and player:GetLevel() >= 30 then
		if player:GetCounter("fools_card") == 0 then
			local group = player:GetRewardGroup();
			group:AddReward(32067, 1, 1, 0);
			Sys:SendSysMail(player:GetID(), "快乐愚人节", "您好！在这欢乐的日子里，炎黄传奇精心为您准备了愚人节整人道具：愚人卡片，奖励已经发放到您的附件中，"
				.. "请注意查收！您可通过使用该道具给您的好友进行大变身哦，最后祝您节日快乐，欢乐多多！", 0, group);
			player:IncCounter("fools_card", 1);
		end
	end
	
	--激情四月,喜迎跨服送豪礼
	if Sys:IsInActivity("a_interservice") and player:GetLevel() >= 30 then
		if player:GetCounter("interservice_gb") == 0 then
			local group = player:GetRewardGroup();
			group:AddReward(32079, 1, 1, 0);
			Sys:SendSysMail(player:GetID(), "喜迎跨服送豪礼", "亲爱的玩家：您好！为了更好地迎接跨服战的到来，炎黄传奇精心为您准备了一份礼物：跨服登录礼包。奖励已经发放到您的附件中，请注意查收！", 0, group);
			player:IncCounter("interservice_gb", 1);
		end
	end
	
	--激情四月,福神登录礼包
	if Sys:IsInActivity("a_viplogin") and player:GetLevel() >= 30 and player:GetVipLvl() > 0 then
			local itemid = 0;
			if player:GetVipLvl() == 1 then
				itemid = 32007;
			elseif player:GetVipLvl() == 2 then
				itemid = 32008;
			elseif player:GetVipLvl() == 3 then 
				itemid = 32009;
			end
			
			if itemid ~= 0 then
				local group = player:GetRewardGroup();
				group:AddReward(itemid, 1, 1, 0);
				Sys:SendSysMail(player:GetID(), "福神登录礼包", "亲爱的福神玩家：您好！炎黄传奇为您准备了一份福神礼包，已经通过附件发送，请您注意查收。明天还可以再领取一份哦~", 0, group);			
			end
	end
	
	--四月个人pk竞技
	if Sys:IsInActivity("pk_day") and player:GetLevel() >= 30 then
		local group = player:GetRewardGroup();
		group:AddReward(32084, 1, 1, 0);
		Sys:SendSysMail(player:GetID(), "迎竞技送豪礼", "亲爱的玩家：您好！激烈的个人竞技已经开启，炎黄传奇精心为您准备了一份礼物：竞技登录礼包。奖励已经发放到您的附件中，请注意查收！", 0, group);			
	end

	--五月 跨服登录大礼包
	if Sys:IsInActivity("world_login") == true and player:GetLevel() >=30 then
		if player:GetCounter("world_login") == 0 then
			local group = player:GetRewardGroup();
			group:AddReward(852, 1, 1, 0);
			Sys:SendSysMail(player:GetID(), "跨服大礼包", "喜迎跨服送大礼，特赠跨服大礼包一个。请及时领取附件，祝您游戏愉快！", 0, group);
			player:IncCounter("world_login", 1);
		end
	end

	if Sys:IsInActivity("june1") == true and player:GetLevel() >= 40 then
		if player:GetCounter("m_login") == 0 then
			local group = player:GetRewardGroup();
			group:AddReward(32136, 1, 1, 0);
			Sys:SendSysMail(player:GetID(), "童真献礼", "恭喜你获得童真礼盒*1，请及时领取附件，祝您游戏愉快！", 0, group);
			player:IncCounter("m_login", 1);
		end
	end

	if Sys:IsInActivity("june3") == true and player:GetLevel() >= 35 then
		if player:GetCounter("m_login") == 0 then
			local group = player:GetRewardGroup();
			group:AddReward(32162, 1, 1, 0);
			Sys:SendSysMail(player:GetID(), "登录献礼", "感谢您登录炎黄传奇，此端午之际特此献礼。请及时领取附件，祝您游戏愉快！", 0, group);
			player:IncCounter("m_login", 1);
		end
	end

	if Sys:IsInActivity("june4") == true then
		if player:GetCounter("m_boy") == 0 and player:GetSex() == 0 and player:GetLevel() >= 35 then
			local group = player:GetRewardGroup();
			group:AddReward(32175, 1, 1, 0);
			Sys:SendSysMail(player:GetID(), "男生礼盒", "恭喜您获得男性节日礼盒，请及时领取附件。祝您游戏愉快！", 0, group);
			player:IncCounter("m_boy", 1);
		end
	end
	
	if Sys:IsInActivity("june4") == true then
		if player:GetCounter("m_girl") == 0 and player:GetSex() == 1 and player:GetLevel() >= 40 then
			local group = player:GetRewardGroup();
			group:AddReward(424, 9, 1, 0);
			Sys:SendSysMail(player:GetID(), "女生礼盒", "恭喜您获得玫瑰*9，请及时领取附件。祝您游戏愉快！", 0, group);
			player:IncCounter("m_girl", 1);
		end
	end
end

--玩家上线
function OnOnline(player, offlineTime)
	local now = Sys:Now();
	
	--黑暗塔下线超过5分钟回城
	local mapid = player:GetScene():GetMapID();
	if mapid >= 131 and mapid <= 137 then
		if now > offlineTime + 5 * 60 * 1000 then
			player:BackToMajorCity();
		end
	end
	
	---- 不在活动期删除圣诞任务
	if Sys:IsInActivity("christmas6")  ~= true then
		player:AbandonTask(2012);
	else
		if player:GetLevel() >= 33 then
			player:AcceptTask(2012);
		end
	end
	
	---- 老玩家上线给与新手称号
	if player:CheckTaskFinished(1009) then
		player:GiveTitle(100)
	end
	
	local	bFirstLogin = not Sys:IsSameDay(now / 1000, offlineTime / 1000);
	if bFirstLogin then
		SendFirstLoginReward(player);
	end
	
	--同步当前金贝消费
	Sys:SyncCounter(player, "xiaofei_d");
end

--0点
function OnDayChanged(player)
	local looptask = player:FindTask(4600);
	if looptask ~= nil then
		if looptask:GetVar("curloop") >= player:GetDailyTaskMaxNum(8) then
			looptask:SetVar("curloop", 0);
		end
	end

	SendFirstLoginReward(player);
end

--升级
function OnLevelup(player)
	if player:GetLevel() >= 30 then
		local looptask = player:FindTask(4600);
		if looptask == nil then
			player:AcceptTask(4600);
		end
	end
	
	if Sys:IsInActivity("christmas6") and player:GetLevel() >= 33 then
		player:AcceptTask(2012);
	end
	
	-- modified by huchenhui
	--if player:GetLevel() == 37 then
	--	player:SendSysMail("炎黄入门进阶宝典", "尊敬的玩家：\n热烈欢迎您来到炎黄传奇！初次体验，您可能有许多不了解的地方，我们为你准备了丰富的游戏攻略，力保您游戏之旅轻松愉快！祝你收获满满！" ..
	--		"<a href=\"http://bbs.open.qq.com/thread-152942-1-1.html\" target=\"_blank\"><font color=\"#FF0000\"><u>点击查看论坛攻略</u></font></a>", 0, nil);
	--end
	
	--升到30级时才发送上线奖励
	if player:GetLevel() == 30 then
		SendFirstLoginReward(player);
	end
end

--队伍boss出生
function OnTeamBossBirth(player, npcId, sceneId, x, y)
end

--排行榜替身召出
function OnSortListSpiritBirth(player, spiritId, spiritName, sceneId)
	if not player:IsDailyTaskAcceptable(6, false) then
		player:RemoveSortListSpirit(sceneId, spiritId);
		return;
	end
	
	if not player:AcceptTask(4001) then
		player:RemoveSortListSpirit(sceneId, spiritId);
		return;
	end
	
	local task = player:FindTask(4001);
	task:SetVar("SCENEID", sceneId);
	task:SetVar("sceneId", sceneId);
	task:SetNameVar("playerName", spiritName);
	task:SetVar("spiritId", spiritId);
	
	local dialog = Sys:GetNpcDialog(player, 103031);
	dialog:AddWord("据可靠情报，" .. spiritName .. "出现在" .. Sys:GetMapName(sceneId) .. "一带，快去挑战吧！雷达区域闪光红点标示了名人所在位置。");
	dialog:Show();
end
--触发散财童子刷新事件处理
function OnWealthNpcBirth(player, typeNotify, value, itemName)
	if typeNotify == "strengthen" then
		Sys:BroadcastNotify( 3, 7, "祝贺{T 3|" .. player:GetName() .. "}的道具{T 3|" .. itemName .. "}强化{T 3|" .. value .. "}成功，散财童子降临陈都，赶快去寻找散财童子获得丰厚的奖励吧");
		Sys:BroadcastNotify( 11, 7, "祝贺{P " .. player:GetBase64ID() .. " " .. player:GetName() .. " " .. player:GetSex() .. " " .. player:GetOccu() .. " " .. player:GetLevel() .. "|" .. player:GetName() .. "}的道具{T 3|" .. itemName .. "}强化{T 3|" .. value .. "}成功，散财童子降临陈都，赶快去寻找散财童子获得丰厚的奖励吧");
	elseif typeNotify == "gemstone" then
		Sys:BroadcastNotify( 3, 7, "祝贺{T 3|" .. player:GetName() .. "}获得镶嵌用的{T 3|" .. value .. "}级宝石，散财童子降临陈都，赶快去寻找散财童子获得丰厚的奖励吧");
		Sys:BroadcastNotify( 11, 7, "祝贺{P " .. player:GetBase64ID() .. " " .. player:GetName() .. " " .. player:GetSex() .. " " .. player:GetOccu() .. " " .. player:GetLevel() .. "|" .. player:GetName() .. "}获得镶嵌用的{T 3|" .. value .. "}级宝石，散财童子降临陈都，赶快去寻找散财童子获得丰厚的奖励吧");
	elseif typeNotify == "fabaolvl" then
		Sys:BroadcastNotify( 3, 7, "祝贺{T 3|" .. player:GetName() .. "}的{T 3|" .. itemName .. "}法位等级达到{T 3|" .. value .. "}级，散财童子降临陈都，赶快去寻找散财童子获得丰厚的奖励吧");
		Sys:BroadcastNotify( 11, 7, "祝贺{P " .. player:GetBase64ID() .. " " .. player:GetName() .. " " .. player:GetSex() .. " " .. player:GetOccu() .. " " .. player:GetLevel() .. "|" .. player:GetName() .. "}的{T 3|" .. itemName .. "}法位等级达到{T 3|" .. value .. "}级，散财童子降临陈都，赶快去寻找散财童子获得丰厚的奖励吧");
	elseif typeNotify == "quench" then
		Sys:BroadcastNotify( 3, 7, "祝贺{T 3|" .. player:GetName() .. "}的{T 3|" .. itemName .. "}道具淬炼{T 3|" .. value .. "}品成功，散财童子降临陈都，赶快去寻找散财童子获得丰厚的奖励吧");
		Sys:BroadcastNotify( 11, 7, "祝贺{P " .. player:GetBase64ID() .. " " .. player:GetName() .. " " .. player:GetSex() .. " " .. player:GetOccu() .. " " .. player:GetLevel() .. "|" .. player:GetName() .. "}的{T 3|" .. itemName .. "}道具淬炼{T 3|" .. value .. "}品成功，散财童子降临陈都，赶快去寻找散财童子获得丰厚的奖励吧");
	elseif typeNotify == "aptitude" then
		Sys:BroadcastNotify( 3, 7, "祝贺{T 3|" .. player:GetName() .. "}的宠物资质达到{T 3|" .. value .. "}，散财童子降临陈都，赶快去寻找散财童子获得丰厚的奖励吧");
		Sys:BroadcastNotify( 11, 7, "祝贺{P " .. player:GetBase64ID() .. " " .. player:GetName() .. " " .. player:GetSex() .. " " .. player:GetOccu() .. " " .. player:GetLevel() .. "|" .. player:GetName() .. "}的宠物资质达到{T 3|" .. value .. "}，散财童子降临陈都，赶快去寻找散财童子获得丰厚的奖励吧");
	elseif typeNotify == "darkgold" then
		Sys:BroadcastNotify( 3, 7, "祝贺{T 3|" .. player:GetName() .. "}获得{T 3|" .. value .. "}级的{T 3|" .. itemName .. "}暗金装备，散财童子降临陈都，赶快去寻找散财童子获得丰厚的奖励吧");
		Sys:BroadcastNotify( 11, 7, "祝贺{P " .. player:GetBase64ID() .. " " .. player:GetName() .. " " .. player:GetSex() .. " " .. player:GetOccu() .. " " .. player:GetLevel() .. "|" .. player:GetName() .. "}获得{T 3|" .. value .. "}级的{T 3|" .. itemName .. "}暗金装备，散财童子降临陈都，赶快去寻找散财童子获得丰厚的奖励吧");

	end

	--在陈都刷新20个散财童子
	Sys:SummonWealth(103, 103200, 20);
end

--第一次进入个人场景
function OnPrivateSceneTimesFull(player, mapid, times)
	if mapid == 401 then
		player:Notify(4, 0, "每天最多只能进入宠物洞天两次，您的进入次数已满");
	elseif mapid == 301 then
		player:Notify(4, 0, "您今天已经闯过通天塔了，明天再来吧！");
	end
end

--判断日常任务
function GetDailyTaskStatus(player, npcId)
	local status = 0;
	
	--[[
	local task = player:FindTask(4600);
	if task ~= nil and task:GetVar("curloop") < player:GetDailyTaskMaxNum(8) then
		local next_taskid = task:GetVar("nexttask");
		local looptask = player:FindTask(next_taskid);
		if looptask ~= nil then
			local npcid = looptask:GetVar("NPCID1");
			if npcid == npcId then
				if looptask:GetStatus() == 2 then
					return 3;
				elseif looptask:GetStatus() == 1 then
					status = 2;
				end
			end
		elseif task:GetVar("npcId") == npcId then
			return 1;
		end
	end
	--]]
	
	if npcId == 103021 then
		if player:GetLevel() >= 36 and player:IsDailyTaskAcceptable(4, false) then
			return 1;
		end
	elseif npcId == 103064 then
		if player:GetLevel() >= 34 and player:IsDailyTaskAcceptable(5, false) and player:GetDailyTaskNum(5) == 0 then
			return 1;
		end
	elseif npcId == 103029 then
		if player:GetLevel() >= 30 and player:GetTribeId() ~= 0  and player:IsDailyTaskAcceptable(7, false) and player:GetDailyTaskNum(7) == 0 then
			return 1;
		end
	elseif npcId == 103031 then
		if player:GetLevel() >= 30 and player:IsDailyTaskAcceptable(6, false) and player:GetDailyTaskNum(6) == 0 then
			return 1;
		end
	elseif npcId == 103005 then
		if player:GetLevel() >= 35 and player:IsDailyTaskAcceptable(9, false) and player:GetDailyTaskNum(9) == 0 then
			return 1;
		end
	elseif npcId == 103044 then
		if player:GetLevel() >= 42 and player:IsDailyTaskAcceptable(11, false) and player:GetDailyTaskNum(11) == 0 then
			return 1;
		end
	elseif npcId == 101031 then
		if player:GetLevel() >= 30 and player:GetCounter("festivalcount") == 0 and Sys:IsInActivity("dati") then
			return 1;
		end
	end
	
	return status;
end

function OnGiveActivityReward(player)
	if Sys:IsInActivity("qingming_day") then
		local group = player:GetRewardGroup();
		group:AddReward(32072, 1, 1, 0);
		Sys:SendSysMail(player:GetID(), "孔明灯", "您好！恭喜您在清明节活动中表现积极，特奖励孔明灯道具，奖励已经发放到您的附件中，请注意查收！祝您游戏愉快！", 0 , group);
	end	

	if Sys:IsInActivity("fools_joy") then
		local group = player:GetRewardGroup();
		group:AddReward(32066, 1, 1, 0);
		Sys:SendSysMail(player:GetID(), "祈愿奇巧盒", "您好！恭喜您在愚人节活动中表现积极，特奖励祈愿奇巧盒一个，请注意查收！祝您游戏愉快！", 0 , group);
	end

	if Sys:IsInActivity("labor_day") then
		local group = player:GetRewardGroup();
		group:AddReward(32094, 1, 1, 0);
		Sys:SendSysMail(player:GetID(), "五一劳动礼包", "您在活动期间完成日常任务，特此奖励五一劳动礼盒一枚，以资鼓励！请及时领取附件，祝您游戏愉快！", 0 , group);
	end

	if Sys:IsInActivity("june1") then
		local group = player:GetRewardGroup();
		group:AddReward(32136, 1, 1, 0);
		Sys:SendSysMail(player:GetID(), "童真献礼", "恭喜你获得童真礼盒*1，请及时领取附件，祝您游戏愉快！", 0 , group);
	end
	if Sys:IsInActivity("june3") then
		local group = player:GetRewardGroup();
		group:AddReward(32162, 1, 1, 0);
		Sys:SendSysMail(player:GetID(), "任务献礼", "恭喜您完成日常活跃，特此献礼。请及时领取附件，祝您游戏愉快！", 0 , group);
	end
	if Sys:IsInActivity("june4") then
		local group = player:GetRewardGroup();
		group:AddReward(424, 1, 1, 0);
		Sys:SendSysMail(player:GetID(), "任务达人", "恭喜您完成日常任务及副本，获得玫瑰*1。请及时领取附件，祝您游戏愉快！", 0 , group);
	end
end

--日常任务接受 星盘处理
function  OnDailyTaskAccepted(player, task)
	local taskType = task:GetType();
	if taskType == 5 then		--运送木材
		if player:GetCounter("transwood_count") == player:GetDailyTaskBaseMaxNum(5) then
			OnGiveActivityReward(player);
		end
	elseif taskType == 4 then	--悬赏任务
		if player:GetCounter("XS_count") == player:GetDailyTaskBaseMaxNum(4) then
			OnGiveActivityReward(player);
		end
	elseif taskType == 6 then	--名人挑战
		if player:GetCounter("challenge_task") == player:GetDailyTaskBaseMaxNum(6) then
			OnGiveActivityReward(player);

		end
	elseif taskType == 9 then	--客栈打工
		if player:GetCounter("innwork_count") == player:GetDailyTaskBaseMaxNum(9) then
			OnGiveActivityReward(player);
		end
	elseif taskType == 7 then	--部落任务
		if player:GetCounter("tribetask_count") == player:GetDailyTaskBaseMaxNum(7) then
			OnGiveActivityReward(player);
		end
	end
end

--日常任务提交
function OnDailyTaskSubmitted(player, task)
	local taskType = task:GetType();
	if taskType == 5 then		--运送木材
		if Sys:IsInActivity("y_transwood") then
			local task_exp = task:GetExp();
			player:IncExp(task_exp * 30 / 100);	
		end
	elseif taskType == 4 then	--悬赏任务
		if Sys:IsInActivity("y_xuanshang") then
			local task_exp = task:GetExp();
			player:IncExp(task_exp * 30 / 100);		
		end
	elseif taskType == 8 then	--安抚四方
		local loop_task = player:FindTask(4600);
		if loop_task ~= nil then
			if loop_task:GetVar("curloop") == player:GetDailyTaskBaseMaxNum(8) then
				OnGiveActivityReward(player);
			end
			if Sys:IsInActivity("y_pacifyall") then
				player:AddSoul(loop_task:GetVar("award") * 30 / 100);
			end
		end
	elseif taskType == 6 then	--名人挑战
		if Sys:IsInActivity("y_challenge") then
			local task_exp = task:GetExp();
			local task_silver = task:GetSilver();
			local task_honour = task:GetSoul();
			player:IncExp(task_exp * 30 / 100);
			player:AddSilver("task", task_silver * 30 / 100);
			player:AddHonour("task", task_honour * 30 / 100);
		end
	elseif taskType == 9 then	--客栈打工
		if Sys:IsInActivity("y_innwork") then
			local task_bsilver = task:GetBindSilver();
			player:AddBindSilver("task", task_bsilver * 30 / 100);
		end
	elseif taskType == 7 then	--部落任务
		if Sys:IsInActivity("y_tribetask") then
			local task_exp = task:GetExp();
			player:IncExp(task_exp * 30 / 100);
			local silver = task:GetSilver();
			player:AddTribeResource("task", silver * 30 / 100, 0, 0, 0);
			local contribute = task:GetContribution();
			player:AddTribeContribution(contribute * 30 / 100);
		end
	elseif taskType == 11 then --黑暗塔
		if player:GetCounter("dttask_count") == player:GetDailyTaskBaseMaxNum(11) then
			OnGiveActivityReward(player);
		end
	end
end

--玩家消费
function OnConsumeGold(player, itemId, itemNum, cost)
	if Sys:IsInActivity("m_consume") then
		player:IncCounter("m_consume", cost);
	end
	
	--消费排名活动
	if Sys:IsInActivity("consume_sort") then
		local total = player:IncCounter("consume_sortval", cost);
		Sys:OnWorldSortValueChanged(player, 25, total, 0, 0);
		Sys:OnSortValueChanged(26, player:GetID(), total, 0, 0);
	end
	
	--消费抽奖
	local	oldCost = player:GetCounter("xiaofei_d");
	local	newCost = oldCost + cost;
	player:IncCounter("xiaofei_d", cost);
	Sys:SyncCounter(player, "xiaofei_d");
	
	--冲刺消费排行榜,天下宝石入我囊
	if Sys:IsInActivity("gs_consumegold") then
		local oldCost = player:GetCounter("consumegold", cost);	
		local newCost = player:IncCounter("consumegold", cost);
		if newCost >= 1000 then
			Sys:OnSortValueChanged(27, player:GetID(), newCost, 0, 0);		
		end
		
		if oldCost < 10000 and newCost >= 10000 then
			local group = player:GetRewardGroup();
			group:AddReward(44304, 100, 1, 0);	
			group:AddReward(36004, 5, 1, 0);			
			Sys:SendSysMail(player:GetID(), "消费奖励  ", "恭喜您今日消费满10000金贝，特此奖励。请及时领取附件，祝您游戏愉快！", 0, group);			
		end
	end
	
	--金券礼包
	if itemId >= 32141 and itemId <= 32148 then
		local jqlbnum = player:GetCounter("jqlb_num");
		if jqlbnum < 10 and (jqlbnum + itemNum) >= 10 then
			local group = player:GetRewardGroup();
			if itemId == 32141 then
				group:AddReward(414,1,1,3);
			elseif itemId == 32142 then
				group:AddReward(35013,1,1,3);
			elseif itemId == 32143 then
				group:AddReward(33,1,1,4);
			elseif itemId == 32144 then
				group:AddReward(14,1,1,3);
			elseif itemId == 32145 then
				group:AddReward(426,1,1,3);
			elseif itemId == 32146 then
				group:AddReward(134,1,1,3);
			elseif itemId == 32147 then
				group:AddReward(35003,1,1,3);
			elseif itemId == 32148 then
				group:AddReward(45,1,1,3);
			end
			Sys:SendSysMail(player:GetID(), "金券礼包", "尊敬的玩家，您好，在本次金券活动中您购买满10次金券礼包，附件中是您获得的额外奖励", 0, group);
		end
		player:SetCounter("jqlb_num", jqlbnum + itemNum);
		Sys:SyncCounter(player, "jqlb_num");
	end
	if Sys:IsInActivity("june3") then
		local group = player:GetRewardGroup();
		local has_reward = 0;
		if player:GetCounter("june3_chongzhi") + cost >= 1988 then
			if player:GetCounter("1988_flag") == 0 then
				group:AddReward(32152, 1, 1, 0);
				player:SetCounter("1988_flag", 1);
				has_reward = 1;
			end
			if player:GetCounter("988_flag") == 0 then
				group:AddReward(32151, 1, 1, 0);
				player:SetCounter("988_flag", 1);
				has_reward = 1;
			end
			if player:GetCounter("188_flag") == 0 then
				group:AddReward(32150, 1, 1, 0);
				player:SetCounter("188_flag", 1);
				has_reward = 1;
			end
		elseif player:GetCounter("june3_chongzhi") + cost >= 988 then
			if player:GetCounter("988_flag") == 0 then
				group:AddReward(32151, 1, 1, 0);
				player:SetCounter("988_flag", 1);
				has_reward = 1;
			end
			if player:GetCounter("188_flag") == 0 then
				group:AddReward(32150, 1, 1, 0);
				player:SetCounter("188_flag", 1);
				has_reward = 1;
			end
		elseif player:GetCounter("june3_chongzhi") + cost >= 188 then
			if player:GetCounter("188_flag") == 0 then
				group:AddReward(32150, 1, 1, 0);
				player:SetCounter("188_flag", 1);
				has_reward = 1;
			end
		end
		player:IncCounter("june3_chongzhi", cost);
		if has_reward == 1 then
			Sys:SendSysMail(player:GetID(), "消费反馈", "恭喜您消费满"..player:GetCounter("june3_chongzhi").."金贝，特此赠礼。请及时领取附件。祝您游戏愉快！", 0, group);
		end
	end
end

--玩家消费q点
function OnConsumePoint(player, itemId, itemNum, cost)
	--充值得金石
	if itemId == 3 then
		local goldstone = 0;
		--每天首充额外获得50金石
		if player:GetCounter("chongzhi_d") == 0 then
			goldstone = 50;
		end
		goldstone = goldstone + cost;
		player:IncCounter("chongzhi_d", cost);
		
		--3366双倍金石活动
		if Sys:IsInActivity("3366jinshi") and player:GetPf() == "3366" then
			goldstone = goldstone + cost;
		end
		
		player:AddGoldStone("mall_buy", goldstone);
	end

	if itemId == 3 and Sys:IsInActivity("june2") then
		if player:GetCounter("first_flag") == 0 then
			local group = player:GetRewardGroup();
			group:AddReward(414, 1, 1, 0);
			Sys:SendSysMail(player:GetID(), "活动首充", "恭喜完成首充送礼活动，特此赠送技能重置卡*1。请及时领取附件。祝您游戏愉快！", 0, group);
			player:SetCounter("first_flag", 1);	
		end
		if player:GetCounter("total_flag") == 0 and player:GetCounter("total_chongzhi") + cost >= 200 then
			local group = player:GetRewardGroup();
			group:AddReward(45, 1, 1, 0);
			Sys:SendSysMail(player:GetID(), "满就送", "恭喜你完成满200金贝赠礼活动，特此赠送价值1199金贝改名符*1。请及时领取附件。祝您游戏愉快！", 0, group);
			player:SetCounter("total_flag", 1);
		end
		player:IncCounter("total_chongzhi", cost);
	end



	if itemId == 3 and Sys:IsInActivity("june3") then
		if player:GetCounter("june3_flag") == 0 then
			local group = player:GetRewardGroup();
			group:AddReward(32149, 1, 1, 0);
			Sys:SendSysMail(player:GetID(), "端午首充", "恭喜您完成端午首充活动，特此赠礼。请及时领取附件。祝您游戏愉快！", 0, group);
			player:SetCounter("june3_flag", 1);	
		end

	end
	
	--[[if itemId == 3 and Sys:IsInActivity("3366charge") and player:GetPf() == "3366" then
		--player:IncCounter("3366charge", cost);
		player:IncCounter("3366charge_day", cost);
		local awardgold = cost / 10;
		if awardgold ~= 0 then
			local group = player:GetRewardGroup();
			group:AddReward(3, awardgold, 1, 0);
			Sys:SendSysMail(player:GetID(), "3366充值大返利", "您好！您刚才充值了"..cost.."金贝，现在正进行充值返利活动，您本次充值可返利"..awardgold.."金贝。请注意查收！", 0, group);
		end
	end

	]]

	--六月充值返利
	if itemId == 3 and Sys:IsInActivity("fanli") then
		local fanli = cost / 10;
		local group = player:GetRewardGroup();
		group:AddReward(3, fanli, 1, 0);
		Sys:SendSysMail(player:GetID(), "充值金贝大返利", "亲爱的玩家：您好！您刚才充值了"..cost.."金贝，现在正进行充值返利活动，您本次充值可返利"..fanli.."金贝。奖励已经发放到您的附件中，请注意查收！", 0, group);
	end

	if Sys:IsInActivity("3366charge_may") and player:GetPf() == "3366" then
		player:IncCounter("3366charge_may", cost);
	end
	
	--开服活动,单笔充值大返利
	if itemId == 3 and Sys:IsInActivity("gs_chongzhi") then
		local awardgold = 0;
		if cost >= 10000 then
			awardgold = cost * 20 / 100;
		elseif cost >= 5000 then
			awardgold = cost * 15 / 100;
		elseif cost >= 1000 then
			awardgold = cost * 10 / 100;
		elseif cost >= 100 then
			awardgold = cost * 5 / 100;
		end
		
		if awardgold ~= 0 then
			local group = player:GetRewardGroup();
			group:AddReward(3, awardgold, 1, 0);
			Sys:SendSysMail(player:GetID(), "充值金贝大返利", "亲爱的玩家：您好！您刚才充值了"..cost.."金贝，现在正进行充值返利活动，您本次充值可返利"..awardgold.."金贝。奖励已经发放到您的附件中，请注意查收！", 0, group);
		end
	end
	if itemId == 3 and Sys:IsInActivity("m_chongzhi") then
		player:IncCounter("m_chongzhi", cost);
	end
	
	--金券礼包
	if itemId >= 32141 and itemId <= 32148 then
		local jqlbnum = player:GetCounter("jqlb_num");
		if jqlbnum < 10 and (jqlbnum + itemNum) >= 10 then
			local group = player:GetRewardGroup();
			if itemId == 32141 then
				group:AddReward(414,1,1,3);
			elseif itemId == 32142 then
				group:AddReward(35013,1,1,3);
			elseif itemId == 32143 then
				group:AddReward(33,1,1,4);
			elseif itemId == 32144 then
				group:AddReward(14,1,1,3);
			elseif itemId == 32145 then
				group:AddReward(426,1,1,3);
			elseif itemId == 32146 then
				group:AddReward(134,1,1,3);
			elseif itemId == 32147 then
				group:AddReward(35003,1,1,3);
			elseif itemId == 32148 then
				group:AddReward(45,1,1,3);
			end
			Sys:SendSysMail(player:GetID(), "金券礼包", "尊敬的玩家，您好，在本次金券活动中您购买满10次金券礼包，附件中是您获得的额外奖励", 0, group);
		end
		player:SetCounter("jqlb_num", jqlbnum + itemNum);
		Sys:SyncCounter(player, "jqlb_num");
	end
end

--获得星盘
function OnGotStarPlate(player, ptype)
	if ptype == 100 then
		player:SendActUdpLog("gotstarplate", "100", 1);
	end
end

--剧情结束
function OnStoryEnd(player, storyid)
end

--检查登录天数限制的道具
function CheckLoginLimitItem(player, item)
	local itemid = item:GetDataID();
	
	if itemid == 410 then
		local loginDays = item:GetParam1();
		local golds = item:GetParam2() % 10000;
		local bindgolds = item:GetParam2() / 10000;
		
		local goldCanGet = 0;
		local bindgoldCanGet = 0;
		if loginDays >= 30 then
			goldCanGet = 1000;
			bindgoldCanGet = 2000;
		elseif loginDays >= 25 then
			goldCanGet = 800;
			bindgoldCanGet = 1600;
		elseif loginDays >= 20 then
			goldCanGet = 600;
			bindgoldCanGet = 1300;
		elseif loginDays >= 15 then
			goldCanGet = 450;
			bindgoldCanGet = 950;
		elseif loginDays >= 10 then
			goldCanGet = 300;
			bindgoldCanGet = 700;
		elseif loginDays >= 6 then
			goldCanGet = 200;
			bindgoldCanGet = 400;
		elseif loginDays >= 3 then
			goldCanGet = 100;
			bindgoldCanGet = 200;
		else
			goldCanGet = 50;
			bindgoldCanGet = 50;
		end
		
		if golds < goldCanGet then
			Sys:SendCaishenInfo(player, 1, loginDays, goldCanGet - golds, bindgoldCanGet - bindgolds);
		else
			local nextdays = 0;
			local nextgold = 0;
			local nextbindgold = 0;
			if loginDays >= 30 then
				return;
			elseif loginDays >= 25 then
				nextdays = 30 - loginDays;
				nextgold = 200;
				nextbindgold = 400;
			elseif loginDays >= 20 then
				nextdays = 25 - loginDays;
				nextgold = 200;
				nextbindgold = 300;
			elseif loginDays >= 15 then
				nextdays = 20 - loginDays;
				nextgold = 150;
				nextbindgold = 350;
			elseif loginDays >= 10 then
				nextdays = 15 - loginDays;
				nextgold = 150;
				nextbindgold = 250;
			elseif loginDays >= 6 then
				nextdays = 10 - loginDays;
				nextgold = 100;
				nextbindgold = 300;
			elseif loginDays >= 3 then
				nextdays = 6 - loginDays;
				nextgold = 100;
				nextbindgold = 200;
			else
				nextdays = 3 - loginDays;
				nextgold = 50;
				nextbindgold = 150;
			end
			Sys:SendCaishenInfo(player, 0, nextdays, nextgold, nextbindgold);
		end
	end
end


--使用烟花
function OnUseFireworks(player, itemId)
	if itemId >= 42014 and itemId <= 42018 then
		local scene = player:GetScene();
		if scene:GetMapID() ~= 707 then
			player:Notify(4, 0, "只能在婚礼庆典地图使用此烟花");
			return false;
		end
		
		local degree = 0;
		if itemId == 42018 then
			degree = 10;
		elseif itemId == 42014 then
			degree = 128;
		elseif itemId == 42015 then
			degree = 128;
		elseif itemId == 42016 then
			degree = 699;
		elseif itemId == 42017 then
			degree = 699;
		end
		
		local sweet_degree = scene:GetParam("sweet_degree");
		if sweet_degree < 3344 and sweet_degree + degree >= 3344 then
			local maleReward = Sys:GetRewardGroup();
			maleReward:AddReward(32059, 1, 1, 0);
			Sys:SendSysMail(scene:GetParam("male_id"), "新婚之喜", "恭喜您和您的伴侣甜蜜值达到3344，月老特侧情侣专属称号一对", 0, maleReward);
			
			local femaleReward = Sys:GetRewardGroup();
			femaleReward:AddReward(32060, 1, 1, 0);
			Sys:SendSysMail(scene:GetParam("female_id"), "新婚之喜", "恭喜您和您的伴侣甜蜜值达到3344，月老特侧情侣专属称号一对", 0, femaleReward);
		end
		sweet_degree = sweet_degree + degree;
		scene:SetParam("sweet_degree", sweet_degree);
		scene:SyncCopySceneParam("sweet_degree", sweet_degree);
		return true;
	end

	--非活动烟花允许燃放
	if itemId ~= 42008 then
		return true;
	end
	if Sys:IsInActivity("y_cuicanjinxiao") then
		if player:GetPosX() >= 115 and player:GetPosX() <= 141 then
			if player:GetPosY() >= 90 and player:GetPosY() <= 119 then
				player:SendCounterToWorld(0, 1);
				local rand = Sys:RandBetween(1, 100);
				if rand <= 3 then
					Sys:BroadcastNotify(3, 7, "{T 3|" .. player:GetName() .. "}在燃放烟花时意外得捡到新年礼盒*1");
					local group = player:GetRewardGroup();
					group:AddReward(32015, 1, 1, 0);
					Sys:SendSysMail(player:GetID(), "新年礼包", "燃放烟花新年礼盒", 0 , group);
				end
				return true
			end
		end
	end
	player:Notify(4, 0, "2月6～12日晚18：00-23：59可在【陈都】摇钱树附近使用");
	return false;
end

--送玫瑰
function  OnGivedFriendFlower(player, friendId, frienName, itemId)
	player:RemoveItem("give_flower", itemId, 1);
	
	if itemId == 426 or itemId == 427 then
		Sys:BroadcastEffectEvent(1);
	end
	--全服送玫瑰
	if Sys:IsInActivity("y_flower") then
		local group = player:GetRewardGroup();
		if itemId == 425 then
			group:AddReward(32018, 1, 1, 0);
			Sys:SendSysMail(player:GetID(), "全服送玫瑰", "您赠送了9朵玫瑰，特此赠送爱情礼盒x1个，请及时收取附件。祝您游戏愉快！", 0, group);
		elseif itemId == 426 then
			group:AddReward(32018, 2, 1, 0);
			Sys:SendSysMail(player:GetID(), "全服送玫瑰", "您赠送了99朵玫瑰，特此赠送爱情礼盒x2个，请及时收取附件。祝您游戏愉快！", 0, group);
			--Sys:BroadcastEffectEvent(1);
		elseif itemId == 427 then
			group:AddReward(32018, 3, 1, 0);
			Sys:SendSysMail(player:GetID(), "全服送玫瑰", "您赠送了999朵玫瑰，特此赠送爱情礼盒x3个，请及时收取附件。祝您游戏愉快！", 0, group);
			--Sys:BroadcastEffectEvent(1);
		end
	end
	
	local qingsiNum = 0;
	local flower_num = 0;
	if itemId == 424 then 
		flower_num = 1;
	elseif itemId == 425 then
		flower_num = 9;
		if Sys:SelectInHundred(15) then
			qingsiNum = 1;
		end
	elseif itemId == 426 then
		flower_num = 99;
		qingsiNum = 1;
		if Sys:SelectInHundred(45) then
			qingsiNum = qingsiNum + 1;
		end
	elseif itemId == 427 then
		flower_num = 999;
		qingsiNum = 10;
	end
	if qingsiNum ~= 0 then
		if player:CheckAddItem(36007, qingsiNum, 1, 0) then
			player:AddItem("give_flower", 36007, qingsiNum, 1, 0);
		else
			local group = player:GetRewardGroup();
			group:AddReward(36007, qingsiNum, 1, 0);
			Sys:SendSysMail(player:GetID(), "送玫瑰获情丝", "您赠送了" .. flower_num .. "朵玫瑰，情丝" .. qingsiNum .. "个，请及时收取附件。祝您游戏愉快！", 0, group);
		end
	end
	
	Sys:BroadcastNotify(3, 7, "{T 3|" .. player:GetName() .. "}送给{T 3|" .. frienName .. "}玩家{T 3|" .. flower_num .. "}朵鲜花，真是羡煞旁人！");
	Sys:BroadcastNotify(11, 0, "{T 3|" .. player:GetName() .. "}送给{T 3|" .. frienName .. "}玩家{T 3|" .. flower_num .. "}朵鲜花，真是羡煞旁人！");
	player:AddIntimacy(friendId, flower_num);
	--全服魅力值
	if Sys:IsInActivity("y_charm") then
		player:IncCounter("flower_num", flower_num % 11);
		local charm = flower_num / 11;
		if player:GetCounter("flower_num") > 11 then
			charm = charm + player:GetCounter("flower_num") / 11;
			player:SetCounter("flower_num", player:GetCounter("flower_num") % 11);
		end
		player:SendCounterToWorld(1, charm);
	end
	--送玫瑰排行榜
	if Sys:IsInActivity("y_flower_rank") then
		player:IncCounter("give_flower", flower_num);
		Sys:OnSortValueChanged(21, player:GetID(), player:GetCounter("give_flower"));
	end	
end

function  OnPassedCopyScene(player, mapid)
	local mapids = { 309, 310, 401, 403, 402, 501, 503, 506 };

	for loopi = 1, table.getn(mapids) do
		if mapid == mapids[loopi] then
			OnGiveActivityReward(player);
		end
	end
end

--上婚车
function TakeOnMarriageVehicle(player, lover, type)
	local scene = player:GetScene();
	
	local subname = player:GetName() .. "和" .. lover:GetName() .. "的婚车";
	if type == 1 then
		local head = scene:SummonNpc(103061, 88, 132, 1, 0);
		if head == nil then
			return false
		end
		local vehicle = scene:SummonVehicle(103059, 88, 134, 1, player);
		if vehicle == nil then
			head:Disappear();
			return false;
		end
		vehicle:GetAI():SetParam("marry_type", type);
		vehicle:SetSubName(subname);
		vehicle:AddBuff(3635, 0, 0);
		vehicle:TakeOn(player);
		vehicle:TakeOn(lover);
		vehicle:GetAI():Follow(head, 3);
		
	elseif type == 2 then
	
		local head = scene:SummonNpc(103061, 88, 132, 1, 0);
		if head == nil then
			return false
		end
		local vehicle = scene:SummonVehicle(103059, 88, 134, 1, player);
		if vehicle == nil then
			head:Disappear();
			return false;
		end
		vehicle:GetAI():SetParam("marry_type", type);
		vehicle:SetSubName(subname);
		vehicle:AddBuff(3635, 0, 0);
		local tail = scene:SummonNpc(103060, 88, 138, 1, 0);
		if tail == nil then
			head:Disappear();
			vehicle:Disappear();
			return false;
		end
		vehicle:TakeOn(player);
		vehicle:TakeOn(lover);
		
		vehicle:GetAI():Follow(head, 3);
		tail:GetAI():Follow(vehicle, 3);
		
	elseif type == 3 then
		local head1 = scene:SummonNpc(103056, 88, 130, 1, 0);
		if head1 == nil then
			return false
		end
		local head2 = scene:SummonNpc(103055, 88, 133, 1, 0);
		if head2 == nil then
			head1:Disappear();
			return false;
		end
		local head3 = scene:SummonNpc(103055, 88, 135, 1, 0);
		if head3 == nil then
			head1:Disappear();
			head2:Disappear();
			return false;
		end
		local vehicle = scene:SummonVehicle(103058, 88, 137, 1, player);
		if vehicle == nil then
			head1:Disappear();
			head2:Disappear();
			head3:Disappear();
			return false;
		end
		vehicle:GetAI():SetParam("marry_type", type);
		vehicle:SetSubName(subname);
		vehicle:AddBuff(3635, 0, 0);
		local tail = scene:SummonNpc(103057, 88, 141, 1, 0);
		if tail == nil then
			head1:Disappear();
			head2:Disappear();
			head3:Disappear();
			vehicle:Disappear();
			return false;
		end
		vehicle:TakeOn(player);
		vehicle:TakeOn(lover);
		
		head2:GetAI():Follow(head1, 2);
		head3:GetAI():Follow(head2, 2);
		vehicle:GetAI():Follow(head3, 4);
		tail:GetAI():Follow(vehicle, 4);
		
	end
	return true;
end

--使用vip卡
function OnUseVipCard(player, itemId)
	local group = Sys:GetRewardGroup();
	if itemId == 402 then
		group:AddReward(425, 1, 1, 0);
		Sys:SendSysMail(player:GetID(), "恭喜成为蓝福神", "恭喜你成为蓝福神，特赠送9朵玫瑰（玫瑰可以赠送给好友增加亲密度）", 0, group);
	elseif itemId == 403 then
		group:AddReward(426, 1, 1, 0);
		Sys:SendSysMail(player:GetID(), "恭喜成为紫福神", "恭喜你成为紫福神，特赠送99朵玫瑰（玫瑰可以赠送给好友增加亲密度）", 0, group);
		Sys:BroadcastNotify(3, 7, "{T 3|" .. player:GetName() .. "}领先一步，使用了{T 6|紫福神卡}, 炎黄征程处处好运气！");
		Sys:BroadcastNotify(11, 0, "{T 3|" .. player:GetName() .. "}领先一步，使用了{L event:showUI 6 vipView|紫福神卡}, 炎黄征程处处好运气！");
	elseif itemId == 404 then
		group:AddReward(427, 1, 1, 0);
		Sys:SendSysMail(player:GetID(), "恭喜成为金福神", "恭喜你成为金福神，特赠送999朵玫瑰（玫瑰可以赠送给好友增加亲密度）", 0, group);
		Sys:BroadcastNotify(3, 7, "{T 3|" .. player:GetName() .. "}一马当先，使用了{T 7|金福神卡}，炎黄征程处处好运气！");
		Sys:BroadcastNotify(11, 0, "{T 3|" .. player:GetName() .. "}一马当先，使用了{L event:showUI 7 vipView|金福神卡}，炎黄征程处处好运气！");
	end
end

	