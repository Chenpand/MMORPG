require "Debug"
require "Task_Define"

local color = 2;
local condition = 9;
--7�ջ Day2
--ȫ��װ��Ʒ�ʽ���
function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindWearEquip();
    task:BindDayChange();
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
    task:SetStatusNoSync(TASK_UNFINISH);	

    local num = player:GetWearEquipNumByQua(color);
	if num >= condition then	--��ɫ
		task:SetVarNoSync("k", condition);
		task:SetStatusNoSync(TASK_FINISHED);
	else
		task:SetVarNoSync("k", num);
	end 
end

function OnSubmitted(player, task, bRp)		---	��ȡ
	if task:GetStatus() ~= TASK_FINISHED then return false end

	task:SetStatus(TASK_SUBMITTING);	
    return true
end

function OnWearEquip(player, task)		---	��װ��
	if task:GetStatus() ~= TASK_UNFINISH then return false end
	
	local num = player:GetWearEquipNumByQua(color);
	if num >= condition then	--��ɫ
		task:SetVar("k", condition);
		task:SetStatus(TASK_FINISHED);
	else
		task:SetVar("k", num);
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
			return
		end
	end

	OnWearEquip(player, task)
end