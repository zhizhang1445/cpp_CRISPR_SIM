#ifndef PARAMETERS_H
#define PARAMETERS_H
#include "rarray"
#include <string>
using namespace std;

class Parameters{
    public:
        long int             Nh;
        long int             N0;
        double               R0;
        int                   M;
        double               mu; 
        double      gamma_shape;         
        int                  Np;
        int                  dc;
        int                   h;
        double                r;
        double              rho;
};

class SimParameters{
    public:
        unsigned long int        xdomain;
        long int                  trange;
        double                    init_x;
        double                    init_y;
        double                  init_var;          
        long int            n_step_prior;
        int                    exp_ratio;
        bool                      norm_f;
        string                foldername;
};

#endif