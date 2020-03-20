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

#include "../include/class_transfer.h"

int readPerturbData(struct perturb_data *data, struct params *pars,
                    struct units *us, struct perturbs *pt, struct background *ba) {
    /* Try getting a source */
    int index_md = pt->index_md_scalars;  // scalar mode
    int index_ic = 0;                     // index of the initial condition
    int index_tp;                         // type of source function

    /* Size of the perturbations */
    size_t k_size = pt->k_size[index_md];
    size_t tau_size = pt->tau_size;

    /* The number of transfer functions to be read */
    const size_t n_functions = pars->MatchedFunctions;

    /* Little Hubble h */
    const double h = ba->h;

    /* CLASS to internal units conversion factor */
    const double unit_length_factor = MPC_METRES / us->UnitLengthMetres;
    const double unit_time_factor = unit_length_factor / us->SpeedOfLight;

    /* Vector of the wavenumbers */
    data->k_size = k_size;
    data->k = (double *)calloc(k_size, sizeof(double));

    /* Vector of the conformal times at which the perturbation is sampled */
    data->tau_size = tau_size;
    data->log_tau = (double *)calloc(tau_size, sizeof(double));

    /* The number of transfer functions to be read */
    data->n_functions = n_functions;

    // /* What functions should be read */
    // int *functions = malloc(n_functions * sizeof(double));
    // functions[0] = pt->index_tp_delta_ncdm1;
    // functions[1] = pt->index_tp_delta_cdm;

    /* Vector with the transfer functions T(tau, k) */
    data->delta = (double *)calloc(n_functions * k_size * tau_size, sizeof(double));

    /* Read out the log conformal times */
    for (size_t index_tau = 0; index_tau < tau_size; index_tau++) {
        /* Convert tau from Mpc to U_T */
        double tau = pt->tau_sampling[index_tau] * unit_time_factor;
        data->log_tau[index_tau] = log(tau);
    }

    /* Read out the wavenumbers */
    for (size_t index_k = 0; index_k < k_size; index_k++) {
        /* Convert k from h/Mpc to 1/U_L */
        double k = pt->k[index_md][index_k] * h / unit_length_factor;
        data->k[index_k] = k;
    }

    /* Convert and store the transfer functions */
    for (size_t index_tau = 0; index_tau < tau_size; index_tau++) {
        for (size_t index_k = 0; index_k < k_size; index_k++) {
            for (size_t index_func = 0; index_func < n_functions; index_func++) {
                index_tp = pars->IndexOfFunctions[index_func];  // type of source function
                double p = pt->sources[index_md][index_ic * pt->tp_size[index_md] +
                                                index_tp][index_tau * k_size + index_k];

                /* Convert transfer functions from CLASS format to CAMB/HeWon/icgen/
                *  Eisenstein-Hu format by multiplying by -1/k^2.
                */
                double k = pt->k[index_md][index_k] * h / unit_length_factor;
                double T = -p / k / k;
                data->delta[tau_size * k_size * index_func + k_size * index_tau + index_k] = T;
            }
        }
    }

    printf("The perturbations are sampled at %zu * %zu points.\n", k_size, tau_size);

    return 0;
}

int cleanPerturbData(struct perturb_data *data) {
    free(data->delta);
    free(data->k);
    free(data->log_tau);

    return 0;
}
