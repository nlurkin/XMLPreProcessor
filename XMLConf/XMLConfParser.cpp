/*
 * XMLConfParser.cpp
 *
 *  Created on: Apr 10, 2015
 *      Author: ncl
 */

#include "XMLConfParser.h"
#include <iostream>
#include <string>
#include <cstring>

/**
 *
 * @param fileName Full path to the XML file to be read
 * @return true if the file could be read and parsed. Else false.
 */
bool XMLConfParser::readFile(std::string fileName){
	if(fDoc != NULL) closeFile();
	fDoc = xmlParseFile(fileName.data());

	if (fDoc == NULL ) {
		std::stringstream ss;
		xmlErrorPtr errPtr = xmlGetLastError();

		ss << "Document not parsed successfully." << std::endl;
		ss << "File: " << errPtr->file << " at (l:";
		ss << errPtr->line << ", c:" << errPtr->int2;
		ss << "): " << errPtr->message << std::endl;
		throw XMLConfParserFatalException(ss);
		return false;
	}

	fRoot = xmlDocGetRootElement(fDoc);

	if (fRoot == NULL) {
		throw XMLConfParserFatalException("Document is empty.");
		return false;
	}
	return true;
}

/**
 * Fill the variable passed by reference with the value found at the specified path in the XML.
 * If the path does not exist in the XML, the variable is untouched.
 * @warning Fill the error stack
 * @param path Path to retrieve. The path "a.b.c" corresponds to the xml structure \<a\>\<b\>\<c\>1\</c\>\</b\>\</a\>
 * @param ref Variable to fill the the value found at path.
 * @return true in case of success (path is found and the value can be transformed into int). Else false.
 */
bool XMLConfParser::getValue(std::string path, int& ref) {
	xmlNodePtr n = findPathNode(path);
	if(n){
		char* endptr;
		std::string s = getNodeString(n);
		int val = strtol(s.data(), &endptr, 0);
		if(!*endptr){
			ref = val;
			fReadSuccess++;
			return true;
		}
		else{
			std::stringstream ss;
			ss << "Unable to convert " << s << " to integer";
			fErrorStack.addError(ss);
		}
	}
	return false;
}

/**
 * Fill the variable passed by reference with the value found at the specified path in the XML.
 * If the path does not exist in the XML, the variable is untouched.
 * @warning Fill the error stack
 * @param path Path to retrieve. The path "a.b.c" corresponds to the xml structure \<a\>\<b\>\<c\>1\</c\>\</b\>\</a\>
 * @param ref Variable to fill the the value found at path.
 * @return true in case of success (path is found and the value can be transformed into unsigned int). Else false.
 */
bool XMLConfParser::getValue(std::string path, unsigned int& ref) {
	xmlNodePtr n = findPathNode(path);
	if(n){
		char* endptr;
		std::string s = getNodeString(n);
		unsigned int val = strtoul(s.data(), &endptr, 0);
		if(!*endptr){
			ref = val;
			fReadSuccess++;
			return true;
		}
		else{
			std::stringstream ss;
			ss << "Unable to convert " << s << " to unsigned integer";
			fErrorStack.addError(ss);
		}
	}
	return false;
}

/**
 * Fill the variable passed by reference with the value found at the specified path in the XML.
 * If the path does not exist in the XML, the variable is untouched.
 * @warning Fill the error stack
 * @param path Path to retrieve. The path "a.b.c" corresponds to the xml structure \<a\>\<b\>\<c\>1\</c\>\</b\>\</a\>
 * @param ref Variable to fill the the value found at path.
 * @return true in case of success (path is found and the value can be transformed into float). Else false.
 */
bool XMLConfParser::getValue(std::string path, float& ref) {
	xmlNodePtr n = findPathNode(path);
	if(n){
		char* endptr;
		std::string s = getNodeString(n);
		double val = strtof(s.data(), &endptr);
		if(!*endptr){
			ref = val;
			fReadSuccess++;
			return true;
		}
		else{
			std::stringstream ss;
			ss << "Unable to convert " << s << " to float";
			fErrorStack.addError(ss);
		}
	}
	return false;
}

/**
 * Fill the variable passed by reference with the value found at the specified path in the XML.
 * If the path does not exist in the XML, the variable is untouched.
 * @warning Fill the error stack
 * @param path Path to retrieve. The path "a.b.c" corresponds to the xml structure \<a\>\<b\>\<c\>1\</c\>\</b\>\</a\>
 * @param ref Variable to fill the the value found at path.
 * @return true in case of success (path is found and the value can be transformed into double). Else false.
 */
bool XMLConfParser::getValue(std::string path, double& ref) {
	xmlNodePtr n = findPathNode(path);
	if(n){
		char* endptr;
		std::string s = getNodeString(n);
		double val = strtod(s.data(), &endptr);
		if(!*endptr){
			ref = val;
			fReadSuccess++;
			return true;
		}
		else{
			std::stringstream ss;
			ss << "Unable to convert " << s << " to double";
			fErrorStack.addError(ss);
		}
	}
	return false;
}

/**
 * Fill the variable passed by reference with the value found at the specified path in the XML.
 * If the path does not exist in the XML, the variable is untouched.
 * @param path Path to retrieve. The path "a.b.c" corresponds to the xml structure \<a\>\<b\>\<c\>1\</c\>\</b\>\</a\>
 * @param ref Variable to fill the the value found at path.
 * @return true in case of success (path is found). Else false.
 */
bool XMLConfParser::getValue(std::string path, std::string& ref) {
	xmlNodePtr n = findPathNode(path);
	if(n){
		std::string s = getNodeString(n);
		ref = s;
		fReadSuccess++;
		return true;
	}
	return false;
}

/**
 * Fill the variable passed by reference with the value found at the specified path in the XML.
 * If the path does not exist in the XML, the variable is untouched.
 * @param path Path to retrieve. The path "a.b.c" corresponds to the xml structure \<a\>\<b\>\<c\>1\</c\>\</b\>\</a\>
 * @param ref Variable to fill the the value found at path.
 * @return true in case of success (path is found). Else false.
 */
bool XMLConfParser::getValue(std::string path, char *ref) {
	xmlNodePtr n = findPathNode(path);
	if(n){
		std::string s = getNodeString(n);
		strcpy(ref, s.data());
		fReadSuccess++;
		return true;
	}
	return false;
}

/**
 *
 * @param path Path whose existence is checked. The path "a.b.c" corresponds to the xml structure \<a\>\<b\>\<c\>1\</c\>\</b\>\</a\>
 * @return true if the path is found in the XML, else false.
 */
bool XMLConfParser::pathExists(std::string path) {
	xmlNodePtr n = findPathNode(path);
	addCheckElement(path);
	if(n){
		addListDiffElement(path);
		fReadSuccess++;
		return true;
	}
	return false;
}

/**
 * The check for additional tags prints a list of tags present in the XML file
 * but not present in the list of provided elements.
 */
void XMLConfParser::startCheckAdditional() {
	xmlNodePtr root = getRoot();

	if(root){
		walkTreeCompare("", root);
	}
}

/**
 *
 * @return Path of the first element in the list of modified elements. Empty string if the list is empty.
 */
std::string XMLConfParser::getFirstDiff() {
	fListDiffIterator = fListDiff.begin();
	if(fListDiffIterator!= fListDiff.end()) return *fListDiffIterator;
	return "";
}

/**
 *
 * @return Path of the next element in the list of modified elements. Empty string if the iteration through the list is over.
 */
std::string XMLConfParser::getNextDiff() {
	fListDiffIterator++;
	if(fListDiffIterator!= fListDiff.end()) return *fListDiffIterator;
	return "";
}

/**
 * Recursively find all the paths found in the XML tree and add them in the list of modified elements.
 * @param prefix Current prefix for the path
 * @param node Pointer to a node
 */
void XMLConfParser::walkTreeCompare(std::string prefix, xmlNodePtr node) {
	xmlNodePtr n = node->children;

	std::string path;
	if(prefix=="") path = (const char*)node->name;
	else path = prefix + "." + std::string((const char*)node->name);
	int arrIndex = isArrayNode(node);
	std::stringstream ss;
	ss << arrIndex;
	if(arrIndex>=0) path = path + "[" + ss.str() + "]";

	if(getNSiblings(n)==1){ // This node has only one child. Can only be a text
		fListAdditional.insert(path);
		return;
	}
	while(n){
		walkTreeCompare(path, n);
		n = n->next;
	}
}

/**
 *
 * @param path Path to add. The path "a.b.c" corresponds to the xml structure \<a\>\<b\>\<c\>1\</c\>\</b\>\</a\>
 */
void XMLConfParser::addCheckElement(std::string path) {
	fListAdditional.erase(path);
}

void XMLConfParser::printAdditional() {
	std::set<std::string>::iterator it;

	if(fListAdditional.size()==0) std::cout << "None" << std::endl;
	else{
		for(it=fListAdditional.begin(); it!=fListAdditional.end(); it++){
			std::cout << *it << std::endl;
		}
	}
}

/**
 *
 * @param path Path to add. The path "a.b.c" corresponds to the xml structure \<a\>\<b\>\<c\>1\</c\>\</b\>\</a\>
 */
void XMLConfParser::addListDiffElement(std::string path) {
	fListDiff.push_back(path);
}
