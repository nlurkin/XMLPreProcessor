/*
 * XMLConfWriter.cpp
 *
 *  Created on: Apr 10, 2015
 *      Author: ncl
 */

#include "XMLConfWriter.h"
#include <sstream>
#include <iostream>

/**
 *
 * @param structName Name of the root node
 */
void XMLConfWriter::createDocument(std::string structName) {
	/*
	 * Creates a new document, a node and set it as a root node
	 */
	fDoc = xmlNewDoc(NULL);

	fRoot = xmlNewNode(NULL, BAD_CAST structName.data());
	xmlDocSetRootElement(fDoc, fRoot);
}

/**
 *
 * @param nodeName Name of the child node to add
 * @param node Pointer to the parent node
 * @return Pointer to the newly created node
 */
xmlNodePtr XMLConfWriter::addNode(std::string nodeName, xmlNodePtr node) {
	return xmlNewChild(node, NULL, BAD_CAST nodeName.data(), NULL);
}

/**
 * An array node as an id attribute giving the index in the array.
 * @param nodeName Name of the child node to add
 * @param index Array index of the node
 * @param node Pointer to the parent node
 * @return Pointer to the newly created node
 */
xmlNodePtr XMLConfWriter::addNodeArray(std::string nodeName, int index,
		xmlNodePtr node) {
	std::stringstream s;
	s << index;
	xmlNodePtr child = xmlNewChild(node, NULL, BAD_CAST nodeName.data(), NULL);
	xmlNewProp(child, BAD_CAST "id", BAD_CAST s.str().data());
	return child;
}

/**
 *
 * @param value Value to assign to the node
 * @param node Pointer to a node
 */
void XMLConfWriter::addNodeValue(std::string value, xmlNodePtr node) {
	xmlNodePtr text = xmlNewText(BAD_CAST value.data());
	xmlAddChild(node, text);
}

/**
 *
 * @param path Path to add. The path "a.b.c" corresponds to the xml structure \<a\>\<b\>\<c\>1\</c\>\</b\>\</a\>
 * @return Pointer to the newly created node. NULL pointer if unsuccessful.
 */
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

/**
 *
 * @param path Vectorised path to add to the node. The vectorised path of "a.b.c" is {a,b,c}
 * and corresponds to the xml structure \<a\>\<b\>\<c\>1\</c\>\</b\>\</a\>
 * @param node Pointer to a node
 * @return Pointer to the newly created node. NULL pointer if unsuccessful.
 */
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

/**
 *
 * @param fileName Full path to the file to write
 * @return true if the file has been successfully written, else false.
 */
bool XMLConfWriter::writeDocument(std::string fileName) {
	return xmlSaveFormatFileEnc(fileName.data(), fDoc, NULL, 1)>0;
}

/**
 * Add a new path to the XML document, with the value contained in ref
 * @param path Path to add. The path "a.b.c" corresponds to the xml structure \<a\>\<b\>\<c\>1\</c\>\</b\>\</a\>
 * @param ref Reference value
 * @return true if the path could be added, else false
 */
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

/**
 * Add a new path to the XML document, with the value contained in ref
 * @param path Path to add. The path "a.b.c" corresponds to the xml structure \<a\>\<b\>\<c\>1\</c\>\</b\>\</a\>
 * @param ref Reference value
 * @return true if the path could be added, else false
 */
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

/**
 * Add a new path to the XML document, with the value contained in ref
 * @param path Path to add. The path "a.b.c" corresponds to the xml structure \<a\>\<b\>\<c\>1\</c\>\</b\>\</a\>
 * @param ref Reference value
 * @return true if the path could be added, else false
 */
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

/**
 * Add a new path to the XML document, with the value contained in ref
 * @param path Path to add. The path "a.b.c" corresponds to the xml structure \<a\>\<b\>\<c\>1\</c\>\</b\>\</a\>
 * @param ref Reference value
 * @return true if the path could be added, else false
 */
bool XMLConfWriter::addPath(std::string path, char* ref) {
	xmlNodePtr node = addPathNode(path);
	if(node){
		addNodeValue(ref, node);
		return true;
	}
	return false;
}

/**
 * Add a new path to the XML document, with the value contained in ref
 * @param path Path to add. The path "a.b.c" corresponds to the xml structure \<a\>\<b\>\<c\>1\</c\>\</b\>\</a\>
 * @param ref Reference value
 * @return true if the path could be added, else false
 */
bool XMLConfWriter::addPath(std::string path, std::string ref) {
	xmlNodePtr node = addPathNode(path);
	if(node){
		addNodeValue(ref, node);
		return true;
	}
	return false;
}

/**
 * Add a new path to the XML document, with the value contained in ref. The integer value is printed
 * in hexadecimal format (0x..)
 * @param path Path to add. The path "a.b.c" corresponds to the xml structure \<a\>\<b\>\<c\>1\</c\>\</b\>\</a\>
 * @param ref Reference value
 * @return true if the path could be added, else false
 */
bool XMLConfWriter::addPathAsHex(std::string path, int ref){
	std::stringstream s;
	s << "0x" << std::hex << ref;
	return addPath(path, s.str());
}
