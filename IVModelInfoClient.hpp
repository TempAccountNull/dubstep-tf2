#pragma once
#include "SourceHelpers.hpp"

class IVModelInfoClient
{
public:
	struct studiohdr_t* GetStudioModel(struct model_t* model)
	{
		using OriginalFn = studiohdr_t*(*)(void*, model_t*);
		return GetVFunc<OriginalFn>(this, 28)(this, model);
	}
};

inline IVModelInfoClient* VModelInfoClient;