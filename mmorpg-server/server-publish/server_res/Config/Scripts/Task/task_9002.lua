function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	---��ע��õ����¼��� (1.���,2.��ȯ,3.����,11.������, 12.������)
	task:BindChangeMoneyEvent(2)
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

function OnChangeMoney(player, task, moneyType, num)	--���ұ仯
	if num > 0 then			--��ʾ���ӻ��� �����д���
		return;
	end
	local point = task:GetVar("parameter");
	point = point + (num * -1);
	task:SetVar("parameter",point);			---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	if point >= 50 then
		task:SetStatus(2);	      			-- ��������ʱ��������Ϊ���״̬
	end
end


function CheckFinish(player, task)		---��������������
end
