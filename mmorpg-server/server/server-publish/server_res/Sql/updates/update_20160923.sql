ALTER TABLE `t_counter` MODIFY `name` varchar(32) NOT NULL DEFAULT '';

ALTER TABLE `t_broadcastmail` ADD COLUMN `sendername`  varchar(32) NOT NULL DEFAULT ''