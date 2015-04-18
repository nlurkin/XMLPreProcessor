#!/bin/sh

export LD_LIBRARY_PATH=../XMLConf:$LD_LIBRARY_PATH
./example_proxy_create
./example_write
out=`./example_parse`

if [ ! -d DocGen ]; then 
	mkdir DocGen
fi

echo "$out" | grep "pe:"  > DocGen/parser_path_exist.txt
echo "$out" | grep "d:"  > DocGen/parser_diff.txt
echo "$out" | grep -v "pe:" | grep -v "d:"  > DocGen/parser_add.txt

echo "/*! \page list_pe" > DocGen/parser_list_pe.dox
cat main_parser.cc | grep "pe:" | sed "s/.*pe: \(.*\) is not.*/ - \1/" >> DocGen/parser_list_pe.dox
echo " */" >> DocGen/parser_list_pe.dox