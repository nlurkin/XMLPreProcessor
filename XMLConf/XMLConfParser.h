/*
 * XMLConfParser.h
 *
 *  Created on: Apr 10, 2015
 *      Author: ncl
 */

#include <exception>
#include <stdexcept>
#include <sstream>
#include <set>
#include "XMLConfDocument.h"

#ifndef XMLCONFPARSER_H_
#define XMLCONFPARSER_H_

class XMLConfParserFatalException: public std::runtime_error
{
public:
	XMLConfParserFatalException(std::string message) : std::runtime_error(message){};
	XMLConfParserFatalException(const std::stringstream &message) : std::runtime_error(message.str()){};
};


/**
 * Class for reading and parsing XML files.
 */
class XMLConfParser : public XMLConfDocument{
public:
	//! Constructor
	XMLConfParser() : fReadSuccess(0) {};
	//! Destructor
	~XMLConfParser() {};

	bool readFile(std::string fileName); //!< Read an XML file

	/**
	 * ReadSuccess is a counter that is incremented for every successful
	 * getValue() or pathExists().\n
	 * A getValue is successful if the requested node exists and the contained
	 * value can be successfully read and transformed into the requested type.\n
	 * A pathExists is successful if the path is found in the XML.
	 * @return Number of ReadSuccess
	 */
	//! Get number of ReadSuccess
	int  getReadSuccess() { return fReadSuccess; };
	//! Reset number of ReadSuccess
	void resetReadSuccess() { fReadSuccess=0; };

	bool getValue(std::string path, int &ref); //!< Get a value as int
	bool getValue(std::string path, unsigned int &ref); //!< Get a value as unsigned int
	bool getValue(std::string path, float &ref); //!< Get a value as float
	bool getValue(std::string path, double &ref); //!< Get a value as double
	bool getValue(std::string path, char *ref); //!< Get a value as c-string (do not require conversion)
	bool getValue(std::string path, std::string &ref); //!< Get a value as std::string (do not require conversion)

	bool pathExists(std::string path); //!< Does the given path exists in the XML file

	void startCheckAdditional(); //!< Initialise the check of additional tags.
	void addCheckElement(std::string path); //!< Add an element to the list of provided elements for the check of additional tags
	void addListDiffElement(std::string path); //!< Add an element to the list of modified elements
	std::string getFirstDiff(); //!< Return the first element in the list of modified elements
	std::string getNextDiff(); //!< Return the next element in the list of modified elements
	void printAdditional(); //!< Print the additional tags
private:
	void walkTreeCompare(std::string prefix, xmlNodePtr node); //!< Recursively walk through the XML tree.
	void addToCache(std::string path); //!< Add path to cache (as non existing)
	bool checkCache(std::string path); //!< Return true if the path is marked non existent in the cache

	int fReadSuccess; //!< Counter for the number of successful reads
	std::set<std::string> fListAdditional; //!< List containing the additional tags
	std::vector<std::string> fListDiff; //!< List containing the modified elements
	std::vector<std::string>::iterator fListDiffIterator; //!< Iterator through the list containing the modified elements
	std::set<std::string> fTagsCache; //!< Cache for non existing tags
};
#endif /* XMLCONFPARSER_H_ */
