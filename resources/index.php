<?php

$files = array();
$code = $_GET["code"];
$region = $_GET["region"];
$covers = glob("$region/$code*");

if (sizeof($covers) > 0)
{
	$filepath = $covers[0];
	
	if (file_exists($filepath))
	{
		touch($filepath,filemtime($filepath),time());
		$path_parts=pathinfo($filepath);
		switch(strtolower($path_parts['extension']))
		{
			case "gif":
			header("Content-type: image/gif");
			break;
			case "jpg":
			case "jpeg":
			header("Content-type: image/jpeg");
			break;
			case "png":
			header("Content-type: image/png");
			break;
			case "bmp":
			header("Content-type: image/bmp");
			break;
		}
		header("Accept-Ranges: bytes");
		header('Content-Length: ' . filesize($filepath));
		readfile($filepath);
	}
}
else
{
	//header( "HTTP/1.0 404 Not Found");
	header("Content-type: image/bmp");
	header('Content-Length: ' . filesize("default.bmp"));
	header("Accept-Ranges: bytes");
	readfile("default.bmp");
}
?>