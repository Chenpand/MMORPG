function Init(task)				---��(ע��)��������Ҫ��ע���¼�
	task:BindAddAuctionEvent()		--��(ע��)�������ϼܵ��¼�
	task:BindSellAuctionEvent()		--��(ע��)�������۳����¼�
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

function OnAddAuction(player, task)
	--local num = task:GetVar("parameter");	---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	--num = num + 1;
	--task:SetVar("parameter",num);				---	���ñ�����ֵ
	--if num >= 100 then
	--	task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
	--end
end

function OnSellAuction(player, task)
	local num = task:GetVar("parameter");	---	��ȡ����, �ر�ע�⣺�������������Ҫ��������������õ�����һ�£�������
	num = num + 1;
	task:SetVar("parameter",num);				---	���ñ�����ֵ
	if num >= 100 then
		task:SetStatus(2);	      -- ��������ʱ��������Ϊ���״̬
	end
end


function CheckFinish(player, task)		---��������������
end
