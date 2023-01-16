#include <fstream>
#include <experimental/filesystem>
#include <gsl/gsl_math.h>
#include "rarray"
#include "parameters.h"
#include "io.h"
#include "init.h"
#include "update_rules.h"
#include "npy.hpp"
using namespace std;

int main(int argc, char* argv[]){ // executable <params.txt> <simparams.txt>
   Parameters params;
   SimParameters simparams;
   const bool fortran_ord{false};
   const string path_f{"single_f_"};
   const string path_nh{"single_nh_"};
   const string path_n{"single_n_"};
   const string ext{".npy"};

   if (argc!=3){
    cerr << "Err wtf are you making me do? check inputs number \n";
    return 1;
   }

   if (not (experimental::filesystem::exists(argv[1]) and experimental::filesystem::exists(argv[2]))){
    cerr << "dafuq you using for parameters? \n";
    return 2;
   }

   int errcode = readandAssignParams(argv[1], params);
   int errcode2 = readandAssignSimParams(argv[2], simparams);
   if (errcode == 1 or errcode2 == 1){
      cerr << "lmao last check failed \n";
      return errcode;
   }

   // Code below is to remind you of what norm does
   // if (simparams.norm_f){
	// 	cout << "normalizing f happens: norm_f = 1" << "\n";
	// } else {
	// 	cout << "normalizing f does not happen: norm_f = 0" << "\n";
	// }

   // rarray is preffered for me but they are ROW ordered so the indice is [row, column]
   rmatrix<int> n(simparams.xdomain, simparams.xdomain);
   init_n(n, params, simparams); // initialized by gaussian modulated to int
   // cout << n << "\n";

   rmatrix<int> nh(simparams.xdomain, simparams.xdomain);
   init_nh(nh, params, simparams); // initialized by uniform distribution
   // cout << nh << "\n";

   rmatrix<double> f(simparams.xdomain, simparams.xdomain);
   init_f(f, params, simparams);//Zero initilization cuz it's fitness
   // cout << f << "\n";

   // SaveArrayAsNumpy needs vectors so I'm making one to it.
   vector<unsigned long> shape{simparams.xdomain, simparams.xdomain};
   vector<double> f_buff(f.size());
   vector<int> n_buff(n.size()); 
   vector<int> nh_buff(nh.size()); 
   string folder = simparams.foldername + "/";
   

   for(int t = 0; t<simparams.trange; t++){

      string time = to_string(t); // fucking cpp saving them in numpy is faster
      npy::SaveArrayAsNumpy(folder + path_f + time + ext, fortran_ord, shape.size(), shape.data(), f_buff);
      npy::SaveArrayAsNumpy(folder + path_n + time + ext, fortran_ord, shape.size(), shape.data(), n_buff);
      npy::SaveArrayAsNumpy(folder + path_nh + time + ext, fortran_ord, shape.size(), shape.data(), nh_buff);

      update_f(f, nh, n, params, simparams);
      f_buff.assign(f.data(), f.data()+f.size());

      update_n(n, f, params, simparams);
      n_buff.assign(n.data(), n.data()+n.size());

      update_nh(nh, n, params, simparams);
      nh_buff.assign(nh.data(), nh.data()+nh.size());

      // So you can easily see when does the time stops.
      if (stop(n, params, simparams) == 1){
         cerr << "Stopped at t=" << time << endl;
         return 1;
      }
   }
}

