#include <unittest++/UnitTest++.h>
#include "Scene.h"

/*
 1. Install UnitTest with "sudo apt-get install libunittest++-dev" or "http://dragly.org/2013/04/19/setting-up-unittest-with-qt-creator/"
 2. Build -> Clean All
 3. Comment main in main.cpp and uncomment All in this file. Compile in Qt Creator => "Success: 1 tests passed."
 (Optionnal 3.) Use "http://unittest-cpp.sourceforge.net/UnitTest++.html" to find correct syntax.
*/

//TEST(SampleTest) {
//    UNITTEST_TIME_CONSTRAINT(50); // fail if test is longer than 50 ms !
//    CHECK(true);
//    CHECK_EQUAL("foo", "foo");
//}



int run(std::string filename, int size) {
    int nOctants = 8, percent = 100;
    bool b = false, oculus = 0, fullscreen = 0;
    int res = -1;

    cout << "Testing "<<filename<<" with a cube of size 1024 ... [expected 0.1s]";
    Scene scene("Immersive-3D visualization for astronomical data", WINDOW_WIDTH, WINDOW_HEIGHT,
        oculus,
        fullscreen,
        "Textures/photorealistic/photorealistic_marble/granit01.jpg",
        size,
        size,
        nOctants,
        nOctants,
        filename,
        percent,
        percent,
        b,
        b
        );
    scene.mainLoopTest();
    res = 1;
    return res;
}

TEST(SceneTest) {
std::string filename = "./File/star_00012";
//    CHECK_EQUAL(1, run(filename, 1024));
//    CHECK_EQUAL(1, run(filename, 2048));
//    CHECK_EQUAL(1, run(filename, 4096));
//    CHECK_EQUAL(1, run(filename, 8192));

filename = "./File/star_00013";
//    CHECK_EQUAL(1, run(filename, 1024));
//    CHECK_EQUAL(1, run(filename, 2048));
//    CHECK_EQUAL(1, run(filename, 4096));
//    CHECK_EQUAL(1, run(filename, 8192));

filename = "./File/star_00014";
//    CHECK_EQUAL(1, run(filename, 1024));
//    CHECK_EQUAL(1, run(filename, 2048));
//    CHECK_EQUAL(1, run(filename, 4096));
//    CHECK_EQUAL(1, run(filename, 8192));

filename = "./File/star_00016";
//    CHECK_EQUAL(1, run(filename, 1024));
//    CHECK_EQUAL(1, run(filename, 2048));
//    CHECK_EQUAL(1, run(filename, 4096));
//    CHECK_EQUAL(1, run(filename, 8192));

filename = "./File/star_00017";
//    CHECK_EQUAL(1, run(filename, 1024));
//    CHECK_EQUAL(1, run(filename, 2048));
//    CHECK_EQUAL(1, run(filename, 4096));
//    CHECK_EQUAL(1, run(filename, 8192));

filename = "./File/star_00020";
//    CHECK_EQUAL(1, run(filename, 1024));
//    CHECK_EQUAL(1, run(filename, 2048));
//    CHECK_EQUAL(1, run(filename, 4096));
//    CHECK_EQUAL(1, run(filename, 8192));

filename = "./File/star_00025";
    CHECK_EQUAL(1, run(filename, 1024));
//    CHECK_EQUAL(1, run(filename, 2048));
//    CHECK_EQUAL(1, run(filename, 4096));
//    CHECK_EQUAL(1, run(filename, 8192));

filename = "./File/star_00030";
//    CHECK_EQUAL(1, run(filename, 1024));
//    CHECK_EQUAL(1, run(filename, 2048));
//    CHECK_EQUAL(1, run(filename, 4096));
//    CHECK_EQUAL(1, run(filename, 8192));
}


// Uncomment this to use UnitTest++
//int main()
//{
//    UnitTest::RunAllTests();
//}
