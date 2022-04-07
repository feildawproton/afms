//from examples at boost.org
#ifndef FILESTUFF_H
#define FILESTUFF_H

#define	BOOST_FILESYSTEM_NO_DEPRECATED

#include "boost\filesystem\path.hpp"

#include <string>
#include <vector>

namespace FS = boost::filesystem;

namespace SCandSVMspace
{
	std::vector<FS::path> get_SubDirectories_inDirectory(const FS::path& directoryPath);

	std::vector<std::string> get_SubDirectoryNames_inDirectory(const std::string& directoryPath);

	std::vector<FS::path> get_FilePaths_inDirectory(const FS::path& directoryPath);

	std::vector<std::string> get_PathNames_ofPaths(const std::vector<FS::path>& paths);

	std::vector<std::string> get_Names_ofPaths(const std::vector<FS::path>& paths);

	std::vector<FS::path> create_SubDirectories_inDirectory(const FS::path& directoryPath, const std::vector<std::string>& names);

	std::vector<FS::path> copy_DirectoryStructure_toDirectory(const FS::path& baseDirectory, const FS::path& copyDirectory);

	std::vector<std::string> copy_DirectoryStructure_toDirectory_FullNames(const std::string& directoryName, const std::string& copyDirName);

	std::vector<std::string> get_FullPath_Names_ofSubDirectories(const std::string& rootDirectoryName);

	std::vector<std::string> get_FullPath_Names_ofFiles(const std::string& directoryName);

	std::vector<std::string> create_File_PathNames(const std::string& baseDirectory, const std::string& copyDirectory, const std::string& extension);

	std::string create_Directory(const std::string& dirName);

	bool copyFile(const std::string& from, const std::string& to);
}

#endif