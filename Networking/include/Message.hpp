#pragma once
#include <cstdint>

template <typename T>
struct Message
{
	struct Header
	{
		uint32_t headerId;
		uint32_t bodySize;
	};
	T body;

	size_t headerSize() constexpr
	{
		return sizeof(headerId) + sizeof(totalSize);
	}

	size_t totalSize() const
	{
		return headerSize() + bodySize();
	}
};
