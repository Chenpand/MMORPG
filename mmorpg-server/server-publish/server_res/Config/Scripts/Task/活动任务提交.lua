function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	--��(ע��)������ύ�¼�(p1:�����ID, ID)
	task:BindSubmitActiveTaskSucceedEvent(3000, 0)
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
	local num = player:GetSignInCount();
	task:SetVar("parameter", num);					---	���ñ�����ֵ
	if num >= 10 then
		task:SetStatus(2);	      					-- ��������ʱ��������Ϊ���״̬
	end
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnSubmitActiveTaskSucceed(player, task, templateId, taskId)
	local num = player:GetSignInCount();			--- ��ȡ��ǩ������
	task:SetVar("parameter",num);					---	���ñ�����ֵ
	if num >= 10 then
		task:SetStatus(2);	      					-- ��������ʱ��������Ϊ���״̬
	end
end


function CheckFinish(player, task)		---��������������
end
