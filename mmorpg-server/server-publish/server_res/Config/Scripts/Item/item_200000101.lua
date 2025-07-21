--金罐子(鬼剑士武器)
function OnUseSelf(player, item)

	local bonusPool = {item1 = {id=114010001,num=1,wt=10}, item2 = {id=134210005,num=1,wt=20},
					item3 = {id=114011001,num=1,wt=30}, item4 = {id=154411002,num=1,wt=40},
					item5 = {id=134711002,num=1,wt=50}}

	local totalWeight = 0
	for k, v in pairs(bonusPool) do
		totalWeight = totalWeight + v.wt
	end

	local randWt = Sys:RandBetween(1, totalWeight)

	--随机出抽到的奖励道具
	local sumWt = 0
	local hitBonus = {}
	for k, v in pairs(bonusPool) do
		sumWt = v.wt + sumWt
		if randWt <= sumWt then
			hitBonus = v
			break	
		end
	end

	local bonusIds = ""
	for k, v in pairs(bonusPool) do
		if bonusIds == "" then
			bonusIds = v.id
		else
			bonusIds = bonusIds.."|"..v.id
		end
	end

	local group = player:GetRewardGroup()
	group:AddReward(hitBonus.id, hitBonus.num)
	local canAdd = player:CheckAddRewards(group, item, 1)
	if canAdd ~= 0 then return canAdd end

	player:ReduceItemNum("2012;200000101;0", item, 1)	--删除道具
	player:SendMagicJarResult(bonusIds, hitBonus.id, hitBonus.num)

	return 0
end