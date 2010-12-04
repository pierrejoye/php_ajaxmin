<?php

$css = file_get_contents(__DIR__ . '/example.css');
$len = strlen($css);
$ret = ajaxmin_minify_css($css);
$len_min = strlen($ret);
echo "$len > $len_min\n";
