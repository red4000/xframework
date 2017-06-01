#ifndef DHASH
#define DHASH

void DHash(BYTE *data, size_t datalen, BYTE *state, size_t statelen, size_t rounds = 47);
void DPermutate(BYTE *state, size_t statelen, size_t rounds);

#endif
