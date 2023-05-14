#pragma once
#include <cstdint>

template <typename T>
struct Message 
{
	uint32_t headerId;
	size_t bodySize;
	T body;

	size_t headerSize() const
	{
		return sizeof(headerId) + sizeof(totalSize);
	}

	size_t totalSize() const 
	{
		return headerSize() + bodySize();
	}
};