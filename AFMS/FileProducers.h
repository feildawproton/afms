#ifndef FILEPRODUCERS_H
#define FILEPRODUCERS_H

//only inculde the stuff needed for the interface
#include <string>
#include <vector>

//it is stateless
//it has SIDE EFFECTS (writting to disk)
//I like long function names apparently
namespace File_System
{
	//DEFINITIONS FOR FUNCTIONS NOT USED IN OTHER COMPILATIONS UNITS (ie not used outside of FileProcesses.cpp) NEED TO BE JUDICIOUSLY REMOVED
	
	std::vector<std::string> copy_DirectoryStructure_toDirectory(const std::string& directoryName, const std::string& copyDirName);

	std::vector<std::string> create_FilePathNames(const std::string& baseDirectory, const std::string& copyDirectory, const std::string& extension);

	std::vector<std::string> create_FilePathNames_fromFolders(const std::string& baseDirectory, const std::string& copyDirectory, const std::string& extension);

	std::string create_Directory(const std::string& dirName);

	bool copyFile(const std::string& from, const std::string& to);


}


#endif