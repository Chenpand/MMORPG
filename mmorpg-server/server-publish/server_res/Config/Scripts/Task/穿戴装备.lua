function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindPutEquipEvent(0, 0, 0)		--����װ��(p1:�ȼ�, p2:Ʒ��, p3:ǿ���ȼ�)
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

function OnPutEquip(player, task, level, subType, strengthen)	--ǿ��
	--(0:���� 1:��,2:��,3:��, 4:��, 5:��, 6:��)
	local num = player:GetEquipSize(20, 0);
	
	task:SetVar("parameter", num);
	if num >= 9 then
		task:SetStatus(2);	      			-- ��������ʱ��������Ϊ���״̬
	end
	
end


function CheckFinish(player, task)		---��������������
end
