function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindAwakenEvent()		--����
	task:BindChangeOccuEvent()	--תְ
	task:BindLevelupEvent(15)	--���� (��ʾ�����ȼ�)
end

function CheckTask(player, task)
	local isInitOccu = player:IsInitOccu();		--��ȡ��ɫ�Ƿ��ǳ�ʼְҵ
	if not isInitOccu then						--��ɫ������15���������תְ
		task:SetVar("parameter",1);
		task:SetStatus(2)						---��������޸�״̬
	end
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���
	local level = player:GetLevel();			--��ȡ��ɫ�ȼ�
	if level >= 15 then
		CheckTask(player, task)
	end
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end

function OnPlayerLevelup(player, task)	--����
	CheckTask(player, task)
end

function OnChangeOccu(player, task)		--תְ
	CheckTask(player, task)
end

function OnAwaken(player, task)			--����
	CheckTask(player, task)
end



function CheckFinish(player, task)		---��������������
end
