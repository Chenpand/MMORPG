--随从礼盒
function OnUseSelf(player, item)
	--奖池道具
	local reason="2012;800000092;0"
	local bonusPool = {	item1 = {id=300001101,num=500,wt=40},
	                    item2 = {id=300001101,num=1000,wt=20},
						item3 = {id=300001001,num=5,wt=100},
						item4 = {id=300001002,num=5,wt=100},
						item5 = {id=300001003,num=1,wt=200},
						item6 = {id=300001004,num=1,wt=200},
						item7 = {id=300001008,num=1,wt=200},
						item8 = {id=300001003,num=3,wt=20},
						item9 = {id=300001004,num=3,wt=20},
						item10 = {id=300001008,num=3,wt=20},
						item11 = {id=300001003,num=5,wt=10},
						item12 = {id=300001004,num=5,wt=10},
						item13 = {id=300001008,num=5,wt=10},
						item14 = {id=300001006,num=1,wt=100},
						item15 = {id=300001007,num=1,wt=100},
						item16 = {id=300001006,num=5,wt=8},
						item17 = {id=300001007,num=5,wt=8},
						item18 = {id=300001006,num=20,wt=2},
						item19 = {id=300001007,num=20,wt=2} }

	local totalWeight = 0
	for k, v in pairs(bonusPool) do
		totalWeight = totalWeight + v.wt
	end

	local randWt = Sys:RandBetween(1, totalWeight)

	--随机出抽到的奖励道具
	local sumWt = 0
	
	local group = player:GetRewardGroup()

	for k, v in pairs(bonusPool) do
		sumWt = v.wt + sumWt
		if randWt <= sumWt then
			group:AddReward(v.id, v.num)
			break	
		end
	end

	local canAdd = player:CheckAddRewards(group, item, 1)
	if canAdd ~= 0 then return canAdd end

	-- 这里写上理由
	player:AddRewards(reason, group, true)
	player:AddItem(reason, 600000007, 100, 0, 0, 1)  
	player:ReduceItemNum(reason, item, 1)

	return 0
end