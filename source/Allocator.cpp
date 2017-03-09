#include <bimp/Allocator.h>

#include <fault.h>

#include <stddef.h>

namespace bimp
{

Allocator::Allocator(int cap)
	: m_cap(cap)
{
	m_buffer = new char[cap];
}

Allocator::~Allocator()
{
	delete[] m_buffer;
}

void* Allocator::Alloc(int sz)
{
	if (sz <= 0) {
		return NULL;
	}
	if (m_cap < sz) {
		fault("Allocator::Alloc too large, sz %d, cap %d\n", sz, m_cap);
		return NULL;
	}
	void* ret = m_buffer;
	m_buffer += sz;
	m_cap -= sz;
	return ret;
}

}
