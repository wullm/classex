/*******************************************************************************
 * This file is part of Classex.
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

#ifndef CLASSEX_H
#define CLASSEX_H

#include <stdio.h>

#include "input.h"
#include "class_titles.h"
#include "class_transfer.h"
#include "output.h"
#include "derivatives.h"

#define TXT_RED "\033[31;1m"
#define TXT_GREEN "\033[32;1m"
#define TXT_RESET "\033[0m"

static inline void error(char *errmsg) {
    printf("%s\n", errmsg);
    exit(1);
}

#endif
