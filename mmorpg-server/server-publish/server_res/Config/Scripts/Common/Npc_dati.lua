require "Npc_Base"

local REWARD_ID = 415;
local COM_REWARD = 419;

function Init(script)

end

function OnBirth(script, npc)							--出生的一些处理
	npc:GetAI():SetParam("index", 0);
end

function OnPublishTaskTalk(dialog, player, taskid)	
end

function OnUnfinishTaskTalk(dialog, player, taskid)		

end

function OnSubmitTaskTalk(dialog, player, taskid)
end

function OnNormalTalk(dialog, player)
	local NPC_INDEX = dialog:GetOwner():GetAI():GetParam("index");
	local task = player:FindTask(2200 + NPC_INDEX);
	--有答题任务才能答题
	if task ~= nil then
		local questionid = Sys:GetPlayerTmpData(player:GetID(), "question" .. NPC_INDEX);
		if questionid == 0 then
			dialog:AddWord("error");			
		else
			dialog:AddWord(Sys:GetQuestion(questionid));
			local rand = Sys:RandBetween(1, 3);
			if rand == 1 then
				dialog:AddOption(1, 0, Sys:GetAnswer(questionid, 1));
				dialog:AddOption(2, 0, Sys:GetAnswer(questionid, 2));
				dialog:AddOption(3, 0, Sys:GetAnswer(questionid, 3));
			elseif rand == 2 then
				dialog:AddOption(2, 0, Sys:GetAnswer(questionid, 2));
				dialog:AddOption(3, 0, Sys:GetAnswer(questionid, 3));
				dialog:AddOption(1, 0, Sys:GetAnswer(questionid, 1));
			else
				dialog:AddOption(1, 0, Sys:GetAnswer(questionid, 1));
				dialog:AddOption(3, 0, Sys:GetAnswer(questionid, 3));
				dialog:AddOption(2, 0, Sys:GetAnswer(questionid, 2));
			end
		end
	else
		dialog:AddWord("科举大赛");
		if NPC_INDEX == 1 and player:GetLevel() >= 30 and player:GetCounter("festivalcount") == 0 and Sys:IsInActivity("dati") then
			dialog:AddOption(4, 1, "答题任务");			
		end
	end
end

function OnClickOption(dialog,player,optionid)
	local NPC_INDEX = dialog:GetOwner():GetAI():GetParam("index");
	if optionid == 4 then
		if NPC_INDEX ~= 1 then
			return;
		end
		
		if player:GetLevel() < 30 then
			return;
		end
		
		if player:GetCounter("festivalcount") ~= 0 then
			player:Notify(4, 0, "每天只能接受一次答题任务");
			return;
		end
		
		if not Sys:IsInActivity("dati") then
			player:Notify(4, 0, "活动时间才能接受答题任务");
			return;
		end
		
		player:AcceptTask(2200 + NPC_INDEX);
		player:SetCounter("festivalcount", 1);
		
		--设置答题开始时间
		local now = Sys:Now() / 1000;
		Sys:SetPlayerTmpData(player:GetID(), "que_start_time", now);
		Sys:BroadcastPlayerTmpData(player:GetID(), "que_start_time");
		
		--抽取题目
		Sys:DrawQuestions(player, 20);
		
		player:SendActUdpLog("dati", "accept", 1);
	else
		local task = player:FindTask(2200 + NPC_INDEX);
		if task == nil then
			return;
		end
		
		local anstime = Sys:GetPlayerTmpData(player:GetID(), "anstime");
		local now = Sys:Now() / 1000;
		if now < anstime then
			player:Notify(4, 0, (anstime - now) .. "秒后才能继续答题");
			return;
		end
		
		local questionid = Sys:GetPlayerTmpData(player:GetID(), "question" .. NPC_INDEX);
		if questionid == 0 then
			return;
		else
			local right = Sys:GetRightAnswer(questionid);
			if right == optionid then
				task:SetStatus(2);
				if not player:SubmitTask(2200 + NPC_INDEX) then
					return;
				end
				
				if NPC_INDEX ~= 20 then
					player:AcceptTask(2201 + NPC_INDEX);
				else
					--答完20题后更新排行榜
					local starttime = Sys:GetPlayerTmpData(player:GetID(), "que_start_time");
					local costtime = now - starttime;
					Sys:OnSortValueChanged(15, player:GetID(), costtime, player:GetLevel(), 0);
					
					if player:CheckAddItem(COM_REWARD, 1, 1, 0) then
						player:AddItem("答题活动", COM_REWARD, 1, 1, 0);
					else
						player:Notify(4, 0, "背包已满，礼包将发送到邮箱");
						local npc = dialog:GetOwner();
						if npc ~= nil then
							local npcai = npc:GetAI();
							local rank = npcai:GetParam("ansnum") + 1;
				
							local group = player:GetRewardGroup();
							group:AddReward(COM_REWARD, 1, 1, 0);
							Sys:SendSysMail(player:GetID(), "科举奖励 ", "恭喜您在科举活动中获得第"..rank.."名，获得以下奖励。请及时领取附件，祝您游戏愉快！", 0, group);
						end
					end
					
					player:SendActUdpLog("dati", "pass", 1);
					player:ShowFireworksEffect(1001,10);
				end
				
				if NPC_INDEX % 5 == 0 then
					local npc = dialog:GetOwner();
					if npc ~= nil then
						local npcai = npc:GetAI();
						local rank = npcai:GetParam("ansnum") + 1;
						npcai:SetParam("ansnum", rank);
						
						--前十名额外奖励
						if rank <= 10 then
							if player:CheckAddItem(REWARD_ID, 1, 1, 0) then
								player:AddItem("答题活动", REWARD_ID, 1, 1, 0);
							else
								player:Notify(4, 0, "背包已满，礼包将发送到邮箱");
								
								local group = player:GetRewardGroup();
								group:AddReward(REWARD_ID, 1, 1, 0);
								Sys:SendSysMail(player:GetID(), "科举奖励", "恭喜您在科举活动中获得第"..rank.."名，获得以下奖励。请及时领取附件，祝您游戏愉快！", 0, group);
							end
							
							local starttime = Sys:GetPlayerTmpData(player:GetID(), "que_start_time");
							local costtime = now - starttime;
							if NPC_INDEX == 5 then
								Sys:BroadcastNotify(3, 0, "{T 3|" .. player:GetName() .. "}花了" .. (costtime / 60) .. "分" .. (costtime % 60) .. "秒第" .. rank .. "个通过5号考官，真是文才出众啊！");
							elseif NPC_INDEX == 10 then
								Sys:BroadcastNotify(3, 0, "{T 3|" .. player:GetName() .. "}花了" .. (costtime / 60) .. "分" .. (costtime % 60) .. "秒第" .. rank .. "个通过10号考官，真是满腹经纶啊！");
							elseif NPC_INDEX == 15 then
								Sys:BroadcastNotify(3, 0, "{T 3|" .. player:GetName() .. "}花了" .. (costtime / 60) .. "分" .. (costtime % 60) .. "秒第" .. rank .. "个通过15号考官，真是才华横溢啊！");
							elseif NPC_INDEX == 20 then
								Sys:BroadcastNotify(3, 0, "{T 3|" .. player:GetName() .. "}花了" .. (costtime / 60) .. "分" .. (costtime % 60) .. "秒第" .. rank .. "个通过20号考官，真是盖世之才啊！");
							end
						end
						local group = player:GetRewardGroup();
						group:AddReward(REWARD_ID, 1, 1, 0);
						Sys:SendSysMail(player:GetID(), "科举幸运奖", "恭喜您获得科举幸运奖，特此奖励。请及时领取附件，祝您游戏愉快！", 0, group);
					end
				end
			else
				player:Notify(4, 0, "回答错误，10秒后才能继续答题");
				Sys:SetPlayerTmpData(player:GetID(), "anstime", now + 10);
			end
		end
	end
end
