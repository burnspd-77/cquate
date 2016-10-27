
#include "random.h"
#include <ctime>

static MersenneTwister twister;

void Seed(unsigned int iSeed)
{
	srand( iSeed );
}

void TimeSeed(void)
{
	srand( (unsigned)time(NULL) );
}

void MersenneSeed(unsigned int iSeed)	{ twister.SetSeed(iSeed); }
void MersenneSeed(double dSeed)
{
	unsigned int iSeed;
	if (dSeed<0.)
		iSeed = 0;
	else if (dSeed>twister.MaxInt)
		iSeed = twister.MaxInt;
	else
		iSeed = (unsigned int) dSeed;

	twister.SetSeed(iSeed);
}

double DRand(void)
{ return twister.Next53(); }

double Rand32(void)
{ return twister.Next32(); }

complex<double> CRand(void)
{
	double a = DRand();
	double b = DRand();
	return complex<double>(a,b);
}

bool MatRandR(cMatrix& Result, int r, int c)
{
	if (Result.Create(r,c)) return true;

	double *pr = Result.GetR();
	int n = r*c;
	for (int i=0; i<n; i++) pr[i] = DRand();

	return false;
}

bool MatRandC(cMatrix& Result, int r, int c)
{
	if (Result.CreateComplex(r,c)) return true;

	double *pr = Result.GetR();
	double *pi = Result.GetI();
	int n = r*c;
	for (int i=0; i<n; i++) {
		pr[i] = DRand();
		pi[i] = DRand();
	}

	return false;
}
