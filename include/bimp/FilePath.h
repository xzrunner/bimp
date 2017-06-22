#ifndef _BIMP_FILE_PATH_H_
#define _BIMP_FILE_PATH_H_

#include <string>

#include <stdint.h>
#include <string.h>

namespace bimp
{

class FilePath
{
public:
	explicit FilePath() : m_offset(MAX_OFFSET) {}
	explicit FilePath(const std::string& filepath, uint32_t offset = MAX_OFFSET)
		: m_filepath(filepath), m_offset(offset) {}

	bool IsSingleFile() const { return m_offset == MAX_OFFSET; }

	const std::string& GetFilepath() const { return m_filepath; }
	uint32_t GetOffset() const { return m_offset; }

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
	std::string m_filepath;
	uint32_t    m_offset;

}; // FilePath

}

#endif // _BIMP_FILE_PATH_H_