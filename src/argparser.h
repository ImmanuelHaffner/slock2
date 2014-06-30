#ifndef ARGPARSER_H
#define ARGPARSER_H


#include "logger.h"


extern Logger::LogLevel logLevel;
extern char const *logfile;
extern bool enableBell;

void parseArguments( char **argv );


#endif
