function Init(task)				---��(ע��)��������Ҫ��ע���¼�
    task:BindChangeMoneyEvent(1)		---��ע��õ����¼���
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
	local moneynum = task:GetVar("parameter");
	if moneynum >= 100000 then							---	�жϽ�������
		task:SetStatus(2)
	end
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnChangeMoney(player, task, moneyType, num)		---	��������л�õ���ʱ��Ҫִ�еĲ���
	if num < 0 then
		num = num * -1;
	end
	local moneynum = task:GetVar("parameter");			---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	moneynum = moneynum + num;							---	�趨������ʽ
	task:SetVar("parameter",moneynum);					---	���ñ�����ֵ
	if moneynum >= 100000 then							---	�жϽ�������
		task:SetStatus(2)
	end
end
