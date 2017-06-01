#include <windows.h>
#include "dhash.h"

void DHash(BYTE *data, size_t datalen, BYTE *state, size_t statelen, size_t rounds) {
	size_t i = 0;

	for (size_t si = 0; si < statelen; si++) {
		state[si] ^= data[i++];

		if (i >= datalen) {
			i = 0;
		}
	}

	DPermutate(state, statelen, rounds);
}

void DPermutate(BYTE *state, size_t statelen, size_t rounds) {
	for (size_t i = 0; i < statelen; i++) {
		for (size_t k = 0; k < rounds; k++) {
			BYTE z = 0;

			for (size_t j = 0; j < statelen; j++) {
				z ^= (state[i] << 16) ^ (state[j] << 8) ^ (i + j + k + statelen + z) ^ 0x7F;
				z = (z << 24) ^ z;
				z = state[z % statelen] ^ z;
			}

			state[i] ^= z;
		}
	}
}
