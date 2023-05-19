#pragma once
#include <cstdint>
#include <vector>

namespace net
{
	struct Message
	{
		struct Header
		{
			uint32_t id;
			uint32_t bodySize;
		};
		Header header;
		std::vector<uint8_t> body;

		Message(uint32_t id, void* obj, uint32_t size)
		{
			header.id = id;
			header.bodySize = size;
			body = std::vector<uint8_t>(size + headerSize(), 0);
			memcpy_s(body.data(), headerSize(), static_cast<void*>(&header), headerSize());
			memcpy_s((body.data() + headerSize()), header.bodySize, obj, size);
		}

		constexpr static uint32_t headerSize()
		{
			return sizeof(Header);
		}

		uint32_t bodySize() const
		{
			return static_cast<uint32_t>(body.size());
		}

		uint32_t messageSize() const
		{
			return headerSize() + bodySize();
		}
	};
}
