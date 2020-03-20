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

#ifndef titles_H
#define titles_H

#include <class.h>
#include "input.h"

#define MAX_NUM_FUNCTIONS 100

struct class_titles {
    int num;
    int* indices;
    char **titles;
};

static inline void store_title(int **indices, char ***titles, int index, char *title, int *i) {
    (*indices)[*i] = index;
    (*titles)[*i] = malloc(strlen(title) + 1);
    strcpy((*titles)[*i], title);
    *i  = *i + 1;
}

int initClassTitles(struct class_titles *cat, struct perturbs *pt, struct background *ba);
int cleanClassTitles(struct class_titles *cat);
int matchClassTitles(struct class_titles *cat, struct params *pars);

#endif
