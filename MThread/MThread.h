#ifndef MTHREAD_H
#define MTHREAD_H

#include <iostream>
#include <string>
#include "LogCpp/Log.h"
#include "File.h"

class MThread
{
public:
    // constructor
    MThread();
    MThread(File f);

    // accessor

    // method
    void load();
    std::vector<double*> getCube(int i);
    int getCubeNumber(double x, double y, double z);
    void print();
    void printCube(std::vector<double*> cube);
    void printCube(int nCube, int i, double x, double y, double z);
    void update(double x, double y, double z, double* data); //, double e

private:
    const double ONETHIRD = (double)1/3;
    const double TWOTHIRDS = (double)2/3;
    // Fields
    File file_;
    std::vector<double*>
    cubeSO1, cubeS1, cubeSE1, cubeO1, cubeC1, cubeE1, cubeNO1, cubeN1, cubeNE1,
    cubeSO2, cubeS2, cubeSE2, cubeO2, cubeC2, cubeE2, cubeNO2, cubeN2, cubeNE2, //2nd floor
    cubeSO3, cubeS3, cubeSE3, cubeO3, cubeC3, cubeE3, cubeNO3, cubeN3, cubeNE3;
};

#endif // MTHREAD_H
