require "Debug"
function Init(task)				---绑定(注册)该任务需要关注的事件
    task:BindGetItemEvent(400000015)		---关注获得道具事件。
    task:BindGetItemEvent(400000016)		---关注获得道具事件。
end

function OnAccepted(player, task)			---	接受任务时需要执行的操作
	local item1Num = player:GetItemNum(400000015);
	local item2Num = player:GetItemNum(400000016);
			task:SetVar("item1", item1Num);	
			task:SetVar("item2", item2Num);
			task:SetVar("submitCount", 5);

        task:SetStatus(1);
		if item1Num >= 5 and item2Num >= 5 then
			task:SetStatus(2);
		end	
end

function OnSubmitted(player, task)		---	提交任务时需要执行的操作
		local item1Num = player:GetItemNum(400000015);
        local item2Num = player:GetItemNum(400000016);
        
		if(item1Num < 5 or item2Num < 5) then
			return false;
		end

        local submitCount = task:GetVar("submitCount") - 1;

		task:SetVar("submitCount", submitCount);
       

		--兑换数量到了
		if submitCount < 0 then	
			task:SetStatus(4);	--兑换数量到了,设置为提交中
		else
			task:SetStatus(1);			-- 兑换数量没到,继续设置为已接受
		end

        player:RemoveItem("16001;1000;0",400000015, 5);        ---  删除变量值的道具
        player:RemoveItem("16001;1000;0",400000016, 5);        ---  删除变量值的道具

        return true
end

function OnAbandomed(player, task)		---	放弃任务时需要执行的操作
end

function OnGetItem(player, task, itemid, num)		---	任务过程中获得道具时需要执行的操作
	local item1Num = player:GetItemNum(400000015);
	local item2Num = player:GetItemNum(400000016);

	if 400000015 == itemid then
		task:SetVar("item1",item1Num);
	elseif 400000016 == itemid then
		task:SetVar("item2",item2Num);
	end

	local submitCount = task:GetVar("submitCount");

	if item1Num >= 5 and item2Num >= 5 and submitCount > 0 then						---	判断结束条件
		task:SetStatus(2)
        return
	end

    if item1Num < 5 and item2Num < 5 and submitCount > 0 then                     --- 判断结束条件
        task:SetStatus(2)
        return
    end

    if submitCount <= 0 then 
        task:SetStatus(5)
        return
    end
end


function OnRmItem(player, task, itemid, num)
	local item1Num = player:GetItemNum(400000015)
    local item2Num = player:GetItemNum(400000016)

    if item1Num < 0 then item1Num = 0 end
    if item2Num < 0 then item2Num = 0 end
    local submitCount = task:GetVar("submitCount");

	if 400000015 == itemid then
		task:SetVar("item1",item1Num);
	elseif 400000016 == itemid then
		task:SetVar("item2",item2Num);
	end

	if(task:GetStatus() == 2 and item1Num < 5 and item2Num < 5) then		-- 如果删除一定数量的道具后不满足完成任务的条件，则设置该任务为未完成状态
			task:SetStatus(1);
            print("task:SetStatus(1)")
            return
	end

    if item1Num >= 5 and item2Num >= 5 and submitCount > 0 then                     --- 判断结束条件
        task:SetStatus(2)
        print("task:SetStatus(2)")
        return
    end

    if submitCount <= 0 then 
        task:SetStatus(5)
        print("task:SetStatus(5)")
        return
    end
end	