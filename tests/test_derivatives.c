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

    /* Check that the matches were successful as expected. */
    assert(pars.NumDesiredFunctions == 7);
    assert(pars.MatchedFunctions == 4);
    assert(strcmp(pars.DesiredFunctions[0], "d_cdm") == 0);
    assert(strcmp(pars.DesiredFunctions[1], "H_T_Nb_prime") == 0);
    assert(strcmp(pars.DesiredFunctions[2], "eta_prime") == 0);
    assert(strcmp(pars.DesiredFunctions[3], "h_prime") == 0);
    assert(strcmp(pars.DesiredFunctions[6], "H_T_Nb_prime_prime") == 0);
    assert(pars.IndexOfFunctions[0] == pt.index_tp_delta_cdm);
    assert(pars.IndexOfFunctions[1] == pt.index_tp_H_T_Nb_prime);
    assert(pars.IndexOfFunctions[2] == pt.index_tp_eta_prime);
    assert(pars.IndexOfFunctions[3] == pt.index_tp_h_prime);

    /* Check that only d_cdm_prime is a new derivative */
    assert(isNewDerivativeTitle(&pars, pars.DesiredFunctions[0]) == -1);
    assert(isNewDerivativeTitle(&pars, pars.DesiredFunctions[1]) == -1);
    assert(isNewDerivativeTitle(&pars, pars.DesiredFunctions[2]) == -1);
    assert(isNewDerivativeTitle(&pars, pars.DesiredFunctions[3]) == -1);
    assert(isNewDerivativeTitle(&pars, pars.DesiredFunctions[4]) == -1);
    assert(isNewDerivativeTitle(&pars, pars.DesiredFunctions[5]) == -1);
    assert(isNewDerivativeTitle(&pars, pars.DesiredFunctions[6]) == 1);

    /* Read perturb data */
    assert(readPerturbData(&data, &pars, &us, &pt, &ba) == 0);

    /* Compute derivatives */
    assert(computeDerivatives(&data, &pars, &us) == 0);

    /* Checks */
    assert(pars.MatchedFunctions == 5); //now one more, because derivative!
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
        // printf("%d %d\n", i, data.k_size * data.tau_size * (data.n_functions -1));
        assert(data.delta[i] != 0);
    }

    /* Check that if we integrate the derivative, we get back the input */
    assert(strcmp(pars.DesiredFunctions[1], "H_T_Nb_prime") == 0); //input
    assert(strcmp(pars.DesiredFunctions[6], "H_T_Nb_prime_prime") == 0); //computed
    int source_index = 1;
    int deriv_index = 4;
    for (int i=0; i<data.k_size; i++) {
        double start = data.delta[data.tau_size * data.k_size * source_index + i];
        double integral = start;
        for (int j=0; j<data.tau_size-1; j++) {
            double dt = exp(data.log_tau[j+1]) - exp(data.log_tau[j]);
            double deriv0 = data.delta[data.tau_size * data.k_size * deriv_index + data.k_size*j + i];
            double deriv1 = data.delta[data.tau_size * data.k_size * deriv_index + data.k_size*(j+1) + i];
            double expected = data.delta[data.tau_size * data.k_size * source_index + data.k_size*j + i];
            integral += dt * 0.5 * (deriv0 + deriv1);

            /* Enforce a reasonable error margin, but not if the derivative is large */
            if (fabs(deriv0/expected) < 0.1) {
                // printf("%d %e %e %f %e\n", j, integral, expected, fabs(integral - expected)/expected, deriv0/expected);
                assert(fabs(integral - expected)/expected < 0.15);
            }
        }
    }

    printf("We have read out %d functions.\n", data.n_functions);

    /* Clean perturb data */
    assert(cleanPerturbData(&data) == 0);



    printf("\nShutting CLASS down again.\n");

    /* Clean up the dictionary */
    assert(cleanClassTitles(&titles) == 0);

    /* Close CLASS again */
    assert(perturb_free(&pt) == _SUCCESS_);
    assert(thermodynamics_free(&th) == _SUCCESS_);
    assert(background_free(&ba) == _SUCCESS_);

    /* Clean up */
    cleanParams(&pars);

    sucmsg("test_derivatives:\t SUCCESS");
}
