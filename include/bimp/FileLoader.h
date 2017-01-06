#ifndef _BIMP_FILE_LOADER_H_
#define _BIMP_FILE_LOADER_H_

#include <CU_Uncopyable.h>

#include <string>

namespace bimp
{

class ImportStream;

class FileLoader : private cu::Uncopyable
{
public:
	FileLoader(const std::string& filepath, bool use_cache = true);
	FileLoader(const char* data, size_t size);
	virtual ~FileLoader() {}

	void Load();

protected:
	virtual void OnLoad(ImportStream& is) = 0;

private:
	enum SrcType
	{
		SRC_FILE = 0,
		SRC_DATA,
	};

private:
	void LoadFile();
	void LoadData();

	const char* PrepareBuf(int sz) const;

private:
	SrcType m_src_type;

	std::string m_filepath;
	bool        m_use_cache;

	const char* m_data;
	size_t      m_size;

}; // FileLoader

}

#endif // _BIMP_FILE_LOADER_H_