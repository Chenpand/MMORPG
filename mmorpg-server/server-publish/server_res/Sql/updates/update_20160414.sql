ALTER TABLE `t_mailitem` ADD COLUMN `phyatk` int(10) unsigned NOT NULL DEFAULT '0';
ALTER TABLE `t_mailitem` ADD COLUMN `magatk` int(10) unsigned NOT NULL DEFAULT '0';
ALTER TABLE `t_mailitem` ADD COLUMN `defense` int(10) unsigned NOT NULL DEFAULT '0';
ALTER TABLE `t_mailitem` ADD COLUMN `strenth` int(10) unsigned NOT NULL DEFAULT '0';
ALTER TABLE `t_mailitem` ADD COLUMN `stamina` int(10) unsigned NOT NULL DEFAULT '0';
ALTER TABLE `t_mailitem` ADD COLUMN `intellect` int(10) unsigned NOT NULL DEFAULT '0';
ALTER TABLE `t_mailitem` ADD COLUMN `spirit` int(10) unsigned NOT NULL DEFAULT '0';
ALTER TABLE `t_mailitem` ADD COLUMN `qualitylv` tinyint(3) unsigned NOT NULL DEFAULT '0';
ALTER TABLE `t_mailitem` ADD COLUMN `sealstate` tinyint(3) unsigned NOT NULL DEFAULT '0';
ALTER TABLE `t_mailitem` ADD COLUMN `sealcount` int(10) unsigned NOT NULL DEFAULT '0';

