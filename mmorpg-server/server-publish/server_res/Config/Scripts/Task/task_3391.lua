function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindStrengthenEquipEvent(false, 10)		--ǿ��(�Ƿ�ֻ��ע�ɹ�, ǿ���ȼ�)
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

function OnStrengthenEquip(player, task, errorCode, oldStrengthen, newStrengthen)	--ǿ��
	local num = task:GetVar("parameter");
--errorCode(1000019:װ��ǿ��ʧ������, 0:�ɹ�)
	if errorCode == 1000019 and oldStrengthen>=10 then			---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
		num = num + 1;
		task:SetVar("parameter",num);
		if num >= 10 then
			task:SetStatus(2);	      			-- ��������ʱ��������Ϊ���״̬
		end
	end
end


function CheckFinish(player, task)		---��������������
end
