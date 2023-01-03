#include "config.h"

void* CConfig::GetItem(const char* Name)
{

	for (const auto& Item : Items)
	{

		if (!strcmp(Item.Name, Name))
		{

			return Item.Value;

		}

	}

	WRAP_IF_DEBUG(
		std::cout << "Failed to get config item with name (" << Name << ")!\n";
	)

	return nullptr;

}

bool CConfig::InitializeConfig()
{

	return true;

}