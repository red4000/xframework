#ifndef MT19937_H
#define MT19937_H

#define N          624
#define M          397
#define MATRIX_A   0x9908b0dfUL
#define UPPER_MASK 0x80000000UL
#define LOWER_MASK 0x7fffffffUL

class MT19937 {
public:
	MT19937();

	void init_genrand(unsigned long s);
	void init_by_array(unsigned long *init_key, int key_length);
	unsigned long genrand_int32(void);
	double genrand_real1(void);

	unsigned long mt[N];
	int           mti;
};

extern MT19937 mainPrng;

#endif
