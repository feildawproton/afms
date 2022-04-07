#ifndef FILEFUNCTIONS_H
#define FILEFUNCTIONS_H

#include <vector>
#include <string>

namespace File_System
{
	std::vector<std::string> get_FullPathNames_ofFiles(const std::string& directoryName);

	std::vector<std::string> get_FullPathNames_ofSubDirectories(const std::string& rootDirectoryName);

	std::vector<std::string> get_SubDirectoryNames(const std::string& directoryPathName);

	std::string get_ParentPathName(const std::string& pathName);

	std::string get_FolderName(const std::string& dirPatName);

	std::vector<std::string> FilePathNames(const std::string& baseDirectory, const std::string& copyDirectory, const std::string& extension);

	std::vector<std::string> FilePathNames_fromFolders(const std::string& baseDirectory, const std::string& copyDirectory, const std::string& extension);
}

#endif
