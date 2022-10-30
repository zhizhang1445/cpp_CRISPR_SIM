#include "update_rules.h"

int update_f(rmatrix<double> f, rmatrix<int> nh, rmatrix<int> n, Parameters& params, SimParameters& simparams){
    double R0 = params.R0;
    int Nh = params.Nh;

    rmatrix<double> eff_p(nh.shape());
    coverage(eff_p, nh, params, simparams);

    for( int i = 0; i < f.size(); i++){
        if (n.data()[i] == 0){
            f.data()[i] = 0;
        }
        else {
            f.data()[i] = log(R0) + log_prob_inf(i, eff_p, params, simparams);
        }
    }

    return 0;
};

double log_prob_inf(int flat_index, rmatrix<double> eff_p, Parameters& params, SimParameters& simparams){
    int Nh = params.Nh;

    if (eff_p.data()[flat_index] == 0) {
        return log(1.0);
    }

    else {
        double p = p_0_spacer(flat_index, eff_p, params, simparams);
        p += p_1_spacer(flat_index, eff_p, params, simparams); 
        return log(p); //kekw
    }
};

int update_n(rmatrix<int> n, rmatrix<double> f, Parameters& params, SimParameters& simparams){
    int tt_length = n.size();
    
    for (int i = 0; i < tt_length; i++){
        virus_growth(i, n, f, params, simparams);
    }

    for (int i = 0; i < tt_length; i++){
        mutation_jump(i, n, params, simparams);
    }
    // cerr << "update_n error" << "\n";
    return 0;
};


int update_nh(rmatrix<int> nh, rmatrix<int> n, Parameters& params, SimParameters& simparams){
    nh = n.copy();
    default_random_engine generator;

    int flat_size = nh.size();
    uniform_int_distribution<int> uni_dist(0, flat_size-1);

    int num_to_remove = int(accumulate(nh.begin(), nh.end(), 0) - params.M*params.Nh);
    // int num_to_remove = int(params.M*params.Nh);
    // int num_to_remove = int(accumulate(nh.begin(), nh.end(), 0));
    for (int i = 0; i < num_to_remove; i++){
        int ind_to_rmv = uni_dist(generator);
        int err = decrement_from_flat(nh, ind_to_rmv);

        if(err == 2){
            --i; //Try again if nothing to remove lmao;
        }
    }

    if (num_to_remove < 0){ // More spacers than virus
        for (int i = 0; i > num_to_remove; i--){
            int ind_to_add = uni_dist(generator);
            int err_code = increment_from_flat(nh, ind_to_add);

            if (err_code == 2){
                i++;
            }
        }
    }
    num_to_remove = int(params.M*params.Nh - accumulate(nh.begin(), nh.end(), 0));

    if (num_to_remove == 0){
        return 0;
    } else {
        cerr << "check this: Nh didn't stay constant: " << num_to_remove << "\n";
        return 1;
    }
};
