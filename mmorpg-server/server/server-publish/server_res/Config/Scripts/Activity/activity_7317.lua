require "Debug"
require "Task_Define"

local condition = 5;
local condNum = 3;
--7�ջ Day3
--��ô�Ԫʯ����
function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindWarpStoneUpLevel();
    task:BindDayChange();
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
    task:SetStatusNoSync(TASK_UNFINISH);

    local lv = player:GetWarpStoneMinLevel(condition, condNum, 0);
	if lv >= condition then
		task:SetVarNoSync("k", condition);
		task:SetStatusNoSync(TASK_FINISHED);
	else
		task:SetVarNoSync("k", lv);
	end		
end

function OnSubmitted(player, task, bRp)		---	��ȡ
	if task:GetStatus() ~= TASK_FINISHED then return false end

	task:SetStatus(TASK_SUBMITTING);	
    return true
end

function OnWarpStoneUpLevel(player, task)
	if task:GetStatus() ~= TASK_UNFINISH then return false end
	
	--5�Ŵ�Ԫʯ�ȼ�Ϊ5
	local lv = player:GetWarpStoneMinLevel(condition, condNum, 0);
	if lv >= condition then
		task:SetVar("k", condition);
		task:SetStatus(TASK_FINISHED);
	else
		task:SetVar("k", lv);
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
