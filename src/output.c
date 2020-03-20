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

int write_perturb(struct perturb_data *data, struct params *pars,
                  struct units *us, char *fname) {
    /* The memory for the transfer functions is located here */
    hid_t h_file, h_grp, h_data, h_err;

    /* Open file */
    h_file = H5Fcreate(fname, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    if (h_file < 0) printf("Error while opening file '%s'.\n", fname);

    printf("Writing the perturbation to '%s'.\n", fname);

    /* Open header to write simulation properties */
    h_grp = H5Gcreate(h_file, "/Header", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    if (h_grp < 0) printf("Error while creating file header\n");

      // /* Print the relevant information and print status */
      // io_write_attribute(h_grp, "k_size", INT, &data->k_size, 1); //H5T_NATIVE_INT
      // io_write_attribute(h_grp, "tau_size", INT, &data->tau_size, 1);
      // io_write_attribute(h_grp, "n_functions", INT, &data->n_functions, 1);
      //
      // /* Write unit system for this data set */
      // io_write_attribute_d(h_grp, "Unit mass in cgs (U_M)",
      //                      units_get_base_unit(us, UNIT_MASS));
      // io_write_attribute_d(h_grp, "Unit length in cgs (U_L)",
      //                      units_get_base_unit(us, UNIT_LENGTH));
      // io_write_attribute_d(h_grp, "Unit time in cgs (U_t)",
      //                      units_get_base_unit(us, UNIT_TIME));
      // io_write_attribute_d(h_grp, "Unit current in cgs (U_I)",
      //                      units_get_base_unit(us, UNIT_CURRENT));
      // io_write_attribute_d(h_grp, "Unit temperature in cgs (U_T)",
      //                      units_get_base_unit(us, UNIT_TEMPERATURE));

    /* Close header */
    H5Gclose(h_grp);

    /* Open group to write the perturbation arrays */
    h_grp = H5Gcreate(h_file, "/Perturb", H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    if (h_grp < 0) printf("Error while creating perturbation group\n");

    /* Create data space */
    const hid_t h_space = H5Screate(H5S_SIMPLE);
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

    /* Set the extent of the transfer function data */
    rank = 3;
    hsize_t shape_delta[3] = {data->n_functions, data->k_size, data->tau_size};
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

    /* Close the properties */
    H5Pclose(h_prop);

    /* Close the group */
    H5Gclose(h_grp);

    /* Close file */
    H5Fclose(h_file);

    return 0;
}
