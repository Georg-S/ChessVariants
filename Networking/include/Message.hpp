#pragma once
#include <cstdint>
#include <vector>

namespace net
{
	enum DestinationID
	{
		SERVER = 0,
		BROADCAST = 1,
	};

	class Message
	{
	public:
		struct Header
		{
			uint32_t toID;
			uint32_t messageType;
			uint32_t bodySize;
		};
		Header header = {};
		std::vector<uint8_t> dataBuffer;

		Message() = default; // Resize should be called once the header is set
		Message(Header header)
			: header(std::move(header))
		{
			allocateAndInitializeBuffer();
		}

		Message(uint32_t messageType, uint32_t destinationID, void* data, uint32_t dataSize)
		{
			header.messageType = messageType;
			header.toID = destinationID;
			header.bodySize = dataSize;
			allocateAndInitializeBuffer();
			memcpy_s(getBodyStart(), header.bodySize, data, dataSize);
		}

		// Resizes the buffer according to the headersize and the buffersize specified in the header
		// Zeroes out the complete body memory
		void resize() 
		{
			allocateAndInitializeBuffer();
		}

		void setToID(uint32_t toID) 
		{
			header.toID = toID;
			copyHeaderIntoDataBuffer();
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
		void copyHeaderIntoDataBuffer() 
		{
			memcpy_s(dataBuffer.data(), headerSize(), static_cast<void*>(&header), headerSize());
		}

		void allocateAndInitializeBuffer()
		{
			dataBuffer = std::vector<uint8_t>(header.bodySize + headerSize(), 0);
			copyHeaderIntoDataBuffer();
		}
	};
}
