function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	--��(ע��)ͨ����Ԩ�¼� (p1:���³�ID p2:�޶�ʱ�� 0:���޶�)
	task:BindClearHellEvent(0, 0)
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
	local num = task:GetVar("parameter");			---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	if num >= 5 then
		task:SetStatus(2)							---��������޸�״̬
	end
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnClearHell(player, task, dungeonId, usedTime)
	local dungeonType = dungeonId % 10;		---��ȡ���³��Ѷ�(0:��ͨ,1:ð��,2:��ʿ,3:����,4:����)
	local num = task:GetVar("parameter");			---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	num = num + 1;
	task:SetVar("parameter",num);					---	���ñ�����ֵ
	if num >= 5 then
		task:SetStatus(2)							---��������޸�״̬
	end
end


function CheckFinish(player, task)		---��������������
end
