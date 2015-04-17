/*
 * XMLConfDocument.cpp
 *
 *  Created on: Apr 10, 2015
 *      Author: ncl
 */

#include "XMLConfDocument.h"

#include <iostream>

#include <boost/foreach.hpp>
#include <boost/tokenizer.hpp>

std::vector<std::string> tokenize(std::string s, char const *separator){
    boost::char_separator<char> sep(separator);
    boost::tokenizer< boost::char_separator<char> > tokens(s, sep);
    std::vector<std::string> ret;
    BOOST_FOREACH (const std::string& t, tokens) {
        ret.push_back(t);
    }
    return ret;
}

xmlNodePtr XMLConfDocument::findChildNode(std::string nodeName, xmlNodePtr node){
	node = node->xmlChildrenNode;
	while (node != NULL) {
		if ((!xmlStrcmp(node->name, (const xmlChar *)nodeName.data()))) return node;
		node = node->next;
	}
	return NULL;
}

xmlNodePtr XMLConfDocument::findNextSiblingNode(std::string nodeName, xmlNodePtr node){
	node = node->next;
	while (node != NULL) {
		if ((!xmlStrcmp(node->name, (const xmlChar *)nodeName.data()))) return node;
		node = node->next;
	}
	return NULL;
}

void XMLConfDocument::printNodeValue(xmlNodePtr node){
	if(node) std::cout << node->name << ": " << getNodeString(node) << std::endl;
}

void XMLConfDocument::closeFile() {
	if (fDoc) {
		xmlFreeDoc(fDoc);
		xmlCleanupParser();
	}
}

std::string XMLConfDocument::getNodeString(xmlNodePtr node){
	xmlChar *key;
	key = xmlNodeListGetString(fDoc, node->xmlChildrenNode, 1);
	std::string sRet(reinterpret_cast<const char*>(key));
	xmlFree(key);
	return sRet;
}

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

void XMLConfDocument::printNode(xmlNodePtr node) {
	std::cout << node->name << std::endl;
	node = node->xmlChildrenNode;
	while(node){
		if(xmlStrcmp(node->name, (const xmlChar *)"text")) std::cout << "\t" << node->name << std::endl;
		node = node->next;
	}
}

xmlNodePtr XMLConfDocument::findArrayNode(std::string nodeName, int index,
		xmlNodePtr node) {
	xmlNodePtr cur = findChildNode(nodeName, node);

	while(cur){
		std::string id = readAttribute("id", cur);
		if(!id.compare("")) return NULL;
		if(atoi(id.data())==index) return cur;
		cur = findNextSiblingNode(nodeName, cur);
	}
	return NULL;
}

int XMLConfDocument::isArrayNode(std::string &name){
	std::vector<std::string> t = tokenize(name, "[]");

	if(t.size()>=2){
		name = t[0];
		return std::atoi(t[1].data());
	}
	return -1;
}

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

std::string XMLConfDocument::readAttribute(std::string attributeName,
		xmlNodePtr node) {
	xmlChar *attr = xmlGetProp(node, (const xmlChar*)attributeName.data());
	std::string sattr(reinterpret_cast<const char*>(attr));
	xmlFree(attr);
	return sattr;
}

void XMLErrorStack::printStack() {
}

std::string XMLErrorStack::stringStack() {
}
