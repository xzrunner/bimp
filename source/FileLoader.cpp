#include "bimp/FileLoader.h"
#include "bimp/ImportStream.h"
#include "bimp/typedef.h"

#include <fault.h>
#include <fs_file.h>

#include <Alloc.h>
#include <LzmaDec.h>

#include <stdint.h>

#include "logger.h"

namespace bimp
{

static char* CACHE_BUF = nullptr;
static int CACHE_SZ = 0;

#define PACKAGE_SIZE 512
#define UNUSED(x)	((void)(x))

#define LZMA_PROPS_SIZE 5

#ifdef LZMA_NEW
static void *SzAlloc(ISzAllocPtr p, size_t size) { UNUSED(p); return MyAlloc(size); }
static void SzFree(ISzAllocPtr p, void *address) { UNUSED(p); MyFree(address); }
#else
static void *SzAlloc(void* p, size_t size) { UNUSED(p); return MyAlloc(size); }
static void SzFree(void* p, void *address) { UNUSED(p); MyFree(address); }
#endif // ISzAllocPtr
static ISzAlloc g_Alloc = { SzAlloc, SzFree };

static inline int
_lzma_uncompress(unsigned char *dest, size_t  *destLen, const unsigned char *src, size_t  *srcLen, 
				 const unsigned char *props, size_t propsSize)
{
	ELzmaStatus status;
	return LzmaDecode(dest, destLen, src, srcLen, props, (unsigned)propsSize, LZMA_FINISH_ANY, &status, &g_Alloc);
}

struct block {
	uint8_t size[4];
	uint8_t prop[LZMA_PROPS_SIZE];
	uint8_t data[119];
};

FileLoader::FileLoader()
	: m_impl(nullptr)
{
}

FileLoader::FileLoader(const CU_STR& filepath, bool use_cache)
{
	m_impl = new FileImpl(*this, filepath, use_cache);
}

FileLoader::FileLoader(const char* data, size_t size)
{
	m_impl = new DataImpl(*this, data, size);
}

FileLoader::FileLoader(fs_file* file, uint32_t offset, bool use_cache)
{
	m_impl = new FileInFileImpl(*this, file, offset, use_cache);
}

FileLoader::~FileLoader()
{
	if (m_impl) {
		delete m_impl;
	}
}

void FileLoader::Load()
{
	if (m_impl) {
		m_impl->Load();
	}
}

static const char* prepare_buf(int sz)
{
	char* buf = nullptr;
	if (!CACHE_BUF) {
		CACHE_BUF = new char[sz];
		if (!CACHE_BUF) {
			fault("FileLoader prepare_buf fail\n");
		}
		CACHE_SZ = sz;
	} else if (CACHE_SZ < sz) {
		delete[] CACHE_BUF;
		CACHE_BUF = new char[sz];
		if (!CACHE_BUF) {
			fault("FileLoader prepare_buf fail\n");
		}
		CACHE_SZ = sz;
	}
	buf = CACHE_BUF;
	return buf;
}

void FileLoader::LoadFromFile(fs_file* file, bool use_cache)
{
	int32_t sz = 0;
	fs_read(file, &sz, sizeof(sz));
	if (sz < 0)
	{
		sz = -sz;

		const char* buf = nullptr;
		if (use_cache) {
			buf = prepare_buf(sz);
		} else {
			buf = new char[sz];
			if (!buf) {
				fault("FileLoader::FileImpl::Load fail");
			}
		}

		if (fs_read(file, (char*)buf, sz) != sz) {
			fault("Invalid uncompress data source\n");
		}

		ImportStream is(buf, sz);
		OnLoad(is);

		if (!use_cache) { 
			delete[] buf; 
		}
	}
	else
	{
		uint8_t ori_sz_arr[4];
		fs_read(file, ori_sz_arr, sizeof(ori_sz_arr));
		fs_seek_from_cur(file, -(int)sizeof(ori_sz_arr));
		size_t ori_sz = ori_sz_arr[0] << 24 | ori_sz_arr[1] << 16 | ori_sz_arr[2] << 8 | ori_sz_arr[3];
		size_t need_sz = sz + 7 + ori_sz;

		const char* buf = nullptr;
		if (use_cache) {
			buf = prepare_buf(need_sz);
		} else {
			buf = new char[need_sz];
			if (!buf) {
				fault("FileLoader::FileImpl::Load fail");
			}
		}

		struct block* block = (struct block*)buf;
		if (sz <= 4 + LZMA_PROPS_SIZE || fs_read(file, block, sz) != sz) {
			fault("Invalid compress data source\n");
		}

		unsigned char* buffer = (unsigned char*)(buf + ALIGN_4BYTE(sz));
		size_t compressed_sz = sz - sizeof(block->size) - LZMA_PROPS_SIZE;

		int r = _lzma_uncompress(buffer, &ori_sz, block->data, &compressed_sz, block->prop, LZMA_PROPS_SIZE);
		if (r != SZ_OK) {
			fault("Uncompress error %d\n",r);
		}

		ImportStream is((char*)buffer, ori_sz);
		OnLoad(is);

		if (!use_cache) { 
			delete[] buf; 
		}
	}
}

/************************************************************************/
/* class FileLoader::FileImpl                                           */
/************************************************************************/

FileLoader::FileImpl::FileImpl(FileLoader& loader, const CU_STR& filepath, bool use_cache)
	: LoadImpl(loader)
	, m_filepath(filepath)
	, m_use_cache(use_cache)
{
}

void FileLoader::FileImpl::Load()
{
	fs_file* file = fs_open(m_filepath.c_str(), "rb");
	if (!file) {
		fault("open file fail: %s\n", m_filepath.c_str());
	}
	m_loader.LoadFromFile(file, m_use_cache);
	fs_close(file);
}

/************************************************************************/
/* class FileLoader::DataImpl                                           */
/************************************************************************/

FileLoader::DataImpl::DataImpl(FileLoader& loader, const char* data, size_t size)
	: LoadImpl(loader)
	, m_data(data)
	, m_size(size)
{
}

void FileLoader::DataImpl::Load()
{
	ImportStream is(m_data, m_size);
	m_loader.OnLoad(is);
}

/************************************************************************/
/* class FileLoader::FileInFileImpl                                     */
/************************************************************************/

FileLoader::FileInFileImpl::
FileInFileImpl(FileLoader& loader, fs_file* file, uint32_t offset, bool use_cache)
	: LoadImpl(loader)
	, m_file(file)
	, m_offset(offset)
	, m_use_cache(use_cache)
{
}

void FileLoader::FileInFileImpl::Load()
{
	if (!m_file) {
		return;
	}

	fs_seek_from_head(m_file, m_offset);
	m_loader.LoadFromFile(m_file, m_use_cache);	
}

}