function Init(task)					---��(ע��)��������Ҫ��ע���¼�
    task:BindPutEquipEvent(0,5)		-- ��ע����װ���¼�
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���	
	local typeMax = 9;
	for i = 1, typeMax, 1
	do
		local keyStr = "type_10_"..tostring(i);
		local tmpType = task:GetVar(keyStr);
		if(tmpType == 1) then
			task:SetStatus(2);
			task:SetVar("parameter", 1);
			return;
		end
	end
	task:SetVar("parameter", 0);			--���ó�ʼֵ
end

function OnSubmitted(player, task)		---	�ύ����ʱ��Ҫִ�еĲ���
end

function OnAbandomed(player, task)		---	��������ʱ��Ҫִ�еĲ���
end


function OnPutEquip(player, task, subtype, strength)
	local typeMax = 9;
	if(subtype > typeMax or subtype < 1) then
		return;
	end
	
	local keyStr = "type_5_"..tostring(subtype);
	local tmpType = task:GetVar(keyStr);
	if(tmpType == 0) then
		task:SetVar(keyStr, 1);
	end

	if(strength >= 10) then
		local keyStr = "type_10_"..tostring(subtype);
		local tmpType = task:GetVar(keyStr);
		if(tmpType == 0) then
			task:SetVar(keyStr, 1);
		end
		local oldnum = task:GetVar("parameter");
		if(oldnum == 0) then
			task:SetVar("parameter", 1);
			task:SetStatus(2);
		end
	end
end
