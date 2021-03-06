/*******************************************************************************
 * This file is part of classex.
 * Copyright (c) 2020 Willem Elbers (whe@willemelbers.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <class.h>

#include "../include/classex.h"

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf("No parameter file specified.\n");
        return 0;
    }

    /* Read command line options */
    const char *fname = argv[1];
    printf("The classex parameter file is %s\n", fname);

    /* Define the classex structures */
    struct params pars;
    struct units us;
    struct class_titles titles;
    struct perturb_data data;

    readParams(&pars, fname);
    readUnits(&us, fname);

    /* Define the CLASS structures */
    struct precision pr;  /* for precision parameters */
    struct background ba; /* for cosmological background */
    struct thermodynamics th;     /* for thermodynamics */
    struct perturbations pt;   /* for source functions */
    struct transfer ptr;   /* for transfer functions */
    struct primordial ppr;  /* for primordial functions */
    struct harmonic phr;  /* for harmonic functions */
    struct fourier pfo;   /* for fourier */
    struct lensing ple;   /* for lensing */
    struct distortions psd;    /* for distortions */
    struct output op;     /* for output files */
    ErrorMsg errmsg;      /* for CLASS-specific error messages */

    /* If no class .ini file was specified, stop. */
    if (pars.ClassIniFile[0] == '\0') {
        printf("No CLASS file specified!\n");
        return 1;
    }

    int class_argc = 2;
    char *class_argv[] = {"", pars.ClassIniFile, pars.ClassPreFile};

    if (input_init(class_argc, class_argv, &pr, &ba, &th, &pt,
                   &ptr, &ppr, &phr, &pfo, &ple, &psd, &op, errmsg) == _FAILURE_) {
        printf("Error running input_init_from_arguments \n=>%s\n", errmsg);
    }

    printf("Running CLASS.\n\n");

    if (background_init(&pr, &ba) == _FAILURE_) {
        printf("Error running background_init \n%s\n", ba.error_message);
    }

    if (thermodynamics_init(&pr, &ba, &th) == _FAILURE_) {
        printf("Error in thermodynamics_init \n%s\n", th.error_message);
    }

    if (perturbations_init(&pr, &ba, &th, &pt) == _FAILURE_) {
        printf("Error in perturbations_init \n%s\n", pt.error_message);
    }

    printf("\n");

    /* Match user-defined titles with CLASS indices */
    initClassTitles(&titles, &pt, &ba);
    matchClassTitles(&titles, &pars);

    /* Read perturb data */
    readPerturbData(&data, &pars, &us, &pt, &ba);

    printf("We have read out %d functions.\n", data.n_functions);
    printf("For %d functions, we also have non-zero Omega(tau).\n", pars.MatchedWithBackground);

    /* Compute derivatives */
    computeDerivatives(&data, &pars, &us);

    /* Retrieve the number of ncdm species and their masses in eV */
    pars.N_ncdm = ba.N_ncdm;
    pars.M_ncdm_eV = ba.m_ncdm_in_eV;
    pars.T_CMB = ba.T_cmb * us.UnitTemperatureKelvin;
    pars.T_ncdm = ba.T_ncdm;
    pars.h = ba.h;
    pars.Omega_lambda = ba.Omega0_lambda;
    pars.Omega_k = ba.Omega0_k;
    pars.Omega_m = ba.Omega0_m;
    pars.Omega_b = ba.Omega0_b;
    pars.Omega_ur = ba.Omega0_ur;

    /* Here neutrinos do not contribute to Omega_m, but in CLASS they do */
    for (int i=0; i<pars.N_ncdm; i++) {
        pars.Omega_m -= ba.Omega0_ncdm[i];
    }

    /* Write it to a file */
    write_perturb(&data, &pars, &us, pars.OutputFilename);

    /* Clean perturb data */
    cleanPerturbData(&data);

    printf("\nShutting CLASS down again.\n");

    /* Pre-empt segfault in CLASS if there is no interacting dark radiation */
    if (ba.has_idr == _FALSE_) {
        pt.alpha_idm_dr = (double *)malloc(0);
        pt.beta_idr = (double *)malloc(0);
    }

    /* Close CLASS again */
    if (perturbations_free(&pt) == _FAILURE_) {
        printf("Error in freeing class memory \n%s\n", pt.error_message);
        return 1;
    }

    if (thermodynamics_free(&th) == _FAILURE_) {
        printf("Error in thermodynamics_free \n%s\n", th.error_message);
        return 1;
    }

    if (background_free(&ba) == _FAILURE_) {
        printf("Error in background_free \n%s\n", ba.error_message);
        return 1;
    }

    /* Clean up */
    cleanClassTitles(&titles);
    cleanParams(&pars);
}
