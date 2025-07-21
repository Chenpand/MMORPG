require "Debug"
require "Task_Define"

local var_condition = 3
--7�ջ Day5
--�������
function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindDayChange();
    task:BindRetinueUpStarEvent();
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
    task:SetStatusNoSync(TASK_UNFINISH); 

    local num = player:GetMaxRetinueStar();
    if num >= var_condition then
    	task:SetVarNoSync("k", var_condition);
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

function OnRetinueUpStar(player, task, skillId, starLv)
	if task:GetStatus() ~= TASK_UNFINISH then return false end
	
	if starLv >= var_condition then
    	task:SetVarNoSync("k", var_condition);
    	task:SetStatusNoSync(TASK_FINISHED);
    else
    	task:SetVarNoSync("k", starLv);
    end	
end

function OnDayChange(player, task, preTask)		---	����
	local createFromNowOn = player:GetCreateFromNowOn()
	local day = createFromNowOn / (24 * 3600)

	--������������
	if day >= 4 then
		OnRetinueUpStar(player, task, 0, player:GetMaxRetinueStar())
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
