function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	---��ע��Ԫʯ�����¼�
	task:BindWarpStoneAddEnergyEvent()
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

function OnWarpStoneAddEnergy(player, task, energy)	--��Ԫʯ����
	local num = task:GetVar("parameter");
	num = num + energy;
	task:SetVar("parameter",num);			---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	if num >= 100 then
		task:SetStatus(2);	      			-- ��������ʱ��������Ϊ���״̬
	end
end


function CheckFinish(player, task)		---��������������
end
