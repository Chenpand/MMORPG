function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindPVPEvent(1)
	--task:BindPVPEvent(0, 0)	--(type, status) p1:����(0.��ͳ����,1.����) p2:PK���(0.��Ч,1.ʤ��,2.ʧ��,3.ƽ��)
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���	
	local maxWin = player:GetMaxWinStreak(1);
	task:SetVar("parameter", maxWin);
	if maxWin >= 10 then
		task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
	end
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnPVP(player, task, pkType, status)
	local maxWin = player:GetMaxWinStreak(1);
	task:SetVar("parameter", maxWin);
	if maxWin >= 10 then
		task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
	end
end


function CheckFinish(player, task)		---��������������
end
