#pragma once


#include "shared.h"
#include "globals.h"

#define SOURCE "src"
#define BINARY "bin"

struct cmd
{
    // Commands
    char* make;
    char* test;
    char* prepare;
    char* install;
    char* special;
    char* download;
};
struct package
{
    // Basic infos
    char* name;
    char* type; // for the type at first i used an enum but im lazy and its stupid;
    char* version;
    char* license;
    char* url;

    char** dependencies;
    int dependenciesCount;

    char** makedependencies;
    int makedependenciesCount;

    char** optionaldependencies;
    int optionaldependenciesCount;

    char ** locations;
    int locationsCount;

    // cmds
    struct cmd info;

};



int readConfig(char* configFilePath);



