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

class XMLConfWriter : XMLConfDocument{
public:
	XMLConfWriter();
	virtual ~XMLConfWriter();

	void createDocument(std::string structName);

	xmlNodePtr addNode(std::string nodeName, xmlNodePtr node);
	xmlNodePtr addNodeArray(std::string nodeName, int index, xmlNodePtr node);
	void addNodeValue(std::string value, xmlNodePtr node);

	xmlNodePtr addPathNode(std::string path);
	xmlNodePtr addPathNode(std::vector<std::string> path, xmlNodePtr cur);

	void printDocument();
	bool writeDocument(std::string fileName);

	bool addPath(std::string path, unsigned int ref);
	bool addPath(std::string path, int ref);
	bool addPath(std::string path, double ref);
	bool addPath(std::string path, char* ref);
	bool addPath(std::string path, std::string ref);

	bool addPathAsHex(std::string path, int ref);
};

#endif /* XMLCONFWRITER_H_ */
