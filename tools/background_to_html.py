from explore import cosmology_background;
from tabulate import tabulate;

ptarr, columns = cosmology_background();

htmlcode = tabulate(ptarr, tablefmt="html", headers=columns);
print(htmlcode);
