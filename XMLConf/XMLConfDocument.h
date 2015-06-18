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

#include "XMLConfVersion.h"

#ifndef XMLCONFDOCUMENT_H_
#define XMLCONFDOCUMENT_H_

std::vector<std::string> tokenize(std::string s, char const *separator); //!< Tokenize a string according to separator
/**
 *
 * @param s String to tokenize
 * @return vector of tokens
 */
//! Tokenize a path (separator is .)
inline std::vector<std::string> tokenizePath(std::string s) {return tokenize(s, ".");}

/**
 * Class containing an error stack. This is used to allow printing the error whenever convenient
 * and not when the error happens.
 */
class XMLErrorStack {
public:
	//! Constructor
	XMLErrorStack() : fStack() {};
	//! Destructor
	virtual ~XMLErrorStack() {};

	//! Clear the stack. Remove all errors.
	void clear() { fStack.clear(); };
	//! Add an error in the stack.
	void addError(std::string s) { fStack.push_back(s); }
	//! Add an error in the stack.
	void addError(std::stringstream &s) { fStack.push_back(s.str()); }

	void printStack(); //!< Print the stack.
	std::string stringStack(); //!< Return the stack as a string.
private:
	std::vector<std::string> fStack; //!< Vector containing the errors.
};

/**
 * Back-end class to navigate the XML file.
 */
class XMLConfDocument {
public:
	//! Constructor
	XMLConfDocument() : fDoc(NULL), fRoot(NULL), fErrorStack() {};
	//! Copy Constructor
	XMLConfDocument(XMLConfDocument & d) : fDoc(d.fDoc), fRoot(d.fRoot), fErrorStack() {};
	//! Destructor
	virtual ~XMLConfDocument() { closeFile(); };

	//! Copy operator
	XMLConfDocument& operator=(XMLConfDocument & d) {fDoc = d.fDoc; fRoot = d.fRoot; return *this;};

	void closeFile(); //!< Close the current document. Free allocated memory.

	int getNSiblings(xmlNodePtr node); //!< Return the number of siblings of node. node is included in the count.

	xmlNodePtr findChildNode(std::string nodeName, xmlNodePtr node); //!< Find a child of node with name nodeName
	xmlNodePtr findNextSiblingNode(std::string nodeName, xmlNodePtr node); //!< Find next sibling of node with name nodeName
	xmlNodePtr findArrayNode(std::string nodeName, int index, xmlNodePtr node); //!< Find a child of node with name nodeName and array index index
	void printNodeValue(xmlNodePtr node); //!< Print the value of node
	void printNode(xmlNodePtr node); //!< Print the node
	std::string getNodeString(xmlNodePtr node); //!< Return a the value of the node as a string

	xmlNodePtr findPathNode(std::string path); //!< Find a node corresponding to path
	xmlNodePtr findPathNode(std::vector<std::string> path, xmlNodePtr cur); //!< Find a node corresponding to path under the current node

	int isArrayNode(std::string &name); //!< Does the provided name corresponds to an array element?
	int isArrayNode(xmlNodePtr node); //!< Does the provided node corresponds to an array element?
	std::string readAttribute(std::string attributeName, xmlNodePtr node); //!< Read an attribute of the node

	/**
	 *
	 * @return Pointer to the root node.
	 */
	//! Return the root node of the document
	xmlNodePtr getRoot() const { return fRoot; };

	/**
	 *
	 * @return Error stack
	 */
	//! Return the error stack.
	XMLErrorStack *getLastError() { return &fErrorStack; };

	/**
	 *
	 * @return true if XML document is initialised and operations
	 * are allowed, else false.
	 */
	//! Is the XML document initialised.
	bool isDocumentInitialised() { return fDoc!=NULL; };

protected:
	xmlDocPtr fDoc; //!< Pointer to the XML document
	xmlNodePtr fRoot; //!< Pointer to the root node of the document

	XMLErrorStack fErrorStack; //!< Error stack
};

#endif /* XMLCONFDOCUMENT_H_ */
