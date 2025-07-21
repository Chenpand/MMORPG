require "Debug"
require "Task_Define"


--7�ջ Day2
--��۽���
function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindDayChange();
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
    task:SetStatusNoSync(TASK_FINISHED);	
end

function OnSubmitted(player, task, bRp)		---	��ȡ
	if task:GetStatus() ~= TASK_FINISHED then return false end

	if player:GetPoint() < 3280 then 
		player:SendNotifyById(1048)
		return 
	end
	
	player:RemovePoint("16002;7750;0", 3280);

	task:SetStatus(TASK_SUBMITTING);	
    return true
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

