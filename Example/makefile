INC = -I/usr/include/libxml2 -I../XMLConf
LIBDIR = -L../XMLConf
LIB = -lxml2 -lxmlpreprocessor
CFLAGS	= -O -Wall -fPIC -g3 --std=c++11


all: outputStruct.xml

outputStruct.xml: example_proxy example_parse example_write
	@./run_all_examples.sh

example_proxy: main_proxy.cc ex_struct.h ex_structProxy.h ex_structProxy.cc
	g++ -o $@ $^ $(CFLAGS) $(INC) $(LIBDIR) $(LIB)   

example_parse: main_parser.cc ex_struct.h
	g++ -o $@ $^ $(CFLAGS) $(INC) $(LIBDIR) $(LIB)   

example_write: main_writer.cc ex_struct.h
	g++ -o $@ $^ $(CFLAGS) $(INC) $(LIBDIR) $(LIB)   

ex_structProxy.h: ex_struct.h
	../xmlpp.py ex_struct.h --struct exampleStruct

.PHONY: all clean dependents
	
clean:
	rm -f ex_structProxy.*
	rm -f example_writer.xml
	rm -f example_parse
	rm -f example_write
	rm -f example_proxy
	rm -f parse_output.txt
	rm -rf DocGen
	rm -f outputStruct.xml