#ifndef R_ARG_HANDLER_CPP
#define R_ARG_HANDLER_CPP

#include "rArgHandler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>	// For stdcmp

rArgHandler::rArgHandler(int _argc, char** _argv) : __requiredArgc(0){
	__argc = _argc;
	__argv = _argv;
}

rArgHandler::~rArgHandler(){

}

void rArgHandler::addArgument(std::string _command, std::string _description, int _nParameter, bool _optional){
	rArgument _argBuf;
	// Store all argument information
	_argBuf.command = _command;
	_argBuf.description = _description;
	_argBuf.nParameter = _nParameter;
	_argBuf.isSet = false;
	_argBuf.optional = _optional;

	if ( !(_optional) ) __requiredArgc += 1 + _nParameter;

	__argumentVector.push_back(_argBuf);	
	return;
}

void rArgHandler::checkArguments() throw(rArg_exception){
	// Firstly check for required argument count	
	if (__argc <= __requiredArgc){ 
		printf("Too few arguments.\n\n");
		printHelp();
	}

	// Loop over argc
	int _index = 0;
	std::vector<rArgument>::iterator _it; 
	for (int _iArgc = 1; _iArgc < __argc; _iArgc++){
		// Compare entries in argv to arguments
		for (_it = __argumentVector.begin(); _it != __argumentVector.end(); _it++){
			// Find match
			if ( strcmp(_it->command.c_str(), __argv[_iArgc]) == 0 ){
				_it->isSet = true;
				for (int _iPar = 0; _iPar < _it->nParameter; _iPar++ ){
					_index = _iArgc + _iPar + 1;
					// Check if one the parameters is an argument
					if (__argv[_index][0] == '-'){
						printHelp();
					} else {
						_it->parameters.push_back(__argv[_index]);
					}
				}
				// Match has been found, coninue with next argv value 
				// The parameters can be skipped for argument search 
				_iArgc += _it->nParameter;
				break;
			}

		}
		// If the end of the command list is reached and the argument has not been set
		// the argument has not been found
		if ( ( _it == __argumentVector.end() ) && !(_it->isSet) ){
			printf("Unknown argument '%s'.\n\n", __argv[_iArgc]);
			printHelp();
		}
	}

	return;
}

bool rArgHandler::argIsSet(int _iArg){
	return __argumentVector.at(_iArg).isSet;
}

char* rArgHandler::getParameter(int _iArg, int _iPar){
	return __argumentVector.at(_iArg).parameters.at(_iPar);
}

void rArgHandler::printHelp(){
	// Print usage including all arguments
	printf("Usage:	%s ", __argv[0]);		
	// Loop over all arguments
	for (std::vector<rArgument>::iterator _it = __argumentVector.begin(); _it != __argumentVector.end(); _it++){
		// Put optional parameters in brackets 
		if (_it->optional) printf("[ ");

		printf("%s ", _it->command.c_str() );
		for (int i = 0; i < _it->nParameter; i++)
			printf("<par%i> ", i);
		
		if (_it->optional) printf("] ");
	}
	printf("\n\n");

	// Print description list
	printf("The following arguments are accepted by '%s':\n", __argv[0]);
	for (std::vector<rArgument>::iterator _it = __argumentVector.begin(); _it != __argumentVector.end(); _it++){
		printf("%s\t%s (%s, requires %i parameters)\n", _it->command.c_str(), _it->description.c_str(), (_it->optional) ? "optional" : "mandatory", _it->nParameter);
	}
	printf("\n");

	exit(1);
	return;
}


void rArgHandler::printParameters(){
	int _nPar;
	for (std::vector<rArgument>::iterator _it = __argumentVector.begin(); _it != __argumentVector.end(); _it++){
		printf("Argument '%s':\n", _it->command.c_str());
		_nPar = _it->nParameter;
		if ( !(_it->isSet) ){
			printf("\t< not set >\n\n");
			continue;
		}
		for (int i = 0; i < _nPar; i++){
			printf("\tpar%i = %s\n", i, _it->parameters.at(i));
		}
		printf("\n");
	}

	return;
}

#endif
