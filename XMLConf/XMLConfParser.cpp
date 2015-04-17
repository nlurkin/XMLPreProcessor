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
	if(fDoc != NULL) closeFile();

	fDoc = xmlParseFile(fileName.data());

	if (fDoc == NULL ) {
		throw XMLConfParserFatalException("Document not parsed successfully.");
		return false;
	}

	fRoot = xmlDocGetRootElement(fDoc);

	if (fRoot == NULL) {
		throw XMLConfParserFatalException("Document is empty.");
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

bool XMLConfParser::getValue(std::string path, unsigned int& ref) {
	xmlNodePtr n = findPathNode(path);
	if(n){
		char* endptr;
		std::string s = getNodeString(n);
		double val = strtol(s.data(), &endptr, 0);
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

void XMLConfParser::startCheckAdditional() {
	xmlNodePtr root = getRoot();

	if(root){
		walkTreeCompare("", root);
	}
}

std::string XMLConfParser::getFirstDiff() {
	fListDiffIterator = fListDiff.begin();
	if(fListDiffIterator!= fListDiff.end()) return *fListDiffIterator;
	return "";
}

std::string XMLConfParser::getNextDiff() {
	fListDiffIterator++;
	if(fListDiffIterator!= fListDiff.end()) return *fListDiffIterator;
	return "";
}

void XMLConfParser::walkTreeCompare(std::string prefix, xmlNodePtr node) {
	xmlNodePtr n = node->children;

	std::string path;
	if(prefix=="") path = (const char*)node->name;
	else path = prefix + "." + std::string((const char*)node->name);

	if(getNSiblings(n)==1){ // This node has only one child. Can only be a text
		fListAdditional.insert(path);
		return;
	}
	while(n){
		walkTreeCompare(path, n);
		n = n->next;
	}
}

void XMLConfParser::addCheckElement(std::string path) {
	fListAdditional.erase(path);
}

void XMLConfParser::printAdditional() {
	for(auto it : fListAdditional){
		std::cout << it << std::endl;
	}
}

void XMLConfParser::addListDiffElement(std::string path) {
	fListDiff.push_back(path);
}
