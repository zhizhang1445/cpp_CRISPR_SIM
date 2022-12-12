#include <fstream>
#include <experimental/filesystem>
#include <gsl/gsl_math.h>
#include <omp.h>
#include <string>
#include "rarray"
#include "parameters.h"
#include "io.h"
#include "init.h"
#include "omp_update_rules.h"
#include "npy.hpp"
using namespace std;


int main(int argc, char **argv) {
	// int rank, size;
	// MPI_Init(&argc, &argv);
	// MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	// MPI_Comm_size(MPI_COMM_WORLD, &size);

	// int Nlocal = (npoints-1) / size;         // this may round down!
	// int ibegin = rank * Nlocal;    // first index for this rank
	// int iend = (rank+1) * Nlocal;  // end index for this rank, i.e., last index + 1
	// 	// except if N is not divisable by size, adjust size of the last rank
	// if (rank == size-1) {
	// 	iend = npoints-1;
	// }


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

	rmatrix<double> f(simparams.xdomain, simparams.xdomain);
	init_f(f, params, simparams);//Zero initilization cuz it's fitness

	rmatrix<int> n(simparams.xdomain, simparams.xdomain);
	init_n(n, params, simparams); // initialized by gaussian modulated to int

	rmatrix<int> nh(simparams.xdomain, simparams.xdomain);
	init_nh(nh, params, simparams); // initialized by uniform distribution

	// if (rank == 0){
		vector<unsigned long> shape{simparams.xdomain, simparams.xdomain};
		vector<double> float_buff(f.data(), f.data()+f.size());
		vector<int> int_buff(f.data(), f.data()+f.size()); // buffer
		vector<int> int_buff2(f.data(), f.data()+f.size()); // buffer
		string folder = simparams.foldername + "/";
	// }
   
	#pragma omp parallel default(none) shared(f, nh, n, params, simparams)
	// #pragma omp parallel default(none) shared(f, nh, n, params, simparams) reduction(+:n sum)

	for(int t = 0; t<simparams.trange; t++){
		string time = to_string(t); // fucking cpp


		update_f_omp(f, nh, n, params, simparams);
		update_n_omp(n, f, params, simparams);
		update_nh_omp(nh, n, params, simparams);

		// MPI_Gather(&local_f[0], &global_f[0], f.size(), MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
		// MPI_Gather(&local_n[0], &global_n[0], n.size(), MPI_INT, MPI_SUM, MPI_COMM_WORLD);
		// MPI_Gather(&local_nh[0], &global_nh[0], nh.size(), MPI_INT, MPI_SUM, MPI_COMM_WORLD);

		// if (rank == 0){
			float_buff.assign(f.data(), f.data()+f.size()); 
			npy::SaveArrayAsNumpy(folder + path_f + time + ext, fortran_ord, shape.size(), shape.data(), float_buff);
			int_buff.assign(n.data(), n.data()+n.size());
			npy::SaveArrayAsNumpy(folder + path_n + time + ext, fortran_ord, shape.size(), shape.data(), int_buff);
			int_buff2.assign(nh.data(), nh.data()+nh.size());
			npy::SaveArrayAsNumpy(folder + path_nh + time + ext, fortran_ord, shape.size(), shape.data(), int_buff2);
		// }

		if (stop(n, params, simparams) == 1){
			return 1;
		};
	}

}