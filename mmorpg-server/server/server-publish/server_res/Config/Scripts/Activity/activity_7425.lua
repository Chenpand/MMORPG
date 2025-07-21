require "Debug"
require "Task_Define"


local var_condition = 1 	--���ټ�
local needStrLv = 12 		--ǿ���ȼ�

--7�ջ Day4
--װ��ǿ���ȼ�
function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindDayChange();
    task:BindEquipStrenth();
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
    task:SetStatusNoSync(TASK_UNFINISH);

    local str = player:GetStrMaxLv(var_condition, needStrLv);
    if str >= needStrLv then
    	task:SetVarNoSync("k", needStrLv);
    	task:SetStatusNoSync(TASK_FINISHED);
    else
    	task:SetVarNoSync("k", str);
    end
end

function OnSubmitted(player, task, bRp)		---	��ȡ
	if task:GetStatus() ~= TASK_FINISHED then return false end

	task:SetStatus(TASK_SUBMITTING);	
    return true
end

function OnEquipStrenth(player, task)
	if task:GetStatus() ~= TASK_UNFINISH then return false end
	
	local str = player:GetStrMaxLv(var_condition, needStrLv);
    if str >= needStrLv then
    	task:SetVar("k", needStrLv);
    	task:SetStatus(TASK_FINISHED);
    else
    	task:SetVar("k", str);
    end
end

function OnDayChange(player, task, preTask)		---	����
	local createFromNowOn = player:GetCreateFromNowOn()
	local day = createFromNowOn / (24 * 3600)

	--�������
	if day >= 3 then
		OnEquipStrenth(player, task)
	end

	--���ǳ���10���������
	if day > 10 then
		task:SetStatus(TASK_FAILED);
		return
	end

	--���ǳ���7��ֻ����ȡ��������
	if day > 7 then
		if task:GetStatus() ~= TASK_FINISHED and
			task:GetStatus() ~= TASK_OVER then
			task:SetStatus(TASK_FAILED);
		end
	end
end
