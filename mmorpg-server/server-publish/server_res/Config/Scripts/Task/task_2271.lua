function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindChangeOccuEvent();
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���	
	if player:GetOccu()%10 ~= 0 then
		task:SetVar("parameter", 1);                   --- ���ñ�����ֵ
		task:SetStatus(2);
	end
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnChangeOccu(player, task)		--תְ
	task:SetVar("parameter", 1);                   --- ���ñ�����ֵ
	task:SetStatus(2);
end

function CheckFinish(player, task)		---��������������
end
