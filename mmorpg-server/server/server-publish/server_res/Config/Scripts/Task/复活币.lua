function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	---��ע������¼�
	task:BindReviveCoinEvent()
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

function OnReviveCoin(player, task, isSelf)	--�����
	if isSelf then							--���Լ�ʹ��
		return;
	end
	
	local num = task:GetVar("parameter");
	num = num + 1;
	task:SetVar("parameter",num);			---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	if num >= 50 then
		task:SetStatus(2);	      			-- ��������ʱ��������Ϊ���״̬
	end
end


function CheckFinish(player, task)		---��������������
end
