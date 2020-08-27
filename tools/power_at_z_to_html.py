import sys;
from tabulate import tabulate;

A_s = 2.097e-09;
n_s = 0.9652;
k_pivot = 0.05;

if (len(sys.argv) < 3):
    print("Usage: power_at_z_to_html.py file redshift<br/>");
else:
    try:
        z = float(sys.argv[2]);

        from explore import power_at_redshift;

        print("Linear power spectra at z = ", z, "<p/>");
        ptarr, columns = power_at_redshift(z, A_s, n_s, k_pivot);

        htmlcode = tabulate(ptarr, tablefmt="html", headers=columns);
        print(htmlcode);

    except ValueError:
        print("Usage: power_at_z_to_html.py file redshift<br/>");
        print("Redshift is not numeric.");
