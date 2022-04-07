#include "FileProducers.h"
#include <iostream>

//define this before includes
#define	BOOST_FILESYSTEM_NO_DEPRECATED
#include "boost\filesystem\operations.hpp"
#include "boost\filesystem\path.hpp"

namespace FS = boost::filesystem;

namespace File_System
{
	//these may be specific to VC++
#pragma region Not declared in interface to protect caller

	std::vector<boost::filesystem::path> get_SubDirectories_inDirectory(const FS::path& directoryPath)
	{
		FS::path full_path = FS::system_complete(directoryPath);

		unsigned long dir_count = 0;
		std::vector<boost::filesystem::path> directories;
		if (FS::is_directory(full_path))
		{
			FS::directory_iterator end_iter;
			for (FS::directory_iterator dir_itr(full_path); dir_itr != end_iter; ++dir_itr)
			{
				if (FS::is_directory(dir_itr->status()))
				{
					++dir_count;
					FS::path full = FS::system_complete(dir_itr->path());
					directories.push_back(full);
				}
			}
		}
		else
		{
			std::cout << std::endl << directoryPath.filename().string() << " is not a directory" << std::endl;
		}
		return directories;
	}

	std::vector<std::string> get_Names_ofPaths(const std::vector<FS::path>& paths)
	{
		std::vector<std::string> names;
		for (int i = 0; i < paths.size(); i++)
		{
			names.push_back(paths[i].filename().string());
		}
		return names;
	}

	std::vector<FS::path> create_SubDirectories_inDirectory(const FS::path& directoryPath, const std::vector<std::string>& names)
	{
		std::vector<FS::path> paths;
		if (FS::is_directory(directoryPath))
		{
			FS::path fullRoot_Path = FS::system_complete(directoryPath);
			std::string fullRoot_PathName = fullRoot_Path.string();

			for (int i = 0; i < names.size(); i++)
			{
				std::string name = fullRoot_PathName + "\\" + names[i];
				FS::path pat(name);
				FS::path p = FS::system_complete(pat);

				if (FS::is_directory(p))
				{
					paths.push_back(p);
				}
				else
				{
					FS::create_directory(p);

					if (!FS::is_directory(p))
					{
						std::cout << "\nUnable to create directory: " << p.filename().string() << std::endl;
					}
					else
					{
						paths.push_back(p);
					}
				}
			}
		}
		else
		{
			std::cout << std::endl << directoryPath.filename().string() << " is not a directory" << std::endl;
		}
		return paths;
	}

	std::vector<FS::path> copy_DirectoryStructure_toDirectory(const FS::path& baseDirectory, const FS::path& copyDirectory)
	{
		std::vector<FS::path> subDirectories_inBase = get_SubDirectories_inDirectory(baseDirectory);
		std::vector<std::string> subDirectory_names = get_Names_ofPaths(subDirectories_inBase);
		std::vector<FS::path> newDirectories = create_SubDirectories_inDirectory(copyDirectory, subDirectory_names);
		return newDirectories;
	}

	std::vector<std::string> get_PathNames_ofPaths(const std::vector<FS::path>& paths)
	{
		std::vector<std::string> names;
		for (int i = 0; i < paths.size(); i++)
		{
			names.push_back(paths[i].string());
		}
		return names;
	}

	std::vector<FS::path> get_FilePaths_inDirectory(const FS::path& directoryPath)
	{
		FS::path full_path = FS::system_complete(directoryPath);

		unsigned long file_count = 0;
		std::vector<FS::path> files;
		if (FS::is_directory(full_path))
		{
			FS::directory_iterator end_iter;
			for (FS::directory_iterator dir_itr(full_path); dir_itr != end_iter; ++dir_itr)
			{
				if (FS::is_regular_file(dir_itr->status()))
				{
					++file_count;
					FS::path full = FS::system_complete(dir_itr->path());
					files.push_back(full);
				}
			}
		}
		else
		{
			std::cout << std::endl << directoryPath.filename().string() << " is not a directory" << std::endl;
		}
		return files;
	}

#pragma endregion
	
#pragma region Declared in interface

	std::vector<std::string> copy_DirectoryStructure_toDirectory(const std::string& directoryName, const std::string& copyDirName)
	{
		FS::path baseDirectory(directoryName);
		baseDirectory = FS::system_complete(baseDirectory);
		FS::path copyDirectory(copyDirName);
		copyDirectory = FS::system_complete(copyDirName);

		FS::create_directory(copyDirectory);

		bool check = FS::is_directory(copyDirectory);

		if (!check)
		{
			std::cout << "n\\Could not create directory " << copyDirectory.filename().string();
		}

		std::vector<FS::path> copyDirectoryPaths = copy_DirectoryStructure_toDirectory(baseDirectory, copyDirectory);
		std::vector<std::string> pathNames = get_PathNames_ofPaths(copyDirectoryPaths);
		return pathNames;
	}



	std::string create_Directory(const std::string& dirName)
	{
		FS::path Directory(dirName);
		Directory = FS::system_complete(Directory);

		if (!FS::is_directory(Directory))
		{
			FS::create_directory(Directory);
		}

		std::string fullPathName = Directory.string();
		return fullPathName;
	}

	bool copyFile(const std::string& from, const std::string& to)
	{
		FS::path File1(from);
		File1 = FS::system_complete(File1);
		FS::path File2(to);
		File2 = FS::system_complete(File2);

		if (FS::is_regular_file(File1))
		{
			FS::copy_file(File1, File2, FS::copy_option::overwrite_if_exists); //overwrite!!
			if (FS::is_regular_file(File2))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
		FS::copy_file(File1, File2);
	}
#pragma endregion
}
