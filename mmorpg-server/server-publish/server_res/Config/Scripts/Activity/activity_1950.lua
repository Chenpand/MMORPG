require "Debug"
require "Task_Define"


--���������
function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindLevelUpEvent();
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
    task:SetStatusNoSync(TASK_UNFINISH);
    task:SetVarNoSync("lv", player:GetLevel());	
    OnLvUpChange(player, task);
end

function OnSubmitted(player, task, bRp)		---	��ȡ
	if task:GetStatus() ~= TASK_FINISHED then return false end

	task:SetStatus(TASK_SUBMITTING);	
    return true
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnLvUpChange(player, task)		---	����
	if task:GetStatus() ~= TASK_UNFINISH then return false end 
	
	local lv = player:GetLevel();
	if lv >= 10 then
		task:SetStatus(TASK_FINISHED);
	end 
end
