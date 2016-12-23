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

	// for debug
	int GetCap() const { return m_cap; }

private:
	char* m_buffer;
	int m_cap;

}; // Allocator

}

#endif // _BIMP_ALLOCATOR_H_