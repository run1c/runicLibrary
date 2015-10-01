#ifndef R_ARG_HANDLER_H
#define R_ARG_HANDLER_H

/*
 *	Class that handles input needed from int main(int argc, char** argv) with following format:
 *		./<exectuable> ... (at position _index) <_cmd> <arg0> <arg1> ... (_nParms arguments)	
 */

#include <string>
#include <vector>

#include <rException.h>

/*
 *	Exception class for error handling
 */

class rArg_exception : public rException{
	rArg_exception(std::string _message, int _errNumber=0) : rException("[rArg_exception] - " + _message, _errNumber) {};
	~rArg_exception() throw() {};
};

// Struct for a signle argument used by the rArgHandler
struct rArgument{
	std::string command;
	std::string description;
	int nParameter;	
	std::vector<char*> parameters;
	bool isSet;
	bool optional;
};

class rArgHandler{

public:
	rArgHandler(int _argc, char** _argv);
	virtual ~rArgHandler();

	// Add argument to handler
	void addArgument(std::string _command, std::string _description, int _nParameter, bool _optional = true);

	// Check and gather arguments from argc and argv
	void checkArguments() throw(rArg_exception);

	// Returns true if argument has been specified
	bool argIsSet(int _iArg);
	
	// Returns the requested parameter
	char* getParameter(int _iArg, int _iPar);

	// Print help and exit
	void printHelp();
	
	// Print all received parameters
	void printParameters();

private:
	int __argc, __requiredArgc;
	char** __argv;
	std::vector<rArgument> __argumentVector;

};

#endif
