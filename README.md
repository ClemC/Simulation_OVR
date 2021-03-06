Immersive-3D visualization for astronomical data
=====================

OpenGL 3.3 visualization of stars with Oculus Rift mode.

Research and Development Project for the Observatoire Astronomique de Strasbourg.
Tested on Linux but should work on other plateforms too with little to no changes.

This project is in no means a model to follow if you're developping an OpenGL application, but is rather a proof-of-concept and demonstration
of what you can do with the Oculus Rift.

##Install

- Clone the repo
- `git submodule init` and `git submodule update`
- Update your graphic card drivers
- Install the Oculus SDK 0.3.2 for your plateform (the files included are for the Linux Oculus SDK).
- Install the SDL2 library
- Install the SDL2 Image library
- Install the Boost Program options library (tested with 1.55)
- Make sure you have a c++11 compliant compiler (clang works fine).
- Compile (either qmake or make - the Makefile here was generated by qmake for Linux)
- Plug-in your Oculus Rift if you have one (tested with DK1, should work with DK2)
- Launch (./Simulation)

#Install tutorial
Installation :
- 1. sudo apt-get install synaptic qtcreator clang libglew-dev libsdl2-dev libboost-all-dev libunittest++-dev
- 2. Run Synaptic (installed in 1.) -> Rechercher -> "sdl2 image library" -> Check all (about 4 elements) -> Apply.
- 3. Run Synaptic -> Rechercher -> "sdl2" -> check librairies which contains "sdl2" (about 10))
- 4. Unpack "ovr_sdk_linux_0.3.2.tar.gz" on the Desktop.
			Then, in administrator mode, copy "./Bureau/OculusSDK/LibOVR/Lib/Linux/Release/i386/*" to "/usr/local/lib/"
- 5. Extract git files. Check if the file "Simulation.pro" exist.
- 6. In QtCreator -> File -> Open File or Project -> Simulation_Stage_2014/Simulation.pro -> Yes
- 7. In qtCreator -> Tools -> Options -> Build & Run -> Compilers -> Add -> Clang -> 
		Compiler path = "/usr/bin/clang++" -> Apply.
- 8. In qtCreator -> Tools -> Options -> Build & Run -> Kit -> Add -> Name="Clang++",
		Debugger ="System GDB at /usr/bin/gdb", Compiler="Clang", QtVersion="Qt 5.3.0 in PATH (qt5)" or QtVersion="Qt 5.2.1 in PATH (qt5)" -> Apply
- 9. In qtCreator -> Projects : "Configure Project", bottom right.
- 10. In qtCreator -> Projects -> Add Kit (en haut à gauche) -> "Clang++" -> Run below "Clang++"
- 11. In qtCreator -> Edit -> Select "Simulation.pro".
- 12. In qtCreator -> Green arrow at the bottom.

# Error Handling in QtCreator :
Error list :
-  "SDL2/SDL.h not found" Alors Lancer Synaptic -> Rechercher "sdl2" et "sdl2 image library" -> Check all
-  "GL/glew.h file not found" Then with Synaptic -> Search "glew" -> Check "libglew-dev"
-  "LogCpp/Log.h File not found" Then use the git file which contains "LogCpp/Log.h"
-  "cannot find -lovr" -> Unpack "ovr_sdk_linux_0.3.2.tar.gz" to the desktop.
				Then, in administrator mode, copy "./Bureau/OculusSDK/LibOVR/Lib/Linux/Release/i386/*" to "/usr/local/lib/"
-  "cannot find -lboost_program_options" -> sudo apt-get install libboost-all-dev
-  "std::regex..." -> sudo add-apt-repository ppa:ubuntu-toolchain-r/test;sudo apt-get update;sudo apt-get install g++-4.9
-  "Error creating the OpenGL context" ->
				If the computer have a graphical card, use proprietary drivers from NVidia.
				Else, install "intel-linux-graphics-installer_1.0.6-0intel1_i386.deb" -> 
						Run intel-linux-graphics -> Upgrade -> Reboot
-  "g++ 4.9 missing"
			sudo add-apt-repository ppa:ubuntu-toolchain-r/test;sudo apt-get update
			sudo apt-get install g++-4.9


## Command line options

```
-h [ --help ]                         Produce help message
-o [ --oculus ]                       Oculus mode
-f [ --fullscreen ]                   Fullscreen mode
-t [ --texture ] arg (=Textures/photorealistic/photorealistic_marble/granit01.jpg)
                                      Set the texture used on the cubes
-n [ --number ] arg (=1024)           Set the number of objects seen
-s [ --size ] arg (=128)              Set the size of the data cube. Must be
                                      a power of 2
--octantSize arg (=8)                 Set the size of an octant. Must be a
                                      power of 2
-d [ --octantDrawnCount ] arg (=2)    Set the number of octant drawn count. 1
                                      to only draw the octant the camera is
                                      currently in, 2 to draw the immediate
                                      neighbors, ...
-r [ --read ] arg (="./File/star.txt")Set the file to read
-a [ --randomPercentage ] arg (=100)  Set the percentage of points to display with the random algorithm. If 100, do not use random method
-c [ --clusteringPercentage ] arg (=100)         
				      Set the percentage of points to display with the hierarchical clustering algorithm. If 100, do not use clustering method
-m [ --isMultiThread ] arg (=false)    Enable multithreading
-- renderTypeSimplified arg (=false)   True for half plane loading, and false for vision cone loading

```

Examples:

```
    ./Simulation -t Textures/photorealistic/photorealistic_marble/granit08.jpg
   ./Simulation -t Textures/photorealistic/photorealistic_marble/granit06.jpg -n 1000
   ./Simulation
   ./Simulation -h
   ./Simulation -d 1
   ./Simulation -d 2
   ./Simulation -s 64
   ./Simulation --octantSize 4

```

##How it works

The scene is an Octree which stores the objects. Only the octant we are actually in is displayed. By tweaking the value of a parameter you can also 
display the neighbour octants.

##Performance
On my computer (16 Gb RAM, 1 Gb VRAM, RADEON HD 8570) it runs at around 30 FPS constant.
The initial generation takes around 4s for 1024 objects and is linear in the number of objects.

##Documentation
Type `doxygen` in console and it should generate the documentation following the `Doxyfile` file.

##Logs
Logs can be filtered. Have a look at `log.log` and `log.err`.
