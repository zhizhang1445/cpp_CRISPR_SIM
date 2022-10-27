#include <experimental/filesystem>
#include <iostream>
#include <fstream>
#include "io.h"

int readandAssignParams(char* filename, Parameters& params){
    ifstream infile(filename);
    infile.exceptions(fstream::failbit|ifstream::badbit); 
    try {
        infile >> params.Nh;
        infile >> params.N0;
        infile >> params.R0;
        infile >> params.M;
        infile >> params.mu;
        infile >> params.gamma_shape;
        infile >> params.Np;
        infile >> params.dc;
        infile >> params.h;
        infile >> params.r;
        infile >> params.rho;
        infile.close();
        return 0;
    }
    catch (ifstream::failure& e) {
        std::cerr << "Error while reading file '" << filename << "'.\n";
        return 1;
    }
};

int readandAssignSimParams(char* filename, SimParameters& simparams){
    ifstream infile(filename);
    infile.exceptions(fstream::failbit|ifstream::badbit); 
    try {
        infile >> simparams.xdomain;
        infile >> simparams.trange;
        infile >> simparams.init_x;
        infile >> simparams.init_y;
        infile >> simparams.init_var;
        infile >> simparams.n_step_prior;
        infile >> simparams.foldername;
        infile.close();
 
        return 0;
    }
    catch (ifstream::failure& e) {
        std::cerr << "Error while reading file '" << filename << "'.\n";
        return 1;
    }
}; 