require "Debug"
require "Task_Define"

local condition = 2000
local nextCondition = 0
--累计在线活动任务
function Init(task)				---绑定(注册)该任务需要关注的事件
  task:BindCharge();
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
  task:SetStatusNoSync(TASK_UNFINISH);	

  task:SetVar("cur", 0);
	task:SetVar("next", condition);
end

function OnSubmitted(player, task, bRp)		---	领取
	if task:GetStatus() ~= TASK_FINISHED then return false end
	local ret = AddReward(player);
	if ret == false then return false end;
	task:SetStatus(TASK_SUBMITTING);
	
  return true
end

function OnCharge(player, task, value)
	if task:GetStatus() ~= TASK_UNFINISH then return end 
	local curValue = task:GetVar("cur");
	curValue = curValue + value;

	if curValue >= condition then
		task:SetStatus(TASK_FINISHED);
		curValue = condition;	
	end 

	task:SetVar("cur", curValue);
	task:SetVar("next", condition - curValue);
end

function AddReward(player)
	local job = math.ceil(player:GetOccu()/10)	
	local giftpack = {}
		if job == 5 then
			giftpack = {item1 = {id=200000312,num=1}, 
			item2 = {id=330000001,num=60}, item3 = {id=200000002,num=100},
			item4 = {id=600000007,num=300000}}		
		elseif job == 1 then
			giftpack = {item1 = {id=200000312,num=1}, 
			item2 = {id=330000001,num=60}, item3 = {id=200000002,num=100},
			item4 = {id=600000007,num=300000}}
		elseif job == 3 then
			giftpack = {item1 = {id=200000312,num=1}, 
			item2 = {id=330000001,num=60}, item3 = {id=200000002,num=100},
			item4 = {id=600000007,num=300000}}		
		elseif job == 2 then
			giftpack = {item1 = {id=200000312,num=1}, 
			item2 = {id=330000001,num=60}, item3 = {id=200000002,num=100},
			item4 = {id=600000007,num=300000}}
		end
		local group = player:GetRewardGroup()
		for k,v in pairs(giftpack) do
			group:AddReward(v.id, v.num)
		end
		
		local canAdd = player:CheckAddRewards(group)
		if canAdd ~= 0 then
			player:SendNotifyById(1000010)
			return false
		end
		
		player:AddRewards("16003;8407;0", group, true)
		return true
end