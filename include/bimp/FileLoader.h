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
	FileLoader(const std::string& filepath);
	virtual ~FileLoader() {}

	void Load();

protected:
	virtual void OnLoad(ImportStream& is) = 0;

private:
	void CachePrepare(int sz);

private:
	std::string m_filepath;

}; // FileLoader

}

#endif // _BIMP_FILE_LOADER_H_