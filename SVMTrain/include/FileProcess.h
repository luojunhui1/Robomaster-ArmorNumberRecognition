//
// Created by root on 2021/2/2.
//

#ifndef FILEPROCESS_H
#define FILEPROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <utility>
/**structure in dirent.h for reading directories**/
//struct dirent
//{
//    long d_ino; /* inode number 索引节点号 */
//    off_t d_off; /* offset to this dirent 在目录文件中的偏移 */
//    unsigned short d_reclen; /* length of this d_name 文件名长 */
//    unsigned char d_type; /* the type of d_name 文件类型 */
//    char d_name [NAME_MAX+1]; /* file name (null-terminated) 文件名，最长255字符 */
//}

class FileReader
{
public:
    std::string basePath;
    std::string fileName;
    std::string suffix;
    int count = 0;
public:
    FileReader(const char* basePath, std::string suffix);
    FileReader(){};
    ~FileReader();
    bool findNext();
};

inline FileReader::FileReader(const char *basePath, std::string suffix = ".jpg")
{
    this->basePath = basePath;
    this->suffix = std::move(suffix);
    count = 0;
}

inline bool FileReader::findNext()
{
//    std::string command = "ls " + basePath + "num_" + std::to_string(count++) + suffix +"|wc -l";
//    int num = system(command.c_str());
    int num  = 1;
    if(num == 1)
    {
        fileName = basePath + "num_" + std::to_string(count++) + suffix;
        return true;
    }
    else if(num > 1)
        std::cout<<"[WARNING]: the amount of target file in directory more than 1,  COUNT: "<<count - 1;
    else if(num == 0)
        std::cout<<"[WARNING]: there are no target file in directory, COUNT: "<<count - 1;
    return false;
}
inline FileReader::~FileReader()
{

}
#endif //FPGA_FILEPROCESS_H
