<?php
// validate inclusion
if (!defined('NEOLOGISM_EXEC')) exit('direct access is not allowed.');

define('LOGIN_METHOD',	'both');			// 'user':'email','both'
define('SUCCESS_URL',	'index.php');		// redirection target on success

$db_config = array(
		'server'	=>	'localhost',	// server name or ip address along with suffix ':port' if needed (localhost:3306)
		'user'		=>	'root',			  // mysql username
		'pass'		=>	'',	          // mysql password
		'name'		=>	'neologism',	// database name
		'tbl_user'	=>	'user'		  // user table name
	);
?>