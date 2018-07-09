<?php
$sure = intval($_GET['sure']);
//$sure = "1";
//libxml_use_internal_errors(true);
header('Content-Type: application/json');
$f = file_get_contents("../data/quran-simple.xml");
//$f = readfile("q.xml");
//print $f;
$xml = simplexml_load_string($f);
//$xml = simplexml_load_string(readfile("../data/quran-simple.xml"));
$json_string = json_encode($xml, JSON_UNESCAPED_UNICODE);
$json = json_decode($json_string);
echo json_encode($json->sura[$sure-1], JSON_UNESCAPED_UNICODE);//{"sura"}[$sure-1];
?>
