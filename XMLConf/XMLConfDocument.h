/*
 * XMLConfDocument.h
 *
 *  Created on: Apr 10, 2015
 *      Author: ncl
 */

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <vector>
#include <string>
#include <sstream>

#ifndef XMLCONFDOCUMENT_H_
#define XMLCONFDOCUMENT_H_

std::vector<std::string> tokenize(std::string s, char const *separator);
inline std::vector<std::string> tokenizePath(std::string s) {return tokenize(s, ".");};

class XMLErrorStack {
public:
	XMLErrorStack() {};
	virtual ~XMLErrorStack() {};

	void clear() { fStack.clear(); };
	void addError(std::string s) { fStack.push_back(s); }
	void addError(std::stringstream &s) { fStack.push_back(s.str()); }

	void printStack();
	std::string stringStack();
private:
	std::vector<std::string> fStack;
};

class XMLConfDocument {
public:
	XMLConfDocument() : fDoc(NULL), fRoot(NULL){};
	virtual ~XMLConfDocument() { closeFile(); };

	void closeFile();

	int getNSiblings(xmlNodePtr node);

	xmlNodePtr findChildNode(std::string nodeName, xmlNodePtr node);
	xmlNodePtr findNextSiblingNode(std::string nodeName, xmlNodePtr node);
	xmlNodePtr findArrayNode(std::string nodeName, int index, xmlNodePtr node);
	void printNodeValue(xmlNodePtr node);
	void printNode(xmlNodePtr node);
	std::string getNodeString(xmlNodePtr node);

	xmlNodePtr findPathNode(std::string path);
	xmlNodePtr findPathNode(std::vector<std::string> path, xmlNodePtr cur);

	int isArrayNode(std::string &name);
	std::string readAttribute(std::string attributeName, xmlNodePtr node);

	xmlNodePtr getRoot() const {
		return fRoot;
	}

	XMLErrorStack getLastError() { return fErrorStack; };

protected:
	xmlDocPtr fDoc;
	xmlNodePtr fRoot;

	XMLErrorStack fErrorStack;
};

#endif /* XMLCONFDOCUMENT_H_ */
