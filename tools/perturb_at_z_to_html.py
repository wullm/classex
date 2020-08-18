import sys;
from tabulate import tabulate;

if (len(sys.argv) < 3):
    print("Usage: perturb_at_z_to_html.py file redshift<br/>");
else:
    try:
        z = float(sys.argv[2]);

        from explore import perturb_at_redshift;

        print("Perturbation vector at z = ", z, "<p/>");
        ptarr, columns = perturb_at_redshift(z);

        htmlcode = tabulate(ptarr, tablefmt="html", headers=columns);
        print(htmlcode);

    except ValueError:
        print("Usage: perturb_at_z_to_html.py file redshift<br/>");
        print("Redshift is not numeric.");
