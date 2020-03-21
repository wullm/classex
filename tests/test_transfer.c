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
    struct perturb_data data;

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
    /* Match titles */
    assert(matchClassTitles(&titles, &pars) == 0);
    /* Clean up the dictionary */
    assert(cleanClassTitles(&titles) == 0);


    /* Read perturb data */
    assert(readPerturbData(&data, &pars, &us, &pt, &ba) == 0);

    assert(data.n_functions == pars.MatchedFunctions);
    assert(data.k_size > 100);
    assert(data.tau_size > 100);

    /* Check that k and log_tau are strictly increasing */
    for (int i=1; i<data.k_size; i++) {
        assert(data.k[i] > data.k[i-1]);
    }
    for (int i=1; i<data.tau_size; i++) {
        assert(data.log_tau[i] > data.log_tau[i-1]);
    }

    /* Check that the perturbations are all non-zero */
    for (int i=0; i<data.k_size * data.tau_size * data.n_functions; i++) {
        assert(data.delta[i] != 0);
    }

    /* Check that the cdm density transfer function is positive */
    assert(strcmp(pars.DesiredFunctions[0], "d_cdm") == 0);
    int our_index_for_d_cdm = 0; // not the CLASS index
    for (int i=0; i<data.k_size * data.tau_size; i++) {
        assert(data.delta[data.tau_size * data.k_size * our_index_for_d_cdm + i] > 0);
    }

    printf("We have read out %d functions.\n", data.n_functions);

    /* Write it to a file */
    // assert(write_perturb(&data, &pars, &us, pars.OutputFilename) == 0);

    /* Clean perturb data */
    assert(cleanPerturbData(&data) == 0);



    printf("\nShutting CLASS down again.\n");

    /* Pre-empt segfault in CLASS if there is no interacting dark radiation */
    if (ba.has_idr == _FALSE_) {
        pt.alpha_idm_dr = (double *)malloc(0);
        pt.beta_idr = (double *)malloc(0);
    }


    /* Close CLASS again */
    assert(perturb_free(&pt) == _SUCCESS_);
    assert(thermodynamics_free(&th) == _SUCCESS_);
    assert(background_free(&ba) == _SUCCESS_);

    /* Clean up */
    cleanParams(&pars);

    sucmsg("test_transfer:\t SUCCESS");
}
