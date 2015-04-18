/*
 * main_parser.cc
 *
 *  Created on: 18 Apr 2015
 *      Author: nlurkin
 */

#include "XMLConfParser.h"
#include "ex_struct.h"
#include <iostream>
#include <string>
using namespace std;

int main(){
	XMLConfParser parser;
	exampleStruct test;
	parser.readFile("partial_ex_struct.xml");
	parser.getValue("exampleStruct.version", test.version);
	parser.getValue("exampleStruct.my_substruct.my_double", test.my_substruct.my_double);


	if(!parser.pathExists("exampleStruct.name"))
		cout << "pe: exampleStruct.name is not found in the XML" << endl;
	if(!parser.pathExists("exampleStruct.my_substruct.my_integer"))
		cout << "pe: exampleStruct.my_substruct.my_integer is not found in the XML" << endl;
	if(!parser.pathExists("exampleStruct.my_substruct.my_double"))
		cout << "pe: exampleStruct.my_substruct.my_double is not found in the XML" << endl;

	parser.startCheckAdditional();
	parser.addCheckElement("exampleStruct.version");
	parser.addCheckElement("exampleStruct.my_array[0]");
	parser.addCheckElement("exampleStruct.my_array[1]");
	cout << "The following fields are present in the XML but not in the above addCheckElement" << endl;
	parser.printAdditional();

	string diff = parser.getFirstDiff();
	while(diff.compare("")!=0){
		cout << "d: " << diff << " has received a new value" << endl;
		diff = parser.getNextDiff();
	}
}

