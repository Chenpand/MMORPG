function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	--��(ע��)����֮����ʼ�¼� (p1:��ID 0��ʾ������)
	task:BindDeathTowerBeginEvent(0)
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

function OnDeathTowerBegin(player, task, towerId)
	local num = task::GetVar("parameter");
	num = num + 1;
	task:SetVar("parameter", num);
	if num >= 1 then
		task:SetStatus(2);						---��������޸�״̬
	end
end


function CheckFinish(player, task)		---��������������
end
