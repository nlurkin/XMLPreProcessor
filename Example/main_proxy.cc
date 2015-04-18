/*
 * main_example.cc
 *
 *  Created on: 18 Apr 2015
 *      Author: nlurkin
 */

#include "ex_struct.h"
#include "ex_structProxy.h"

int main(){
	exampleStruct test;

	//test.a = 5;
	//test.b.a = 2;

	xml_create_exampleStruct(&test, "ex_struct.xml");
}
