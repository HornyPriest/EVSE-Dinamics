<?php

if ( $_SERVER["REMOTE_ADDR"] == "195.201.75.105" || $_SERVER["REMOTE_ADDR"] == "195.201.75.107" || $_SERVER["REMOTE_ADDR"] == "176.9.29.14" || $_SERVER["REMOTE_ADDR"] == "93.87.177.6")
{
	$price =$_REQUEST['p'];
	$id =$_REQUEST['id'];
	if ($id==1)
	{
	  $s="mosquitto_pub -h 195.201.75.107 -p 31883 -t parkomat/vilharjeva/Price -m " . $price;
	}
	else if ($id==2)
	{
		$s="mosquitto_pub -h 195.201.75.107 -p 31883 -t parkomat/test_trgof/Price -m " . $price;
	}
	system ($s);
}
else
{
	header("HTTP/1,0 403 Zabranjeno za ovu IP adresu");
}


?>
