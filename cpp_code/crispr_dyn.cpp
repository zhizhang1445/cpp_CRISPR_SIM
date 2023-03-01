#include "crispr_dyn.h"
#include "init.h"

double hill_function_alpha(int d, Parameters& params){ //Hill-like function to activate the immune response
    double dc = params.dc;
    double h = params.h;

    return pow(d, h)/(pow(d, h) + pow(dc, h));
};

int coverage(rmatrix<double> eff_p, rmatrix<int> nh, Parameters& params, SimParameters& simparams){
    int Nh = params.Nh;
    int M = params.M;

    if(params.r == 0){
        for(int i = 0; i < nh.size(); i++){
            if (nh.data()[i] <= 0){
                eff_p.data()[i] = 0;
            }
            else {
                eff_p.data()[i] = double(nh.data()[i])/double(Nh*M);
            };
        }
        return 0;
    }

    double r = params.r;
    int conv_size = int(2*r);
    int xdomain = simparams.xdomain;
    rmatrix<double> conv_kernel(conv_size, conv_size); // I did not write the convolution algorithm, just included it
    rmatrix<double> buff_nh(eff_p.shape());

    for (int i = 0; i < buff_nh.size(); i++){
        buff_nh.data()[i] = double(nh.data()[i]);
    }

    for (int i = 0; i < conv_size; i++){
        for (int j = 0; j < conv_size; j++){
            double center = double(conv_size)/2;
            // cout << center << endl;
            double radii = sqrt(double(pow(i-center, 2) + pow(j-center, 2)));
            conv_kernel[i][j] = exp(-1*radii/r)/double(Nh*M);
        }
    }
    // cout << buff_nh << "\n";
    // cout << conv_kernel << "\n";
    // convolve2D(buff_nh.data(), eff_p.data(), xdomain, xdomain, conv_kernel.data(), conv_size, conv_size);
    // cout << eff_p << "\n";
    return 0;
};

double p_0_spacer(int flat_index, rmatrix<double> eff_p, Parameters& params, SimParameters& simparams){ //p0 case would be equivalent to original paper
    double p = eff_p.data()[flat_index];
    int M = params.M;

    return gsl_ran_binomial_pdf(0, p, M);
};

double p_1_spacer(int flat_index, rmatrix<double> eff_p, Parameters& params, SimParameters& simparams){ //p1 case, a lot more complicated
    double p = eff_p.data()[flat_index];
    int M = params.M;
    int Np = params.Np;
    double res = 0.0;

    double p1 = gsl_ran_binomial_pdf(1, p, M);

    for(int d = 0; d<= Np; d++){
        double p_q = 1./(double(M));
        double q = gsl_ran_binomial_pdf(d, p_q, Np);
        res += p1*hill_function_alpha(d, params)*q;
    }

    return res;
};

int virus_growth(int flat_index, rmatrix<int> n, rmatrix<double> f, Parameters& params, SimParameters& simparams){ //Essentially Poisson Growth but sligtly more complex
    int n_at_loc = n.data()[flat_index];

    if (n_at_loc == 0){
        return 0;
    }

    else {
        double growth_f = f.data()[flat_index];
        int res;

        if (growth_f <= -1) { //This kill everything if the effective fitness is smaller than -1
            res = 0; 
            n.data()[flat_index] = res;
        }

        if (growth_f > -1){ //Poisson Growth
            double mean = (1 + growth_f)*n_at_loc;

            random_device r;
            default_random_engine generator(r());
            poisson_distribution<int> distribution(mean);
            res = distribution(generator);
            n.data()[flat_index] = res; // this is the actual result
        }
        return res; // oh I'm a FUCKING IDIOT the function return doesn't matter
    }
    return -1; //cannot be negative
};

int num_mutants(int n_at_loc, Parameters& params, SimParameters& simparams){
    double mu = params.mu;
    double p = 1 - exp(-1*mu); //Probability of having a single mutant

    default_random_engine generator;
    binomial_distribution<int> distribution(n_at_loc, p); //Binomial probabily for each at that location
    int num = distribution(generator);
};

int num_mutation(Parameters& params, SimParameters& simparams){
    double mu = params.mu;
    poisson_distribution<int> distribution(mu); //Number of mutation is a Poisson Process with an average rate mu
    int res;

    random_device r;
    default_random_engine generator(r());
    for(int i = 0; i < 100; i++){
        res = distribution(generator);
        if (res >= 1){
            return res;
        }       
    }
    return 0;
};

int mutation_jump(int flat_index, rmatrix<int> n, Parameters& params, SimParameters& simparams){
    int n_at_loc = n.data()[flat_index];
    double alpha = params.gamma_shape;
    double beta = params.gamma_shape/2;

    if (n_at_loc == 0){
        return 0;
    }
    random_device r;
    default_random_engine generator(r());
    gamma_distribution<double> distribution(alpha, beta);
    uniform_real_distribution<double> uni_dist(0.0, 2*PI);

    int num_muts = num_mutants(n_at_loc, params, simparams);
    
    for (int i = 0; i< num_muts; i++){
        int num_single_mutation = num_mutation(params, simparams);
        double x_jump = double(flat_index/simparams.xdomain);
        double y_jump = double(flat_index%simparams.xdomain);

        for (int j = 0; j < num_single_mutation; j++){
            double radius = distribution(generator);
            double angle = uni_dist(generator);

            x_jump += radius*cos(angle);
            y_jump += radius*sin(angle);
        }

        int int_x_jump = (int) round(x_jump);
        int int_y_jump = (int) round(y_jump);

        if (int_x_jump < simparams.xdomain && int_x_jump >= 0){
            if (int_y_jump < simparams.xdomain && int_y_jump >= 0){ //The and condition is actually not that efficient but WTV
                decrement_from_flat(n, flat_index); //this should probably be moved out of the if statement
            
                ++n[int_x_jump][int_y_jump];
            }
        }

    }
    return 0;
};
