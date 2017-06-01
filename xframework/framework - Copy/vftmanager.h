#ifndef VFTMANAGER_H
#define VFTMANAGER_H

size_t CountVFTFunctions(void **vft);

class VFTManager {
public:
	void Initialize(void ***inst);
	void Destroy();
	void Hook();
	void Unhook();
	void HookFunction(int i, void *to);
	void UnhookFunction(int i);
	bool IsHooked();

	void ***instance, **newVFT, **oldVFT;
	int     numFuncs;
};

#endif
