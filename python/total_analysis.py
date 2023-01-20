import numpy as np
import numpy.ma as ma
import pandas as pd
import matplotlib as mpl
import matplotlib.pyplot as plt
from scipy.ndimage import convolve
import json
import os
import sys
from sklearn.mixture import GaussianMixture

def load_stuff(frame_cut):
    params = json.load(open("_params.json"))
    sim_params = json.load(open("_sim_params.json"))
    try:
        frames_f = np.load("frames_f.npy")[frame_cut:]
        frames_n = np.load("frames_n.npy")[frame_cut:]
        frames_nh = np.load("frames_nh.npy")[frame_cut:]

    except FileNotFoundError:
        frames_f = np.load("all_f.npy")[frame_cut:]
        frames_n = np.load("all_n.npy")[frame_cut:]
        frames_nh = np.load("all_nh.npy")[frame_cut:]

    except IndexError:
        frames_f = np.load("frames_f.npy")
        frames_n = np.load("frames_n.npy")
        frames_nh = np.load("frames_nh.npy")
    return params, sim_params, frames_f, frames_n, frames_nh

def total_analysis_fitness(foldername, frame_cut):
    return 0

def total_analysis_trajectory(foldername, frame_cut):
    
    return 0

def main():
    if len(sys.argv) > 1 :
        foldername = sys.argv[1]
    if(not os.path.isdir(foldername)):{
        os.mkdir(foldername)
    }
    os.chdir(sys.argv[1])

    subfolders = [ f.path for f in os.scandir(foldername) if f.is_dir() ]

    return 0

if __name__ == "__main__":
    main()