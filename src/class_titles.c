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

/* Simply create a dictionary between CLASS title strings and indices */
int initClassTitles(struct class_titles *cat, struct perturbs *pt, struct background *ba) {
    cat->num = 0;
    cat->pairs = malloc(MAX_NUM_FUNCTIONS * sizeof(struct index_title));
    if (cat->pairs == NULL) return 1;

    /* The order in which these are added is irrelevant, unlike in CLASS. */
    store_title(&cat->pairs, pt->index_tp_delta_g, ba->index_bg_rho_g, "d_g", &cat->num);
    store_title(&cat->pairs, pt->index_tp_delta_b, ba->index_bg_rho_b, "d_b", &cat->num);
    store_title(&cat->pairs, pt->index_tp_delta_cdm, ba->index_bg_rho_cdm, "d_cdm", &cat->num);
    store_title(&cat->pairs, pt->index_tp_delta_idm_dr, ba->index_bg_rho_idm_dr, "d_idm_dr", &cat->num);
    store_title(&cat->pairs, pt->index_tp_delta_fld, ba->index_bg_rho_fld, "d_fld", &cat->num);
    store_title(&cat->pairs, pt->index_tp_delta_ur, ba->index_bg_rho_ur, "d_ur", &cat->num);
    store_title(&cat->pairs, pt->index_tp_delta_idr, ba->index_bg_rho_idr, "d_idr", &cat->num);
    for (int i=0; i<ba->N_ncdm; i++) {
        char tmp[40];
        sprintf(tmp, "d_ncdm[%d]", i);
        store_title(&cat->pairs, pt->index_tp_delta_ncdm1 + i, ba->index_bg_rho_ncdm1 + i, tmp, &cat->num);
    }
    store_title(&cat->pairs, pt->index_tp_delta_dcdm, ba->index_bg_rho_dcdm, "d_dcdm", &cat->num);
    store_title(&cat->pairs, pt->index_tp_delta_dr, ba->index_bg_rho_dr, "d_dr", &cat->num);
    store_title(&cat->pairs, pt->index_tp_delta_scf, ba->index_bg_rho_scf, "d_scf", &cat->num);
    store_title(&cat->pairs, pt->index_tp_delta_tot, ba->index_bg_rho_tot, "d_tot", &cat->num);
    store_title(&cat->pairs, pt->index_tp_phi, -1, "phi", &cat->num);
    store_title(&cat->pairs, pt->index_tp_psi, -1, "psi", &cat->num);
    store_title(&cat->pairs, pt->index_tp_phi_prime, -1, "phi_prime", &cat->num);
    store_title(&cat->pairs, pt->index_tp_h, -1, "h", &cat->num);
    store_title(&cat->pairs, pt->index_tp_h_prime, -1, "h_prime", &cat->num);
    store_title(&cat->pairs, pt->index_tp_eta, -1, "eta", &cat->num);
    store_title(&cat->pairs, pt->index_tp_eta_prime, -1, "eta_prime", &cat->num);
    store_title(&cat->pairs, pt->index_tp_H_T_Nb_prime, -1, "H_T_Nb_prime", &cat->num);
    store_title(&cat->pairs, pt->index_tp_k2gamma_Nb, -1, "k2gamma_Nb", &cat->num);
#ifdef delta_shift_Nb_defined
    store_title(&cat->pairs, pt->index_tp_delta_shift_Nb_m, -1, "delta_shift_Nb_m", &cat->num);
#endif

    store_title(&cat->pairs, pt->index_tp_theta_g, -1, "t_g", &cat->num);
    store_title(&cat->pairs, pt->index_tp_theta_b, -1, "t_b", &cat->num);
    store_title(&cat->pairs, pt->index_tp_theta_cdm, -1, "t_cdm", &cat->num);
    store_title(&cat->pairs, pt->index_tp_theta_idm_dr, -1, "t_idm_dr", &cat->num);
    store_title(&cat->pairs, pt->index_tp_theta_fld, -1, "t_fld", &cat->num);
    store_title(&cat->pairs, pt->index_tp_theta_ur, -1, "t_ur", &cat->num);
    store_title(&cat->pairs, pt->index_tp_theta_idr, -1, "t_idr", &cat->num);
    for (int i=0; i<ba->N_ncdm; i++) {
        char tmp[40];
        sprintf(tmp, "t_ncdm[%d]", i);
        store_title(&cat->pairs, pt->index_tp_theta_ncdm1 + i, -1, tmp, &cat->num);
    }
    store_title(&cat->pairs, pt->index_tp_theta_dcdm, -1, "t_dcdm", &cat->num);
    store_title(&cat->pairs, pt->index_tp_theta_dr, -1, "t_dr", &cat->num);
    store_title(&cat->pairs, pt->index_tp_theta_scf, -1, "t_scf", &cat->num); //beware: there is a typo here in the current version of CLASS
    store_title(&cat->pairs, pt->index_tp_theta_tot, -1, "t_tot", &cat->num);

#ifdef extra_neutrino_functions_defined
    for (int i=0; i<ba->N_ncdm; i++) {
        char tmp[40];
        sprintf(tmp, "shear_ncdm[%d]", i);
        store_title(&cat->pairs, pt->index_tp_shear_ncdm1 + i, -1, tmp, &cat->num);
    }
    for (int i=0; i<ba->N_ncdm; i++) {
        char tmp[40];
        sprintf(tmp, "cs2_ncdm[%d]", i);
        store_title(&cat->pairs, pt->index_tp_cs2_ncdm1 + i, -1, tmp, &cat->num);
    }
    for (int i=0; i<ba->N_ncdm; i++) {
        char tmp[40];
        sprintf(tmp, "l3_ncdm[%d]", i);
        store_title(&cat->pairs, pt->index_tp_l3_ncdm1 + i, -1, tmp, &cat->num);
    }
#endif

    return 0;
}

int cleanClassTitles(struct class_titles *cat) {
    for (int i=0; i<cat->num; i++) {
        free(cat->pairs[i].title);
    }
    free(cat->pairs);

    return 0;
}


/* Compare the user specified titles in pars->DesiredFunctions with
 * the CLASS titles given in the dictionary cat->pairs. */
int matchClassTitles(struct class_titles *cat, struct params *pars) {
    /* Reset indices */
    for (int j=0; j<pars->NumDesiredFunctions; j++) {
        pars->ClassPerturbIndices[j] = -1;
    }

    /* Do the matching */
    int matched = 0;
    int matched_with_ba = 0; //not all functions have a background quantity
    for (int i=0; i<cat->num; i++) {
        for (int j=0; j<pars->NumDesiredFunctions; j++) {
            if (strcmp(cat->pairs[i].title, pars->DesiredFunctions[j]) == 0) {
                pars->ClassPerturbIndices[j] = cat->pairs[i].pt_index;
                pars->ClassBackgroundIndices[j] = cat->pairs[i].ba_index;
                printf("We found a match '%s': class_pt_index = %d, class_ba_index = %d\n", cat->pairs[i].title, cat->pairs[i].pt_index, cat->pairs[i].ba_index);
                matched++;
                if (cat->pairs[i].ba_index >= 0) matched_with_ba ++;
            }
        }
    }

    /* The number of functions that have a match */
    pars->MatchedFunctions = matched;
    pars->MatchedWithBackground = matched_with_ba;

    /* Print warnings for titles that don't have matches */
    for (int j=0; j<pars->NumDesiredFunctions; j++) {
        if (pars->ClassPerturbIndices[j] < 0) {
            printf("WARNING: '%s' (%d) has no matching perturbation index in CLASS.\n", pars->DesiredFunctions[j], j);
        }
    }
    printf("\n");

    return 0;
}
