/*
 * XMLConfParser.h
 *
 *  Created on: Apr 10, 2015
 *      Author: ncl
 */

#include <exception>
#include <stdexcept>
#include <sstream>
#include "XMLConfDocument.h"

#ifndef XMLCONFPARSER_H_
#define XMLCONFPARSER_H_

class XMLConfParserException: public std::runtime_error
{
public:
	XMLConfParserException(std::string message) : runtime_error(message){};
	XMLConfParserException(const std::stringstream &message) : runtime_error(message.str()){};
};
class XMLConfParserFatalException: public std::runtime_error
{
public:
	XMLConfParserFatalException(std::string message) : runtime_error(message){};
	XMLConfParserFatalException(const std::stringstream &message) : runtime_error(message.str()){};
};

class XMLConfParser : public XMLConfDocument{
public:
	XMLConfParser() : fReadSuccess(0) {};
	~XMLConfParser() {};

	bool readFile(std::string fileName);
	int  getReadSuccess() { return fReadSuccess; };

	bool getValue(std::string path, int &ref);
	bool getValue(std::string path, unsigned int &ref);
	bool getValue(std::string path, double &ref);
	bool getValue(std::string path, char *ref);
	bool getValue(std::string path, std::string &ref);

private:
	int fReadSuccess;
};
#endif /* XMLCONFPARSER_H_ */
