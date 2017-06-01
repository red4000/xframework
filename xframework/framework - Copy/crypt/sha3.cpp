#include <string>
#include "sha3.h"
#include "KeccakF-1600-interface.h"

std::string SHA3(std::string str)
{
	unsigned int state[1391];

	memset(state, 0, sizeof(state));
	strcpy_s((char*)&state[0], sizeof(state), str.c_str());
	KeccakInitialize();
	KeccakPermutation((unsigned char*)state);
	KeccakPermutation((unsigned char*)state);

	std::string res("");

	for (size_t i = 0; i < (64 - 15); i++)
	{
		char tmp[16];

		sprintf_s(tmp, sizeof(tmp), "%02X", state[i] & 0xFF);
		res += tmp;
	}

	return res;
}
