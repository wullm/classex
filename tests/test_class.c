#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <class.h>

#include "../include/classex.h"

static inline void sucmsg(const char *msg) {
    printf("%s%s%s\n\n", TXT_GREEN, msg, TXT_RESET);
}

int main() {
    /* Test reading parameters */
    const char fname[] = "test_cosmology.ini";
    struct params pars;
    struct units us;

    readParams(&pars, fname);
    readUnits(&us, fname);

    /* Define the CLASS structures */
    struct precision pr;  /* for precision parameters */
    struct background ba; /* for cosmological background */
    struct thermo th;     /* for thermodynamics */
    struct perturbs pt;   /* for source functions */
    struct transfers tr;  /* for transfer functions */
    struct primordial pm; /* for primordial spectra */
    struct spectra sp;    /* for output spectra */
    struct nonlinear nl;  /* for non-linear spectra */
    struct lensing le;    /* for lensed spectra */
    struct output op;     /* for output files */
    ErrorMsg errmsg;      /* for CLASS-specific error messages */

    /* If no class .ini file was specified, infer parameters from the cosmology */
    if (pars.ClassIniFile[0] == '\0') {
        printf("No CLASS file specified!\n");
        return 1;
    }

    int class_argc = 2;
    char *class_argv[] = {"", pars.ClassIniFile, pars.ClassPreFile};

    assert (input_init_from_arguments(class_argc, class_argv, &pr, &ba, &th, &pt, &tr,
                                  &pm, &sp, &nl, &le, &op, errmsg) == _SUCCESS_);

    printf("Running CLASS.\n");

    assert(background_init(&pr, &ba) == _SUCCESS_);
    assert(thermodynamics_init(&pr, &ba, &th) == _SUCCESS_);
    assert(perturb_init(&pr, &ba, &th, &pt) == _SUCCESS_);

    /* Do some order of magnitude checks */
    assert(ba.h > 0.60 && ba.h < 0.70);
    assert(ba.age > 13.0 && ba.age <= 14.0); //Gyr
    assert(ba.Neff > 2 && ba.Neff < 4);
    assert(ba.z_eq > 1e3 && ba.z_eq < 1e4);

    /* Check the equation of state at early times for neutrinos */
    int index_tau_early = 10; //10 steps after the start date
    int index_rho_nu = ba.index_bg_rho_ncdm1; //neutrino (species 1) density
    int index_p_nu = ba.index_bg_p_ncdm1; //neutrino (species 1) pressure
    double rho_early = ba.background_table[index_tau_early * ba.bg_size + index_rho_nu];
    double p_early = ba.background_table[index_tau_early * ba.bg_size + index_p_nu];
    double w_early = p_early/rho_early;

    /* Check the equation of state at late times for neutrinos */
    int index_tau_late = ba.bt_size - 10; //10 steps before the end date
    double rho_late = ba.background_table[index_tau_late * ba.bg_size + index_rho_nu];
    double p_late = ba.background_table[index_tau_late * ba.bg_size + index_p_nu];
    double w_late = p_late/rho_late;

    /* Does it make sense? */
    assert(fabs(w_early - 0.33333) < 1e-4); //radiation
    assert(fabs(w_late - 0.0) < 1e-4); //matter

    printf("Shutting CLASS down again.\n");

    /* Pre-empt segfault in CLASS if there is no interacting dark radiation */
    if (ba.has_idr == _FALSE_) {
        pt.alpha_idm_dr = (double *)malloc(0);
        pt.beta_idr = (double *)malloc(0);
    }

    /* Close CLASS again */
    assert(perturb_free(&pt) == _SUCCESS_);
    assert(thermodynamics_free(&th) == _SUCCESS_);
    assert(background_free(&ba) == _SUCCESS_);

    sucmsg("test_class:\t SUCCESS");
}
