#include "../../Header/Yazf.hpp"
#include <iostream>
#include <filesystem>
#include <string>
#include <stdexcept>
#include <fstream>
void LoopFolder(YAZF::Yazf& yazfFile, std::filesystem::path Directory, std::filesystem::path currentPath)
{
        const std::filesystem::path currentPath2 = std::filesystem::current_path();
    try
    {
        for (const auto &entry : std::filesystem::directory_iterator(Directory))
        {
            if (entry.is_regular_file())
            {
                std::filesystem::path EntryPath = entry.path();

                std::filesystem::path RelativePath = std::filesystem::relative(EntryPath.parent_path(),currentPath);
                std::filesystem::path filename = EntryPath.filename();
                std::ifstream file(entry.path());
                if (!file)
                {
                    std::cerr << "Failed to open: " << entry.path() << "\n";
                    continue;
                }
                std::string line;
                std::string ctx = "";
                while (std::getline(file, line))
                {
                    ctx += line + "\n"; // process each line
                }
                YAZF::File TheFile(RelativePath.c_str(), filename.c_str(), ctx);
                yazfFile.Pushfile(TheFile);
            }else if(entry.is_directory())
            {
               LoopFolder(yazfFile, entry,currentPath); 
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "fs error: " << e.what() << std::endl;
    }
}
void WriteExample()
{
    const std::filesystem::path currentPath = std::filesystem::current_path();
    YAZF::Yazf yazfFile = YAZF::Yazf::NewWithRandomEncryptionKey();
    const std::string Directory = "./input";
    LoopFolder(yazfFile, Directory,currentPath);
    std::filesystem::path NewFilePath = "./output/example.yazf";
    yazfFile.Write(NewFilePath);
}
void ReadExample()
{
    std::filesystem::path File = "./output/example.yazf";
    YAZF::Yazf ExampleFile =  YAZF::Yazf::Parse(File);  
}
int main()
{
     WriteExample();
    ReadExample();
    return 0;
}