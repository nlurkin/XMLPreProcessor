/*
 * XMLConfParser.cpp
 *
 *  Created on: Apr 10, 2015
 *      Author: ncl
 */

#include "XMLConfParser.h"
#include <iostream>
#include <cstring>

bool XMLConfParser::readFile(std::string fileName){
	fDoc = xmlParseFile(fileName.data());

	if (fDoc == NULL ) {
		throw XMLConfParserException("Document not parsed successfully.");
		return false;
	}

	fRoot = xmlDocGetRootElement(fDoc);

	if (fRoot == NULL) {
		throw XMLConfParserException("Document is empty.");
		return false;
	}
	return true;
}

bool XMLConfParser::getValue(std::string path, int& ref) {
	xmlNodePtr n = findPathNode(path);
	if(n){
		char* endptr;
		std::string s = getNodeString(n);
		double val = strtol(s.data(), &endptr, 0);
		if(!*endptr) ref = val;
		else std::cout << "Unable to convert " << s << " to integer" << std::endl;
		return true;
	}
	return false;
}

bool XMLConfParser::getValue(std::string path, unsigned int& ref) {
	xmlNodePtr n = findPathNode(path);
	if(n){
		char* endptr;
		std::string s = getNodeString(n);
		double val = strtol(s.data(), &endptr, 0);
		if(!*endptr) ref = val;
		else std::cout << "Unable to convert " << s << " to integer" << std::endl;
		return true;
	}
	return false;
}

bool XMLConfParser::getValue(std::string path, double& ref) {
	xmlNodePtr n = findPathNode(path);
	if(n){
		char* endptr;
		std::string s = getNodeString(n);
		double val = strtod(s.data(), &endptr);
		if(!*endptr) ref = val;
		else std::cout << "Unable to convert " << s << " to integer" << std::endl;
		return true;
	}
	return false;
}

bool XMLConfParser::getValue(std::string path, std::string& ref) {
	xmlNodePtr n = findPathNode(path);
	if(n){
		std::string s = getNodeString(n);
		ref = s;
		return true;
	}
	return false;
}

bool XMLConfParser::getValue(std::string path, char *ref) {
	xmlNodePtr n = findPathNode(path);
	if(n){
		std::string s = getNodeString(n);
		strcpy(ref, s.data());
		return true;
	}
	return false;
}

