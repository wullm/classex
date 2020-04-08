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
    struct class_titles titles;

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

    /* Initialize the CLASS title/index dictionary */
    assert(initClassTitles(&titles, &pt, &ba) == 0);

    /* Check that we some other expected titles */
    char found_d_g = 0;
    char found_t_idr = 0;
    char found_d_ncdm_2 = 0;
    char found_H_T_Nb_prime = 0;

    for (int i=0; i<titles.num; i++) {
        if (strcmp(titles.pairs[i].title, "d_g") == 0) {
            found_d_g = 1;
            assert(titles.pairs[i].index == pt.index_tp_delta_g);
        } else if (strcmp(titles.pairs[i].title, "t_idr") == 0) {
            found_t_idr = 1;
            assert(titles.pairs[i].index == pt.index_tp_theta_idr);
        } else if (strcmp(titles.pairs[i].title, "d_ncdm[2]") == 0) {
            found_d_ncdm_2 = 1;
            assert(titles.pairs[i].index == pt.index_tp_delta_ncdm1 + 2);
        } else if (strcmp(titles.pairs[i].title, "H_T_Nb_prime") == 0) {
            found_H_T_Nb_prime = 1;
            assert(titles.pairs[i].index == pt.index_tp_H_T_Nb_prime);
        }
    }

    assert(found_d_g == 1);
    assert(found_t_idr == 1);
    assert(found_d_ncdm_2 == 1);
    assert(found_H_T_Nb_prime == 1);

    /* Match titles */
    assert(matchClassTitles(&titles, &pars) == 0);

    /* Check that the matches were successful as expected. */
    assert(pars.NumDesiredFunctions == 7);
    assert(pars.MatchedFunctions == 4);
    assert(strcmp(pars.DesiredFunctions[0], "d_cdm") == 0);
    assert(strcmp(pars.DesiredFunctions[1], "H_T_Nb_prime") == 0);
    assert(strcmp(pars.DesiredFunctions[2], "eta_prime") == 0);
    assert(strcmp(pars.DesiredFunctions[3], "h_prime") == 0);
    assert(pars.IndexOfFunctions[0] == pt.index_tp_delta_cdm);
    assert(pars.IndexOfFunctions[1] == pt.index_tp_H_T_Nb_prime);
    assert(pars.IndexOfFunctions[2] == pt.index_tp_eta_prime);
    assert(pars.IndexOfFunctions[3] == pt.index_tp_h_prime);

    /* Clean up the dictionary */
    assert(cleanClassTitles(&titles) == 0);

    /* Close CLASS again */
    assert(perturb_free(&pt) == _SUCCESS_);
    assert(thermodynamics_free(&th) == _SUCCESS_);
    assert(background_free(&ba) == _SUCCESS_);

    /* Clean up */
    cleanParams(&pars);

    sucmsg("test_titles:\t SUCCESS");
}
