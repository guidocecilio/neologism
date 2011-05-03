<?php
// validate inclusion
if (!defined('NEOLOGISM_EXEC')) exit('direct access is not allowed.');

define('LOGIN_METHOD',	'both');			// 'user':'email','both'
define('SUCCESS_URL',	'index.php');		// redirection target on success

$db_config = array(
		'server'	=>	'mysql.deri.ie',	// server name or ip address along with suffix ':port' if needed (localhost:3306)
		'user'		=>	'neologism',			// mysql username
		'pass'		=>	'!!9i8u7y6t',	    // mysql password
		'name'		=>	'neologism'				// database name
	);
?>