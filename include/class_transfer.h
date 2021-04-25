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

#ifndef CLASS_TRANSFER_H
#define CLASS_TRANSFER_H

#include <class.h>

#include "input.h"

struct perturb_data {
  int k_size;
  int tau_size;
  int n_functions;
  double *delta;
  double *k;
  double *log_tau;
  double *redshift;
  double *Omega;
  double *Omega_m;
  double *Omega_r;
  double *Hubble_H;
  double *Hubble_H_prime;
  double *growth_D;
  double *growth_f;
  double *growth_f_prime;
  // char **titles;
};

int readPerturbData(struct perturb_data *data, struct params *pars,
                    struct units *us, struct perturbations *pt, struct background *ba);
int cleanPerturbData(struct perturb_data *data);
double unitConversionFactor(char *title, double unit_length_factor,
                            double unit_time_factor);

#endif
