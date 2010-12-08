<?php

$js = 'function foo(a)
{
    function bar() {c = 233;}

    a.b = 12345;
    a.c = 12345;
    a.d = 12345;
    a.e = 12345;
}
';
echo $js . "\n";
$ret = ajaxmin_minify_js($js);
echo $ret;

$settings = new CodeSettings();
//$settings->IndentSize = 3;
var_dump($settings->IndentSize);

$ret = ajaxmin_minify_js($js, $settings);
echo $ret;
