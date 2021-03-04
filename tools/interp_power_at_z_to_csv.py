import sys;
from tabulate import tabulate;
import numpy as np;

A_s = 2.097e-09;
n_s = 0.9652;
k_pivot = 0.05;

if (len(sys.argv) < 3):
    print("Usage: interp_power_at_z_to_html.py file redshift k_file");
else:
    z = float(sys.argv[2]);

    source_fname = sys.argv[3]
    source_table = np.loadtxt(source_fname)
    source_k = source_table[:,0]
    nk = len(source_k)

    from explore import power_at_redshift;

    print("Linear power spectra at z = ", z, "<p/>");
    ptarr, columns = power_at_redshift(z, A_s, n_s, k_pivot);

    ncol = len(columns)

    full_k = ptarr[:,0]
    out_arr = np.zeros((nk, ncol))
    out_arr[:,0] = source_k
    for i in range(ncol-1):
        out_arr[:,1+i] = np.exp(np.interp(np.log(source_k), np.log(full_k), np.log(ptarr[:,1+i])))

    htmlcode = tabulate(out_arr, tablefmt="html", headers=columns);
    print(htmlcode);
