import sys;
from tabulate import tabulate;

if (len(sys.argv) < 3):
    print("Usage: perturb_at_k_to_html.py file wavenumber");
else:
    try:
        the_k = float(sys.argv[2]);

    except ValueError:
        print("Usage: perturb_at_k_to_html.py file wavenumber<br/>");
        print("Wavenumber is not numeric.");

    from explore import perturb_at_wavenumber;

    print("Perturbation vector at k = ", the_k, "<p/>");
    ptarr, columns = perturb_at_wavenumber(the_k);

    htmlcode = tabulate(ptarr, tablefmt="html", headers=columns);
    print(htmlcode);
