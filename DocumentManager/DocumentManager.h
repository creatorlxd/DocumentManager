#pragma once
#include <fstream>
#include <filesystem>
#include <map>
#include <string>
#include <cctype>

enum class Endian :uint8_t
{
	Small = 0,
	Big = 1
};

Endian GetEndian(uint32_t val);

extern uint32_t g_MagicNumber;

template<typename T>
T GetCorrectResult(const T& val, Endian e1, Endian e2)
{
	if (e1 == e2)
		return val;
	else
	{
		T re;
		uint8_t* pre = (uint8_t*)&re;
		uint8_t* pval = (uint8_t*)&val;
		for (std::size_t i = 0; i < sizeof(T); i++)
		{
			pre[i] = pval[sizeof(T) - 1 - i];
		}
		return re;
	}
}

class DocumentLibrary
{
public:
	DocumentLibrary(const std::filesystem::path& root_path);
	~DocumentLibrary();
	void Synchronous(DocumentLibrary& lib);
	void CheckFileExist();
private:
	void UpdateFileInformation(const std::filesystem::path& dir_path);
private:
	std::filesystem::path m_RootPath;
	std::filesystem::path m_InformationFilePath;
	std::map < std::string, long long> m_FileInformations;
	Endian m_ThisLibEndian, m_ThisPCEndian;
};