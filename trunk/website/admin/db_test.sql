CREATE TABLE `tbl_user` 
(
	`userid` integer (11) NOT NULL AUTO_INCREMENT , 
	`username` varchar (50) NOT NULL, 
	`useremail` varchar (100) NOT NULL, 
	`userpassword` varchar (50) NOT NULL,
	PRIMARY KEY (`userid`)
) TYPE=MyISAM CHARACTER SET utf8 COLLATE utf8_general_ci;

BEGIN;
INSERT INTO `tbl_user` (`userid`, `username`, `useremail`, `userpassword`) VALUES(1, 'admin', 'admin@chazzuka.com', 'e10adc3949ba59abbe56e057f20f883e');
INSERT INTO `tbl_user` (`userid`, `username`, `useremail`, `userpassword`) VALUES(2, 'user', 'user@chazzuka.com', 'e10adc3949ba59abbe56e057f20f883e');
COMMIT;