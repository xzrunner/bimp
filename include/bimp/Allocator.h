#ifndef _BIMP_ALLOCATOR_H_
#define _BIMP_ALLOCATOR_H_

#include <CU_Uncopyable.h>

namespace bimp
{

class Allocator : private cu::Uncopyable
{
public:
	Allocator(int cap);
	~Allocator();

	void* Alloc(int sz);

	bool Empty() const { return m_cap == 0; }

	void Reset();

	int GetSize() const {return m_size; }

	// for debug
	int GetCap() const { return m_cap; }

private:
	char* m_buffer;
	int m_cap;
    int m_size;

}; // Allocator

}

#endif // _BIMP_ALLOCATOR_H_