/*
 * main_writer.cc
 *
 *  Created on: 18 Apr 2015
 *      Author: nlurkin
 */

#include "XMLConfWriter.h"
#include "ex_struct.h"
#include <iostream>
#include <cstring>
using namespace std;


int main(){
	XMLConfWriter writer;
	exampleStruct test;

	test.version = 42;
	strcpy(test.name, "my_example");
	test.my_substruct.my_double = 5.5;

	writer.createDocument("exampleStruct");
	writer.addPathAsHex("exampleStruct.version", test.version);
	writer.addPath("exampleStruct.name", test.name);
	writer.addPath("exampleStruct.my_substruct.my_double", test.my_substruct.my_double);

	writer.writeDocument("example_writer.xml");
}
