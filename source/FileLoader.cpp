#include "FileLoader.h"
#include "ImportStream.h"
#include "bimp_typedef.h"

#include <fault.h>
#include <fs_file.h>

#include <Alloc.h>
#include <LzmaDec.h>

#include <stdint.h>

namespace bimp
{

static char* CACHE_BUF = NULL;
static int CACHE_SZ = 0;

#define PACKAGE_SIZE 512
#define UNUSED(x)	((void)(x))

#define LZMA_PROPS_SIZE 5

static void *SzAlloc(void *p, size_t size) { UNUSED(p); return MyAlloc(size); }
static void SzFree(void *p, void *address) { UNUSED(p); MyFree(address); }
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

FileLoader::FileLoader(const std::string& filepath, bool use_cache)
	: m_src_type(SRC_FILE)
	, m_filepath(filepath)
	, m_use_cache(use_cache)
	, m_data(NULL)
	, m_size(0)
{
}

FileLoader::FileLoader(const char* data, size_t size)
	: m_src_type(SRC_DATA)
	, m_use_cache(false)
	, m_data(data)
	, m_size(size)
{
}

void FileLoader::Load()
{
	switch (m_src_type)
	{
	case SRC_FILE:
		LoadFile();
		break;
	case SRC_DATA:
		LoadData();
		break;
	}
}

void FileLoader::LoadFile()
{
	fs_file* file = fs_open(m_filepath.c_str(), "rb");
	if (!file) {
		fault("open file fail: %s\n", m_filepath.c_str());
	}

	int32_t sz = 0;
	fs_read(file, &sz, sizeof(sz));
	if (sz < 0)
	{
		sz = -sz;

		const char* buf = PrepareBuf(sz);
		if (fs_read(file, (char*)buf, sz) != sz) {
			fault("Invalid uncompress data source\n");
		}

		ImportStream is(buf, sz);
		OnLoad(is);

		if (!m_use_cache) { delete[] buf; }
	}
	else
	{
		uint8_t ori_sz_arr[4];
		fs_read(file, ori_sz_arr, sizeof(ori_sz_arr));
		fs_seek_from_cur(file, -(int)sizeof(ori_sz_arr));
		size_t ori_sz = ori_sz_arr[0] << 24 | ori_sz_arr[1] << 16 | ori_sz_arr[2] << 8 | ori_sz_arr[3];
		size_t need_sz = sz + 7 + ori_sz;

		const char* buf = PrepareBuf(need_sz);

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

		if (!m_use_cache) { delete[] buf; }
	}

	fs_close(file);
}

void FileLoader::LoadData()
{
	ImportStream is(m_data, m_size);
	OnLoad(is);
}

const char* FileLoader::PrepareBuf(int sz) const
{
	char* buf = NULL;
	if (m_use_cache)
	{
		if (!CACHE_BUF) {
			CACHE_BUF = new char[sz];
			if (!CACHE_BUF) {
				fault("FileLoader::PrepareBuf fail\n");
			}
			CACHE_SZ = sz;
		} else if (CACHE_SZ < sz) {
			delete[] CACHE_BUF;
			CACHE_BUF = new char[sz];
			if (!CACHE_BUF) {
				fault("FileLoader::PrepareBuf fail\n");
			}
			CACHE_SZ = sz;
		}
		buf = CACHE_BUF;
	}
	else
	{
		buf = new char[sz];
		if (!buf) {
			fault("FileLoader::PrepareBuf fail\n");
		}
	}
	return buf;
}

}