function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindAddRetinueEvent(0);
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnAddRetinue(player, task, quality)	--������
	if quality < 2 then
		return;
	end
	
	task:SetStatus(2);
end


function CheckFinish(player, task)		---��������������
end
