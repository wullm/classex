import h5py;
import numpy as np;
from matplotlib import pyplot as plt;
from scipy.interpolate import interp1d;
from scipy.interpolate import interp2d;
import sys;

fname = sys.argv[1];
f = h5py.File(fname, "r");

print("Parsing cosmological data from", fname, "<br/>");

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
#    titlestrings.append(tbytes.decode("utf-8"));
    titlestrings.append(tbytes);

#Sizes of the perturbation vector
k_size = len(k);
tau_size = len(tau);
nr_titles = len(titles);

def find_title(titlestr):
    if not titlestr in titlestrings:
        return(-1);
    else:
        return(titlestrings.index(titlestr));


#Add a column with merged cdm & baryon
if ("d_cdm" in titlestrings and "d_b" in titlestrings):
    cdm_index = find_title("d_cdm");
    b_index = find_title("d_b");

    Omega_c = Omegas[cdm_index];
    Omega_b = Omegas[b_index];

    today_index = -1;
    Omega_c0 = Omega_c[today_index];
    Omega_b0 = Omega_b[today_index];

    weight_c = Omega_c0 / (Omega_c0 + Omega_b0);
    weight_b = Omega_b0 / (Omega_c0 + Omega_b0);

    print("weights [cdm, b] = [", weight_c, ",", weight_b, "]<br/>");

    titlestrings.append("d_cb_merge");

    delta_c = delta[cdm_index];
    delta_b = delta[b_index];

    delta_cb = weight_c * delta_c + weight_b * delta_b;
    delta = np.insert(delta, nr_titles, delta_cb, axis=0);

    Omega_cb = Omega_c + Omega_b;
    Omegas = np.insert(Omegas, nr_titles, Omega_cb, axis=0);

    nr_titles+=1;

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

    #Also compute the physical density at z=0
    H = ptarr[5][-1]
    #G_newt = 4.492389e-05 #Mpc^3/(1e10 M_sol)/Gyr^2 (what I used before)
    G_newt = 4.49233855e-05 #Mpc^3/(1e10 M_sol)/Gyr^2 (my Mpc,Gyr,M_sol no leap year correction)
    rho_crit = 3*H*H/(8*np.pi*G_newt)
    print("The critical density is ", rho_crit, " (10^10 M_sol / Mpc^3) for H = ", H ,"<br/>")

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
                twoPP = 2 * np.pi**2
                P = A_s * (pt*pt) * (k / k_pivot) ** (n_s - 1) * k * twoPP;
                Parr[col_counter] = P;
                columns.append(titlestrings[i]);
                col_counter = col_counter + 1;

        #Get rid of the empry columns
        Parr = Parr[:col_counter,];

        #Transpose the table
        Parr = Parr.T;

        return(Parr, columns);
