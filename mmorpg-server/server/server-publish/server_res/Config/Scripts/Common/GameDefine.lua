
--战场类型定义
--初级武斗场
BATTLESCENE_WUDOU_LOW		= 1;
--中级武斗场
BATTLESCENE_WUDOU_MIDDLE 	= 2;
--高级武斗场
BATTLESCENE_WUDOU_HIGH		= 3;

--部落战场
BATTLESCENE_TRIBE			= 5;
--擂台赛
BATTLESCENE_WORLDCHALLENGE	= 6;
--部落混战战场
BATTLESCENE_TRIBEMELEE		= 7;
--结界BOSS战场
BATTLESCENE_JIEJIE			= 8;

--初级跨服武斗场
BATTLESCENE_WORLD_WUDOU_LOW = 11;
--中级跨服武斗场
BATTLESCENE_WORLD_WUDOU_MIDDLE = 12;
--高级跨服武斗场
BATTLESCENE_WORLD_WUDOU_HIGH = 13;
--终极跨服武斗场
BATTLESCENE_WORLD_WUDOU_FINAL = 14;


--根据战场类型获取战场名
function GetBattleSceneNameByType(battleSceneType)
	if battleSceneType == BATTLESCENE_WUDOU_LOW then
		return "初级武斗场";
	elseif battleSceneType == BATTLESCENE_WUDOU_MIDDLE then
		return "中级武斗场";
	elseif battleSceneType == BATTLESCENE_WUDOU_HIGH then
		return "高级武斗场";
	elseif battleSceneType == BATTLESCENE_TRIBE then
		return "部落宝地战";
	elseif battleSceneType == BATTLESCENE_WORLDCHALLENGE then
		return "跨服擂台赛";
	elseif battleSceneType == BATTLESCENE_TRIBEMELEE then
		return "部落水晶战";
	elseif battleSceneType == BATTLESCENE_JIEJIE then
		return "结界";
	elseif battleSceneType == BATTLESCENE_WORLD_WUDOU_LOW then
		return "初级跨服武斗场";
	elseif battleSceneType == BATTLESCENE_WORLD_WUDOU_MIDDLE then
		return "中级跨服武斗场";
	elseif battleSceneType == BATTLESCENE_WORLD_WUDOU_HIGH then
		return "高级跨服武斗场";
	elseif battleSceneType == BATTLESCENE_WORLD_WUDOU_FINAL then
		return "终极跨服武斗场";
	end
	
	return "战场";
end

--任务状态定义
--初始状态
TASK_INIT 		= 0;
--已接任务状态
TASK_UNFINISH	= 1;
--已完成，未提交状态
TASK_FINISHED 	= 2;
--任务失败状态
TASK_FAILED 	= 3;
