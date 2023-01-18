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

def calculate_variance(single_frame, params, sim_params):
    x_range = sim_params["xdomain"] #Initialize the spaces
    dx = sim_params["dx"]

    x_linspace = np.arange(-x_range, x_range, dx)
    coordmap = np.meshgrid(x_linspace, x_linspace)
    radius = coordmap[0]**2 + coordmap[1]**2

    rqrd = 0
    for n, r2 in zip(np.ravel(single_frame), np.ravel(radius)):
        rqrd += n*r2
    
    return rqrd/np.sum(single_frame)

def get_Gaussian_Fit(frames_n, params, sim_params):
    frame_means = []
    frame_cov = []
    x_range = sim_params["xdomain"] #Initialize the spaces
    dx = sim_params["dx"]

    x_linspace = np.arange(-x_range, x_range, dx)
    coordmap = np.meshgrid(x_linspace, x_linspace)

    for single_frame in frames_n:
        point_data = []
        try:
            for nonzero_ind in np.argwhere(single_frame):
                n = single_frame[nonzero_ind[0], nonzero_ind[1]]
                x_ind = coordmap[1][nonzero_ind[0], nonzero_ind[1]]
                y_ind = coordmap[0][nonzero_ind[0], nonzero_ind[1]]
                for _ in range(n):
                    point_data.append([x_ind, y_ind])

            point_data = np.array(point_data).squeeze()

            gaussian_estimator = GaussianMixture(covariance_type="full", max_iter=20)
            gaussian_estimator.fit(point_data)

            covariances = gaussian_estimator.covariances_.squeeze()
            means = gaussian_estimator.means_.squeeze()

            frame_means.append(means)
            frame_cov.append(covariances)
            
        except ValueError:
            continue

    frame_means = np.array(frame_means).squeeze()
    return frame_means, frame_cov

def make_ellipse(means, covariances, color = "navy"):
    v, w = np.linalg.eigh(covariances)
    u = w[0] / np.linalg.norm(w[0])
    angle = np.arctan2(u[1], u[0])
    angle = 180 * angle / np.pi  # convert to degrees
    v = 2.0 * np.sqrt(2.0) * np.sqrt(v)
    ell = mpl.patches.Ellipse(
        means, v[0], v[1], 180 + angle, color=color
    )
    return ell

def createJsons(subfolders, frame_cut):
    for current_folder in subfolders:
        os.chdir(current_folder)
        results = {
            "avg_r2":                   0,
            "avg_f":                    0,
            "avg_grp_vel":              0,
            "diff_coeff":               0,
            "avg_tt_cov":               0,

            "var_r2":                   0,
            "var_f":                    0,
            "var_grp_vel":              0,
            "var_diff_coeff":           0,
            "var_tt_cov":               0,
        }
        with open('results.json', 'w') as fp:
            json.dump(results, fp)
    else:
        return 0

def analyzeDiffCeoff(subfolders, frame_cut):
    for current_folder in subfolders:
# current_folder = subfolders[0]
        print(current_folder)
        os.chdir(current_folder)
        params, sim_params, frames_f, frames_n, frames_nh = load_stuff(frame_cut)

        frames_nvar = []
        for frame in frames_n:
            frames_nvar.append(calculate_variance(frame, params, sim_params))

        frames_nvar = np.array(frames_nvar).squeeze()
        avg_var = np.mean(frames_nvar)
        var_var = np.var(frames_nvar)
        err_var = var_var

        plt.figure()
        plt.title("$<r^2(t)>$ of N")
        plt.ylabel("dist")
        plt.xlabel("frame number")
        plt.plot(frames_nvar)

        try:
            x = np.arange(0,len(frames_nvar),1)
            fit = np.polyfit(x, frames_nvar, 1)
            plt.plot(x, np.polyval(fit, x), color="r")
            var_diff = np.var(np.polyval(fit, x)-frames_nvar)
            err_diff = var_diff
        except TypeError:
            fit = [np.NaN, np.NaN]
            err_diff = np.NaN
            pass
        plt.savefig("variance_plot.png")
        plt.close()

        results = json.load(open("results.json"))
        results["avg_r2"] = avg_var
        results["var_r2"] = err_var
        results["diff_coeff"] = fit[0]/2
        results["var_diff_coeff"] = err_diff/2
        with open('results.json', 'w') as fp:
            json.dump(results, fp)
    else:
        return 0

def analyzeFitDist(subfolders, frame_cut):
    for current_folder in subfolders:
    # current_folder = subfolders[3]
        print(current_folder)
        os.chdir(current_folder)
        params, sim_params, frames_f, frames_n, frames_nh = load_stuff(frame_cut)

        try:
            frame = ma.array(frames_f[0], mask = (frames_n[0] == 0)).compressed()
            height, bins = np.histogram(frame, 25, range = (-1.1, 1.1))
            cummulative = np.zeros(height.shape)

            frame_avg = []
            frame_var = []
            
            for single_f, single_n in zip(frames_f, frames_n):
                ma_f = (single_n == 0)

                if np.sum(ma_f) == 0:
                    print("population Death")
                    break

                frame = ma.array(single_f, mask = ma_f).compressed()
                frame_avg.append(np.mean(frame))
                frame_var.append(np.var(frame))
                height, _ = np.histogram(frame, 25, range = (-1.1, 1.1))
                cummulative += height

            norm_height = cummulative/np.sum(cummulative)

            plt.figure()
            plt.bar(bins[:-1], norm_height, width = 0.05)
            plt.title("Fitness Distribution")
            plt.savefig("fitness_plot.png")
            plt.close()

            results = json.load(open("results.json"))
            results["avg_f"] = np.mean(frame_avg)
            results["var_f"] = np.mean(frame_var)
            with open('results.json', 'w') as fp:
                json.dump(results, fp)

        except IndexError:
            pass
    else:
        return 0

def analyzePopulation(subfolders, frame_cut):
    for current_folder in subfolders:
        print(current_folder)
        os.chdir(current_folder)
        params, sim_params, frames_f, frames_n, frames_nh = load_stuff(frame_cut)

        N = []
        for single_n in frames_n:
            n = np.sum(single_n)
            N.append(n)

        plt.figure()
        plt.plot(N)
        plt.title("Number of active phage over time")
        plt.ylabel("Population")
        plt.xlabel("Frame number")
        plt.savefig("Inflected_Population_plot.png")
        plt.close()
    else:
        return 0

def analyzeTrajectory(subfolders, frame_cut):
    for current_folder in subfolders:
        print(current_folder)
        os.chdir(current_folder)
        params, sim_params, frames_f, frames_n, frames_nh = load_stuff(frame_cut)
        try:
            frames_means, frames_cov = get_Gaussian_Fit(frames_n, params, sim_params)

            fig, ax = plt.subplots()
            ax.plot(frames_means[:,0], frames_means[:,1])
            plt.title("Trajectory of Gaussian Fit")

            ax.scatter(frames_means[0, 0], frames_means[0, 1], color = 'red')
            ell1 = make_ellipse(frames_means[0], frames_cov[0], 'red')
            ell1.set_clip_box(ax.bbox)
            ell1.set_alpha(0.15)
            ax.add_patch(ell1)
            ax.set_aspect("equal", "datalim")

            ax.scatter(frames_means[-1, 0], frames_means[-1, 1], color = 'black')
            ell2 = make_ellipse(frames_means[-1], frames_cov[-1], 'black')
            ell2.set_clip_box(ax.bbox)
            ell2.set_alpha(0.15)
            ax.add_patch(ell2)
            ax.set_aspect("equal", "datalim")

            plt_bd = sim_params["xdomain"]
            plt.xlim(-plt_bd, plt_bd)
            plt.ylim(-plt_bd, plt_bd)

            plt.savefig("Gaussian_Fit_Plot.png")
            plt.close()

            frame_det_cov = []
            for cov in frames_cov:
                frame_det_cov.append(np.linalg.det(cov))
            frame_det_cov = np.array(frame_det_cov).squeeze()

            plt.figure()
            plt.title("Total Variance over Frames")
            plt.xlabel("Frame Number")
            plt.plot(frame_det_cov)
            plt.savefig("Total_Variance_Plot.png")
            plt.close()

            results = json.load(open("results.json"))
            results["avg_tt_cov"] = np.mean(frame_det_cov)
            results["var_tt_cov"] = np.var(frame_det_cov)
            with open('results.json', 'w') as fp:
                json.dump(results, fp)

        except IndexError:
            pass

    else:
        return 0

def main():
    if len(sys.argv) > 1 :
        foldername = sys.argv[1]
    if(not os.path.isdir(foldername)):{
        os.mkdir(foldername)
    }
    os.chdir(sys.argv[1])

    subfolders = [ f.path for f in os.scandir(foldername) if f.is_dir() ]
    frame_cut = 0

    createJsons(subfolders, frame_cut)
    analyzeDiffCeoff(subfolders, frame_cut)
    analyzeFitDist(subfolders, frame_cut)
    analyzePopulation(subfolders, frame_cut)
    analyzeTrajectory(subfolders, frame_cut)

    return 0

if __name__ == "__main__":
    main()