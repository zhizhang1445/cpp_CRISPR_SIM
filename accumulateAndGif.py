import numpy as np
import numpy.ma as ma
import pandas as pd
import os
import sys
import matplotlib.pyplot as plt
import matplotlib.animation as animation

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

    stack_f = np.squeeze(np.array(stack_f))
    stack_n = np.squeeze(np.array(stack_n))
    stack_nh = np.squeeze(np.array(stack_nh))
        
    makeGif(stack_f, "all_f")
    makeGif(stack_n, "all_n")
    makeGif(stack_nh, "all_nh")
    return 0

if __name__ == "__main__":
    main()

