<?php
define('NEOLOGISM_EXEC', true);
require_once('includes/admin.config.php');

/**
 * This class is to handle the customer's registration webservice. The incoming request 
 * should contain the following parameters:
 * - customer_name 
 * 		Customer identification.
 * - organization	
 * 		Customer organization.
 * - email
 * 		Customer email, the form automatically select the same email taken in the installation.
 * - website_uri 
 * 		URI of the installation.
 * - plan
 * 		Purpose of the installation.
 * 
 * usage: http://domain/registration.php?customer_name=guido&organization=DERI
 * 
 */
class Registration {
  function __construct() {
  }
  
  public function index() {
    global $db_config;
			
		$db = @mysql_connect($db_config['server'], $db_config['user'], $db_config['pass']);
		if (!$db) {
		  echo json_encode(array('result' => 'error', 'error_msg' => 'Error connecting to the MySQL server.')); 
		  return; 
		}
		
		$opendb = @mysql_select_db($db_config['name'], $db);
		if (!$opendb) {
		  echo json_encode(array('result' => 'error', 'error_msg' => 'Error selecting the database.'));  
		  return;
		}
		
    $sql = "INSERT INTO registrations (".implode(',', array_keys($_POST)).",created_at) VALUES ('".implode("','", array_values($_POST))."',NOW())";
		  
		$rs = @mysql_query($sql, $db);
		
		if (!$rs) {
		  echo json_encode(array('result' => 'error', 'error_msg' => 'Error executing query: "'.$sql.'"')); 
		  return; 
		}
		
		mysql_free_result($rs);
		mysql_close($db);
		unset($rs, $sql);
	
		header('Content-Type: application/json');
	  echo json_encode(array('result' => 'success'));	 
  }
}

$registrationController = new Registration();
$registrationController->index();