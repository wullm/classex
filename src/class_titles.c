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

#include "../include/class_titles.h"

int initClassTitles(struct class_titles *cat, struct perturbs *pt, struct background *ba) {
    cat->indices = malloc(MAX_NUM_FUNCTIONS * sizeof(int));
    cat->titles = malloc(MAX_NUM_FUNCTIONS * sizeof(char*));

    /* The order in which these are added is irrelevant, unlike in CLASS. */
    store_title(&cat->indices, &cat->titles, pt->index_tp_delta_g, "d_g", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_delta_b, "d_b", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_delta_cdm, "d_cdm", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_delta_idm_dr, "d_idm_dr", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_delta_fld, "d_fld", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_delta_ur, "d_ur", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_delta_idr, "d_idr", &cat->num);
    for (int i=0; i<ba->N_ncdm; i++) {
        char tmp[40];
        sprintf(tmp, "d_ncdm[%d]", i);
        store_title(&cat->indices, &cat->titles, pt->index_tp_delta_ncdm1 + i, tmp, &cat->num);
    }
    store_title(&cat->indices, &cat->titles, pt->index_tp_delta_dcdm, "d_dcdm", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_delta_dr, "d_dr", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_delta_scf, "d_scf", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_delta_tot, "d_tot", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_phi, "phi", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_psi, "psi", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_phi_prime, "phi_prime", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_h, "h", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_h_prime, "h_prime", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_eta, "eta", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_eta_prime, "eta_prime", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_H_T_Nb_prime, "H_T_Nb_prime", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_k2gamma_Nb, "k2gamma_Nb", &cat->num);

    store_title(&cat->indices, &cat->titles, pt->index_tp_theta_g, "t_g", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_theta_b, "t_b", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_theta_cdm, "t_cdm", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_theta_idm_dr, "t_idm_dr", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_theta_fld, "t_fld", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_theta_ur, "t_ur", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_theta_idr, "t_idr", &cat->num);
    for (int i=0; i<ba->N_ncdm; i++) {
        char tmp[40];
        sprintf(tmp, "t_ncdm[%d]", i);
        store_title(&cat->indices, &cat->titles, pt->index_tp_theta_ncdm1 + i, tmp, &cat->num);
    }
    store_title(&cat->indices, &cat->titles, pt->index_tp_theta_dcdm, "t_dcdm", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_theta_dr, "t_dr", &cat->num);
    store_title(&cat->indices, &cat->titles, pt->index_tp_theta_scf, "t_scf", &cat->num); //beware: there is a typo here in the current version of CLASS
    store_title(&cat->indices, &cat->titles, pt->index_tp_theta_tot, "t_tot", &cat->num);

    return 0;
}


int cleanClassTitles(struct class_titles *cat) {
    for (int i=0; i<cat->num; i++) {
        free(cat->titles[i]);
    }
    free(cat->titles);
    free(cat->indices);

    return 0;
}


int matchClassTitles(struct class_titles *cat, struct params *pars) {
    /* Compare the user specified titles with the CLASS titles */
    for (int i=0; i<cat->num; i++) {
        for (int j=0; j<pars->NumFunctions; j++) {
            if (strcmp(cat->titles[i], pars->DesiredFunctions[j]) == 0) {
                pars->IndexOfFunctions[j] = cat->indices[i];
                printf("We found a match '%s': class_index = %d\n", cat->titles[i], cat->indices[i]);
            }
        }
    }

    return 0;
}
