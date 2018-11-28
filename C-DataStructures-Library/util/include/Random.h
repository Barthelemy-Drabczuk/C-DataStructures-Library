/**
 * @file Random.h
 * 
 * @author Leonardo Vencovsky (https://github.com/LeoVen)
 *
 * @date 07/11/2018
 */

#ifndef C_DATASTRUCTURES_LIBRARY_RANDOM_H
#define C_DATASTRUCTURES_LIBRARY_RANDOM_H

#include "Core.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Generates a random number between 0 and 1.
double rrandom();

/// Generates an integer between min and max with both included.
int rand_int(index_t min, index_t max);

/// Generates a long integer between min and max with both included.
long long rand_long(index_t min, index_t max);

/// Generates a double between min and max.
double rand_double(double min, double max);

/// Generates a float between min and max.
float rand_float(float min, float max);

#ifdef __cplusplus
}
#endif

#endif //C_DATASTRUCTURES_LIBRARY_RANDOM_H
