#include "parameters.h"
#include "init.h"
#include <random>
#include <cmath>
using namespace std; //I should really remove this but its kinda hard Forgive me


// increment function is very useful 
int increment_from_flat(rmatrix<int> n, int flat_index){
    int tt_size = n.size();
    // const int shape_ar[2] = {n.shape()};

    if (flat_index >= tt_size or flat_index<0){
        cerr << "flat conversion noped \n";
        return 1;
    }

    ++n.data()[flat_index]; // ++ before makes it assign new before returning it.
    return 0;
}

int decrement_from_flat(rmatrix<int> n, int flat_index){
    int tt_size = n.size();
    // const int shape_ar[2] = {n.shape()};
    if (n.data()[flat_index] <= 0){
        // cout << "check this" << '\n';
        return 2; //cannot remove zero
    }
    if (flat_index >= tt_size or flat_index < 0){
        cerr << "flat conversion noped \n";
        return 1;
    }

    --n.data()[flat_index]; //Similar for decrement, the decrement is done before. 
    return 0;
}

int increment_polar2cartesian(rmatrix<int> n, double angle, double radius){
    int center_x = n.shape()[0]/2;
    int center_y = n.shape()[1]/2;

    int x_ind = int(radius*cos(angle)+center_x);
    int y_ind = int(radius*sin(angle)+center_y);
    // cout << x_ind << "&" << y_ind << "\n";

    if (x_ind >= n.shape()[0] or x_ind < 0){ //Array out of bounds check
        return 1;
    }

    if (y_ind >= n.shape()[1] or y_ind < 0){
        return 1;
    }

    ++n[x_ind][y_ind];//Increment successful
    return 0;
};


int init_n(rmatrix<int> n, Parameters& params, SimParameters& simparams){
    n.fill(0);
    random_device r;
    default_random_engine generator(r()); // I have no idea wheter creating a random engine is right or not but I'm not passing it
    // cout << "center" << center << endl;
    normal_distribution<double> gaussian_dist(simparams.init_x, simparams.init_var); //magnitude
    uniform_real_distribution<double> uni_dist(0.0, PI); //angle

    for (int i = 0;  i < params.N0; i++){
        double radius = gaussian_dist(generator);
        double angle = uni_dist(generator);
        // cout << radius << "@" << angle << "\n";
        int err = increment_polar2cartesian(n, angle, radius);
        
        if (err == 1){
            --i; //retry if increment failed
        }
    }
};

int init_nh(rmatrix<int> nh, Parameters& params, SimParameters& simparams){
    nh.fill(0);
    random_device r;
    default_random_engine generatror(r());
    int flat_size = nh.size();
    // cout << "flat_size " << flat_size << "\n";
    uniform_int_distribution<int> distribution(0, flat_size-1);

    for (int i = 0; i < params.Nh*params.M; i++){
        int rand_index = distribution(generatror);
        // cout << rand_index << "\n";
        int errcode = increment_from_flat(nh, rand_index);
    }
    return 0;
};

int init_f(rmatrix<double> f, Parameters& params, SimParameters& simparams){
    f.fill(0); //No initialization, I should probably make those nonspecific
};

int stop(rmatrix<int> n, Parameters& params, SimParameters& simparams){ // Stop Conditions important because our host cannot die
    int tt_phage = accumulate(n.begin(), n.end(), 0);
    if (tt_phage > simparams.exp_ratio*params.Nh){
        cerr << "stopped because of population explosion" << endl;
        return 1;
    }
    else if (tt_phage <= 0){
        cerr << "stopped because of population death" << endl;
        return 1;
    }
    return 0;
};