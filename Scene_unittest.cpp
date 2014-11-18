#include <limits.h>
#include "Scene.h"
#include "gtest/gtest.h"

//Parsing Test

TEST(IsRandomPointTest, LimiteValues) {
	Scene scene("Immersive-3D visualization for astronomical data",
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		vm.count("oculus"),
		vm.count("fullscreen"),
		vm["texture"].as<std::string>(),
		vm["number"].as<unsigned long>(),
		vm["size"].as<int>(),
		vm["octantSize"].as<int>(),
		vm["octantDrawnCount"].as<int>(),
		vm["read"].as<std::string>(),
		vm["randomPercentage"].as<int>()
	);

	EXPECT_EQ(true, sc.isRandomPoint(100));
	EXPECT_EQ(false, sc.isRandomPoint(0));
}
