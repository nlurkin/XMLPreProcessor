/*
 * XMLConfParser.h
 *
 *  Created on: Apr 10, 2015
 *      Author: ncl
 */

#include <exception>
#include <stdexcept>
#include "XMLConfDocument.h"

#ifndef XMLCONFPARSER_H_
#define XMLCONFPARSER_H_

class XMLConfParserException: public std::runtime_error
{
public:
	XMLConfParserException(std::string message) : runtime_error(message){};
};

class XMLConfParser : public XMLConfDocument{
public:
	XMLConfParser() {};
	~XMLConfParser() {};

	bool readFile(std::string fileName);

	bool getValue(std::string path, int &ref);
	bool getValue(std::string path, unsigned int &ref);
	bool getValue(std::string path, double &ref);
	bool getValue(std::string path, char *ref);
	bool getValue(std::string path, std::string &ref);

};
#endif /* XMLCONFPARSER_H_ */
