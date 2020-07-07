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

#ifndef CLASS_TITLES_H
#define CLASS_TITLES_H

#include <class.h>
#include "input.h"

#define MAX_NUM_FUNCTIONS 100

struct index_title {
    int pt_index; //CLASS perturbation index (always used)
    int ba_index; //CLASS background index (sometimes used)
    char *title;
};

struct class_titles {
    int num;
    struct index_title *pairs;
};

/* Adds index to indices, adds title to titles, increments the counter by one. */
static inline void store_title(struct index_title **pairs, int pt_index, int ba_index, char *title, int *i) {
    (*pairs + *i)->pt_index = pt_index;
    (*pairs + *i)->ba_index = ba_index;
    (*pairs + *i)->title = malloc(strlen(title) + 1);
    strcpy((*pairs + *i)->title, title);
    *i  = *i + 1;
}

int initClassTitles(struct class_titles *cat, struct perturbs *pt, struct background *ba);
int cleanClassTitles(struct class_titles *cat);
int matchClassTitles(struct class_titles *cat, struct params *pars);

#endif
