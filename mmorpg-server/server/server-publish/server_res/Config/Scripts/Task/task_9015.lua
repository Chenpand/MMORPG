function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	--��(ע��)�������¼�
	task:BindOpenMagicJarEvent(1, 1)
	task:BindOpenMagicJarEvent(102, 1)
	task:BindOpenMagicJarEvent(103, 1)
	--��(ע��)������10�����¼�
	task:BindOpenMagicJarEvent(1, 10)
	task:BindOpenMagicJarEvent(102, 10)
	task:BindOpenMagicJarEvent(103, 10)
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

function OnOpenMagicJar(player, task, type, combo)
	local num = task:GetVar("parameter");			---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	num = num + combo;
	task:SetVar("parameter",num);					---	���ñ�����ֵ
	if num >= 4 then
		task:SetStatus(2);	      					-- ��������ʱ��������Ϊ���״̬
	end
end


function CheckFinish(player, task)		---��������������
end
