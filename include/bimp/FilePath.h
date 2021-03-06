#ifndef _BIMP_FILE_PATH_H_
#define _BIMP_FILE_PATH_H_

#include <cu/cu_stl.h>

#include <string>

#include <stdint.h>
#include <string.h>

namespace bimp
{

class FilePath
{
public:
	explicit FilePath() : m_offset(MAX_OFFSET) {}
	explicit FilePath(const CU_STR& filepath, uint32_t offset = MAX_OFFSET)
		: m_filepath(filepath.c_str()), m_offset(offset) {}

	bool operator != (const FilePath& path) const {
		return !(*this == path);
	}
	bool operator == (const FilePath& path) const {
		return m_filepath == path.m_filepath && m_offset == path.m_offset;
	}
	bool operator < (const FilePath& path) const {
		return m_filepath < path.m_filepath
			|| m_filepath == path.m_filepath && m_offset < path.m_offset;
	}

	bool IsSingleFile() const { return m_offset == MAX_OFFSET; }

	const auto& GetFilepath() const { return m_filepath; }
	uint32_t GetOffset() const { return m_offset; }

	void SetFilepath(const CU_STR& filepath) { m_filepath = filepath.c_str(); }

	void Serialize(char* buf) const {
		memcpy(buf, &m_offset, sizeof(m_offset));
		strcpy(buf + sizeof(m_offset), m_filepath.c_str());
	}
	void Deserialization(const char* buf) {
		memcpy(&m_offset, buf, sizeof(m_offset));
		m_filepath.assign(buf + sizeof(m_offset));
	}

private:
	static const uint32_t MAX_OFFSET = 0xffffffff;

private:
	CU_STR   m_filepath;
	uint32_t m_offset;

}; // FilePath

}

#endif // _BIMP_FILE_PATH_H_