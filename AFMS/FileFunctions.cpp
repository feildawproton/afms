#include "FileFunctions.h"

//define this before includes
#define	BOOST_FILESYSTEM_NO_DEPRECATED
#include "boost\filesystem\operations.hpp"
#include "boost\filesystem\path.hpp"

namespace FS = boost::filesystem;

namespace File_System
{
#pragma region not declared in interface to protect caller from includes

	std::vector<FS::path> get_FilePaths(const FS::path& directoryPath)
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
		return files;
	}

	std::vector<std::string> get_PathNames(const std::vector<FS::path>& paths)
	{
		std::vector<std::string> names;
		for (int i = 0; i < paths.size(); i++)
		{
			names.push_back(paths[i].string());
		}
		return names;
	}

	std::vector<boost::filesystem::path> get_SubDirectories(const FS::path& directoryPath)
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
		return directories;
	}

#pragma endregion

#pragma region declared in interface

	std::vector<std::string> get_FullPathNames_ofFiles(const std::string& directoryName)
	{
		FS::path directory(directoryName);
		directory = FS::system_complete(directory);
		std::vector<FS::path> paths = get_FilePaths(directory);
		std::vector<std::string> pathNames = get_PathNames(paths);
		return pathNames;
	}

	std::vector<std::string> get_FullPathNames_ofSubDirectories(const std::string& rootDirectoryName)
	{
		FS::path rootDirectory(rootDirectoryName);
		rootDirectory = FS::system_complete(rootDirectory);
		std::vector<FS::path> paths = get_SubDirectories(rootDirectory);
		std::vector<std::string> pathNames = get_PathNames(paths);
		return pathNames;
	}

	std::vector<std::string> get_SubDirectoryNames(const std::string& directoryPathName)
	{
		FS::path path(directoryPathName);
		FS::path full_path = FS::system_complete(path);

		unsigned long dir_count = 0;
		std::vector<std::string> directories;

		if (FS::is_directory(full_path))
		{
			FS::directory_iterator end_iter;
			for (FS::directory_iterator dir_itr(full_path); dir_itr != end_iter; ++dir_itr)
			{
				if (FS::is_directory(dir_itr->status()))
				{
					++dir_count;
					FS::path full = FS::system_complete(dir_itr->path());
					directories.push_back(full.filename().string());
				}
			}
		}
		return directories;
	}

	std::string get_ParentPathName(const std::string& pathName)
	{
		FS::path path(pathName);
		FS::path full_path = FS::system_complete(path);

		FS::path p = path.parent_path();

		std::string ming = p.string();

		return ming;
	}

	std::string get_FolderName(const std::string& dirPatName)
	{
		FS::path path(dirPatName);
		FS::path full_path = FS::system_complete(path);

		std::string ding;
		if (FS::is_directory(dirPatName))
		{
			ding = full_path.filename().string();
		}
		else
		{
			ding = "not a directory";
		}
		return ding;
	}

	std::vector<std::string> FilePathNames(const std::string& baseDirectory, const std::string& copyDirectory, const std::string& extension)
	{
		FS::path directory(baseDirectory);
		directory = FS::system_complete(directory);
		std::vector<FS::path> paths = get_FilePaths(directory);

		std::vector<std::string> fileNames;
		for (int i = 0; i < paths.size(); i++)
		{
			FS::path gath = paths[i];
			gath.replace_extension(extension);
			std::string ming = gath.filename().string();
			std::string ding = copyDirectory + "\\" + ming;
			fileNames.push_back(ding);
		}
		return fileNames;
	}

	std::vector<std::string> FilePathNames_fromFolders(const std::string& baseDirectory, const std::string& copyDirectory, const std::string& extension)
	{
		FS::path directory(baseDirectory);
		directory = FS::system_complete(directory);

		std::vector<FS::path> paths = get_SubDirectories(directory);

		std::vector<std::string> folderNames;
		for (int i = 0; i < paths.size(); i++)
		{
			FS::path gath = paths[i];

			std::string ming = gath.filename().string();
			std::string ding = copyDirectory + "\\" + ming;
			folderNames.push_back(ding);
		}
		return folderNames;
	}
#pragma endregion
}