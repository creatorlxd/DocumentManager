#include "DocumentManager.h"
#include <iostream>
#include <array>
#include <stack>

uint32_t g_MagicNumber = 0x00000012;

Endian GetEndian(uint32_t val)
{
	uint8_t* ptr = (uint8_t*)&val;
	if (*ptr == (g_MagicNumber & 0x000000ff))
		return Endian::Small;
	else
		return Endian::Big;
}

DocumentLibrary::DocumentLibrary(const std::filesystem::path & root_path)
{
	if (!std::filesystem::exists(root_path))
	{
		std::cout << "error:\tthe path does not exist" << std::endl;
		std::abort();
		return;
	}

	m_RootPath = root_path;

	m_InformationFilePath = root_path / "doc_lib_info.dat";

	m_ThisPCEndian = GetEndian(g_MagicNumber);
	m_ThisLibEndian = m_ThisPCEndian;

	if (std::filesystem::exists(m_InformationFilePath))
	{
		std::fstream info_file(m_InformationFilePath, std::ios::binary | std::ios::in);

		uint32_t magic_number;
		info_file.read((char*)&magic_number, sizeof(magic_number));
		m_ThisLibEndian = GetEndian(magic_number);

		std::size_t size;
		info_file.read((char*)&size, sizeof(size));
		size = GetCorrectResult(size, m_ThisLibEndian, m_ThisPCEndian);

		for (std::size_t i = 0; i < size; i++)
		{
			std::size_t str_size;
			info_file.read((char*)&str_size, sizeof(str_size));
			str_size = GetCorrectResult(str_size, m_ThisLibEndian, m_ThisPCEndian);
			std::string name(str_size, ' ');
			info_file.read(name.data(), str_size);
			long long code;
			info_file.read((char*)&code, sizeof(code));
			code = GetCorrectResult(code, m_ThisLibEndian, m_ThisPCEndian);
			m_FileInformations[name] = code;
		}

		info_file.close();
	}
	UpdateFileInformation(m_RootPath);
	CheckFileExist();
}

DocumentLibrary::~DocumentLibrary()
{
	UpdateFileInformation(m_RootPath);
	CheckFileExist();

	std::fstream info_file(m_InformationFilePath, std::ios::binary | std::ios::out);

	info_file.write((char*)&g_MagicNumber, sizeof(g_MagicNumber));

	std::size_t size = m_FileInformations.size();

	info_file.write((char*)&size, sizeof(size));
	for (auto& iter : m_FileInformations)
	{
		std::size_t str_size = iter.first.size();
		info_file.write((char*)&str_size, sizeof(str_size));
		info_file.write(iter.first.data(), str_size);
		info_file.write((char*)&iter.second, sizeof(iter.second));
	}

	info_file.close();
}

enum SynchronousDirection
{
	ToDestination = -1,
	FromDestination = 1
};

void DocumentLibrary::Synchronous(DocumentLibrary & lib)
{
	auto find_in_map = [](const std::string& str, const std::map<std::string, long long>& m)->long long {
		auto iter = m.find(str);
		if (iter == m.end())
			return 0;
		else
			return iter->second;
	};
	std::vector<std::pair<std::string, SynchronousDirection>> change_list;
	for (auto& i : m_FileInformations)
	{
		if (i.second != find_in_map(i.first, lib.m_FileInformations) && (std::filesystem::directory_entry(m_RootPath / std::filesystem::path(i.first)).is_directory() == false))
			change_list.push_back(std::make_pair(i.first, (i.second > find_in_map(i.first, lib.m_FileInformations) ? ToDestination : FromDestination)));
	}
	for (auto& i : lib.m_FileInformations)
	{
		if (i.second != find_in_map(i.first, m_FileInformations) && (std::filesystem::directory_entry(lib.m_RootPath / std::filesystem::path(i.first)).is_directory() == false))
			change_list.push_back(std::make_pair(i.first, (i.second > find_in_map(i.first, m_FileInformations) ? FromDestination : ToDestination)));
	}

	//interactive begin
	for (auto& i : change_list)
		std::cout << "sync:\t" << i.first << "\t" << (i.second == ToDestination ? (m_RootPath.generic_string() + " -> " + lib.m_RootPath.generic_string()) : (lib.m_RootPath.generic_string() + " -> " + m_RootPath.generic_string())) << std::endl;;

	bool is_sync = false;
	char ans = ' ';
	while (ans != 'y'&&ans != 'n')
	{
		std::cout << "if sync? [y/n]" << std::endl;
		std::cin >> ans;
	}
	is_sync = (ans == 'y' ? true : false);
	//interactive end
	if (is_sync)
	{
		//useful but slow
		//std::filesystem::copy(m_RootPath, lib.m_RootPath, std::filesystem::copy_options::recursive | std::filesystem::copy_options::update_existing);
		//std::filesystem::copy(lib.m_RootPath, m_RootPath, std::filesystem::copy_options::recursive | std::filesystem::copy_options::update_existing);

		auto check_and_create_dir = [](const std::filesystem::path& path, const std::filesystem::path& root)->void {
			//we assume that the path is a path of a file
			auto pb = path;
			std::stack<std::filesystem::path> ps;
			while (pb.parent_path() != root)
			{
				auto ppath = pb.parent_path();
				if (std::filesystem::exists(ppath) == false)
					ps.push(ppath);
				pb = ppath;
			}
			while (ps.empty() == false)
			{
				auto pdir = ps.top();
				std::filesystem::create_directory(pdir);
				ps.pop();
			}
		};

		for (auto& i : change_list)
		{
			auto src_path = m_RootPath / i.first;
			auto dst_path = lib.m_RootPath / i.first;
			if (i.second == ToDestination)
			{
				check_and_create_dir(dst_path, lib.m_RootPath);
				std::filesystem::copy_file(src_path, dst_path, std::filesystem::copy_options::update_existing);
			}
			else
			{
				check_and_create_dir(src_path, m_RootPath);
				std::filesystem::copy_file(dst_path, src_path, std::filesystem::copy_options::update_existing);
			}
		}
	}
}

void DocumentLibrary::UpdateFileInformation(const std::filesystem::path & dir_path)
{
	for (auto& iter : std::filesystem::directory_iterator(dir_path))
	{
		if (iter.path() == m_InformationFilePath)
			continue;

		std::string filename = std::filesystem::relative(iter.path(), m_RootPath).generic_string();
		long long new_result = iter.last_write_time().time_since_epoch().count();

		auto map_iter = m_FileInformations.find(filename);

		if (map_iter == m_FileInformations.end())
		{
			std::cout << "add:\t" << filename << std::endl;
			m_FileInformations[filename] = new_result;
			if (iter.is_directory())
			{
				UpdateFileInformation(iter.path());
			}
		}
		else
		{
			if (map_iter->second != new_result)
			{
				std::cout << "modify:\t" << filename << std::endl;
				m_FileInformations[filename] = new_result;
				if (iter.is_directory())
				{
					UpdateFileInformation(iter.path());
				}
			}
		}

	}
}

void DocumentLibrary::CheckFileExist()
{
	decltype(m_FileInformations) new_file_info;
	for (auto& i : m_FileInformations)
	{
		if (std::filesystem::exists(m_RootPath / i.first))
			new_file_info.insert(i);
		else
			std::cout << "delete:\t" << i.first << std::endl;
	}
	m_FileInformations = new_file_info;
}
