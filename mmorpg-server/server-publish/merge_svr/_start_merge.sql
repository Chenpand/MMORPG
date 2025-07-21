delimiter $$
drop procedure if exists start_merge $$
create procedure start_merge(IN zid INT)
begin
	call delete_overdue_player();
	call merge_player(zid);
	call merge_account();
	call merge_dungeon_hard();
	call merge_guild();
	call merge_auction();

end $$

delimiter ;