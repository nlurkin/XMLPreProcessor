/*
 * ex_struct.h
 *
 *  Created on: 18 Apr 2015
 *      Author: nlurkin
 */

#ifndef EX_STRUCT_H_
#define EX_STRUCT_H_

#include "xmlstring.h"
#include "hexinteger.h"

typedef struct exampleSubStruct_t {
	int my_integer;
	double my_double;
} exampleSubStruct;

typedef struct exampleStruct_t {
	hexinteger version;
	xmlchar name[XMLSTRING];
	int my_array[5];
	exampleSubStruct my_substruct;
} exampleStruct;


#endif /* EX_STRUCT_H_ */
