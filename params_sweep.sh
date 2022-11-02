#!/bin/bash
Params_to_sweep=r #Change this

Nh=10000
N0=5000 
R0=5      
M=10    
mu=1 
gamma_shape=20   
Np=10 
dc=3 
h=4
r=10
rho=5e-5
param_text="#The above parameters are interpreted in this specific order: # Nh # N0 # R0 # M # mu # gamma_shape # Np # dc # h # r # rho"

xdomain=10000
trange=1000
init_x=0
init_y=0
init_var=5
n_step_prior=1
simparam_text="#The above parameters are interpreted in this specific order: # xdomain # trange # init_x # init_y UNUSED # init_var # n_step_prior # outputfolder" 




if [ -d "${Params_to_sweep}_simulation/" ]
then rm -rf "${Params_to_sweep}_simulation/"
fi

Parent_dir="${Params_to_sweep}_simulation/"

if [ ! -d $Parent_dir ]
then mkdir $Parent_dir
fi


for r in {0..100..10} #And change this
do
    Child_dir="${Parent_dir}simulation${r}/" #And change this 
    mkdir $Child_dir
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
    echo $Child_dir >> "${Child_dir}simparams.txt"
    echo $simparam_text >> "${Child_dir}simparams.txt"

    ./CRISPR_wave_main ${Child_dir}params.txt ${Child_dir}simparams.txt
    python3 accumulateAndGif.py ${Child_dir}
done 