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

    /* Compute derivatives */
    assert(computeDerivatives(&data, &pars, &us) == 0);

    printf("We have read out %d functions.\n", data.n_functions);

    /* Write it to a file */
    assert(write_perturb(&data, &pars, &us, pars.OutputFilename) == 0);

    /* Test reading the file */
    hid_t h_file, h_grp, h_data, h_err, h_attr;

    /* New perturb_data structure, with data loaded from the file */
    struct perturb_data read_data;

    /* Open file */
    h_file = H5Fopen(pars.OutputFilename, H5F_ACC_RDONLY, H5P_DEFAULT);
    assert(h_file >= 0);

    printf("Reading the perturbation from '%s'.\n", pars.OutputFilename);

    /* Open header to read simulation properties */
    h_grp = H5Gopen(h_file, "/Header", H5P_DEFAULT);
    assert (h_grp >= 0);

    /* Open the attributes describe the number of wavenumbers */
    h_attr = H5Aopen(h_grp, "k_size", H5P_DEFAULT);
    assert(h_attr >= 0);
    h_err = H5Aread(h_attr, H5T_NATIVE_INT, &read_data.k_size);
    assert(h_err >= 0);

    /* Open the attributes describe the number of conformal time steps */
    h_attr = H5Aopen(h_grp, "tau_size", H5P_DEFAULT);
    assert(h_attr >= 0);
    h_err = H5Aread(h_attr, H5T_NATIVE_INT, &read_data.tau_size);
    assert(h_err >= 0);

    /* Open the attributes describe the number of transfer functions */
    h_attr = H5Aopen(h_grp, "n_functions", H5P_DEFAULT);
    assert(h_attr >= 0);
    h_err = H5Aread(h_attr, H5T_NATIVE_INT, &read_data.n_functions);
    assert(h_err >= 0);

    /* Does it make sense? */
    assert(read_data.k_size == data.k_size);
    assert(read_data.tau_size == data.tau_size);
    assert(read_data.n_functions == data.n_functions);

    double UnitLengthCGS, UnitTimeCGS, UnitMassCGS;

    /* Open the attributes describe the mass units used */
    h_attr = H5Aopen(h_grp, "Unit mass in cgs (U_M)", H5P_DEFAULT);
    assert(h_attr >= 0);
    h_err = H5Aread(h_attr, H5T_NATIVE_DOUBLE, &UnitMassCGS);
    assert(h_err >= 0);

    /* Open the attributes describe the length units used */
    h_attr = H5Aopen(h_grp, "Unit length in cgs (U_L)", H5P_DEFAULT);
    assert(h_attr >= 0);
    h_err = H5Aread(h_attr, H5T_NATIVE_DOUBLE, &UnitLengthCGS);
    assert(h_err >= 0);

    /* Open the attributes describe the time units used */
    h_attr = H5Aopen(h_grp, "Unit time in cgs (U_t)", H5P_DEFAULT);
    assert(h_attr >= 0);
    h_err = H5Aread(h_attr, H5T_NATIVE_DOUBLE, &UnitTimeCGS);
    assert(h_err >= 0);

    /* Does it make sense? */
    assert(UnitLengthCGS == us.UnitLengthMetres * 100);
    assert(UnitTimeCGS == us.UnitTimeSeconds);
    assert(UnitMassCGS == us.UnitMassKilogram * 1000);

    /* Open the string attribute */
    h_attr = H5Aopen(h_grp, "Name", H5P_DEFAULT);
    assert(h_attr >= 0);

    /* Get the datatype of the string */
    hid_t h_tp = H5Aget_type(h_attr);
    assert(h_tp >= 0);

    /* Read out the string */
    char str[50];
    h_err = H5Aread(h_attr, h_tp, str);

    printf("The name stored was \"%s\".\n", str);
    assert(strcmp(str, pars.Name) == 0);

    /* Close the attribute */
    H5Aclose(h_attr);


    /* Read out the function titles */
    h_attr = H5Aopen(h_grp, "FunctionTitles", H5P_DEFAULT);
    assert(h_attr >= 0);

    /* Get the datatype of the string */
    h_tp = H5Aget_type(h_attr);
    assert(h_tp >= 0);

    /* Get the dimensions of the attribute */
    h_data = H5Aget_space(h_attr);
    hsize_t dims[1];
    int ndims = H5Sget_simple_extent_dims(h_data, dims, NULL);
    char **read_titles = malloc (dims[0] * sizeof (char*));

    printf("We found %lld (%d) titles\n", dims[0], ndims);
    assert(ndims == 1);
    assert(dims[0] == pars.MatchedFunctions);

    /* Read the titles */
    h_err = H5Aread(h_attr, h_tp, read_titles);
    assert(h_err >= 0);

    /* Check that the titles are as expected */
    int j=0;
    for (int i=0; i<pars.NumDesiredFunctions; i++) {
        /* Ignore unknown functions (without matching CLASS index),
         * which should not be in the file, except for derivatives */
        if (pars.ClassPerturbIndices[i] >= 0) {
            assert(strcmp(pars.DesiredFunctions[i], read_titles[j]) == 0);
            printf("Checked '%s'.\n", read_titles[j]);
            j++;
        }
    }

    /* Free space */
    H5Dvlen_reclaim (h_tp, h_data, H5P_DEFAULT, read_titles);
    free(read_titles);

    /* Close the attribute again */
    H5Aclose(h_attr);
    H5Tclose(h_tp);
    H5Sclose(h_data);

    /* Close header */
    H5Gclose(h_grp);

    /* Allocate memory */
    read_data.k = calloc(read_data.k_size, sizeof(double));
    read_data.log_tau = calloc(read_data.tau_size, sizeof(double));
    read_data.delta = malloc(read_data.n_functions * read_data.k_size
                                * read_data.tau_size * sizeof(double));

    /* Allocation successful? */
    assert(read_data.k != NULL);
    assert(read_data.log_tau != NULL);
    assert(read_data.delta != NULL);

    /* Open the perturbation data group */
    h_grp = H5Gopen(h_file, "/Perturb", H5P_DEFAULT);
    assert(h_grp >= 0);

    /* Read the wavenumbers */
    h_data = H5Dopen2(h_grp, "Wavenumbers", H5P_DEFAULT);
    assert(h_data >= 0);
    h_err = H5Dread(h_data, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, read_data.k);
    H5Dclose(h_data);

    /* Read the conformal times */
    h_data = H5Dopen2(h_grp, "Log conformal times", H5P_DEFAULT);
    assert(h_data >= 0);
    h_err = H5Dread(h_data, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, read_data.log_tau);
    H5Dclose(h_data);

    /* Read the transfer functions */
    h_data = H5Dopen2(h_grp, "Transfer functions", H5P_DEFAULT);
    assert(h_data >= 0);
    h_err = H5Dread(h_data, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, read_data.delta);
    H5Dclose(h_data);

    /* Check that data matches */
    for (int i=0; i<read_data.k_size; i++) {
        assert(read_data.k[i] == data.k[i]);
    }
    for (int i=0; i<read_data.tau_size; i++) {
        assert(read_data.log_tau[i] == data.log_tau[i]);
    }
    for (int i=0; i<read_data.k_size * read_data.tau_size * read_data.n_functions; i++) {
        assert(read_data.delta[i] == data.delta[i]);
    }

    /* Close the perturbation group */
    H5Gclose(h_grp);

    /* Close file */
    H5Fclose(h_file);

    /* Clean perturb data (both the generated and the read data)*/
    assert(cleanPerturbData(&data) == 0);
    assert(cleanPerturbData(&read_data) == 0);

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

    sucmsg("test_output:\t SUCCESS");
}
