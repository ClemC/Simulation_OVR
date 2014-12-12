//#include <unittest++/UnitTest++.h>
//#include "MThread/MThread.h"
//#include "File.h"

///**
// * This test will load a file of 27 lines in which each line belongs to one of the 27 sub cubes.
//**/
//TEST(loadTest) {
//    File file("./File/test.txt");
//    file.parseText(true);
//    CHECK_EQUAL(27, file.getTotalLines());
//    MThread mt(( MThread(file) ));
//    mt.load(); // split data into 27 cubes
//    mt.print();

//    int i;
//    for (i=1; i<=27; i++) {
//        CHECK_EQUAL(1, mt.getCube(i).size());
//    }
//}

///**
// * This test is aimed to check some subcubes and the correctness of getCubeNumberTest method.
//**/
//TEST(getCubeNumberTest) {
//    File file("./File/test.txt");
//    file.parseText(true);
//    CHECK_EQUAL(27, file.getTotalLines());
//    MThread mt(( MThread(file) ));
//    mt.load(); // split data into 27 cubes
////    mt.print();

//    CHECK_EQUAL(16, mt.getCubeNumber(0.1, 0.7, 0.6));
//    CHECK_EQUAL(4, mt.getCubeNumber(0.2, 0.4, 0.1));
//    CHECK_EQUAL(9, mt.getCubeNumber(0.9, 0.7, 0.2));
//    CHECK_EQUAL(23, mt.getCubeNumber(0.6, 0.5, 0.8));
//    CHECK_EQUAL(21, mt.getCubeNumber(0.7, 0.1, 0.9));
//}

//int main()
//{
//    UnitTest::RunAllTests();
//}
