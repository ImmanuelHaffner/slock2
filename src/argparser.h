#ifndef ARGPARSER_H
#define ARGPARSER_H


#include "logger.h"


extern Logger::LogLevel logLevel;
extern char const *logfile;
extern bool enableBell;
extern bool suspend;

void parseArguments( char **argv );


#endif
