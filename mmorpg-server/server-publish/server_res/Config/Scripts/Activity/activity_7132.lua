require "Debug"
require "Task_Define"


local condition = 24501;
--7�ջ Day1
--pk��λ
function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindPkLvUpEvent();
    task:BindDayChange();
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
    task:SetStatusNoSync(TASK_UNFINISH);	

    local pklv = player:GetSeasonLevel();
     if pklv >= condition then	
    	task:SetVarNoSync("k",1);
		task:SetStatusNoSync(TASK_FINISHED);
	end
end

function OnSubmitted(player, task, bRp)		---	��ȡ
	if task:GetStatus() ~= TASK_FINISHED then return false end

	task:SetStatus(TASK_SUBMITTING);	
    return true
end

--tp:0��ͭ 1�ƽ� 2����
function OnPkLvUp(player, task, tp, lv)
	if task:GetStatus() ~= TASK_UNFINISH then return false end

	--����5
	local pklv = player:GetSeasonLevel();
     if pklv >= condition then	
    	task:SetVar("k",1);
		task:SetStatus(TASK_FINISHED);
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