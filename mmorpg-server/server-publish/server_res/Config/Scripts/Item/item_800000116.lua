function OnUseSelf(player, item)
	--开罐子,参数:罐子id, 连抽次数
	local ret = player:OpenJar(604, 1)
	if ret == 0 then
		local reason="2012;800000116;0"
		player:ReduceItemNum(reason, item, 1)
	end

	return ret
end