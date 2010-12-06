<?php

$css = file_get_contents(__DIR__ . '/example.css');
$len = strlen($css);
$ret = ajaxmin_minify_css($css);
$len_min = strlen($ret);
echo "$len > $len_min\n";


$settings = new CssSettings();
var_dump($settings->IndentSpaces);
var_dump($settings->MinifyExpressions);

$settings->MinifyExpressions = FALSE;
var_dump($settings->MinifyExpressions);

$settings->IndentSpaces = 8;
var_dump($settings->IndentSpaces);
