<?php
// validate inclusion
if (!defined('NEOLOGISM_EXEC')) exit('direct access is not allowed.');

$NEOLOGISM_LANG = array (
		'USERNAME'			=>	'Username',
		'EMAIL'				=>	'E-mail',
		'PASSWORD'			=>	'Password',
		'LOGIN'				=>	'Login!',
		'SESSION_ACTIVE'	=>	'Your Session is already active, click <a href="'.SUCCESS_URL.'">here</a> to continue.',
		'LOGIN_SUCCESS'		=>	'You have successfuly authorized, click <a href="'.SUCCESS_URL.'">here</a> to continue.',
		'LOGIN_FAILED'		=>	'Login Failed: wrong combination of '.((LOGIN_METHOD=='user'||LOGIN_METHOD=='both')?'Username':''). 
								((LOGIN_METHOD=='both')?'/':'').
								((LOGIN_METHOD=='email'||LOGIN_METHOD=='both')?'email':'').
								' and password.',
	);
?>