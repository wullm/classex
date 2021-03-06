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

#include "../include/output.h"
#include "../include/derivatives.h"

int write_perturb(struct perturb_data *data, struct params *pars,
                  struct units *us, char *fname) {
    /* The memory for the transfer functions is located here */
    hid_t h_file, h_grp, h_data, h_err, h_space, h_attr;

    /* Open file */
    h_file = H5Fcreate(fname, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    if (h_file < 0) printf("Error while opening file '%s'.\n", fname);

    printf("Writing the perturbation to '%s'.\n", fname);

    /* Open header to write simulation properties */
    h_grp = H5Gcreate(h_file, "/Header", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    if (h_grp < 0) printf("Error while creating file header\n");

    /* We will write attributes, each consisting of a single scalar number */
    int ndims = 1;
    hsize_t dim[1] = {1};

    /* Create dataspace */
    h_space = H5Screate(H5S_SIMPLE);
    H5Sset_extent_simple(h_space, ndims, dim, NULL);

    /* Write the size of the perturbation (along the k-dimension) */
    h_attr = H5Acreate1(h_grp, "k_size", H5T_NATIVE_INT, h_space, H5P_DEFAULT);
    h_err = H5Awrite(h_attr, H5T_NATIVE_INT, &data->k_size);
    H5Aclose(h_attr);

    /* Write the size of the perturbation (along the tau-direction) */
    h_attr = H5Acreate1(h_grp, "tau_size", H5T_NATIVE_INT, h_space, H5P_DEFAULT);
    h_err = H5Awrite(h_attr, H5T_NATIVE_INT, &data->tau_size);
    H5Aclose(h_attr);

    /* Write the number of transfer functions */
    h_attr = H5Acreate1(h_grp, "n_functions", H5T_NATIVE_INT, h_space, H5P_DEFAULT);
    h_err = H5Awrite(h_attr, H5T_NATIVE_INT, &data->n_functions);
    H5Aclose(h_attr);

    /* Determine the units used */
    double unit_mass_cgs = us->UnitMassKilogram * 1000;
    double unit_length_cgs = us->UnitLengthMetres * 100;
    double unit_time_cgs = us->UnitTimeSeconds;
    double unit_temperature_cgs = us->UnitTemperatureKelvin;

    /* Write the internal unit system */
    h_attr = H5Acreate1(h_grp, "Unit mass in cgs (U_M)", H5T_NATIVE_DOUBLE, h_space, H5P_DEFAULT);
    h_err = H5Awrite(h_attr, H5T_NATIVE_DOUBLE, &unit_mass_cgs);
    H5Aclose(h_attr);

    h_attr = H5Acreate1(h_grp, "Unit length in cgs (U_L)", H5T_NATIVE_DOUBLE, h_space, H5P_DEFAULT);
    h_err = H5Awrite(h_attr, H5T_NATIVE_DOUBLE, &unit_length_cgs);
    H5Aclose(h_attr);

    h_attr = H5Acreate1(h_grp, "Unit time in cgs (U_t)", H5T_NATIVE_DOUBLE, h_space, H5P_DEFAULT);
    h_err = H5Awrite(h_attr, H5T_NATIVE_DOUBLE, &unit_time_cgs);
    H5Aclose(h_attr);

    h_attr = H5Acreate1(h_grp, "Unit temperature in cgs (U_T)", H5T_NATIVE_DOUBLE, h_space, H5P_DEFAULT);
    h_err = H5Awrite(h_attr, H5T_NATIVE_DOUBLE, &unit_temperature_cgs);
    H5Aclose(h_attr);

    /* For strings, we need to prepare a datatype */
    const hid_t h_type = H5Tcopy(H5T_C_S1);
    h_err = H5Tset_size(h_type, strlen(pars->Name)); //length of simname

    /* Create another attribute, which is the name of the simulation */
    h_attr = H5Acreate1(h_grp, "Name", h_type, h_space, H5P_DEFAULT);

    /* Write the name attribute */
    h_err = H5Awrite(h_attr, h_type, pars->Name);

    /* Done with the single entry dataspace */
    H5Sclose(h_space);

    /* Create array of titles of only those functions that are exported. */
    char **output_titles = malloc(data->n_functions * sizeof(char*));

    /* First, add the titles that have a matching index. */
    int j = 0;
    for (int i=0; i<pars->NumDesiredFunctions; i++) {
        if (pars->ClassPerturbIndices[i] >= 0) { //has a matching index
            output_titles[j] = malloc(strlen(pars->DesiredFunctions[i]) + 1);
            strcpy(output_titles[j], pars->DesiredFunctions[i]);
            j++;
        }
    }

    /* Then, add the titles of computed derivatives */
    for (int i=0; i<pars->NumDesiredFunctions; i++) {
        if (isNewDerivativeTitle(pars, pars->DesiredFunctions[i]) >= 0) {
            output_titles[j] = malloc(strlen(pars->DesiredFunctions[i]) + 1);
            strcpy(output_titles[j], pars->DesiredFunctions[i]);
            j++;
        }
    }

    /* Write array of column titles, corresponding to the exported functions */
    hsize_t dim_columns[1] = {data->n_functions};

    /* Create dataspace */
    h_space = H5Screate(H5S_SIMPLE);
    H5Sset_extent_simple(h_space, ndims, dim_columns, NULL);

    /* Set string length to be variable */
    h_err = H5Tset_size(h_type, H5T_VARIABLE); //length of strings

    /* Create another attribute, which is the name of the simulation */
    h_attr = H5Acreate1(h_grp, "FunctionTitles", h_type, h_space, H5P_DEFAULT);

    /* Write the name attribute */
    h_err = H5Awrite(h_attr, h_type, output_titles);


    /* Done with the dataspace */
    H5Sclose(h_space);

    /* Close header */
    H5Gclose(h_grp);

    /* Create the Cosmology group */
    h_grp = H5Gcreate(h_file, "/Cosmology", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    if (h_grp < 0) printf("Error while creating Cosmology group\n");

    /* Create scalar value dataspace */
    h_space = H5Screate(H5S_SIMPLE);
    H5Sset_extent_simple(h_space, ndims, dim, NULL);

    /* Write the CMB temperature */
    h_attr = H5Acreate1(h_grp, "T_CMB (U_T)", H5T_NATIVE_DOUBLE, h_space, H5P_DEFAULT);
    h_err = H5Awrite(h_attr, H5T_NATIVE_DOUBLE, &pars->T_CMB);
    H5Aclose(h_attr);

    /* Write the Hubble parameter in units of 100 km/s/Mpc */
    h_attr = H5Acreate1(h_grp, "h", H5T_NATIVE_DOUBLE, h_space, H5P_DEFAULT);
    h_err = H5Awrite(h_attr, H5T_NATIVE_DOUBLE, &pars->h);
    H5Aclose(h_attr);

    /* Write the present dark energy density as fraction of the critical density */
    h_attr = H5Acreate1(h_grp, "Omega_lambda", H5T_NATIVE_DOUBLE, h_space, H5P_DEFAULT);
    h_err = H5Awrite(h_attr, H5T_NATIVE_DOUBLE, &pars->Omega_lambda);
    H5Aclose(h_attr);

    /* Write the curvature density parameter */
    h_attr = H5Acreate1(h_grp, "Omega_k", H5T_NATIVE_DOUBLE, h_space, H5P_DEFAULT);
    h_err = H5Awrite(h_attr, H5T_NATIVE_DOUBLE, &pars->Omega_k);
    H5Aclose(h_attr);

    /* Write the present energy density of total matter (excluding ncdm) */
    h_attr = H5Acreate1(h_grp, "Omega_m", H5T_NATIVE_DOUBLE, h_space, H5P_DEFAULT);
    h_err = H5Awrite(h_attr, H5T_NATIVE_DOUBLE, &pars->Omega_m);
    H5Aclose(h_attr);

    /* Write the present energy density of baryons */
    h_attr = H5Acreate1(h_grp, "Omega_b", H5T_NATIVE_DOUBLE, h_space, H5P_DEFAULT);
    h_err = H5Awrite(h_attr, H5T_NATIVE_DOUBLE, &pars->Omega_b);
    H5Aclose(h_attr);

    /* Write the present energy density of ultra-relativistic species (excluding photons) */
    h_attr = H5Acreate1(h_grp, "Omega_ur", H5T_NATIVE_DOUBLE, h_space, H5P_DEFAULT);
    h_err = H5Awrite(h_attr, H5T_NATIVE_DOUBLE, &pars->Omega_ur);
    H5Aclose(h_attr);

    /* Write the total number of ncdm species in the cosmology (not all are necessarily exported) */
    h_attr = H5Acreate1(h_grp, "N_ncdm", H5T_NATIVE_INT, h_space, H5P_DEFAULT);
    h_err = H5Awrite(h_attr, H5T_NATIVE_INT, &pars->N_ncdm);
    H5Aclose(h_attr);

    /* If we have at least one neutrino, write some attributes for each neutrino species */
    if (pars->N_ncdm > 0) {
        /* Resize the dataspace to allow for N_ncdm different masses */
        hsize_t dim_ncdm[1] = {pars->N_ncdm};
        H5Sset_extent_simple(h_space, ndims, dim_ncdm, NULL);

        /* Write the mass of each ncdm species in the cosmology */
        h_attr = H5Acreate1(h_grp, "M_ncdm (eV)", H5T_NATIVE_DOUBLE, h_space, H5P_DEFAULT);
        h_err = H5Awrite(h_attr, H5T_NATIVE_DOUBLE, pars->M_ncdm_eV);
        H5Aclose(h_attr);

        /* Write the present temperature of each ncdm species (as fraction of T_CMB) */
        h_attr = H5Acreate1(h_grp, "T_ncdm (T_CMB)", H5T_NATIVE_DOUBLE, h_space, H5P_DEFAULT);
        h_err = H5Awrite(h_attr, H5T_NATIVE_DOUBLE, pars->T_ncdm);
        H5Aclose(h_attr);
    }

    /* Done with the dataspace */
    H5Sclose(h_space);

    /* Close Cosmology group */
    H5Gclose(h_grp);



    /* Open group to write the perturbation arrays */
    h_grp = H5Gcreate(h_file, "/Perturb", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    if (h_grp < 0) printf("Error while creating perturbation group\n");

    /* Create data space */
    h_space = H5Screate(H5S_SIMPLE);
    if (h_space < 0) printf("Error while creating data space.\n");

    /* Set the extent of the data */
    int rank = 1;
    hsize_t shape[1] = {data->k_size};
    h_err = H5Sset_extent_simple(h_space, rank, shape, shape);
    if (h_err < 0) printf("Error while changing data space shape.\n");

    /* Dataset properties */
    const hid_t h_prop = H5Pcreate(H5P_DATASET_CREATE);

    /* Create dataset */
    h_data = H5Dcreate(h_grp, "Wavenumbers", H5T_NATIVE_DOUBLE, h_space,
                       H5P_DEFAULT, h_prop, H5P_DEFAULT);
    if (h_data < 0) printf("Error while creating dataspace '%s'.\n", "Wavenumbers");

    /* Write temporary buffer to HDF5 dataspace */
    h_err = H5Dwrite(h_data, H5T_NATIVE_DOUBLE, h_space, H5S_ALL, H5P_DEFAULT, data->k);
    if (h_err < 0) printf("Error while writing data array '%s'.\n", "data->k");

    /* Close the dataset */
    H5Dclose(h_data);

    /* Set the extent of the tau data */
    rank = 1;
    hsize_t shape_tau[1] = {data->tau_size};
    h_err = H5Sset_extent_simple(h_space, rank, shape_tau, shape_tau);
    if (h_err < 0) printf("Error while changing data space shape.\n");

    /* Create dataset */
    h_data = H5Dcreate(h_grp, "Log conformal times", H5T_NATIVE_DOUBLE,
                       h_space, H5P_DEFAULT, h_prop, H5P_DEFAULT);
    if (h_data < 0)
    printf("Error while creating dataspace '%s'.\n", "Log conformal times");

    /* Write temporary buffer to HDF5 dataspace */
    h_err = H5Dwrite(h_data, H5T_NATIVE_DOUBLE, h_space, H5S_ALL, H5P_DEFAULT, data->log_tau);
    if (h_err < 0) printf("Error while writing data array '%s'.\n", "data->log_tau");

    /* Close the dataset */
    H5Dclose(h_data);

    /* Create dataset with the same extensions */
    h_data = H5Dcreate(h_grp, "Redshifts", H5T_NATIVE_DOUBLE,
                       h_space, H5P_DEFAULT, h_prop, H5P_DEFAULT);
    if (h_data < 0)
    printf("Error while creating dataspace '%s'.\n", "Redshifts");

    /* Write temporary buffer to HDF5 dataspace */
    h_err = H5Dwrite(h_data, H5T_NATIVE_DOUBLE, h_space, H5S_ALL, H5P_DEFAULT, data->redshift);
    if (h_err < 0) printf("Error while writing data array '%s'.\n", "data->redshift");

    /* Close the dataset */
    H5Dclose(h_data);

    /* Create dataset with the same extensions */
    h_data = H5Dcreate(h_grp, "Omega matter", H5T_NATIVE_DOUBLE,
                       h_space, H5P_DEFAULT, h_prop, H5P_DEFAULT);
    if (h_data < 0)
    printf("Error while creating dataspace '%s'.\n", "Omega matter");

    /* Write temporary buffer to HDF5 dataspace */
    h_err = H5Dwrite(h_data, H5T_NATIVE_DOUBLE, h_space, H5S_ALL, H5P_DEFAULT, data->Omega_m);
    if (h_err < 0) printf("Error while writing data array '%s'.\n", "data->Omega_m");

    /* Close the dataset */
    H5Dclose(h_data);

    /* Create dataset with the same extensions */
    h_data = H5Dcreate(h_grp, "Omega radiation", H5T_NATIVE_DOUBLE,
                       h_space, H5P_DEFAULT, h_prop, H5P_DEFAULT);
    if (h_data < 0)
    printf("Error while creating dataspace '%s'.\n", "Omega radiation");

    /* Write temporary buffer to HDF5 dataspace */
    h_err = H5Dwrite(h_data, H5T_NATIVE_DOUBLE, h_space, H5S_ALL, H5P_DEFAULT, data->Omega_r);
    if (h_err < 0) printf("Error while writing data array '%s'.\n", "data->Omega_r");

    /* Close the dataset */
    H5Dclose(h_data);

    /* Create dataset with the same extensions */
    h_data = H5Dcreate(h_grp, "Hubble rates", H5T_NATIVE_DOUBLE,
                       h_space, H5P_DEFAULT, h_prop, H5P_DEFAULT);
    if (h_data < 0)
    printf("Error while creating dataspace '%s'.\n", "Hubble rates");

    /* Write temporary buffer to HDF5 dataspace */
    h_err = H5Dwrite(h_data, H5T_NATIVE_DOUBLE, h_space, H5S_ALL, H5P_DEFAULT, data->Hubble_H);
    if (h_err < 0) printf("Error while writing data array '%s'.\n", "data->Hubble_H");

    /* Close the dataset */
    H5Dclose(h_data);

    /* Create dataset with the same extensions */
    h_data = H5Dcreate(h_grp, "Hubble rate conformal time derivatives", H5T_NATIVE_DOUBLE,
                       h_space, H5P_DEFAULT, h_prop, H5P_DEFAULT);
    if (h_data < 0)
    printf("Error while creating dataspace '%s'.\n", "Hubble rate conformal time derivatives");

    /* Write temporary buffer to HDF5 dataspace */
    h_err = H5Dwrite(h_data, H5T_NATIVE_DOUBLE, h_space, H5S_ALL, H5P_DEFAULT, data->Hubble_H_prime);
    if (h_err < 0) printf("Error while writing data array '%s'.\n", "data->Hubble_H_prime");

    /* Close the dataset */
    H5Dclose(h_data);

    /* Create dataset with the same extensions */
    h_data = H5Dcreate(h_grp, "Growth factors (D)", H5T_NATIVE_DOUBLE,
                       h_space, H5P_DEFAULT, h_prop, H5P_DEFAULT);
    if (h_data < 0)
    printf("Error while creating dataspace '%s'.\n", "Growth factors (D)");

    /* Write temporary buffer to HDF5 dataspace */
    h_err = H5Dwrite(h_data, H5T_NATIVE_DOUBLE, h_space, H5S_ALL, H5P_DEFAULT, data->growth_D);
    if (h_err < 0) printf("Error while writing data array '%s'.\n", "data->growth_D");

    /* Close the dataset */
    H5Dclose(h_data);

    /* Create dataset with the same extensions */
    h_data = H5Dcreate(h_grp, "Logarithmic growth rates (f)", H5T_NATIVE_DOUBLE,
                       h_space, H5P_DEFAULT, h_prop, H5P_DEFAULT);
    if (h_data < 0)
    printf("Error while creating dataspace '%s'.\n", "Logarithmic growth rates (f)");

    /* Write temporary buffer to HDF5 dataspace */
    h_err = H5Dwrite(h_data, H5T_NATIVE_DOUBLE, h_space, H5S_ALL, H5P_DEFAULT, data->growth_f);
    if (h_err < 0) printf("Error while writing data array '%s'.\n", "data->growth_f");

    /* Close the dataset */
    H5Dclose(h_data);

    /* Create dataset with the same extensions */
    h_data = H5Dcreate(h_grp, "Logarithmic growth rate conformal derivatives (f')", H5T_NATIVE_DOUBLE,
                       h_space, H5P_DEFAULT, h_prop, H5P_DEFAULT);
    if (h_data < 0)
    printf("Error while creating dataspace '%s'.\n", "Logarithmic growth rate conformal derivatives (f')");

    /* Write temporary buffer to HDF5 dataspace */
    h_err = H5Dwrite(h_data, H5T_NATIVE_DOUBLE, h_space, H5S_ALL, H5P_DEFAULT, data->growth_f_prime);
    if (h_err < 0) printf("Error while writing data array '%s'.\n", "data->growth_f_prime");

    /* Close the dataset */
    H5Dclose(h_data);

    /* Set the extent of the transfer function data */
    rank = 3;
    hsize_t shape_delta[3] = {data->n_functions, data->tau_size, data->k_size};
    h_err = H5Sset_extent_simple(h_space, rank, shape_delta, shape_delta);
    if (h_err < 0) printf("Error while changing data space shape.");

    /* Create dataset */
    h_data = H5Dcreate(h_grp, "Transfer functions", H5T_NATIVE_DOUBLE, h_space,
                       H5P_DEFAULT, h_prop, H5P_DEFAULT);
    if (h_data < 0)
    printf("Error while creating dataspace '%s'.", "Transfer functions");

    /* Write temporary buffer to HDF5 dataspace */
    h_err = H5Dwrite(h_data, H5T_NATIVE_DOUBLE, h_space, H5S_ALL, H5P_DEFAULT, data->delta);
    if (h_err < 0) printf("Error while writing data array '%s'.", "data->delta");

    /* Close the dataset */
    H5Dclose(h_data);

    /* Set the extent of the background densities data */
    rank = 2;
    hsize_t shape_Omega[2] = {data->n_functions, data->tau_size};
    h_err = H5Sset_extent_simple(h_space, rank, shape_Omega, shape_Omega);
    if (h_err < 0) printf("Error while changing data space shape.");

    /* Create dataset */
    h_data = H5Dcreate(h_grp, "Omegas", H5T_NATIVE_DOUBLE, h_space,
                       H5P_DEFAULT, h_prop, H5P_DEFAULT);
    if (h_data < 0)
    printf("Error while creating dataspace '%s'.", "Omegas");

    /* Write temporary buffer to HDF5 dataspace */
    h_err = H5Dwrite(h_data, H5T_NATIVE_DOUBLE, h_space, H5S_ALL, H5P_DEFAULT, data->Omega);
    if (h_err < 0) printf("Error while writing data array '%s'.", "data->Omega");

    /* Close the dataset */
    H5Dclose(h_data);

    /* Close the properties */
    H5Pclose(h_prop);

    /* Close the group */
    H5Gclose(h_grp);

    /* Close file */
    H5Fclose(h_file);

    return 0;
}
