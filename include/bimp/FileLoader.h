#ifndef _BIMP_FILE_LOADER_H_
#define _BIMP_FILE_LOADER_H_

#include "bimp/FilePath.h"

#include <cu/uncopyable.h>
#include <fs_file.h>

#include <stdint.h>

#include <string>

namespace bimp
{

class ImportStream;

class FileLoader : private cu::Uncopyable
{
public:
	FileLoader();
	FileLoader(const ResString& filepath, bool use_cache = true);
	FileLoader(const char* data, size_t size);
	FileLoader(fs_file* file, uint32_t offset, bool use_cache = true);
	virtual ~FileLoader();

	void Load();

protected:
	virtual void OnLoad(ImportStream& is) = 0;

private:
	void LoadFromFile(fs_file* file, bool use_cache);

private:
	class LoadImpl
	{
	public:
		LoadImpl(FileLoader& loader)
			: m_loader(loader) {}
		virtual ~LoadImpl() {}

		virtual void Load() = 0;

	protected:
		FileLoader& m_loader;

	}; // LoadImpl

	class FileImpl : public LoadImpl
	{
	public:
		FileImpl(FileLoader& loader, const ResString& filepath, bool use_cache);

		virtual void Load();

	private:
		ResString m_filepath;
		bool      m_use_cache;

	}; // FileImpl

	class DataImpl : public LoadImpl
	{
	public:
		DataImpl(FileLoader& loader, const char* data, size_t size);

		virtual void Load();

	private:
		const char* m_data;
		size_t      m_size;

	}; // DataImpl

	class FileInFileImpl : public LoadImpl
	{
	public:
		FileInFileImpl(FileLoader& loader, fs_file* file, uint32_t offset, bool use_cache);

		virtual void Load();

	private:
		fs_file* m_file;
		uint32_t m_offset;
		uint32_t m_size;
		bool     m_use_cache;

	}; // FileInFileImpl

private:
	LoadImpl* m_impl;

}; // FileLoader

}

#endif // _BIMP_FILE_LOADER_H_