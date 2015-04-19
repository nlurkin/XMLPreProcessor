/*
 * XMLConfWriter.h
 *
 *  Created on: Apr 10, 2015
 *      Author: ncl
 */

#include <libxml/xmlmemory.h>
#include <libxml/xmlwriter.h>
#include <string>
#include <vector>
#include "XMLConfDocument.h"

#ifndef XMLCONFWRITER_H_
#define XMLCONFWRITER_H_

/**
 * Class for creating XML files
 */
class XMLConfWriter : public XMLConfDocument{
public:
	//! Constructor
	XMLConfWriter() {};
	//! Destructor
	virtual ~XMLConfWriter() {};

	void createDocument(std::string structName); //!< Create a new XML document with the specified root node

	void printDocument(); //!< Print the XML document in the output
	bool writeDocument(std::string fileName); //!< Write the document at the specified path

	bool addPath(std::string path, unsigned int ref); //!< Add a new path to the document
	bool addPath(std::string path, int ref); //!< Add a new path to the document
	bool addPath(std::string path, double ref); //!< Add a new path to the document
	bool addPath(std::string path, char* ref); //!< Add a new path to the document
	bool addPath(std::string path, std::string ref); //!< Add a new path to the document
	bool addPathAsHex(std::string path, int ref); //!< Add a new path to the document as hexadecimal integer

private:
	xmlNodePtr addNode(std::string nodeName, xmlNodePtr node); //!< Add a new child node
	xmlNodePtr addNodeArray(std::string nodeName, int index, xmlNodePtr node); //!< Add a new child node array
	void addNodeValue(std::string value, xmlNodePtr node); //!< Add a value to the node

	xmlNodePtr addPathNode(std::string path); //!< Add a path (string version)
	xmlNodePtr addPathNode(std::vector<std::string> path, xmlNodePtr cur);  //!< Add a path (vector version)
};

#endif /* XMLCONFWRITER_H_ */
