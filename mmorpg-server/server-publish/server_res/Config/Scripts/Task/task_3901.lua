function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	---��ע�������֮���¼���
	task:BindDeathTowerEvent(2003000, 20, 0)
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

function OnDeathTower(player, task, towerId, layer, usedTime)	--����֮��
	if layer < 20 then
		return;
	end
	
	task:SetStatus(2);	      			-- ��������ʱ��������Ϊ���״̬
end


function CheckFinish(player, task)		---��������������
end
