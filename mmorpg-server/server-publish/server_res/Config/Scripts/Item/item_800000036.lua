--开服礼盒
function OnUseSelf(player, item)
	--奖池道具
    local reason="2012;800000036;0"
	local bonusPool = {item1 = {id=130193017,num=1,wt=50},
item2 = {id=152443002,num=1,wt=5},
item3 = {id=152453001,num=1,wt=5},
item4 = {id=152473001,num=1,wt=5},
item5 = {id=152473002,num=1,wt=5},
item6 = {id=800000050,num=1,wt=50},
item7 = {id=300000106,num=20,wt=200},
item8 = {id=300000105,num=10,wt=200},
item9 = {id=600000006,num=1,wt=200},
item10 = {id=600000006,num=2,wt=200},
item11 = {id=200002003,num=1,wt=50},
item12 = {id=200005003,num=1,wt=30},
item13 = {id=200006003,num=1,wt=30},
item14 = {id=200004003,num=1,wt=30},
item15 = {id=200003003,num=1,wt=30},
item16 = {id=200005002,num=1,wt=200},
item17 = {id=200006002,num=1,wt=200},
item18 = {id=200004002,num=1,wt=200},
item19 = {id=200003002,num=1,wt=200},
item20 = {id=200001002,num=3,wt=200},
item21 = {id=200001002,num=5,wt=200}
}

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

    local group = player:GetRewardGroup()
    group:AddReward(hitBonus.id, hitBonus.num)
    local canAdd = player:CheckAddRewards(group, item, 1)
    if canAdd ~= 0 then return canAdd end
    player:ReduceItemNum("useItem", item, 1)
    player:AddItem(reason, hitBonus.id, hitBonus.num, 0, 0, 1)
    player:AddItem(reason, 600000001, 10000, 0, 0, 1)

    return 0
end