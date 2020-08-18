import h5py;
import numpy as np;
from matplotlib import pyplot as plt;
from scipy.interpolate import interp1d;
from scipy.interpolate import interp2d;
import sys;

fname = sys.argv[1];
f = h5py.File(fname, "r");

#Unpack the wavenumbers and times
k = np.array(f["Perturb/Wavenumbers"]);
redshift = np.array(f["Perturb/Redshifts"]);
log_tau = np.array(f["Perturb/Log conformal times"]);
tau = np.exp(log_tau);

#Unpack the transfer functions (N_functions * N_k * N_tau)
delta = np.array(f["Perturb/Transfer functions"]);

#Unpack the background densities
Omegas = np.array(f["Perturb/Omegas"]);

#Decode the transfer function titles
titles = f["Header"].attrs["FunctionTitles"];
titlestrings = [];
for tbytes in titles:
    titlestrings.append(tbytes.decode("utf-8"));

#Sizes of the perturbation vector
k_size = len(k);
tau_size = len(tau);
nr_titles = len(titles);

def find_title(titlestr):
    if not titlestr in titlestrings:
        return(-1);
    else:
        return(titlestrings.index(titlestr));

#Interpolate redshift to log_tau
log_tau_func = interp1d(redshift, log_tau);

def perturb_at_redshift(z):
    if (z < redshift.min() or z > redshift.max()):
        print("z is out of bounds");
        return("", []);
    else:
        lt = log_tau_func(z);
        ptarr = np.zeros((nr_titles + 1, k_size));

        #The first column should be the wavenumbers
        ptarr[0] = k;

        #The column titles
        columns = ["k"];
        columns = columns + titlestrings;

        #The other columns should be filled with the functions interpolated to this time
        for i in np.arange(nr_titles):
            delta_func = interp2d(log_tau, k, delta[i].T);
            pt = delta_func(lt, k);

            ptarr[i+1] = pt.reshape(k_size);

        #Transpose the table
        ptarr = ptarr.T;

        return(ptarr, columns);

def perturb_at_wavenumber(the_k):

    if (the_k < k.min() or the_k > k.max()):
        print("k is out of bounds");
        return("", []);
    else:
        ptarr = np.zeros((nr_titles + 2, tau_size));

        #The first column should be the redshifts
        ptarr[0] = redshift;

        #The second column should be the conformal times
        ptarr[1] = tau;

        #The column titles
        columns = ["z", "conformal_time"];
        columns = columns + titlestrings;

        #The other columns should be filled with the functions interpolated to this wavenumber
        for i in np.arange(nr_titles):
            delta_func = interp2d(log_tau, k, delta[i].T);
            pt = delta_func(log_tau, the_k);

            ptarr[i+2] = pt.reshape(tau_size);

        #Transpose the table
        ptarr = ptarr.T;

        return(ptarr, columns);

def cosmology_background():
    ptarr = np.zeros((nr_titles + 2, tau_size));

    #The first column should be the redshifts
    ptarr[0] = redshift;

    #The second column should be the conformal times
    ptarr[1] = tau;

    #Add some background functions
    ptarr[2] = np.array(f["Perturb/Growth factors (D)"]);
    ptarr[3] = np.array(f["Perturb/Logarithmic growth rates (f)"]);
    ptarr[4] = np.array(f["Perturb/Logarithmic growth rate conformal derivatives (f')"]);
    ptarr[5] = np.array(f["Perturb/Hubble rates"]);
    ptarr[6] = np.array(f["Perturb/Hubble rate conformal time derivatives"]);
    ptarr[7] = np.array(f["Perturb/Omega matter"]);
    ptarr[8] = np.array(f["Perturb/Omega radiation"]);

    #The column titles
    columns = ["z", "conformal_time", "growth_factor_D", "growth_rate_f", "growth_rate_f_prime", "Hubble_rate", "Hubble_rate_prime", "Omega_m", "Omega_r"];
    col_counter = 9;

    #The other columns should be filled with background densities to this wavenumber
    for i in np.arange(nr_titles):
        #Ignore columns that are all zeroes
        if (not (0 == Omegas[i].max() == Omegas[i].min())):
            ptarr[col_counter] = Omegas[i];
            col_counter = col_counter + 1;
            columns.append(titlestrings[i]);

    #Get rid of the empty columns
    ptarr = ptarr[:col_counter,];

    #Transpose the table
    ptarr = ptarr.T;

    return(ptarr, columns);

def power_at_redshift(z,A_s,n_s,k_pivot):
    if (z < redshift.min() or z > redshift.max()):
        print("z is out of bounds");
        return("", []);
    else:
        lt = log_tau_func(z);
        Parr = np.zeros((nr_titles + 1, k_size));

        #The first column should be the wavenumbers
        Parr[0] = k;

        #The column titles
        columns = ["k"];
        col_counter = 1;
        # columns = columns + titlestrings;

        #The other columns should be filled with the functions interpolated to this time
        for i in np.arange(nr_titles):
            delta_func = interp2d(log_tau, k, delta[i].T);
            pt = delta_func(lt, k).reshape(k_size);
            if (titlestrings[i][0:2] == "d_"):
                P = A_s * (pt*pt) * (k / k_pivot) ** n_s;
                Parr[col_counter] = P;
                columns.append(titlestrings[i]);
                col_counter = col_counter + 1;

        #Get rid of the empry columns
        Parr = Parr[:col_counter,];

        #Transpose the table
        Parr = Parr.T;

        return(Parr, columns);
