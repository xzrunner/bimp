#ifndef _BIMP_IMPORT_STREAM_H_
#define _BIMP_IMPORT_STREAM_H_

#include <CU_Uncopyable.h>

#include <string>

#include <stdint.h>

namespace bimp
{

class Allocator;

class ImportStream : private cu::Uncopyable
{
public:
	ImportStream(const char* data, int sz);
	virtual ~ImportStream();

	uint8_t  UInt8();
	uint16_t UInt16();
	uint32_t UInt32();

	float Float();

	const char* Block(int sz);

	const char* Stream() const { return m_stream; }
	int Size() const { return m_size; }

	const char* String(Allocator& alloc);
	std::string String();

	bool Empty() const { return m_size == 0; }

protected:
	const char* m_stream;
	int m_size;

}; // ImportStream

}

#endif // _BIMP_IMPORT_STREAM_H_