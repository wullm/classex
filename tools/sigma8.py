import sys
import numpy as np
from tabulate import tabulate;

A_s = 2.215e-09;
n_s = 0.9619;
k_pivot = 0.05;
h = 0.67

if (len(sys.argv) < 3):
    print("Usage: sigma_R.py file R");
else:
    z = 0
    R = float(sys.argv[2]) / h

    from explore import power_at_redshift;

    print("Linear power spectra at z = ", 0, " smoothed on ", R, " Mpc scales");
    print("")
    ptarr, columns = power_at_redshift(z, A_s, n_s, k_pivot);

    #The vector of wavenumbers in 1/Mpc
    k_src = ptarr[:,0]

    #The number of finer wavenumbers and transfer functions
    nk = 100000
    nfunc = ptarr.shape[1] - 1 # one column is "k"

    #Interpolate the power spectra on a finer grid
    k = np.exp(np.linspace(np.log(k_src[0]), np.log(k_src[-1]), nk))
    pt = np.zeros((nk, nfunc))
    for i in range(nfunc):
        pt[:,i] = np.exp(np.interp(np.log(k), np.log(k_src), np.log(ptarr[:,1+i])))

    #Integrate k^2 * dk * Pk * W(kR)^2
    x = k * R
    W = 3.0 / (x*x*x) * (np.sin(x) - x * np.cos(x))
    integrand = k*k*pt.T / (2*np.pi**2) * W**2
    dk = k[1:] - k[:-1]
    int = 0.5 * (integrand[:,1:] + integrand[:,:-1])

    #Execute the integral
    sigmaR2 = (dk * int).sum(axis=1)
    sigmaR = np.sqrt(sigmaR2)

    #Return the square root
    for i in range(nfunc):
        print(columns[1+i], ":", sigmaR[i])
