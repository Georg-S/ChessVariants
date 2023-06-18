#pragma once
#include <cstdint>
#include <vector>
#include <type_traits>
#include <cstddef>

namespace net
{
	enum DestinationID
	{
		SERVER = 0,
		BROADCAST = 1,
	};

	enum SystemMessages : uint32_t
	{
		NEW_CONNECTION, // Has no body data and writes the id of the new connected client to the "toID"
		CONNECTION_CLOSED, // Has no body data and writes the id of the lost connection client id to the "toID"
		SYSTEM_MESSAGES_END
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
		std::vector<std::byte> dataBuffer;

		Message() = default; // Resize should be called once the header is set

		template <typename Messagetype, typename struc>
		Message(uint32_t toID, Messagetype messageType, const struc& data)
		{
			static_assert(std::is_trivially_copyable<struc>::value, "Muste be trivially copyable");
			static_assert(std::is_integral_v<Messagetype> || std::is_enum_v<Messagetype>, "Message type must be an integral or an enum");
			header.toID = toID;
			header.messageType = static_cast<uint32_t>(messageType);
			header.bodySize = sizeof(data);
			allocateAndInitializeBuffer();
			copyDataIntoBuffer(&data, header.bodySize);
		}

		template <typename Messagetype>
		Message(uint32_t toID, Messagetype messageType, const std::string& dataStr) 
		{
			static_assert(std::is_integral_v<Messagetype> || std::is_enum_v<Messagetype>, "Message type must be an integral or an enum");
			header.toID = toID;
			header.messageType = static_cast<uint32_t>(messageType);
			header.bodySize = static_cast<uint32_t>(dataStr.size() + 1);
			allocateAndInitializeBuffer();
			copyDataIntoBuffer(dataStr.c_str(), static_cast<uint32_t>(dataStr.size()));
		}

		Message(Header header)
			: header(std::move(header))
		{
			allocateAndInitializeBuffer();
		}

		Message(uint32_t messageType, uint32_t destinationID, const void* data, uint32_t dataSize)
		{
			header.messageType = messageType;
			header.toID = destinationID;
			header.bodySize = dataSize;
			allocateAndInitializeBuffer();
			copyDataIntoBuffer(data, dataSize);
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

		const void* getBodyStart() const
		{
			return dataBuffer.data() + headerSize();
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

		/// Only works if the message body ONLY consists of one string
		std::string bodyToString() const 
		{
			return std::string(static_cast<const char*>(getBodyStart()), bodySize());
		}

	private:
		void copyHeaderIntoDataBuffer() 
		{
			memcpy_s(dataBuffer.data(), headerSize(), static_cast<void*>(&header), headerSize());
		}

		void allocateAndInitializeBuffer()
		{
			dataBuffer = std::vector<std::byte>(header.bodySize + headerSize(), std::byte(0));
			copyHeaderIntoDataBuffer();
		}

		void copyDataIntoBuffer(const void* data, uint32_t dataSize)
		{
			memcpy_s(getBodyStart(), header.bodySize, data, dataSize);
		}
	};

	class ServerMessage : public Message
	{
	public:
		uint32_t fromID;
	};
}
