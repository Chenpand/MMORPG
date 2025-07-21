require "Scene_Base"
setfenv(1, PARENT_ENV);

--关卡持续10分钟
CUSTOMS_PASS_TIME = 		600;
--关卡结束超时
CUSTOMS_PASS_END_TIMEOUT = 	30;
--最大通天塔层数
MAX_SIXIANG_LEVEL = 		30;

--副本状态
STATUS_INIT = 0;		--初始状态
STATUS_RUNNING = 1;		--闯关
STATUS_FAILED = 2;		--失败
STATUS_SUCC = 3;		--成功


FENGMO_TOWER = 1;      	--封魔
TONGTIAN_TOWER = 2;		--通天
CHENGSHI_TOWER = 3;		--成神


--提示关闭
function NotifyEndVisitor(player)
	local scene = player:GetScene();
	player:Notify(3, 10, "通天塔副本将会在" .. CUSTOMS_PASS_END_TIMEOUT .. "秒后关闭");
	player:SyncCopySceneLeftTime(CUSTOMS_PASS_END_TIMEOUT);
end

--心跳
function OnTick(scene, now)
	local status = scene:GetParam("status");
	if status == STATUS_FAILED then
		--结束后半分钟踢出玩家
		local endTime = scene:GetParam("end_time");
		if now > endTime + CUSTOMS_PASS_END_TIMEOUT * 1000 then	--踢出玩家
			scene:UnLoad();
			scene:SetParam("status", STATUS_INIT);
		elseif scene:GetPlayerNum() == 0 and scene:GetWaitPlayerNum() == 0 then
			scene:UnLoad();
		end
	elseif status == STATUS_RUNNING then
		--时间到，询问重置
		local beginTime = scene:GetParam("begin_time");
		if now > beginTime + CUSTOMS_PASS_TIME * 1000 then
			scene:VisitPlayers("NotifyEndVisitor");
			scene:SetParam("end_time", now);
			scene:SetParam("status", STATUS_FAILED);
		end
	end
 end



