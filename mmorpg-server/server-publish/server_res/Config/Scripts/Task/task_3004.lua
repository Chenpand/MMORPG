function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindPVPEvent(1, 1)
	--task:BindPVPEvent(0, 0)	--(type, status) p1:����(0.��ͳ����,1.����) p2:PK���(0.��Ч,1.ʤ��,2.ʧ��,3.ƽ��)
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���	
	local winNum = task:GetVar("parameter");
	if winNum >= 50 then
		task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
	else
		task:SetVar("parameter", winNum);
	end
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnPVP(player, task, pkType, status)
	local winNum = task:GetVar("parameter");	--��ȡ�����ʤ
	winNum = winNum + 1;	
	task:SetVar("parameter", winNum);

	if winNum >= 50 then
		task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
	end
end


function CheckFinish(player, task)		---��������������
end
