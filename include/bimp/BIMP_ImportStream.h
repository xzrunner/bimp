#ifndef _BIMP_IMPORT_STREAM_H_
#define _BIMP_IMPORT_STREAM_H_

#include <cu/uncopyable.h>
#include <memmgr/Allocator.h>

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
	mm::AllocString String();

	const char* LongString(Allocator& alloc);
	mm::AllocString LongString();

	bool Empty() const { return m_size == 0; }

protected:
	const char* m_stream;
	int m_size;

}; // ImportStream

}

#endif // _BIMP_IMPORT_STREAM_H_