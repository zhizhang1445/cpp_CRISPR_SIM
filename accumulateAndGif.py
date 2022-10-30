import numpy as np
import numpy.ma as ma
import pandas as pd
import os
import sys
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import json 

def makeGif(frame_stack, name):
    fig = plt.figure()

    animation_frames = []
    for frame in frame_stack:
        frame = np.squeeze(frame)
        animation_frames.append([plt.imshow(frame, animated=True)])

    ani = animation.ArtistAnimation(
        fig, animation_frames, interval=50, blit=False, repeat_delay=1000)

    # FFwriter = animation.PillowWriter(fps=60)

    ani.save(name + ".gif")
    plt.close()
    return 0

def write2json(name, params, sim_params):
    with open(name + '_params.json', 'w') as fp:
        json.dump(params, fp)

    with open(name + '_sim_params.json', 'w') as fp:
        json.dump(sim_params, fp)

def main(start_time=0, end_time=1000000):
    if len(sys.argv) > 1 :
        os.chdir(sys.argv[1])

    stack_f = []
    stack_n = []
    stack_nh = []

    getinput = lambda name, index : "single_" + name + "_" + str(index) + ".npy"
    getoutput = lambda name : "all_" + name + ".npy"

    start_time=0
    end_time=10000

    for i in range(start_time, end_time):

        if not (os.path.isfile(getinput("f", i))):
            break

        stack_f.append(np.load(getinput("f", i)))
        stack_n.append(np.load(getinput("n", i)))
        stack_nh.append(np.load(getinput("nh", i)))

        os.remove(getinput("f", i))
        os.remove(getinput("n", i))
        os.remove(getinput("nh", i))

    stack_f = np.squeeze(np.array(stack_f))
    stack_n = np.squeeze(np.array(stack_n))
    stack_nh = np.squeeze(np.array(stack_nh))
    
    np.save("all_f.npy", stack_f)
    np.save("all_n.npy", stack_n)
    np.save("all_nh.npy", stack_nh)

    makeGif(stack_f, "all_f")
    makeGif(stack_n, "all_n")
    makeGif(stack_nh, "all_nh")

    params = {}
    sim_params = { #parameters relevant for the simulation (including Inital Valuess)
        "dx":                         1,
        "t0":                         0, 
        "dt":                         1,
    }

    file = open("params.txt", "r")
    params["Nh"] = int(file.readline().strip())
    params["N0"] = int(file.readline().strip())
    params["R0"] = int(file.readline().strip())
    params["M"] = int(file.readline().strip())
    params["mu"] = float(file.readline().strip())
    params["gamma_shape"] = float(file.readline().strip())
    params["Np"] = int(file.readline().strip())
    params["dc"] = int(file.readline().strip())
    params["h"] = int(file.readline().strip())
    params["r"] = float(file.readline().strip())
    params["rho"] = float(file.readline().strip())

    
    
    file = open("simparams.txt", "r")
    sim_params["xdomain"] = int(file.readline().strip())
    sim_params["tf"] = int(file.readline().strip())
    loc_x = float(file.readline().strip())
    loc_y = float(file.readline().strip())
    sim_params["initial_mean"] = [loc_x, loc_y]
    sim_params["inital_var"] = float(file.readline().strip())
    sim_params["n_step_prior"] = int(file.readline().strip())

    write2json("", params, sim_params)
    return 0

if __name__ == "__main__":
    main()

