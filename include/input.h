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

#ifndef INPUT_H
#define INPUT_H

#define DEFAULT_STRING_LENGTH 50

#define MPC_METRES 3.085677581282e22
#define SPEED_OF_LIGHT_METRES_SECONDS 2.99792e8

/* The .ini parser library is minIni */
#include "../parser/minIni.h"

struct params {
    /* Simulation parameters */
    char *Name;
    char *BackgroundFile;
    char *BackgroundFormat;

    /* CLASS parameter file names */
    char *ClassIniFile;
    char *ClassPreFile;

    /* Output parameters */
    char *OutputFilename;
    char **DesiredFunctions; //titles of columns that need to be exported
    int *IndexOfFunctions; //the corresponding CLASS indices
    int NumDesiredFunctions; //the number of requested functions
    int MatchedFunctions; //the number of functions with data
};

struct units {
    double UnitLengthMetres;
    double UnitTimeSeconds;
    double UnitMassKilogram;

    /* Physical constants in internal units */
    double SpeedOfLight;

    /* Units for the transfer function input data */
    double BackgroundUnitLengthMetres;
};

int readParams(struct params *parser, const char *fname);
int readUnits(struct units *us, const char *fname);

int cleanParams(struct params *parser);

#endif
