function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	--��(ע��)ƣ�������¼� (������ʾ������ƣ��ֵ,0��ʾ���޶�ƣ��ֵ)
	task:BindRemoveFatigueEvent(0)
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

function OnRemoveFatigue(player, task, fatigue)
	local num = task:GetVar("parameter");
	num = num + fatigue;
	task:SetVar("parameter", num);
	if(num >= 150) then
		task:SetStatus(2);	      					-- ��������ʱ��������Ϊ���״̬
	end
end


function CheckFinish(player, task)		---��������������
end
