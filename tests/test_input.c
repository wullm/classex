#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include "../include/classex.h"

static inline void sucmsg(const char *msg) {
    printf("%s%s%s\n\n", TXT_GREEN, msg, TXT_RESET);
}

int main() {
    /* Test reading parameters */
    const char fname[] = "test_cosmology.ini";
    struct params pars;

    readParams(&pars, fname);

    assert(strcmp(pars.Name, "Test Simulation") == 0);
    assert(strcmp(pars.OutputDirectory, "../tests") == 0);

    /* Test the transfer function titles */
    assert(pars.NumFunctions == 4);
    assert(strcmp(pars.DesiredFunctions[0], "d_cdm") == 0);
    assert(strcmp(pars.DesiredFunctions[1], "H_T_Nb_prime") == 0);
    assert(strcmp(pars.DesiredFunctions[2], "eta_prime") == 0);
    assert(strcmp(pars.DesiredFunctions[3], "h_prime") == 0);

    /* Test reading units */
    struct units us;
    readUnits(&us, fname);

    assert(us.UnitLengthMetres == 3.086e22);
    assert(us.UnitTimeSeconds == 3.154e16);
    assert(us.UnitMassKilogram == 1.989e40);

    /* Test reading cosmology */
    struct cosmology cosmo;
    readCosmology(&cosmo, fname);

    assert(cosmo.h == 0.67556);

    sucmsg("test_input:\t SUCCESS");
}
