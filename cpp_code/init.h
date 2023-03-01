#ifndef INIT_H
#define INIT_H
#include <cmath>
#include "rarray"
#include <armadillo>
#define PI 3.14159265
// #include <gsl>
#include "parameters.h"

int increment_from_flat(rmatrix<int> n, int flat_index);
int decrement_from_flat(rmatrix<int> n, int flat_index);
int increment_polar2cartesian(rmatrix<int> n, double angle, double radius); //angle in radians
int init_n(rmatrix<int> n, Parameters& params, SimParameters& simparams);
int init_nh(rmatrix<int> nh, Parameters& params, SimParameters& simparams);
int init_f(rmatrix<double> f, Parameters& params, SimParameters& simparams);
int stop(rmatrix<int> n, Parameters& params, SimParameters& simparams);

#endif