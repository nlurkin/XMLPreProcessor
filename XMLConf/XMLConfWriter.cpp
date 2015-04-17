/*
 * XMLConfWriter.cpp
 *
 *  Created on: Apr 10, 2015
 *      Author: ncl
 */

#include "XMLConfWriter.h"
#include <sstream>
#include <iostream>

XMLConfWriter::XMLConfWriter() {
}

XMLConfWriter::~XMLConfWriter() {
}

void XMLConfWriter::createDocument(std::string structName) {
	/*
	 * Creates a new document, a node and set it as a root node
	 */
	fDoc = xmlNewDoc(NULL);

	fRoot = xmlNewNode(NULL, BAD_CAST structName.data());
	xmlDocSetRootElement(fDoc, fRoot);
}

xmlNodePtr XMLConfWriter::addNode(std::string nodeName, xmlNodePtr node) {
	return xmlNewChild(node, NULL, BAD_CAST nodeName.data(), NULL);
}

xmlNodePtr XMLConfWriter::addNodeArray(std::string nodeName, int index,
		xmlNodePtr node) {
	std::stringstream s;
	s << index;
	xmlNodePtr child = xmlNewChild(node, NULL, BAD_CAST nodeName.data(), NULL);
	xmlNewProp(child, BAD_CAST "id", BAD_CAST s.str().data());
	return child;
}

void XMLConfWriter::addNodeValue(std::string value, xmlNodePtr node) {
	xmlNodePtr text = xmlNewText(BAD_CAST value.data());
	xmlAddChild(node, text);
}

xmlNodePtr XMLConfWriter::addPathNode(std::string path) {
	std::vector<std::string> list = tokenizePath(path);

	xmlNodePtr cur;
	if(!xmlStrcmp(getRoot()->name, (const xmlChar *)list.front().data())) cur = getRoot();
	else return NULL;
	if(cur){
		list.erase(list.begin());
		return addPathNode(list, cur);
	}
	return NULL;
}

xmlNodePtr XMLConfWriter::addPathNode(std::vector<std::string> path, xmlNodePtr node) {
	xmlNodePtr cur;
	int id = isArrayNode(path.front());
	if(id==-1) cur = findChildNode(path.front(), node);
	else cur = findArrayNode(path.front(), id, node);

	if(!cur){
		if(id==-1) cur = addNode(path.front(), node);
		else cur = addNodeArray(path.front(), id, node);
	}
	if(path.size()==1) return cur;

	path.erase(path.begin());
	return addPathNode(path, cur);
}

void XMLConfWriter::printDocument() {
	/*
	 * Dumping document to stdio or file
	 */
	xmlSaveFormatFileEnc("-", fDoc, NULL, 1);
}

bool XMLConfWriter::writeDocument(std::string fileName) {
	return xmlSaveFormatFileEnc(fileName.data(), fDoc, NULL, 1)>0;
}

bool XMLConfWriter::addPath(std::string path, unsigned int ref) {
	std::stringstream s;
	s << ref;
	xmlNodePtr node = addPathNode(path);
	if(node){
		addNodeValue(s.str(), node);
		return true;
	}
	return false;
}

bool XMLConfWriter::addPath(std::string path, int ref) {
	std::stringstream s;
	s << ref;
	xmlNodePtr node = addPathNode(path);
	if(node){
		addNodeValue(s.str(), node);
		return true;
	}
	return false;
}

bool XMLConfWriter::addPath(std::string path, double ref) {
	std::stringstream s;
	s << ref;
	xmlNodePtr node = addPathNode(path);
	if(node){
		addNodeValue(s.str(), node);
		return true;
	}
	return false;
}

bool XMLConfWriter::addPath(std::string path, char* ref) {
	xmlNodePtr node = addPathNode(path);
	if(node){
		addNodeValue(ref, node);
		return true;
	}
	return false;
}

bool XMLConfWriter::addPath(std::string path, std::string ref) {
	xmlNodePtr node = addPathNode(path);
	if(node){
		addNodeValue(ref, node);
		return true;
	}
	return false;
}

bool XMLConfWriter::addPathAsHex(std::string path, int ref){
	std::stringstream s;
	s << "0x" << std::hex << ref;
	return addPath(path, s.str());
}
