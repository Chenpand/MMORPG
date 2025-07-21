require "Scene_Base"
setfenv(1, PARENT_ENV);

--关闭超时
COPYSCENE_END_TIMEOUT = 5 * 60;


--副本状态进行中
COPYSCENE_STATUS_NORMAL = 0;
--副本状态通关
COPYSCENE_STATUS_PASSED = 1;


--通知新副本剩余时间
function NotifyCopySceneLeftTime(scene, player)
	local now = Sys:Now() / 1000;
	local closeTime = scene:GetCloseTime();
	if closeTime > now then
		player:SyncCopySceneLeftTime(closeTime - now);
	end
end

--玩家进入场景
function OnPlayerEnter(player)
	local scene = player:GetScene();
	NotifyCopySceneLeftTime(scene, player);
	player:SyncCopySceneParam("boss_num", scene:GetAliveNpcNumByType(4));
	player:SyncCopySceneParam("monster_num", scene:GetAliveNpcNumByType(2));
	return;
end

--玩家离开场景
function OnPlayerLeave(player)
	return;
end

--通知副本关闭
function NotifyEndVisitor(player)
	local scene = player:GetScene();
	local end_min = scene:GetParam("notifyend_time");
	player:Notify(3, 5, "{T 3|" .. scene:GetName() .. "}副本将会在" .. end_min .. "分钟后自动关闭");
end

--心跳
function OnTick(scene, now)
	local closeTime = scene:GetCloseTime();
	local nowTime = now / 1000;
	
	if closeTime > nowTime then
		local leftMin = math.floor((closeTime - nowTime - 1) / 60) + 1;
		local notifyend_time = scene:GetParam("notifyend_time");
		if leftMin ~= notifyend_time and leftMin < 11 then
			scene:SetParam("notifyend_time", leftMin);
			scene:VisitPlayers("NotifyEndVisitor");
		end
	end
	
	local status = scene:GetParam("status");
	if status == COPYSCENE_STATUS_PASSED then
		if scene:GetPlayerNum() == 0 and scene:GetWaitPlayerNum() == 0 then
			scene:UnLoad();
		end
	end
end

--通关
function OnScenePassed(player)
	local scene = player:GetScene();
	NotifyCopySceneLeftTime(scene, player);
	
	if scene:GetMapID() == 401 then
		player:SendActUdpLog("passcopy", "401", 1);
	end
end

--怪物死亡
function OnNpcDied(npc)
	local scene = npc:GetScene();
	local bossNum = scene:GetAliveNpcNumByType(4);
	local monsterNum = scene:GetAliveNpcNumByType(2);
	
	if npc:GetNpcType() == 4 then
		scene:SyncCopySceneParam("boss_num", bossNum);
	elseif npc:GetNpcType() == 2 then
		scene:SyncCopySceneParam("monster_num", monsterNum);
	end
	
	if bossNum == 0 and monsterNum == 0 then
		local now = Sys:Now();
		scene:SetCloseTime(now / 1000 + COPYSCENE_END_TIMEOUT);
		scene:SetParam("status", COPYSCENE_STATUS_PASSED);
		scene:VisitPlayers("OnScenePassed");
	end
end


