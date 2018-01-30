#ifndef HPP_MATH_INCLUDE
#define HPP_MATH_INCLUDE

#include <random> //For random generation
#include <iomanip> //For setprecision()

float sqrt_approx (float);

float eD_approx (float x1, float y1, float x2, float y2);

uint pi (uint32_t seed, int32_t min, int32_t max);

float pf (uint seed);
bool rb (float dist = 0.5f);
double rf (double min, double max);

int ri (int min, int max);

double rf_nd (double average, double spread);
float normaliseAng (float ang);

float vecToAng (float dir_X, float dir_Y);

void angToVec (float rot, float &dir_X, float &dir_Y);

void targToVec (double ourX, double ourY, double tarX, double tarY, double &dirX, double &dirY);

void random_coord (uint16_t W, uint16_t H, uint16_t &x, uint16_t &y);

float decimal (float num);

std::string to_string_with_precision (double val, int n = 2);

#endif
