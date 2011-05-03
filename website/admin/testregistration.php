<?php
$ch = curl_init();

curl_setopt($ch, CURLOPT_URL, "http://neologism.deri.ie/admin/registration.php");
curl_setopt($ch, CURLOPT_POST, 1);
curl_setopt($ch, CURLOPT_POSTFIELDS, "customer_name=guido&organization=DERI&email=".urlencode('guido.cecilio@deri.org')."&website_uri=".urlencode('http://vocab.deri.ie')."&plan=".urlencode('this is a test'));

curl_setopt($ch, CURLOPT_RETURNTRANSFER, 1);
curl_setopt($ch, CURLOPT_HEADER, 0);

$output = curl_exec ($ch);
curl_close ($ch); 

echo $output;
