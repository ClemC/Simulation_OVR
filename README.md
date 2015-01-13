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

#Install Tutorial
Installation :
1. sudo apt-get install synaptic qtcreator clang libglew-dev libsdl2-dev libboost-all-dev libunittest++-dev
2. Lancer Synaptic (installé en 1.) -> Rechercher -> "sdl2 image library" -> Tout cocher (environ 4 éléments) -> Appliquer.
3. Lancer Synaptic -> Rechercher -> "sdl2" -> cocher les librairies avec sdl2 dedans (environ 10))
4. Décompresser "ovr_sdk_linux_0.3.2.tar.gz" vers le bureau
			Ensuite, en mode Administrateur, copier "./Bureau/OculusSDK/LibOVR/Lib/Linux/Release/i386"  dans "/usr/local/lib/"
5. Extraire CLEAN.zip sur le bureau (ou les fichiers de GIT). Vérifier qu'il y ait bien le fichier Simulation.pro
6. Dans qtCreator -> File -> Open File or Project -> Simulation_Stage_2014/Simulation.pro -> Yes
7. Dans qtCreator -> Tools -> Options -> Build & Run -> Compilers -> Add -> Clang -> 
		Compiler path = "/usr/bin/clang++" -> Apply.
8. Dans qtCreator -> Tools -> Options -> Build & Run -> Kit -> Add -> Name="Clang++",
		Debugger ="System GDB at /usr/bin/gdb", Compiler="Clang", QtVersion="Qt 5.3.0 in PATH (qt5)" ou QtVersion="Qt 5.2.1 in PATH (qt5)" -> Apply
9. Dans qtCreator -> Projects : "Configure Project" en bas à droite.
10. Dans qtCreator -> Projects -> Add Kit (en haut à gauche) -> "Clang++" -> Run en dessous de "Clang++"
11. Dans qtCreator -> Edit -> Selectionner "Simulation.pro" du projet de Philippe.
12. Dans qtCreator -> Flèche verte en bas

#GESTION DES ERREURS dans QtCreator: Si Erreur :
		* "SDL2/SDL.h not found" Alors Lancer Synaptic -> Rechercher "sdl2" et "sdl2 image library" -> Tout Cocher
		* "GL/glew.h file not found" Alors Lancer Synaptic -> Rechercher "glew" -> Cocher "libglew-dev"
		* "LogCpp/Log.h File not found" Alors Utiliser le fichier "CLEAN.zip" qui contient le fichier "LogCpp/Log.h"
		* "cannot find -lovr" -> Décompresser "ovr_sdk_linux_0.3.2.tar.gz" vers le bureau
				Ensuite, en mode Administrateur, copier "./Bureau/OculusSDK/LibOVR/Lib/Linux/Release/i386"  dans "/usr/local/lib/"
		* "cannot find -lboost_program_options" -> sudo apt-get install libboost-all-dev
		* "std::regex..." -> sudo add-apt-repository ppa:ubuntu-toolchain-r/test;sudo apt-get update;sudo apt-get install g++-4.9
		* "Error creating the OpenGL context" ->
				Si ordi avec carte graphique : Utiliser les pilotes propriétaires NVidia.
				Si ordi sans carte graphique : Installer "intel-linux-graphics-installer_1.0.6-0intel1_i386.deb" -> 
						Lancer intel-linux-graphics -> Upgrade -> Reboot
		* "g++ 4.9 missing"
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
