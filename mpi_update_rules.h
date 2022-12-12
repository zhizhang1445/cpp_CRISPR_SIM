#ifndef UPDATE_RULES_H
#define UPDATE_RULES_H
#include <cmath>
#include <random>
#include <numeric>
#include <mpi.h>
#include <gsl/gsl_randist.h>
#include "rarray"
#include "parameters.h"
#include "init.h"
#include "crispr_dyn.h"

using namespace std;

int update_f_mpi(int& rank, rmatrix<double> f, rmatrix<int> nh, rmatrix<int> n, Parameters& params, SimParameters& simparams);
double log_prob_inf(int flat_index, rmatrix<double> eff_p, Parameters& params, SimParameters& simparams);
int update_n_mpi(int& rank, rmatrix<int> n, rmatrix<double> f, Parameters& params, SimParameters& simparams);
int update_nh_mpi(int& rank, rmatrix<int> nh, rmatrix<int> n, Parameters& params, SimParameters& simparams);

#endif