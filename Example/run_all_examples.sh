#!/bin/sh

export LD_LIBRARY_PATH=../XMLConf:$LD_LIBRARY_PATH
outproxy=`./example_proxy` 
./example_write
outparse=`./example_parse`

if [ ! -d DocGen ]; then 
	mkdir DocGen
fi

echo "$outproxy" | grep "val:" > DocGen/proxy_values.txt
echo "$outproxy" | grep "mod:" > DocGen/proxy_mod.txt
echo "$outproxy" | grep -v "val:" | grep -v "mod:" > DocGen/proxy_diff.txt

echo "$outparse" | grep "pe:"  > DocGen/parser_path_exist.txt
echo "$outparse" | grep "d:"  > DocGen/parser_diff.txt
echo "$outparse" | grep -v "pe:" | grep -v "d:"  > DocGen/parser_add.txt

echo "/*! \page list_pe" > DocGen/parser_list_pe.dox
cat main_parser.cc | grep "pe:" | sed "s/.*pe: \(.*\) is not.*/ - \1/" >> DocGen/parser_list_pe.dox
echo " */" >> DocGen/parser_list_pe.dox