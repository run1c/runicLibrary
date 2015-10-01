#ifndef R_EXCEPTION_H
#define R_EXCEPTION_H

/*
 *	Basic exception class
 *	All class specific exception should be derived from this one
 */

#include <exception>
#include <string>

class rException : public std::exception{

public:
	// Constructor with message and error number
	rException(std::string _newMessage,int _errNumber=0);

	// Default desctructor
	virtual ~rException() throw() {};

	// String that is thrown
	const char* what() const throw();

	// Returns error number
	int getErrorNumber() throw();

protected:
	std::string __message;
	int __errNo;

};

#endif
