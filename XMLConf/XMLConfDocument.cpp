/*
 * XMLConfDocument.cpp
 *
 *  Created on: Apr 10, 2015
 *      Author: ncl
 */

#include "XMLConfDocument.h"

#include <iostream>

#include <boost/tokenizer.hpp>

/**
 *
 * @param s String to tokenize
 * @param separator List of separators
 * @return vector of string containing the tokens
 */
std::vector<std::string> tokenize(std::string s, char const *separator){
    boost::char_separator<char> sep(separator);
    boost::tokenizer< boost::char_separator<char> > tokens(s, sep);
    boost::tokenizer< boost::char_separator<char> >::iterator it;

    std::vector<std::string> ret;
    for(it = tokens.begin(); it != tokens.end(); it++){
        ret.push_back(*it);
    }
    return ret;
}

/**
 *
 * @param nodeName Searched child name(tag)
 * @param node Pointer to a node
 * @return If found, pointer to the node. Else NULL pointer.
 */
xmlNodePtr XMLConfDocument::findChildNode(std::string nodeName, xmlNodePtr node){
	node = node->xmlChildrenNode;
	while (node != NULL) {
		if ((!xmlStrcmp(node->name, (const xmlChar *)nodeName.data()))) return node;
		node = node->next;
	}
	return NULL;
}

/**
 *
 * @param nodeName Searched sibling name(tag)
 * @param node Pointer to a node
 * @return If found, pointer to the node. Else NULL pointer.
 */
xmlNodePtr XMLConfDocument::findNextSiblingNode(std::string nodeName, xmlNodePtr node){
	node = node->next;
	while (node != NULL) {
		if ((!xmlStrcmp(node->name, (const xmlChar *)nodeName.data()))) return node;
		node = node->next;
	}
	return NULL;
}

/**
 *
 * @param node Pointer to a node
 */
void XMLConfDocument::printNodeValue(xmlNodePtr node){
	if(node) std::cout << node->name << ": " << getNodeString(node) << std::endl;
}

void XMLConfDocument::closeFile() {
	if (fDoc) {
		xmlFreeDoc(fDoc);
		xmlCleanupParser();
	}
}

/**
 *
 * @param node Pointer to a node
 * @return String representation of the node value if found. Else empty string.
 */
std::string XMLConfDocument::getNodeString(xmlNodePtr node){
	xmlChar *key;
	key = xmlNodeListGetString(fDoc, node->xmlChildrenNode, 1);
	std::string sRet;
	if(key!=NULL) sRet = reinterpret_cast<const char*>(key);
	else sRet = "";
	xmlFree(key);
	return sRet;
}

/**
 * @warning Fills the error stack
 * @param path Path a.b.c corresponds to the XML structure <a><b><c>1</c></b></a>
 * @return If found, pointer to the node. Else NULL pointer.
 */
xmlNodePtr XMLConfDocument::findPathNode(std::string path) {
	std::vector<std::string> list = tokenizePath(path);

	xmlNodePtr cur;
	// First element of the path must be the root
	if(!xmlStrcmp(getRoot()->name, (const xmlChar *)list.front().data())) cur = getRoot();
	else{
		std::stringstream ss;
		ss << "Root node " << list.front() << " is not found in XML" << std::endl;
		fErrorStack.addError(ss);
		return NULL;
	}
	if(cur){
		list.erase(list.begin());
		return findPathNode(list, cur);
	}
	return NULL;
}

/**
 *
 * @param node Pointer to a node
 */
void XMLConfDocument::printNode(xmlNodePtr node) {
	std::cout << node->name << std::endl;
	node = node->xmlChildrenNode;
	while(node){
		if(xmlStrcmp(node->name, (const xmlChar *)"text")) std::cout << "\t" << node->name << std::endl;
		node = node->next;
	}
}

/**
 *
 * @param nodeName Searched child array name(tag)
 * @param index Searched array index
 * @param node Pointer to a node
 * @return If found, pointer to the node. Else NULL pointer.
 */
xmlNodePtr XMLConfDocument::findArrayNode(std::string nodeName, int index,
		xmlNodePtr node) {
	xmlNodePtr cur;

	for(cur = findChildNode(nodeName, node); cur; cur = findNextSiblingNode(nodeName, cur)){
		std::string id = readAttribute("id", cur);
		char* endptr;
		int i_id = strtol(id.data(), &endptr, 0);
		//if(!id.compare("")) return NULL;
		if(!id.compare("") || *endptr) continue;
		if(i_id==index) return cur;

	}
	return NULL;
}

/**
 *
 * @param name name to check. my_name[12] corresponds to an array.
 * @return Index of the array or -1 if not an array.
 */
int XMLConfDocument::isArrayNode(std::string &name){
	std::vector<std::string> t = tokenize(name, "[]");

	if(t.size()>=2){
		name = t[0];
		return std::atoi(t[1].data());
	}
	return -1;
}

/**
 * @warning Fills the error stack
 * @param path Vectorised path a.b.c is vectorised as {a,b,c} and corresponds to the XML structure <a><b><c>1</c></b></a>
 * @param cur Pointer to a node
 * @return If found, pointer to the node. Else NULL pointer.
 */
xmlNodePtr XMLConfDocument::findPathNode(std::vector<std::string> path, xmlNodePtr cur) {
	int id = isArrayNode(path.front());
	if(id==-1) cur = findChildNode(path.front(), cur);
	else cur = findArrayNode(path.front(), id, cur);

	if(!cur){
		std::stringstream ss;
		if(id==-1) ss << "Node " << path.front() << " is not found" << std::endl;
		else ss << "Node " << path.front() << "[" << id << "] is not found" << std::endl;
		fErrorStack.addError(ss);
		return NULL;
	}
	if(path.size()==1) return cur;

	path.erase(path.begin());
	return findPathNode(path, cur);
}

/**
 *
 * @param attributeName Name of the attribute
 * @param node Pointer to a node
 * @return String representation of the attribute if found, or empty string
 */
std::string XMLConfDocument::readAttribute(std::string attributeName,
		xmlNodePtr node) {
	xmlChar *attr = xmlGetProp(node, (const xmlChar*)attributeName.data());
	std::string sattr;
	if(attr!=NULL) sattr = reinterpret_cast<const char*>(attr);
	else sattr = "";
	xmlFree(attr);
	return sattr;
}

void XMLErrorStack::printStack() {
	std::cout << stringStack() << std::endl;
}

/**
 *
 * @return Single string containing all the error stack.
 */
std::string XMLErrorStack::stringStack() {
	std::stringstream ss;
	for(auto el : fStack){
		ss << el;
	}
	return ss.str();
}

/**
 * @param node Pointer to a node
 * @return Number of siblings of the provided node. If node is not NULL, the returned value is greater than 0.
 */
int XMLConfDocument::getNSiblings(xmlNodePtr node) {
	int i=0;
	while(node){
		i++;
		node = node->next;
	}
	return i;
}

/**
 * \<my_tag id="3"\>\</my_tag\> is an array node of index 3.
 * @param node A pointer to a node. A node with an id attribute compatible with an integer
 * @return Index of the array or -1 if not an array.
 */
int XMLConfDocument::isArrayNode(xmlNodePtr node) {
	std::string id = readAttribute("id", node);
	if(!id.compare("")) return -1;
	return atoi(id.data());
}

