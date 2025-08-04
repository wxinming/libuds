#include "Type.h"

uds::Buffer::Buffer(size_t size)
	:send{}, recv{}
{
	send.data = new uint8_t[size];
	memset(send.data, 0, size * sizeof(uint8_t));
	send.size = size;
	recv.data = new uint8_t[size];
	memset(recv.data, 0, size * sizeof(uint8_t));
	recv.size = size;
}

uds::Buffer::~Buffer()
{
	if (send.data) {
		delete[] send.data;
		send.data = nullptr;
	}

	if (recv.data) {
		delete[] recv.data;
		recv.data = nullptr;
	}
}

uds::Buffer::Buffer(const uds::Buffer& buffer)
	:send{}, recv{}
{
	send.size = buffer.send.size;
	send.data = new uint8_t[send.size];
	memset(send.data, 0x00, send.size * sizeof(uint8_t));
	memcpy(send.data, buffer.send.data, buffer.send.size);

	recv.size = buffer.recv.size;
	recv.data = new uint8_t[recv.size];
	memset(recv.data, 0x00, recv.size * sizeof(uint8_t));
	memcpy(recv.data, buffer.recv.data, buffer.recv.size);
}

uds::Buffer& uds::Buffer::operator=(const uds::Buffer& buffer)
{
	if (this == &buffer) {
		return *this;
	}
	send.size = buffer.send.size;
	if (send.data) {
		delete[] send.data;
		send.data = nullptr;
	}
	send.data = new uint8_t[send.size];
	memset(send.data, 0x00, send.size * sizeof(uint8_t));
	memcpy(send.data, buffer.send.data, buffer.send.size);

	recv.size = buffer.recv.size;
	if (recv.data) {
		delete[] recv.data;
		recv.data = nullptr;
	}
	recv.data = new uint8_t[recv.size];
	memset(recv.data, 0x00, recv.size * sizeof(uint8_t));
	memcpy(recv.data, buffer.recv.data, buffer.recv.size);
	return *this;
}

uds::Buffer::Buffer(Buffer&& buffer) noexcept
	:send{}, recv{}
{
	send = buffer.send;
	buffer.send.data = nullptr;
	recv = buffer.recv;
	buffer.recv.data = nullptr;
}

uds::Buffer& uds::Buffer::operator=(Buffer&& buffer) noexcept
{
	if (this == &buffer) {
		return *this;
	}

	delete[] send.data;
	send = buffer.send;
	buffer.send.data = nullptr;

	delete[] recv.data;
	recv = buffer.recv;
	buffer.recv.data = nullptr;
	return *this;
}
