--初始npcid
INIT_NPCID = 103010
INIT_MAPID = 103

--最大环数
MAX_LOOP_NUM = 20

--任务npc列表
local	NPC_LIST = 
	{
		{ 103010, 103 },
		{ 103003, 103 },
		{ 103007, 103 },
		{ 105003, 118 },
		{ 105006, 118 },
		{ 105010, 105 },
		{ 106006, 106 },
		{ 106007, 106 },
		{ 106009, 106 },
		{ 106011, 106 },
		{ 107001, 107 },
		{ 107002, 107 },
		{ 107007, 107 },
		{ 107008, 107 },
		{ 108001, 108 },
		{ 108002, 108 },
		{ 108006, 108 },
		{ 108008, 108 },
		{ 109001, 109 },
		{ 109004, 109 },
		{ 109005, 109 }
	};

--任务怪物列表
local	MONSTER_LIST = 
	{
		{ 105201, 118 },
		{ 105202, 118 },
		{ 105203, 118 },
		{ 105204, 105 },
		{ 105205, 105 },
		{ 105206, 105 },
		{ 106201, 106 },
		{ 106202, 106 },
		{ 106203, 106 },
		{ 106204, 106 },
		{ 106205, 106 },
		{ 106206, 106 },
		{ 107201, 107 },
		{ 107202, 107 },
		{ 107203, 107 },
		{ 107204, 107 },
		{ 107205, 107 },
		{ 107206, 107 },
		{ 108201, 108 },
		{ 108202, 108 },
		{ 108203, 108 },
		{ 108204, 108 },
		{ 108205, 108 },
		{ 108206, 108 },
		{ 109201, 109 },
		{ 109202, 109 },
		{ 109203, 109 },
		{ 109204, 109 },
		{ 109205, 109 }
	};
	
--采集物列表
local	GATHER_LIST = 
	{
		{ 105401, 118 },
		{ 105402, 118 },
		{ 105403, 105 },
		{ 106401, 106 },
		{ 106402, 106 },
		{ 106403, 106 },
		{ 107401, 107 },
		{ 107402, 107 },
		{ 107403, 107 },
		{ 108401, 108 },
		{ 108402, 108 },
		{ 108403, 108 },
		{ 109401, 109 },
		{ 109402, 109 },
		{ 109403, 109 }
	};
	
--获取功能npc的地图
function LoopTask_GetNpcMapId(npcid)
	for i = 1,table.getn(NPC_LIST) do
		if NPC_LIST[i][1] == npcid then
			return NPC_LIST[i][2];
		end
	end
	return 0;
end

--获取怪物的地图
function LoopTask_GetMonsterMapId(npcid)
	for i = 1,table.getn(MONSTER_LIST) do
		if MONSTER_LIST[i][1] == npcid then
			return MONSTER_LIST[i][2];
		end
	end
	return 0;
end

--获取采集的地图
function LoopTask_GetGatherMapId(npcid)
	for i = 1,table.getn(GATHER_LIST) do
		if GATHER_LIST[i][1] == npcid then
			return GATHER_LIST[i][2];
		end
	end
	return 0;
end
	
--初始杀怪环任务
function LoopTask_InitKillTask(task)
	for i = 1,table.getn(MONSTER_LIST) do
		task:BindKillNpcEvent(MONSTER_LIST[i][1]);
	end
end

--初始采集任务
function LoopTask_InitGatherTask(task)
	for i = 1,table.getn(GATHER_LIST) do
		task:BindKillNpcEvent(GATHER_LIST[i][1]);
	end
end

--初始设置安抚四方
function LoopTask_Init(task)
	task:SetVar("sceneId", 	INIT_MAPID);
	task:SetVar("npcId", 	INIT_NPCID);
	task:SetVar("NPCID", 	INIT_NPCID);
end

--随机下一环任务
function LoopTask_SelectNextTask(player ,npcid, task)

	local visit_seed = 	table.getn(NPC_LIST) - 1;
	local kill_seed = 	visit_seed + table.getn(MONSTER_LIST);
	local gather_seed =	kill_seed + table.getn(GATHER_LIST);
	
	local seed = Sys:RandBetween(1, gather_seed);
	if seed <= visit_seed then  	--访问任务
		--获取随机列表
		local rand_list = {};
		for i=1,table.getn(NPC_LIST) do
			local npc_id = NPC_LIST[i][1];
			if npc_id ~= npcid then
				table.insert(rand_list, npc_id);
			end
		end
		if table.getn(rand_list) > 0 then
			local index = Sys:RandBetween(1, table.getn(rand_list));
			task:SetVar("nextnpc", rand_list[index]);
			task:SetVar("nexttask", 4601);
			return 4601;
		end
	elseif seed <= kill_seed then	--杀怪任务
		--获取随机列表
		local rand_list = {};
		for i=1, table.getn(MONSTER_LIST) do
			local monster_id = MONSTER_LIST[i][1];
			local level = Sys:GetNpcLevel(monster_id);
			if level + 5 >= player:GetLevel() and level <= player:GetLevel() + 5 then
				table.insert(rand_list, monster_id);
			end
		end
		if table.getn(rand_list) > 0 then
			local index = Sys:RandBetween(1, table.getn(rand_list));
			task:SetVar("nextnpc", rand_list[index]);		
			task:SetVar("nexttask", 4602);
			return 4602;
		end
	else							--采集任务
		local index = Sys:RandBetween(1, table.getn(GATHER_LIST));
		task:SetVar("nextnpc", GATHER_LIST[index][1]);
		task:SetVar("nexttask", 4603);
		return 4603;
	end
	
	return 0;
end

--普通对话
function LoopTask_OnNormalTalk(dialog, player)
	--[[
	local task = player:FindTask(4600);
	if task ~= nil then
		local next_taskid = task:GetVar("nexttask");
		if next_taskid == 0 then
			LoopTask_SelectNextTask(player, dialog:GetOwnerId(), task);
			next_taskid = task:GetVar("nexttask");
		end
		local loop_task = player:FindTask(next_taskid);
		if loop_task == nil then 	--加接取任务选项
			if task:GetVar("curloop") < player:GetDailyTaskMaxNum(8) then
				local npcId = task:GetVar("npcId");
				if npcId == dialog:GetOwnerId() then
					dialog:AddTaskOption(next_taskid, 0, "安抚四方【魂念】");
				end	
			end
		else						--其他任务选项
			local npcId = loop_task:GetVar("NPCID1");
			if npcId == dialog:GetOwnerId() then
				dialog:AddTaskOption(next_taskid, loop_task:GetStatus(), "安抚四方【魂念】");
			end
		end
	end
	--]]
end

--发布任务对话
function LoopTask_OnPublishTalk(dialog, player, taskid)
	if taskid == 4601 then
		dialog:AddWord("我是大王派在此地的探马之一，我们需要时常联系，但我又不能离开这里，你能帮我联系一下其他人吗？");
		dialog:AddReply("这还不容易？我马上去！");
		dialog:AddReward();
	elseif taskid == 4602 then
		dialog:AddWord("我是大王派在此地的探马之一，我们不但要打探消息，还要维护地方的安定，但最近有怪物危害地方……");
		dialog:AddReply("这还了得，我去除掉他们！");
		dialog:AddReward();
	elseif taskid == 4603 then
		dialog:AddWord("我是大王派在此地的探马之一，有时候我们为了打探消息不得不进行一些伪装，这就需要很多道具，你能不能帮忙？");
		dialog:AddReply("当然可以，我帮你采集去。");
		dialog:AddReward();
	end
end

--未完成对话
function LoopTask_OnUnfinishTalk(dialog, player, taskid)
	if taskid == 4601 then
		dialog:AddWord("我可不认识你，你别乱说。");
		dialog:AddReply("离开。");
	elseif taskid == 4602 then
		dialog:AddWord("据我所知还有怪物在危害地方呢。");
		dialog:AddReply("我会把他们都除掉的！");
	elseif taskid == 4603 then
		dialog:AddWord("我需要一定数量的道具进行伪装，你不是要帮忙？");
		dialog:AddReply("是，我是要帮忙，你等着我回来。");
	end
end

--提交任务对话
function LoopTask_OnSubmitTalk(dialog, player, taskid)
	if taskid == 4601 then
		dialog:AddWord("唉，最近虽然打探到了不少消息，但都是大王不感兴趣的。");
		dialog:AddReply("消息只看有没有用，怎么能看自己喜欢不喜欢？");
		dialog:AddReward();
	elseif taskid == 4602 then
		dialog:AddWord("怪物已经不见了，百姓重新安定下来了，真是多谢你了。");
		dialog:AddReply("这都是我该做的。");
		dialog:AddReward();
	elseif taskid == 4603 then
		dialog:AddWord("唔，有了你采集来的这些道具我再准备准备就可以进行伪装了。");
		dialog:AddReply("到时候别忘了叫我啊，我也看看这伪装和变身有什么不同。");
		dialog:AddReward();
	end
end

--检查是否可发布环任务
function LoopTask_CheckPublishTask(player, npcid, taskid)
	local task = player:FindTask(4600);
	if task ~= nil and task:GetVar("curloop") < player:GetDailyTaskMaxNum(8) then
		local next_taskid = task:GetVar("nexttask");
		if next_taskid ~= taskid then
			return false;
		end
		local loop_task = player:FindTask(next_taskid);
		if loop_task == nil and task:GetVar("npcId") == npcid then
			return true;
		end
	end
	return false;
end

--检查是否可提交环任务
function LoopTask_CheckSubmitTask(player, npcid, taskid)
	local task = player:FindTask(4600);
	if task ~= nil then
		local next_taskid = task:GetVar("nexttask");
		if next_taskid ~= taskid then
			return false;
		end
		local loop_task = player:FindTask(next_taskid);
		if loop_task ~= nil and loop_task:GetVar("NPCID1") == npcid then
			return true;
		end
	end
	return false;
end

--提交任务
function LoopTask_OnSubmitted(player, looptask, task)
	
	local cur_loopnum = looptask:GetVar("curloop");
	looptask:SetVar("award", task:GetSoul());
	local sub_time = looptask:GetVar("submit_time");
	local now = Sys:Now();
	if not Sys:IsSameDay(sub_time, now/1000) and cur_loopnum ~= 0 then
		cur_loopnum = 0;
		looptask:SetVar("curloop", 0);
	elseif cur_loopnum < player:GetDailyTaskMaxNum(8) then
		cur_loopnum = cur_loopnum + 1;
		looptask:SetVar("curloop", cur_loopnum);		
	end
	looptask:SetVar("submit_time", now / 1000);
	
	if cur_loopnum == 0 or cur_loopnum >= player:GetDailyTaskMaxNum(8) then
		LoopTask_Init(looptask);
		LoopTask_SelectNextTask(player, INIT_NPCID, looptask);
	else
		local npc_id = task:GetVar("NPCID1");
		local map_id = LoopTask_GetNpcMapId(npc_id);
		
		looptask:SetVar("sceneId", 	map_id);
		looptask:SetVar("npcId", 	npc_id);
		looptask:SetVar("NPCID", 	npc_id);
		LoopTask_SelectNextTask(player, npc_id, looptask);
		
		local npc = player:GetScene():FindNpc(player:GetSelectObject());
		if npc ~= nil and npc:GetDataID() == npc_id then
			npc:ShowTaskDialog(player, looptask:GetVar("nexttask"), 0);
		end
	end
end
