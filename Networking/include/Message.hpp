#pragma once
#include <cstdint>
#include <vector>

namespace net
{
	class Message
	{
	public:
		struct Header
		{
			uint32_t id;
			uint32_t bodySize;
		};
		Header header;
		std::vector<uint8_t> dataBuffer;

		Message(Header header)
			: header(std::move(header))
		{
			allocateAndInitializeBuffer();
		}

		Message(uint32_t id, void* data, uint32_t dataSize)
		{
			header.id = id;
			header.bodySize = dataSize;
			allocateAndInitializeBuffer();
			memcpy_s(getBodyStart(), header.bodySize, data, dataSize);
		}

		void* getMessageStart() 
		{
			return dataBuffer.data();
		}

		void* getBodyStart() 
		{
			return dataBuffer.data() + headerSize();
		}

		constexpr static uint32_t headerSize()
		{
			return sizeof(Header);
		}

		uint32_t bodySize() const
		{
			return static_cast<uint32_t>(header.bodySize);
		}

		uint32_t messageSize() const
		{
			return headerSize() + bodySize();
		}

	private:
		void allocateAndInitializeBuffer()
		{
			dataBuffer = std::vector<uint8_t>(header.bodySize + headerSize(), 0);
			memcpy_s(dataBuffer.data(), headerSize(), static_cast<void*>(&header), headerSize());
		}
	};
}
