#ifndef FILE_H
#define FILE_H

#include <iostream>
#include <string>
#include <regex>
#include "LogCpp/Log.h"

class File
{
public:
    // constructor
    File();
    File(std::string filename);

    // static
    int mass = 1;    // column with mass
    int xpos = 2;       // column with x
    int ypos = 3;       // column with y
    int zpos = 4;       // column with z
    int age = 5;     // column with age

    // accessor
    int getTotalLines();
    double** getData();

    // method
    void parseText();
    void exists_test();
    int getCount(std::string text, std::regex regex);
    std::sregex_iterator split(std::string text, std::regex regex);
    std::sregex_iterator checkIfExceptionInFile(std::string text, std::regex regex, int* i);
    void registerData(std::sregex_iterator words_begin, std::sregex_iterator words_end, int i, bool skip);
    void printData();

private:
    // static
    int lineDimension_ = 3; // this is to automatically detect the dimension of the file.

    std::string filename_;
    int dimension_;
    int totalLines_;
    double** data_;
};

#endif // FILE_H
