function Init(task)					---��(ע��)��������Ҫ��ע���¼�
    task:BindAddRetinueEvent(0)		-- ��ע�������¼�(p1:���Ʒ��, 0,�̴� 1,��ʿ 2,���� 3,��˵)
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���	
	local rsize = player:GetRetinueSize();				--��ȡ��Ӹ���
	
	--��ȡָ��Ʒ�����ϵ�����(0:�̴�, 1:��ʿ, 2:����, 3:��˵)
	local qrSize = player:GetRetinueSizeByQuality();
	
	if(rsize >= 1) then
		task:SetVar("parameter", rsize);
		task:SetStatus(2);
	end
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end


function OnAddRetinue(player, task, quality)
	local rsize = player:GetRetinueSize();				--��ȡ��Ӹ���
	
	--��ȡָ��Ʒ�����ϵ�����(0:�̴�, 1:��ʿ, 2:����, 3:��˵)
	local qrSize = player:GetRetinueSizeByQuality();
	
	if(rsize >= 1) then
		task:SetVar("parameter", rsize);
		task:SetStatus(2);
	end
end


function CheckFinish(player, task)		---��������������
end
