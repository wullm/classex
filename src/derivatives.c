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

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/derivatives.h"

#define arr_id(func,ik,itau,nk,ntau) ntau*nk*(func) + nk*(itau) + ik

/* Is this the title of a new derivative, i.e. a function ending in "_prime",
 * NOT computed by CLASS, of some function that is computed by CLASS.
 *
 * Returns -1 if not a new derivative, otherwise returns the order of the
 * differentiated function among the matched functions (>=0).
 */
int isNewDerivativeTitle(struct params *pars, char *title) {
    /* Is the title long enough to end in '_prime'? */
    if (strlen(title) < 6) return -1;

    /* Does the title end in '_prime'? */
    char *end = title + strlen(title) - 6;
    if (strncmp(end, "_prime", 6) != 0) return -1;

    /* Then check if it is a new function, i.e. not yet matched */
    char isnew = 0;
    for (int i=0; i<pars->NumDesiredFunctions; i++) {
        if (strcmp(pars->DesiredFunctions[i], title) == 0
            && pars->IndexOfFunctions[i] < 0) { //new, i.e. has no matching index
            isnew = 1;
        }
    }

    /* If it isn't new, it's not a new derivative title */
    if (isnew == 0) return -1;

    /* Is it the derivative of a title XXX that exists (title = XXX_prime) */
    int matched_index = -1;
    int count = 0;
    for (int i=0; i<pars->NumDesiredFunctions; i++) {
        if (pars->IndexOfFunctions[i] >= 0) { //function exists, has index
            /* Does it match XXX? */
            if (strncmp(pars->DesiredFunctions[i], title, strlen(title)-7) == 0) {
                matched_index = count;
            }
            count++;
        }
    }

    return matched_index;
}

/* This needs to happen after reading the perturb data */
int computeDerivatives(struct perturb_data *data, struct params *pars,
                       struct units *us) {
    /* Find the titles of new derivatives */
    int derivatives = 0;
    for (int i=0; i<pars->NumDesiredFunctions; i++) {
        if (isNewDerivativeTitle(pars, pars->DesiredFunctions[i]) >= 0) {
            printf("Dit is moeilijk hoor.\n");
            derivatives++;
        }
    }

    /* We are done if there are no new derivatives */
    if (derivatives == 0) return 0;

    /* Reallocate enough memory for the derivatives */
    data->n_functions += derivatives;
    pars->MatchedFunctions += derivatives;
    data->delta = realloc(data->delta, data->n_functions * data->k_size * data->tau_size * sizeof(double));

    printf("Computing %d extra derivatives.\n", derivatives);

    if (data->delta == NULL) {
        printf("Error: could not reallocate memory to compute derivatives.\n");
        return 1;
    }

    size_t Nk = data->k_size;
    size_t Ntau = data->tau_size;
    size_t Nf = data->n_functions;

    /* Compute the new derivatives */
    int j =  Nf - derivatives;
    for (int i=0; i<pars->NumDesiredFunctions; i++) {
        int deriv_of = isNewDerivativeTitle(pars, pars->DesiredFunctions[i]);
        if (deriv_of < 0) continue;

        /* This is a new derivative */

        /* For each wavenumber */
        for (size_t index_k = 0; index_k < Nk; index_k++) {
            /* Set the derivative to a forward difference at the initial time */
            double Ti = data->delta[arr_id(deriv_of,index_k,0,Nk,Ntau)];
            double Tp = data->delta[arr_id(deriv_of,index_k,1,Nk,Ntau)];
            double dtau_i = exp(data->log_tau[1]) - exp(data->log_tau[0]);
            data->delta[arr_id(j,index_k,0,Nk,Ntau)] = (Tp-Ti)/dtau_i;

            /* Set the derivative to a backward estimate at the final time */
            double Tf = data->delta[arr_id(deriv_of,index_k,Ntau-1,Nk,Ntau)];
            double Tm = data->delta[arr_id(deriv_of,index_k,Ntau-2,Nk,Ntau)];
            double dtau_f = exp(data->log_tau[Ntau-1]) - exp(data->log_tau[Ntau-2]);
            data->delta[arr_id(j,index_k,Ntau-1,Nk,Ntau)] = (Tf-Tm)/dtau_f;

            /* For each intermediate time, use a centred difference */
            for (size_t index_tau = 1; index_tau < Ntau-1; index_tau++) {
                double T0 = data->delta[arr_id(deriv_of,index_k,(index_tau-1),Nk,Ntau)];
                double T1 = data->delta[arr_id(deriv_of,index_k,(index_tau+1),Nk,Ntau)];
                double tau0 = exp(data->log_tau[index_tau - 1]);
                double tau1 = exp(data->log_tau[index_tau + 1]);
                double dTdtau = (T1 - T0)/(tau1 - tau0);
                data->delta[arr_id(j,index_k,index_tau,Nk,Ntau)] = dTdtau;
            }
        }

        j++;
    }

    return 0;

}
