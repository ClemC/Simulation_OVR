#ifndef FILE_H
#define FILE_H

#include <iostream>
#include <string>
#include <regex>
#include "LogCpp/Log.h"

class File
{
public:
    File();
    File(std::string filename, int dimension);
    void parseText();
    void exists_test();
    int getCount(std::string text, std::regex regex);
    std::sregex_iterator split(std::string text, std::regex regex);
    std::sregex_iterator checkIfExceptionInFile(std::string text, std::regex regex, int* i);
    void registerData(std::sregex_iterator words_begin, std::sregex_iterator words_end, int i, bool skip);
    void printData();

private:
    std::string filename_;
    int dimension_;
    int totalLines_;
    int lineDimension_ = 3; // this is to automatically detect the dimension of the file.
    double** data_;
};

#endif // FILE_H
