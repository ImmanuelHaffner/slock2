#ifndef ARGPARSER_H
#define ARGPARSER_H


#include "logger.h"


#ifndef LOGFILE
#define LOGFILE "/var/log/slock.log"
#endif

extern Logger::LogLevel logLevel;
extern char const *logfile;
extern bool enableBell;

void parseArguments( char **argv );


#endif
