
function Init(task)
end

function CheckAccept(player)
	return true;
end

function OnAccepted(player)
	local task = player:FindTask(1034);
	task:SetStatus(2);
end

function OnSubmitted(player)
end

function OnAbandomed(player)
end