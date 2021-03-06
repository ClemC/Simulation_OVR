#include "LogCpp/Log.h"
#include "Scene.h"

#include <boost/program_options.hpp>
namespace po = boost::program_options;

int main(int argc, char** argv)
{
    logger->debug(logger->get() << "Command line arguments: " << argc-1);

    try {
        //128 2 8
        po::options_description desc("Allowed options");
        desc.add_options()
                ("help,h", "Produce help message")
                ("oculus,o", "Oculus mode")
                ("fullscreen,f", "Fullscreen mode")
                ("texture,t", po::value<std::string>()->default_value("Textures/photorealistic/photorealistic_marble/granit01.jpg"), "Set the texture used on the cubes")
                ("number,n", po::value<unsigned long>()->default_value(1024), "Set the number of objects seen") // useless with file.
                ("size,s", po::value<int>()->default_value(1024), "Set the size of the data cube. Must be a power of 2") // Up to 8192 with 20fps !
                ("octantSize", po::value<int>()->default_value(8), "Set the size of an octant. Must be a power of 2")
                ("octantDrawnCount,d", po::value<int>()->default_value(8), "Set the number of octant drawn count. 1 to only draw the octant the camera is currently in, 2 to draw the immediate neighbors, ...")
                ("read,r", po::value<std::string>()->default_value("./File/star_00020"), "Set the file to read.") // set "./File/star.txt" or "" here.
                ("randomPercentage,a", po::value<int>()->default_value(100), "Set the percentage of points to display with the random algorithm. If 100, do not use random method.")
                ("clusteringPercentage,c", po::value<int>()->default_value(100), "Set the percentage of points to display with the hierarchical clustering algorithm. If 100, do not use clustering method.")
                ("isMultiThread,m", po::value<bool>()->default_value(false), "Enable multithreading.")
                ("renderTypeSimplified", po::value<bool>()->default_value(false), "True for half plane loading, and false for vision cone loading")
                ;

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if(vm.count("help"))
        {
            std::cout << desc << std::endl;
            return 0;
        }
        Scene scene("Immersive-3D visualization for astronomical data", WINDOW_WIDTH, WINDOW_HEIGHT,
                    1,
                    1,
                    vm["texture"].as<std::string>(),
                vm["number"].as<unsigned long>(),
                vm["size"].as<int>(),
                vm["octantSize"].as<int>(),
                vm["octantDrawnCount"].as<int>(),
                vm["read"].as<std::string>(),
                vm["randomPercentage"].as<int>(),
                vm["clusteringPercentage"].as<int>(),
                vm["isMultiThread"].as<bool>(),
                vm["renderTypeSimplified"].as<bool>()
                );
        scene.mainLoop();
    }
    catch(exception& e) {
        cerr << "error: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        cerr << "Exception of unknown type!\n";
    }


    return 0;
}
