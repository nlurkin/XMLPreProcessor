/*
 * XMLConfParser.h
 *
 *  Created on: Apr 10, 2015
 *      Author: ncl
 */

#include <exception>
#include <stdexcept>
#include <sstream>
#include <unordered_set>
#include "XMLConfDocument.h"

#ifndef XMLCONFPARSER_H_
#define XMLCONFPARSER_H_

class XMLConfParserException: public std::runtime_error
{
public:
	XMLConfParserException(std::string message) : std::runtime_error(message){};
	XMLConfParserException(const std::stringstream &message) : std::runtime_error(message.str()){};
};
class XMLConfParserFatalException: public std::runtime_error
{
public:
	XMLConfParserFatalException(std::string message) : std::runtime_error(message){};
	XMLConfParserFatalException(const std::stringstream &message) : std::runtime_error(message.str()){};
};

class XMLConfParser : public XMLConfDocument{
public:
	XMLConfParser() : fReadSuccess(0) {};
	~XMLConfParser() {};

	bool readFile(std::string fileName);
	int  getReadSuccess() { return fReadSuccess; };
	void resetReadSuccess() { fReadSuccess=0; };

	bool getValue(std::string path, int &ref);
	bool getValue(std::string path, unsigned int &ref);
	bool getValue(std::string path, double &ref);
	bool getValue(std::string path, char *ref);
	bool getValue(std::string path, std::string &ref);

	bool pathExists(std::string path);

	void startCheckAdditional();
	void addCheckElement(std::string path);
	void addListDiffElement(std::string path);
	std::string getFirstDiff();
	std::string getNextDiff();
	void printAdditional();
private:
	void walkTreeCompare(std::string prefix, xmlNodePtr node);

	int fReadSuccess;
	std::unordered_set<std::string> fListAdditional;
	std::vector<std::string> fListDiff;
	std::vector<std::string>::iterator fListDiffIterator;
};
#endif /* XMLCONFPARSER_H_ */
