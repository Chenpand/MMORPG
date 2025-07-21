require "Debug"
require "Task_Define"

local condition = 80

--Ʒ�����������
function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindRandQL();
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
    task:SetStatusNoSync(TASK_UNFINISH);
    task:SetVarNoSync("k", 0);	
end

function OnSubmitted(player, task, bRp)		---	��ȡ
	if task:GetStatus() ~= TASK_FINISHED then return false end

	task:SetStatus(TASK_SUBMITTING);	
    return true
end

function OnRandQL(player, task)		---	Ʒ������
	if task:GetStatus() ~= TASK_UNFINISH then return false end 

	local newNum = task:GetVar("k") + 1;
	if newNum >= condition then
		task:SetStatus(TASK_FINISHED);
		task:SetVar("k", condition);
	else
		task:SetVar("k", newNum);
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
