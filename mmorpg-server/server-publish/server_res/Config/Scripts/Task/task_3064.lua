function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	--��(ע��)����֮���¼� (p1:��ID, p2:����(���15) ,p3:�޶�ʱ�� 0:���޶�)
	task:BindDeathTowerEvent(2003003, 15, 0)
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
	local oldTowerId = task:GetVar("towerId");			--��ȡ�����������ID
	local oldLayer = task:GetVar("layer");				--��ȡ�������߲���
	if (oldTowerId == 2003003 and oldLayer >= 15) or (oldTowerId > 2003003) then	--����������߸��߼�����
		task:SetVar("parameter", 1);
		task:SetStatus(2)							---��������޸�״̬
	else
		task:SetVar("parameter", 0);
	end
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnDeathTower(player, task, towerId, layer, usedTime)
	local oldTowerId = task:GetVar("towerId");			--��ȡ�����������ID
	local oldLayer = task:GetVar("layer");				--��ȡ�������߲���
	if towerId > oldTowerId then						--�����α�֮ǰ�������
		task:SetVar("towerId", towerId);				--������ID�Ͳ���
		task:SetVar("layer", layer);
		oldTowerId = towerId;
		oldLayer = layer;
	elseif towerId == oldTowerId and layer > oldLayer then	--�����֮ǰ�Ĳ�����  �������
		task:SetVar("layer", layer);
		oldLayer = layer;
	end
	if (oldTowerId == 2003003 and oldLayer >= 15) or (oldTowerId > 2003003) then	--����������߸��߼�����
		task:SetVar("parameter", 1);
		task:SetStatus(2);							---��������޸�״̬
	end
end


function CheckFinish(player, task)		---��������������
end
