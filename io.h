#ifndef IO_H
#define IO_H 
#include <experimental/filesystem>
#include <iostream>
#include <fstream>
#include "parameters.h"
using namespace std;

int readandAssignParams(char* filename, Parameters& params);
int readandAssignSimParams(char* filename, SimParameters& simparams);

#endif