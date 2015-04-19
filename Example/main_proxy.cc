/*
 * main_example.cc
 *
 *  Created on: 18 Apr 2015
 *      Author: nlurkin
 */

#include "ex_struct.h"
#include "ex_structProxy.h"
#include <iostream>
#include <cstring>
using namespace std;

#define PRINTVAR(v) #v << "= " << v << " "

int main(){
	exampleStruct test;

	xml_read_file("ex_struct.xml");
	xml_apply_exampleStruct(&test);

	xml_read_file("partial_ex_struct.xml");
	xml_apply_exampleStruct(&test);

	cout << "Differences:" << endl;
	xml_test_exampleStruct();
	cout << endl;

	void* ptr = xml_start_compare_exampleStruct(&test);

	while(ptr!=NULL){
		if(ptr==&(test.version)) cout << "mod: test.version was modified" << endl;
		else if(ptr==&(test.name)) cout << "mod: test.name was modified" << endl;
		else if(ptr==&(test.my_substruct.my_double)) cout << "mod: test.my_substruct.my_double was modified" << endl;
		else if(ptr==&(test.my_substruct.my_integer)) cout << "mod: test.my_substruct.my_integer was modified" << endl;
		else if(ptr==&(test.my_array[2])) cout << "mod: test.my_array[2] was modified" << endl;
		ptr = xml_next_compare_exampleStruct(&test);
	}

	std::cout << "val: " << PRINTVAR(test.version) << endl;
	std::cout << "val: " << PRINTVAR(test.name) << endl;
	std::cout << "val: " << PRINTVAR(test.my_array[0]) << endl;
	std::cout << "val: " << PRINTVAR(test.my_array[1]) << endl;
	std::cout << "val: " << PRINTVAR(test.my_array[2]) << endl;
	std::cout << "val: " << PRINTVAR(test.my_array[3]) << endl;
	std::cout << "val: " << PRINTVAR(test.my_array[4]) << endl;
	std::cout << "val: " << PRINTVAR(test.my_substruct.my_double) << endl;
	std::cout << "val: " << PRINTVAR(test.my_substruct.my_integer) << endl;

	strcpy(test.name, "my_modified_struct");
	xml_create_exampleStruct(&test, "outputStruct.xml");
}
