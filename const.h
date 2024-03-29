#ifndef _CONST_H_
#define _CONST_H_

#include <vector>


#define INF_N -std::numeric_limits<double>::max()
#define INF_P std::numeric_limits<double>::max()

typedef double Coord;
typedef std::vector<double> Coord_V;
typedef std::vector<Coord_V> Coord_VV;

const size_t DIM = 2;
const double EPS = 1e-9;

const double MAXDIST = 1.1;
const int MAXKWNUM = 1;

const int KNUM = 10;
const double KMAXCOF = 3;
const double ALPHA = 0.5;
const int QRATIO = 10;

const int DATASIZE = 10000;


#endif /*_CONST_H_*/
