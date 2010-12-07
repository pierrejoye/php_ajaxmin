<?php
error_reporting(E_ALL|E_NOTICE);

$css = file_get_contents(__DIR__ . '/example.css');
$len = strlen($css);
$ret = ajaxmin_minify_css($css);
$len_min = strlen($ret);
echo "$len > $len_min\n";


$settings = new CssSettings();
var_dump($settings->IndentSpaces);
$settings->IndentSpaces = 8;
var_dump($settings->IndentSpaces);

var_dump($settings->MinifyExpressions);
$settings->MinifyExpressions = FALSE;
var_dump($settings->MinifyExpressions);

echo "\n";

var_dump($settings->TermSemicolons);
$settings->TermSemicolons = TRUE;
var_dump($settings->TermSemicolons);

echo "\n";

var_dump($settings->ExpandOutput);
$settings->ExpandOutput = FALSE;
var_dump($settings->ExpandOutput);

echo "\n";

var_dump($settings->CommentMode);
$settings->CommentMode = CssSettings::COMMENT_ALL;
var_dump($settings->CommentMode);

echo "\n";

var_dump($settings->ColorNames);
$settings->ColorNames = CssSettings::COLOR_HEX;
var_dump($settings->ColorNames);

$ret = ajaxmin_minify_css($css, $settings);
$len_min = strlen($ret);
echo "$len > $len_min\n";
file_put_contents('a.txt', $ret);
