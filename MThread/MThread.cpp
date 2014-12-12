#include "MThread/MThread.h"
#include "File.h"

MThread::MThread()
{
}

MThread::MThread(File f):
    file_ {f}
{
}

void MThread::load() {
    // A. Define varriables
    int i=0, n=file_.getTotalLines();
    double **data = file_.getData();
    double x, y, z;

    // B. Main loop : associate each point to one cube.
    for (i=0; i<=n - 1; i++) {
        x = data[i][file_.xpos]; // return data[i][file_.xpos]*(size_ - 1)
        y = data[i][file_.ypos];
        z = data[i][file_.zpos];

        // C. Test position of the point
        update(x, y, z, data[i]);
    }
}

void MThread::print() {
    int i;
    cout << "\nCube id\tPoints\tPercentage";
    for (i=1; i<=27; i++) {
        cout << "\nCube "<<i<<"\t";
        printCube(getCube(i));
    }
    cout << "\n";
//    exit(77);
}

void MThread::printCube(std::vector<double*> cube) {
    double cs = cube.size();
    int tot = file_.getTotalLines();
    cout << cs << "\t" << (double)(100*cs/tot) << " %";
//    for(std::vector<double*>::iterator it = cube.begin(); it != cube.end(); ++it) {
//        std::cout << *(it)[0]<<", ";
//    }
}

void MThread::printCube(int nCube, int i, double x, double y, double z) {
    cout <<"["<<i<<"] ("<<x << ", " <<y << ", " <<z << ")\tCube "<<nCube<<"\t";
    printCube(getCube(nCube));
    cout << "\n";
}

// todo : use e to preload before entering in a new cube.
void MThread::update(double x, double y, double z, double* data) { //, double e
    if (x<ONETHIRD) {
        if (y<ONETHIRD) {
            if (z<ONETHIRD) {
                cubeSO1.push_back(data);
            } else if (z<TWOTHIRDS){
                cubeSO2.push_back(data);
            } else {
                cubeSO3.push_back(data);
            }
        } else if (y<TWOTHIRDS) {
            if (z<ONETHIRD) {
                cubeO1.push_back(data);
            } else if (z<TWOTHIRDS){
                cubeO2.push_back(data);
            } else {
                cubeO3.push_back(data);
            }
        } else {
            if (z<ONETHIRD) {
                cubeNO1.push_back(data);
            } else if (z<TWOTHIRDS){
                cubeNO2.push_back(data);
            } else {
                cubeNO3.push_back(data);
            }
        }
    } else if (x<TWOTHIRDS) {
        if (y<ONETHIRD) {
            if (z<ONETHIRD) {
                cubeS1.push_back(data);
            } else if (z<TWOTHIRDS){
                cubeS2.push_back(data);
            } else {
                cubeS3.push_back(data);
            }
        } else if (y<TWOTHIRDS) {
            if (z<ONETHIRD) {
                cubeC1.push_back(data);
            } else if (z<TWOTHIRDS){
                cubeC2.push_back(data);
            } else {
                cubeC3.push_back(data);
            }
        } else {
            if (z<ONETHIRD) {
                cubeN1.push_back(data);
            } else if (z<TWOTHIRDS){
                cubeN2.push_back(data);
            } else {
                cubeN3.push_back(data);
            }
        }
    } else {
        if (y<ONETHIRD) {
            if (z<ONETHIRD) {
                cubeSE1.push_back(data);
            } else if (z<TWOTHIRDS){
                cubeSE2.push_back(data);
            } else {
                cubeSE3.push_back(data);
            }
        } else if (y<TWOTHIRDS) {
            if (z<ONETHIRD) {
                cubeE1.push_back(data);
            } else if (z<TWOTHIRDS){
                cubeE2.push_back(data);
            } else {
                cubeE3.push_back(data);
            }
        } else {
            if (z<ONETHIRD) {
                cubeNE1.push_back(data);
            } else if (z<TWOTHIRDS){
                cubeNE2.push_back(data);
            } else {
                cubeNE3.push_back(data);
            }
        }
    }
}

std::vector<double*> MThread::getCube(int i) { // 26 threads max
    switch(i)
    {
    case 1:
        return cubeSO1;
        break;
    case 2:
        return cubeS1;
        break;
    case 3:
        return cubeSE1;
        break;
    case 4:
        return cubeO1;
        break;
    case 5:
        return cubeC1;
        break;
    case 6:
        return cubeE1;
        break;
    case 7:
        return cubeNO1;
        break;
    case 8:
        return cubeN1;
        break;
    case 9:
        return cubeNE1;
        break;
    case 10:
        return cubeSO2;
        break;
    case 11:
        return cubeS2;
        break;
    case 12:
        return cubeSE2;
        break;
    case 13:
        return cubeO2;
        break;
    case 14:
        return cubeC2;
        break;
    case 15:
        return cubeE2;
        break;
    case 16:
        return cubeNO2;
        break;
    case 17:
        return cubeN2;
        break;
    case 18:
        return cubeNE2;
        break;
    case 19: // 3rd floor
        return cubeSO3;
        break;
    case 20:
        return cubeS3;
        break;
    case 21:
        return cubeSE3;
        break;
    case 22:
        return cubeO3;
        break;
    case 23:
        return cubeC3;
        break;
    case 24:
        return cubeE3;
        break;
    case 25:
        return cubeNO3;
        break;
    case 26:
        return cubeN3;
        break;
    case 27:
        return cubeNE3;
        break;
    default:
        cout << "getCube: E: Invalid range for i. i must be in [1, 27] : i="<<i<<"\n";
        exit(1);
    }
}

/**
 * @brief getCubeNumber The cube number, such as 1<=res<=27
 * @param x
 * @param y
 * @param z
 * @return
 */
int MThread::getCubeNumber(double x, double y, double z) {
    int res=1;
    if (x<ONETHIRD) {
        if (y<ONETHIRD) {
            if (z<ONETHIRD) {
                res = 1;
            } else if (z<TWOTHIRDS){
                res = 10;
            } else {
                res = 19;
            }
        } else if (y<TWOTHIRDS) {
            if (z<ONETHIRD) {
                res = 4;
            } else if (z<TWOTHIRDS){
                res = 13;
            } else {
                res = 22;
            }
        } else {
            if (z<ONETHIRD) {
                res = 7;
            } else if (z<TWOTHIRDS){
                res = 16;
            } else {
                res = 25;
            }
        }
    } else if (x<TWOTHIRDS) {
        if (y<ONETHIRD) {
            if (z<ONETHIRD) {
                res = 2;
            } else if (z<TWOTHIRDS){
                res = 11;
            } else {
                res = 20;
            }
        } else if (y<TWOTHIRDS) {
            if (z<ONETHIRD) {
                res = 5;
            } else if (z<TWOTHIRDS){
                res = 14;
            } else {
                res = 23;
            }
        } else {
            if (z<ONETHIRD) {
                res = 8;
            } else if (z<TWOTHIRDS){
                res = 17;
            } else {
                res = 26;
            }
        }
    } else {
        if (y<ONETHIRD) {
            if (z<ONETHIRD) {
                res = 3;
            } else if (z<TWOTHIRDS){
                res = 12;
            } else {
                res = 21;
            }
        } else if (y<TWOTHIRDS) {
            if (z<ONETHIRD) {
                res = 6;
            } else if (z<TWOTHIRDS){
                res = 15;
            } else {
                res = 24;
            }
        } else {
            if (z<ONETHIRD) {
                res = 9;
            } else if (z<TWOTHIRDS){
                res = 18;
            } else {
                res = 27;
            }
        }
    }

    return res;
}
