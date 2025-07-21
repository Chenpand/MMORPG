delimiter $$
DROP PROCEDURE IF EXISTS merge_auction $$

CREATE PROCEDURE merge_auction()
BEGIN

		UPDATE `t_auction_new` SET duetime=0;
	
END $$

delimiter ;