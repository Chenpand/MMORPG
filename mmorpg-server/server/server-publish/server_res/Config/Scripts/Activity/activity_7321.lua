require "Debug"
require "Task_Define"

local condition = 1;
--7�ջ Day3
--���й�����
function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindBuyBlackShop();
    task:BindDayChange();
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
    task:SetStatusNoSync(TASK_UNFINISH);	
    task:SetVarNoSync("k", 0)
end

function OnSubmitted(player, task, bRp)		---	��ȡ
	if task:GetStatus() ~= TASK_FINISHED then return false end

	task:SetStatus(TASK_SUBMITTING);	
    return true
end

function OnBuyBlackShop(player, task)		---	���й���
	if task:GetStatus() ~= TASK_UNFINISH then return false end
	
	local tmpCondition = task:GetVar("k") + 1;
	if tmpCondition >= condition then
		task:SetVar("k", condition);
		task:SetStatus(TASK_FINISHED)
	else
		task:SetVar("k", tmpCondition);
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