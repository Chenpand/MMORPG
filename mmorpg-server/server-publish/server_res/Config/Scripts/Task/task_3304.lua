function Init(task)					---��(ע��)��������Ҫ��ע���¼�
    task:BindPutEquipEvent(0,10)		-- ��ע����װ���¼�
end

function CheckAccept(player)		---	������������������ֱ�ӷ���return true
	return true;
end

function OnAccepted(player, task)			---	��������ʱ��Ҫִ�еĲ���	
	local itemnum = 0;
	local typeMax = 9;
	for i = 1, typeMax, 1
	do
		local keyStr = "type_10_"..tostring(i);
		local tmpType = task:GetVar(keyStr);
		if(tmpType == 1) then
			itemnum = itemnum + 1;
		end
	end
	
	task:SetVar("parameter", itemnum);
	
	if(itemnum >= typeMax) then
		task:SetStatus(2);
	end
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
	
	local itemnum = 0;
	for i = 1, typeMax, 1
	do
		local keyStr = "type_10_"..tostring(i);
		local tmpType = task:GetVar(keyStr);
		if(i == subtype and tmpType == 0) then
			task:SetVar(keyStr, 1);
			tmpType = 1;
		end
		
		if(tmpType == 1) then
			itemnum = itemnum + 1;
		end
	end

	local oldnum = task:GetVar("parameter");
	if(oldnum ~= itemnum) then
		task:SetVar("parameter", itemnum);
	end
	
	if(itemnum >= typeMax) then
		task:SetStatus(2);
	end
end
