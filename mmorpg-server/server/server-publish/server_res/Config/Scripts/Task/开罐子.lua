function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	--��(ע��)�������¼�  (p1: 0-ȫ�� 1-�����; 8-ħ��; 9-��� ,  p2:combo)
	task:BindOpenMagicJarEvent(0,1)
	--��(ע��)������10�����¼�
	task:BindOpenMagicJarEvent(0,10)
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
	local num = task:GetVar("parameter");		---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	if num >= 5 then
		task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
	end
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnOpenMagicJar(player, task, type, combo)
	local num = task:GetVar("parameter");			---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	num = num + 1;
	task:SetVar("parameter",num);					---	���ñ�����ֵ
	if num >= 5 then
		task:SetStatus(2);	      					-- ��������ʱ��������Ϊ���״̬
	end
end


function CheckFinish(player, task)		---��������������
end
