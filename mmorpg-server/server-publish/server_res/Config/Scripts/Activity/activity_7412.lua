require "Debug"
require "Task_Define"

local var_condition = 10
--7�ջ Day4
--�Ƕ�������
function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindDayChange();
    task:BindPk();
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
    task:SetStatusNoSync(TASK_UNFINISH); 	

    local winNum = player:GetTotalWinNum(1);
    if winNum >= var_condition then
    	task:SetVarNoSync("k", var_condition);
    	task:SetStatusNoSync(TASK_FINISHED);
    else
    	task:SetVarNoSync("k", winNum);
    end
end

function OnSubmitted(player, task, bRp)		---	��ȡ
	if task:GetStatus() ~= TASK_FINISHED then return false end

	task:SetStatus(TASK_SUBMITTING);	
    return true
end

function OnPk(player, task)
	if task:GetStatus() ~= TASK_UNFINISH then return false end

	local winNum = player:GetTotalWinNum(1);
    if winNum >= var_condition then
    	task:SetVar("k", var_condition);
    	task:SetStatus(TASK_FINISHED);
    else
    	task:SetVar("k", winNum);
    end
end

function OnDayChange(player, task, preTask)		---	����
	local createFromNowOn = player:GetCreateFromNowOn()
	local day = createFromNowOn / (24 * 3600)

	--�������
	if day >= 3 then
		OnPk(player, task)
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
