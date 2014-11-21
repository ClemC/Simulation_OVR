#ifndef FILE_H
#define FILE_H

#include <iostream>
#include <string>
#include <regex>
#include <random>
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
    int getTotalLinesCah();
    double getAge();
    double** getData();
    double** getDataCah();

    // method
    /* parsing */
    void parseText();
    void exists_test();
    int getCount(std::string text, std::regex regex);
    std::sregex_iterator split(std::string text, std::regex regex);
    std::sregex_iterator checkIfExceptionInFile(std::string text, std::regex regex, int* i);
    void registerData(std::sregex_iterator words_begin, std::sregex_iterator words_end, int i, bool skip);
    void printData();
    /* hierarchical clustering */
    void cah(int cluster);
    double dissimMax(std::vector<int> c1, std::vector<int> c2);
    double dissimG(std::vector<int> c1, std::vector<int> c2);
    double dissimAlea(std::vector<int> c1, std::vector<int> c2);
    std::vector<int> minMatDissim (double **m, int size);
    void registerClusters();
    void printDataCah();

private:
    // static
    int lineDimension_ = 3; // this is to automatically detect the dimension of the file.

    // fields
    std::string filename_;
    double age_;
    int dimension_;
    int totalLines_;
    double** data_;
    int totalLinesCah_;
    double** dataCah_;
    std::vector<std::vector<int>> clusters_;
};

#endif // FILE_H
