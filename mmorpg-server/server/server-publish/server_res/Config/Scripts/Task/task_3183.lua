function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindLevelupEvent(20)	--���� (��ʾ�����ȼ�)
end

function CheckTask(player, task)
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
	local level = player:GetLevel();			--��ȡ��ɫ�ȼ�
	task:SetVar("parameter", level);
	if level >= 30 then
		task:SetStatus(2);
	end
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnPlayerLevelup(player, task)	--����
	local level = player:GetLevel();			--��ȡ��ɫ�ȼ�
	task:SetVar("parameter", level);
	if level >= 30 then
		task:SetStatus(2);
	end
end




function CheckFinish(player, task)		---��������������
end
