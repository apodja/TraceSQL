--TEST--
test1() Basic test
--EXTENSIONS--
query_profiler
--FILE--
<?php
$ret = test1();

var_dump($ret);
?>
--EXPECT--
The extension query_profiler is loaded and working!
NULL
