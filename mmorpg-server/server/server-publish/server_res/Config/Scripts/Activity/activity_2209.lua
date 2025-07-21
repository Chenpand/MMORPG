require "Debug"
require "Task_Define"


--�ۼ����߻����
function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindTickEvent();
    task:BindDayChange();
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
    task:SetStatusNoSync(TASK_UNFINISH);	
end

function OnSubmitted(player, task, bRp)		---	��ȡ
	if task:GetStatus() ~= TASK_FINISHED then return false end

	task:SetStatus(TASK_SUBMITTING);	
    return true
end

function OnMinTick(player, task)		---	5��tick
	if task:GetStatus() ~= TASK_UNFINISH then return end 
	
	if player:GetDayOnlineTime() >= 8 * 60 * 60 then
		task:SetStatus(TASK_FINISHED);
	end 
end

function OnDayChange(player, task, preTask, online)		---	����
	task:SetStatus(TASK_UNFINISH);
end