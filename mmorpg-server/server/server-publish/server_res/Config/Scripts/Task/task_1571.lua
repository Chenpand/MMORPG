function Init(task)				---绑定(注册)该任务需要关注的事件
	task:BindKillNpcEvent(509, 602000)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(509, 602001)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(509, 602002)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(509, 602003)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(510, 602000)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(510, 602001)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(510, 602002)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(510, 602003)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(511, 602000)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(511, 602001)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(511, 602002)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(511, 602003)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(512, 602000)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(512, 602001)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(512, 602002)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(512, 602003)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(513, 602000)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(513, 602001)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(513, 602002)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(513, 602003)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(835, 602000)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(835, 602001)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(835, 602002)	 -- 关注杀死指定怪物事件
	task:BindKillNpcEvent(835, 602003)	 -- 关注杀死指定怪物事件

    task:BindGetItemEvent(400000001)		---关注获得道具事件。
	
	
end

function CheckAccept(player)		---	检查接受条件，无条件直接返回return true
	return true;
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
	local itemnum = player:GetItemNum(400000001);
			task:SetVar("parameter", itemnum);	
		if itemnum >= 1500 then
			task:SetStatus(2);
		end	
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
	local itemnum = task:GetVar("parameter");	---	得到变量值
		if(itemnum >= 1500) then           -- 检查道具数量是否足够
			task:SetStatus(4);
			player:RemoveItem(task:GetRewardsReason(), 400000001, 1500);        ---	删除变量值的道具
		end
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnKillNpc(player, task, npcid, npctype, dungeonId)		---	任务过程中杀死NPC时需要执行的操作
	local dungeonType = dungeonId % 10
	if dungeonType == 0 then
		if npcid == 835 then
			task:AddTaskItem(400000001, 100);					-- 添加任务道具
			task:AddTaskItem(400000001, 100);					-- 添加任务道具
			task:AddTaskItem(400000001, 100);					-- 添加任务道具
			task:AddTaskItem(400000001, 100);					-- 添加任务道具

			task:AddTaskItem(400000001, 60);					-- 添加任务道具
			task:AddTaskItem(400000001, 30);					-- 添加任务道具
			task:AddTaskItem(400000001, 10);					-- 添加任务道具
		else 
			task:AddTaskItem(400000001, 60);					-- 添加任务道具
			task:AddTaskItem(400000001, 30);					-- 添加任务道具
			task:AddTaskItem(400000001, 10);					-- 添加任务道具
		end
	elseif dungeonType == 1 then
		if npcid == 835 then
			task:AddTaskItem(400000001, 100);					-- 添加任务道具
			task:AddTaskItem(400000001, 100);					-- 添加任务道具
			task:AddTaskItem(400000001, 100);					-- 添加任务道具
			task:AddTaskItem(400000001, 100);					-- 添加任务道具

			task:AddTaskItem(400000001, 75);					-- 添加任务道具
			task:AddTaskItem(400000001, 55);					-- 添加任务道具
			task:AddTaskItem(400000001, 35);					-- 添加任务道具
			task:AddTaskItem(400000001, 20);					-- 添加任务道具
		else 
			task:AddTaskItem(400000001, 75);					-- 添加任务道具
			task:AddTaskItem(400000001, 55);					-- 添加任务道具
			task:AddTaskItem(400000001, 35);					-- 添加任务道具
			task:AddTaskItem(400000001, 20);					-- 添加任务道具
		end
	elseif dungeonType == 2 then
		if npcid == 835 then
			task:AddTaskItem(400000001, 100);					-- 添加任务道具
			task:AddTaskItem(400000001, 100);					-- 添加任务道具
			task:AddTaskItem(400000001, 100);					-- 添加任务道具
			task:AddTaskItem(400000001, 100);					-- 添加任务道具
			task:AddTaskItem(400000001, 100);					-- 添加任务道具

			task:AddTaskItem(400000001, 100);					-- 添加任务道具
			task:AddTaskItem(400000001, 80);					-- 添加任务道具
			task:AddTaskItem(400000001, 50);					-- 添加任务道具
			task:AddTaskItem(400000001, 20);					-- 添加任务道具
		else 
			task:AddTaskItem(400000001, 100);					-- 添加任务道具
			task:AddTaskItem(400000001, 80);					-- 添加任务道具
			task:AddTaskItem(400000001, 50);					-- 添加任务道具
			task:AddTaskItem(400000001, 20);					-- 添加任务道具
		end
	else
		if npcid == 835 then
			task:AddTaskItem(400000001, 100);					-- 添加任务道具
			task:AddTaskItem(400000001, 100);					-- 添加任务道具
			task:AddTaskItem(400000001, 100);					-- 添加任务道具
			task:AddTaskItem(400000001, 100);					-- 添加任务道具
			task:AddTaskItem(400000001, 100);					-- 添加任务道具

			task:AddTaskItem(400000001, 95);					-- 添加任务道具
			task:AddTaskItem(400000001, 85);					-- 添加任务道具
			task:AddTaskItem(400000001, 75);					-- 添加任务道具
			task:AddTaskItem(400000001, 50);					-- 添加任务道具
			task:AddTaskItem(400000001, 30);					-- 添加任务道具
		else 
			task:AddTaskItem(400000001, 95);					-- 添加任务道具
			task:AddTaskItem(400000001, 85);					-- 添加任务道具
			task:AddTaskItem(400000001, 75);					-- 添加任务道具
			task:AddTaskItem(400000001, 50);					-- 添加任务道具
			task:AddTaskItem(400000001, 30);					-- 添加任务道具
		end
	end



end

function OnGetItem(player, task, itemid, num)		---	任务过程中获得道具时需要执行的操作
	local itemnum = task:GetVar("parameter");		---	获取变量, 特别注意：任务变量的名字要和任务表里面配置的名字一致！！！！
	itemnum = itemnum + num;					---	设定函数公式
	task:SetVar("parameter",itemnum);				---	设置变量新值
	if itemnum >= 1500 then						---	判断结束条件
		task:SetStatus(2)
	end
end

function OnRmItem(player, task, itemid, num)
	local itemnum = task:GetVar("parameter");
	itemnum = itemnum - num;
	if(itemnum < 0) then
		itemnum = 0;
	end
	task:SetVar("parameter",itemnum);

	if(task:GetStatus() == 2 and itemnum < 1500) then		-- 如果删除一定数量的道具后不满足完成任务的条件，则设置该任务为未完成状态
			task:SetStatus(1);
	end
end	