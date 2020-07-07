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
    // const double h = ba->h;

    /* CLASS to internal units conversion factor */
    const double unit_length_factor = MPC_METRES / us->UnitLengthMetres;
    const double unit_time_factor = unit_length_factor / us->SpeedOfLight;

    /* Vector of the wavenumbers */
    data->k_size = k_size;
    data->k = (double *)calloc(k_size, sizeof(double));

    /* Vector of the conformal times at which the perturbation is sampled */
    data->tau_size = tau_size;
    data->log_tau = (double *)calloc(tau_size, sizeof(double));
    data->redshift = (double *)calloc(tau_size, sizeof(double));

    /* The number of transfer functions to be read */
    data->n_functions = n_functions;

    /* Vector with the transfer functions T(tau, k) */
    data->delta = (double *)calloc(n_functions * k_size * tau_size, sizeof(double));

    /* Vector of background quantities at each time Omega(tau) */
    data->Omega = (double *)calloc(n_functions * tau_size, sizeof(double));

    /* Read out the log conformal times */
    for (size_t index_tau = 0; index_tau < tau_size; index_tau++) {
        /* Convert tau from Mpc to U_T */
        double tau = pt->tau_sampling[index_tau] * unit_time_factor;
        data->log_tau[index_tau] = log(tau);
    }

    /* Read out the wavenumbers */
    for (size_t index_k = 0; index_k < k_size; index_k++) {
        /* Note: CLASS exports transfer function files with k in h/Mpc,
         * but internally it uses 1/Mpc. */

         /* Convert k from 1/Mpc to 1/U_L */
        double k = pt->k[index_md][index_k] / unit_length_factor;
        data->k[index_k] = k;
    }

    /* The index for data->delta, not CLASS index, nor index in titles string */
    int index_func = 0;
    /* Convert and store the transfer functions */
    for (size_t i = 0; i < pars->NumDesiredFunctions; i++) {
        /* Ignore functions that have no matching CLASS index */
        if (pars->ClassPerturbIndices[i] < 0) continue;

        /* Determine the unit conversion factor */
        char *title = pars->DesiredFunctions[i];
        double unit_factor = unitConversionFactor(title, unit_length_factor, unit_time_factor);

        printf("Unit conversion factor for '%s' is %f\n", title, unit_factor);

        /* For each timestep and wavenumber */
        for (size_t index_tau = 0; index_tau < tau_size; index_tau++) {
            for (size_t index_k = 0; index_k < k_size; index_k++) {
                /* Transfer the corresponding data */
                index_tp = pars->ClassPerturbIndices[i];  // CLASS index
                double p = pt->sources[index_md][index_ic * pt->tp_size[index_md] +
                                                index_tp][index_tau * k_size + index_k];

                /* Convert transfer functions from CLASS format to CAMB/HeWon/dexm/
                *  Eisenstein-Hu format by multiplying by -1/k^2.
                */
                double k = pt->k[index_md][index_k] / unit_length_factor;
                double T = -p/k/k;

                /* Convert from CLASS units to output units */
                T *= unit_factor;

                data->delta[tau_size * k_size * index_func + k_size * index_tau + index_k] = T;
            }
        }
        index_func++;
    }

    printf("\n");

    /* Finally, we also want to get the redshifts and background densities.
     * To do this, we need to let CLASS populate a vector of background
     * quantities at each time step.
     */

    /* Allocate array for background quantities */
    double *pvecback = malloc(ba->bg_size * sizeof(double));
    int last_index; //not used, but is output by CLASS

    /* Read out the redshifts and background densities */
    for (size_t index_tau = 0; index_tau < tau_size; index_tau++) {
        /* Conformal time in Mpc/c (the internal time unit in CLASS) */
        double tau = pt->tau_sampling[index_tau];

        /* Make CLASS evaluate background quantities at this time*/
        background_at_tau(ba, tau, ba->bg_size, ba->inter_normal,
                          &last_index, pvecback);

        /* The scale-factor and redshift */
        double a = pvecback[ba->index_bg_a];
        double z = 1./a - 1.;

        /* Store the redshift */
        data->redshift[index_tau] = z;

        /* The critical density at this redshift */
        double rho_crit = pvecback[ba->index_bg_rho_crit];

        /* Read out the background densities */
        int index_func = 0;
        for (size_t i = 0; i < pars->NumDesiredFunctions; i++) {
            /* Ignore functions that have no matching CLASS perturbation index */
            if (pars->ClassPerturbIndices[i] < 0) continue;

            /* For functions that also have a CLASS background index */
            if (pars->ClassBackgroundIndices[i] >= 0) {
                /* The density corresponding to this function */
                double rho = pvecback[pars->ClassBackgroundIndices[i]];

                /* The density, as fraction of the critical density */
                double Omega = rho/rho_crit;

                /* Store the dimensionless background density */
                data->Omega[tau_size * index_func + index_tau] = Omega;
            } else {
                //We just store zero, to keep it simple. The memory was calloc'ed.
            }

            index_func++;
        }
    }

    free(pvecback);

    printf("The perturbations are sampled at %zu * %zu points.\n", k_size, tau_size);

    return 0;
}

/* Unit conversion factor for transfer functions, depending on the title. */
double unitConversionFactor(char *title, double unit_length_factor,
                            double unit_time_factor) {

    /* Note the difference between strcmp and strncmp! */
    char *title_end = &title[strlen(title)];

    /* Most transfer functions are dimensionless, (e.g. overdensities) */
    double factor = 1.0;

    /* Energy flux transfer functions (theta = nabla.v) have dimension
    * inverse time. These have titles starting with "t_". */
    if (strncmp(title, "t_", 2) == 0) {
        factor /= unit_time_factor;
    }

    /* Functions that are time derivatives have dimension inverse time */
    if (strncmp(title_end-12, "_prime_prime", 12) == 0) {
        factor /= pow(unit_time_factor, 2);
    } else if (strncmp(title_end-6, "_prime", 6) == 0) {
        factor /= unit_time_factor;
    }

    /* Potential transfer functions have dimensions of energy per mass or
    * (Length/Time)^2. This applies to "phi", "psi", "h", "eta" and their
    * derivatives (and possibly higher order derivatives).
    */
    if (strcmp(title, "h") == 0   || strncmp(title, "h_", 2) == 0 ||
        strcmp(title, "phi") == 0 || strncmp(title, "phi_", 4) == 0 ||
        strcmp(title, "eta") == 0 || strncmp(title, "eta_", 4) == 0 ||
        strcmp(title, "psi") == 0 || strncmp(title, "psi_", 4) == 0) {
            factor *= pow(unit_length_factor / unit_time_factor, 2);
    }

    return factor;
}

int cleanPerturbData(struct perturb_data *data) {
    free(data->delta);
    free(data->k);
    free(data->log_tau);
    free(data->redshift);
    free(data->Omega);

    return 0;
}
