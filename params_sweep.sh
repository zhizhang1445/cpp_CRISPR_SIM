#!/bin/bash
Params_to_sweep="M" #Change this

Nh=10000
N0=2000
R0=1.5

M=10
mu=0.001
gamma_shape=20
Np=10
dc=5
h=4
r=10
rho=5e-5
param_text="#The above parameters are interpreted in this specific order: # Nh # N0 # R0 # M # mu # gamma_shape # Np # dc # h # r # rho"

xdomain=1000
trange=100
init_x=0
init_y=0
init_var=5
n_step_prior=1
exp_ratio=10
norm_f=1 
# norm_f = 0 for no normalization 1 for normalization
simparam_text="#The above parameters are interpreted in this specific order: # xdomain # trange # init_x # init_y UNUSED # init_var # n_step_prior # outputfolder" 


Parent_dir="/media/homes/thisiszhi/data/${Params_to_sweep}Simulation/"
# Parent_dir="Test/"

if [ -d $Parent_dir ] 
then 
    rm -rf $Parent_dir
fi

mkdir -p $Parent_dir

sweep_index=0 #do not delete this

# for ((R0 = 0 ; R0 < 10 ; i++));#And change this
# for R0 in {0..10..0.5} #Change This {start..end..step}
for M in $(seq 0 5 100) #Change this (seq start step end)
do
    Child_dir="${Parent_dir}simulation${sweep_index}/" 
    # echo Child_dir
    mkdir -p $Child_dir
    echo $Nh >> "${Child_dir}params.txt"
    echo $N0 >> "${Child_dir}params.txt"
    echo $R0 >> "${Child_dir}params.txt"
    echo $M >> "${Child_dir}params.txt"
    echo $mu >> "${Child_dir}params.txt"
    echo $gamma_shape >> "${Child_dir}params.txt"
    echo $Np >> "${Child_dir}params.txt"
    echo $dc >> "${Child_dir}params.txt"
    echo $h >> "${Child_dir}params.txt"
    echo $r >> "${Child_dir}params.txt"
    echo $rho >> "${Child_dir}params.txt"
    echo $param_text >> "${Child_dir}params.txt"

    echo $xdomain >> "${Child_dir}simparams.txt"
    echo $trange >> "${Child_dir}simparams.txt"
    echo $init_x >> "${Child_dir}simparams.txt"
    echo $init_y >> "${Child_dir}simparams.txt"
    echo $init_var >> "${Child_dir}simparams.txt"
    echo $n_step_prior >> "${Child_dir}simparams.txt"
    echo $exp_ratio >> "${Child_dir}simparams.txt"
    echo $norm_f >> "${Child_dir}simparams.txt"
    echo $Child_dir >> "${Child_dir}simparams.txt"
    echo $simparam_text >> "${Child_dir}simparams.txt"

    ./CRISPR_wave_main_omp ${Child_dir}params.txt ${Child_dir}simparams.txt
    python3 -u python/accumulateAndGif.py ${Child_dir}
    let sweep_index++
done
