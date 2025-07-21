require "Debug"
require "Task_Define"

local DungeonId = 2003000;
local condition = 10;

--7�ջ Day2
--ͨ������������
function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindDeathTower();
    task:BindDayChange();
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
    task:SetStatusNoSync(TASK_UNFINISH);
    task:SetVarNoSync("k",0);	
end

function OnSubmitted(player, task, bRp)		---	��ȡ
	if task:GetStatus() ~= TASK_FINISHED then return false end

	task:SetStatus(TASK_SUBMITTING);	
    return true
end

function OnDeathTower(player, task, id, layer, usedTime)
	if task:GetStatus() ~= TASK_UNFINISH then return false end
	
	--��ͨ�Ѷ�
	if DungeonId == id then
		if condition <= layer then
			task:SetVar("k",condition);
			task:SetStatus(TASK_FINISHED);
		else
			task:SetVar("k",layer);
		end
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
