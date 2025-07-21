require "Debug"
require "Task_Define"

local condition = 25;
--7�ջ Day1
--��������
function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindLevelUpEvent();
    task:BindDayChange();
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
    task:SetStatusNoSync(TASK_UNFINISH);	

    local lv = player:GetLevel();
	if condition <= lv then
		task:SetVarNoSync("k",condition);
		task:SetStatus(TASK_FINISHED);
	else
		task:SetVarNoSync("k",lv);
	end
end

function OnSubmitted(player, task, bRp)		---	��ȡ
	if task:GetStatus() ~= TASK_FINISHED then return false end

	task:SetStatus(TASK_SUBMITTING);	
    return true
end

function OnLvUpChange(player, task)		---	����
	if task:GetStatus() ~= TASK_UNFINISH then return false end
	
	local lv = player:GetLevel();
	if condition <= lv then
		task:SetVar("k",condition);
		task:SetStatus(TASK_FINISHED);
	else
		task:SetVar("k",lv);
	end 
end

function OnDayChange(player, task, preTask)		---	����
	local createFromNowOn = player:GetCreateFromNowOn()
	local day = createFromNowOn / (24 * 3600)

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