function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindStrengthenEquipEvent(true, 14)		--ǿ��(�Ƿ��ע�ɹ�, ǿ���ȼ�)
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
	local maxNum = task:GetVar("max");		---���ǿ��ֵ	
	if maxNum >= 14 then
		task:SetVar("parameter",1);		---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
		task:SetStatus(2);	     		 -- ��������ʱ��������Ϊ���״̬
	else
		task:SetVar("parameter", 0);
	end
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnStrengthenEquip(player, task, errorCode, oldStrengthen, newStrengthen)	--ǿ��
	local maxNum = task:GetVar("max");
	if maxNum < newStrengthen then
		maxNum = newStrengthen;
		task:SetVar("max", maxNum);
	end
	
	if maxNum >= 14 then
		task:SetVar("parameter",1);			---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
		task:SetStatus(2);	      			-- ��������ʱ��������Ϊ���״̬
	end
end


function CheckFinish(player, task)		---��������������
end
