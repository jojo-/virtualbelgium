/****************************************************************
 * RANDOM.CPP
 *
 * This file contains all the definitions of the methods of
 * Random.hpp (see this file for methods' documentation)
 *
 * Authors: J. Barthelemy and L. Hollaert
 * Date   : 17 july 2012
 ****************************************************************/

#include "../include/Random.hpp"

float norm_rand(float mu, float sigma) {

  float x1, x2, w;

  do {
    x1 = 2.0 * rand() / (float) RAND_MAX - 1.0;
    x2 = 2.0 * rand() / (float) RAND_MAX - 1.0;
    w = x1 * x1 + x2 * x2;
  } while ( w >= 1.0 || w < 0.00001 );

  w = sqrt((-2.0 * log(w)) / w);
  x1 = mu + sigma * (x1 * w);

  return x1;

}

float norm_rand(float mu, float sigma, float max) {

  float result = norm_rand(mu, sigma);

  while ( result > max ) {
	  result = norm_rand(mu, sigma);
  }

  return result;

}

float log_rand(float mu, float sigma) {

  return exp(norm_rand(mu, sigma));

}

float log_rand(float mu, float sigma, float max) {

  float result = log_rand(mu, sigma);

  while ( result > max ) {
	  result = log_rand(mu, sigma);
  }

  return result;

}
