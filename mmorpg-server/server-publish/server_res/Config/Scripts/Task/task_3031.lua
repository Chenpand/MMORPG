function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindPVPEvent(1)
	--task:BindPVPEvent(0, 0)	--(type, status) p1:����(0.��ͳ����,1.����) p2:PK���(0.��Ч,1.ʤ��,2.ʧ��,3.ƽ��)
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
	local pkLevelType = player:GetPkLevelType();	--��ȡ����(0.��ͭ,1.�ƽ�,2.����)
	local pkLevel = player:GetPkLevel();			--��ȡ��λ

	if pkLevelType >= 0 and pkLevel >= 3 then
		task:SetVar("parameter", 1);
		task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
	end
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnPVP(player, task, pkType, status)
	local pkLevelType = player:GetPkLevelType();	--��ȡ����(0.��ͭ,1.�ƽ�,2.����)
	local pkLevel = player:GetPkLevel();			--��ȡ��λ

	if pkLevelType >= 0 and pkLevel >= 3 then
		task:SetVar("parameter", 1);
		task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
	end
end

function CheckFinish(player, task)		---��������������
end
