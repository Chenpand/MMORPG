function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	--��(ע��)���ϴ���¼�
	task:BindRetinueChangeSkillEvent()
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
	local num = task:GetVar("parameter");		---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	if num >= 500 then
		task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
	else
		task:SetVar("parameter", num);
	end
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnRetinueChangeSkill(player, task, buffId)
	local num = task:GetVar("parameter");			---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	num = num + 1;
	task:SetVar("parameter",num);					---	���ñ�����ֵ
	if num >= 500 then
		task:SetStatus(2);	      					-- ��������ʱ��������Ϊ���״̬
	end
end


function CheckFinish(player, task)		---��������������
end
