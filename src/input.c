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
#include "../include/input.h"

int readParams(struct params *pars, const char *fname) {
    /* Read strings */
    int len = DEFAULT_STRING_LENGTH;
    pars->OutputDirectory = malloc(len);
    pars->OutputFilename = malloc(len);
    pars->Name = malloc(len);
    pars->ClassIniFile = malloc(len);
    pars->ClassPreFile = malloc(len);
    ini_gets("Output", "Directory", "./output", pars->OutputDirectory, len, fname);
    ini_gets("Output", "Filename", "perturb.hdf5", pars->OutputFilename, len, fname);
    ini_gets("Simulation", "Name", "No Name", pars->Name, len, fname);
    ini_gets("Input", "ClassIniFile", "", pars->ClassIniFile, len, fname);
    ini_gets("Input", "ClassPreFile", "", pars->ClassPreFile, len, fname);

    char *listStr = malloc(1000);
    ini_gets("Output", "Functions", "", listStr, 1000, fname);
    listStr = realloc(listStr, strlen(listStr) + 1);

    /* Parse the list of desired transfer functions */
    int num = 0, i = 0;
    int read = 0, bytes;
    char str[40];

    /* First count the number of entries */
    while(sscanf(listStr + read, "%[^,],%n", str, &bytes) > 0) {
        if (listStr[read + bytes] == '\0') break; /* reached the end */
        read += bytes;
        num += 1;
    }

    /* Allocate memory for the array of the titles */
    pars->NumDesiredFunctions = num;
    pars->DesiredFunctions = malloc(num * sizeof(char*));
    pars->IndexOfFunctions = malloc(num * sizeof(int));

    /* Rewind and actually read out the titles */
    read = 0;
    i = 0;
    while(sscanf(listStr + read, "%[^,],%n", str, &bytes) > 0) {
        if (listStr[read + bytes] == '\0') break; /* reached the end */

        /* Strip white space */
        char title[40];
        sscanf(str, "%s", title);

        /* Store the title */
        pars->DesiredFunctions[i] = malloc(strlen(title) + 1);
        strcpy(pars->DesiredFunctions[i], title);
        read += bytes;
        i++;
    }

    free(listStr);

    return 0;
}

int readUnits(struct units *us, const char *fname) {
    /* Internal units */
    us->UnitLengthMetres = ini_getd("Units", "UnitLengthMetres", 1.0, fname);
    us->UnitTimeSeconds = ini_getd("Units", "UnitTimeSeconds", 1.0, fname);
    us->UnitMassKilogram = ini_getd("Units", "UnitMassKilogram", 1.0, fname);

    /* Physical constants */
    us->SpeedOfLight = SPEED_OF_LIGHT_METRES_SECONDS * us->UnitTimeSeconds
                        / us->UnitLengthMetres;

    return 0;
}

int cleanParams(struct params *parser) {
    for (int i=0; i<parser->NumDesiredFunctions; i++) {
        free(parser->DesiredFunctions[i]);
    }
    free(parser->DesiredFunctions);
    free(parser->IndexOfFunctions);
    free(parser->OutputDirectory);
    free(parser->OutputFilename);
    free(parser->Name);
    free(parser->ClassIniFile);
    free(parser->ClassPreFile);

    return 0;
}
