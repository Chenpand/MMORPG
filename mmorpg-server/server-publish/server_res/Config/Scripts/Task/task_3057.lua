function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	--��(ע��)��Ԫʯ�����¼� p1:ʯͷ����(0.��Ч,1.��,2.��,3.��,4.��,5.��) p2:�ȼ�(0��ʾ��Ч)
	task:BindWarpStoneUpLevelEvent(0, 20)
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
	local num = player:GetWarpStoneSize(0, 20);		--(ʯͷ����, �ȼ�)
	task:SetVar("parameter",num);				---	���ñ�����ֵ
	if num >= 9 then
		task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
	end
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnWarpStoneUpLevel(player, task, wsType, level)
	local num = player:GetWarpStoneSize(0, 20);		--(ʯͷ����, �ȼ�)
	task:SetVar("parameter",num);				---	���ñ�����ֵ
	if num >= 9 then
		task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
	end
end


function CheckFinish(player, task)		---��������������
end
