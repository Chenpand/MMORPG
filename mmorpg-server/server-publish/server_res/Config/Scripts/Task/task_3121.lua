function Init(task)					---��(ע��)��������Ҫ��ע���¼�
    task:BindAddRetinueEvent()		-- ��ע�������¼�
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���	
	--��ȡָ��Ʒ�����ϵ�����(0:�̴�, 1:��ʿ, 2:����, 3:��˵)
	local num = task:GetVar("parameter");	---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	local qrSize = player:GetRetinueSizeByQuality(2);
	
	if(qrSize >= 1) then
		num = num + 1
		task:SetVar("parameter",num)
		task:SetStatus(2);
	end
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end


function OnAddRetinue(player, task, quality)
	--��ȡָ��Ʒ�����ϵ�����(0:�̴�, 1:��ʿ, 2:����, 3:��˵)
	local num = task:GetVar("parameter");	---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	local qrSize = player:GetRetinueSizeByQuality(2);
	
	if(qrSize >= 1) then
		num = num + 1
		task:SetVar("parameter",num)
		task:SetStatus(2);
	end
end


function CheckFinish(player, task)		---��������������
end
