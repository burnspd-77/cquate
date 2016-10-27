
#pragma once

#include "matrix.h"
#include <complex>
#include <stdint.h>

using std::complex;
class cMatrix;

//typedef signed __int32 int32_t;
//typedef unsigned __int32 uint32_t;
//typedef signed __int64 int64_t;
//typedef unsigned __int64 uint64_t;
//typedef unsigned int uint;

// Random number generator

class RandGen
{
protected:
	int32_t _seed;
public:
	RandGen(int32_t s = 0) {
		SetSeed(s);
	}
	virtual ~RandGen() {
	}
	virtual double Next() {
		return NextInt() / RAND_MAX;
	}
	virtual uint32_t NextInt() {
		return rand();
	}
	virtual uint64_t NextInt64() {
		// Same procedure as GenSeed
		// Gen upper 32 as 32, shift left 32, gen lower 32
		uint64_t r = NextInt();
		r <<= 32;
		r |= NextInt();
		return r;
	}
	virtual int32_t NextInt(uint32_t min, uint32_t max) {
		int32_t r = 0;

		return r;
	}
	virtual void SetSeed(uint32_t s = 0) {
		if (s == 0)
			_seed = RandGen::GenSeed();
		else
			_seed = s;
	}
	virtual uint32_t GetSeed() {
		return _seed;
	}

	static uint32_t GenSeed() {
		return rand();
	}
	static uint64_t GenSeed64() {
		// What will be upper 32 bits
		int64_t seed = rand();
		// Shift to upper 32 bits
		seed <<= 32;
		// Generate lower 32 bits
		seed |= rand();
		return seed;
	}

	static const uint32_t MaxUInt = 4294967295U; //2^32
	static const int32_t MaxInt = 2147483647; //2^31
	static const int64_t MaxInt64 = 9223372036854775807; //2^63
};

class MersenneTwister
{
protected:
	// Period parameters
	static const uint N = 624;
	static const uint M = 397;
	// constant vector a
	static const uint MATRIX_A = 0x9908b0dfUL;
	// most significant w-r bits
	static const uint UPPER_MASK = 0x80000000UL;
	// least significant r bits
	static const uint LOWER_MASK = 0x7fffffffUL;

	uint32_t _seed;
	uint mt[N];
	uint mti;

public:
	static const uint32_t MaxUInt = 4294967295U; //2^32
	static const int32_t MaxInt = 2147483647; //2^31
	static const int64_t MaxInt64 = 9223372036854775807; //2^63

public:		// interface

	MersenneTwister() { _seed = GenSeed(); init(); }
	MersenneTwister(int32_t s) { _seed = s; init(); }

	void init() {
	}
	uint32_t GenSeed() { return (unsigned int)rand(); }
	uint64_t GenSeed64()
	{
		// What will be upper 32 bits
		uint64_t seed = rand();
		// Shift to upper 32 bits
		seed <<= 32;
		// Generate lower 32 bits
		seed |= rand();
		return seed;
	}
	void SetSeed(uint32_t s) {
		s = (_seed == 0) ? time(NULL) : _seed << 1;

		_seed = s >> 1;

		mt[0] = s & 0xffffffffUL;
		for (mti = 1; mti < N; mti++) {
			mt[mti] = (1812433253UL * (mt[mti - 1] ^ (mt[mti - 1] >> 30)) + mti);
			mt[mti] &= 0xffffffffUL;
		}
	}
	uint32_t GetSeed() { return _seed; }

	/* generates a random number on [0,1)-real-interval */
	double Next32() { return double(NextInt()) / 4294967296.0; }

	/* generates a random number on [0,1) with 53-bit resolution*/
	double Next53(void)
	{
		unsigned long a=NextInt()>>5, b=NextInt()>>6; 
		return (a*67108864.0+b)*(1.0/9007199254740992.0); 
	}

	uint32_t NextInt() {
		uint32_t y;
		static uint32_t mag01[2] = { 0x0UL, MATRIX_A };

		if (mti >= N) { /* generate N words at one time */
			uint32_t kk;

			for (kk = 0; kk < N - M; kk++) {
				y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
				mt[kk] = mt[kk + M] ^ (y >> 1) ^ mag01[y & 0x1UL];
			}
			for (; kk < N - 1; kk++) {
				y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
				mt[kk] = mt[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
			}
			y = (mt[N - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
			mt[N - 1] = mt[M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];

			mti = 0;
		}

		y = mt[mti++];

		/* Tempering */
		y ^= (y >> 11);
		y ^= (y << 7) & 0x9d2c5680UL;
		y ^= (y << 15) & 0xefc60000UL;
		y ^= (y >> 18);

		return y;
	}
};

