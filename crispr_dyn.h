#ifndef CRISPR_DYN_H
#define CRISPR_DYN_H
#include "parameters.h"
#include <cmath>
#include <gsl/gsl_randist.h>
#include <random>
using namespace std;

double hill_functio_alpha(int d, Parameters& params);
int coverage(rmatrix<double> eff_p, rmatrix<int> nh, Parameters& params, SimParameters& simparams);
double p_0_spacer(int flat_index, rmatrix<double> eff_p, Parameters& params, SimParameters& simparams);
double p_1_spacer(int flat_index, rmatrix<double> eff_p, Parameters& params, SimParameters& simparams);

int virus_growth(int flat_index, rmatrix<int> n, rmatrix<double> f, Parameters& params, SimParameters& simparams);
int num_mutants(int n_at_loc, Parameters& params, SimParameters& simparams);
int num_mutation(Parameters& params, SimParameters& simparams);
int mutation_jump(int flat_index, rmatrix<int> n, Parameters& params, SimParameters& simparams);

#endif