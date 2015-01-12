#include "Scene.h"
#include "Cube.h"
#include "Input.h"
#include "Crate.h"
#include "Texture.h"
#include "Camera.h"
#include "GraphicObject.h"
#include "Plane.h"
#include "Utils.h"
#include "LogCpp/Log.h"

#include "File.h"
#include "MThread/MThread.h"

#include <unistd.h>
#include <numeric>
#include <random>
#include <chrono>
#include <thread>

std::unique_ptr<Logger> logger(new Logger("log.log", "log.err", true, Severity::error | Severity::info));
std::unique_ptr<NullOculus> nullOculus(new NullOculus);

Scene::Scene(std::string windowTitle, int windowWidth, int windowHeight, bool oculusRender, bool fullscreen,
             std::string textureName, unsigned long objectsCount, int size, int octantSize,
             int  octantsDrawnCount, std::string filename, int randomPercentage, int clusteringPercentage,
             bool isMultiThread, bool renderTypeSimplified):
    gObjectsCount_ {objectsCount}, // R&D: if reading files, should be set to the number of lines.
    size_ {size},
    //1 to only draw the octant the camera is in, 2 to draw the immediate neighbours, etc. Power of 2
    octantsDrawnCount_ {octantsDrawnCount},
    octantSize_  {octantSize},
    windowTitle_ {windowTitle},
    windowWidth_ {windowWidth},
    windowHeight_ {windowHeight},
    window_ {nullptr},
    gObjects_ {size_},
    fullscreen_ {fullscreen},
    oculusRender_ {oculusRender},
    fps_ {0},
    frameCount_ {0},
    textureName_ {textureName},
    filename_ {filename},
    randomPercentage_ {randomPercentage},
    clusteringPercentage_ {clusteringPercentage},
    isMultiThread_ {isMultiThread},
    renderTypeSimplified_ {renderTypeSimplified}
{
    logger->trace(logger->get() << "Scene constructor");
    input_ = std::unique_ptr<Input>(new Input(this));
    input_->showCursor(false);
    input_->capturePointer(true);
    gObjects_.setEmptyValue(std::shared_ptr<NullGraphicObject>(new NullGraphicObject));
    file_ = parseFile();
//    locked_=false;
//    mthread_ = getThread();
    getThread();
    camera_ = std::unique_ptr<Camera>(new Camera(
                                          glm::vec3(size_/2, size_/2, size_/2),
                                          glm::vec3(0, 0, 0), glm::vec3(0, 1, 0),
                                          0.5,
                                          0.2,
                                          *(input_.get()),
                                          gObjects_
                                          ));


    assert(initWindow());
    logger->debug(logger->get() << "Window was initialized");

    assert(initGL());
    logger->debug(logger->get() << "OpenGL was initialized");

    if(oculusRender_)
    {
        input_->setOculus(std::unique_ptr<GenericOculus>(new Oculus<Scene>(*this)));
        logger->debug(logger->get() << "Oculus view");
    }

    initGObjects();
}

Scene::~Scene()
{
    TextureFactory::destroyTextures();

    SDL_GL_DeleteContext(context_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

bool Scene::initWindow()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        logger->error(logger->get() << "Error opening the SDL : " << SDL_GetError());
        SDL_Quit();

        return false;
    }

    //Version
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Double Buffer
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
    if(fullscreen_)
    {
        flags |= SDL_WINDOW_MAXIMIZED;
    }

    window_ = SDL_CreateWindow(windowTitle_.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth_, windowHeight_, flags);

    if(window_ == nullptr)
    {
        logger->error(logger->get() << "Error creating the window: " << SDL_GetError());
        SDL_Quit();

        return -1;
    }

    //Context
    context_ = SDL_GL_CreateContext(window_);

    if( ! context_)
    {
        logger->error(logger->get() << "Error creating the OpenGl context: " << SDL_GetError());
        SDL_DestroyWindow(window_);
        SDL_Quit();

        return false;
    }

    return true;
}

bool Scene::initGL()
{
    glewExperimental = GL_TRUE;
    GLenum initGLEW( glewInit() );
    if(initGLEW != GLEW_OK)
    {
        logger->error(logger->get() << "Error opening GLEW : " << glewGetErrorString(initGLEW));

        SDL_GL_DeleteContext(context_);
        SDL_DestroyWindow(window_);
        SDL_Quit();

        return false;
    }

    glEnable(GL_DEPTH_TEST);

    return true;
}

void Scene::initOrigin() { // original method with random points.
    std::default_random_engine generator;
    std::uniform_int_distribution<> distribution(0, size_ - 1);
    auto startGeneration = std::chrono::high_resolution_clock::now();

    for(ulong i=1; i <= gObjectsCount_; i++)
    {
        int x = distribution(generator);
        int y = distribution(generator);
        int z = distribution(generator);

        auto startCrateGeneration = std::chrono::high_resolution_clock::now();
        glm::vec4 dimensions(0.5,0.5,0.0,0.0);
        gObjects_(x, y, z) = std::shared_ptr<Crate>(new Crate(x, y, z, dimensions, 1.0, textureName_));
        auto endCrateGeneration = std::chrono::high_resolution_clock::now();

        logger->debug(logger->get() << "Generated crate n°" << i << " at position ("
                      << x << ", " << y << ", " << z << ") in "
                      << chrono::duration_cast<std::chrono::milliseconds>(endCrateGeneration - startCrateGeneration).count() << " ms");
    }
    auto endGeneration = std::chrono::high_resolution_clock::now();
    auto generationTime = std::chrono::duration_cast<std::chrono::milliseconds>(endGeneration - startGeneration).count();
    logger->info(logger->get() << "Summary: the generation of " << gObjectsCount_ << " graphic objects took " << generationTime << " ms");
}

/**
 * @brief initGObjectsFile Simple load of the file, with random algorythm if randomPercentage!=1.
 * Plot star.txt
 */
void Scene::initGObjectsFile() {
    int i=0, n=file_.getTotalLines();
    double **data = file_.getData();
    if(clusteringPercentage_!=100){
        int cluster = (int)((double)clusteringPercentage_ * (double)file_.getTotalLines() / 100.0);
        file_.cah(cluster);
        file_.registerClusters();
        n=file_.getTotalLinesCah();
        data=file_.getDataCah();
    }
    if (!isMultiThread_) {
        srand ( time(NULL) ); // seed for random algorithm.
        for (i=0; i<=n - 1; i++) {
            if (isRandomPoint(randomPercentage_)) { // if randomPercentage==100, always true.
                int x = file_.convert(data[i][file_.xpos]); // return data[i][file_.xpos]*(size_ - 1)
                int y = file_.convert(data[i][file_.ypos]);
                int z = file_.convert(data[i][file_.zpos]);
                double massV = data[i][file_.mass],
                        ageV = data[i][file_.age]; // textureBigStar_
                if (ageV==0) { // new stars must not be invisible
                    ageV = file_.MinCubeSize;
                }
                if (n>=10000) { // when too much stars are printed, it becomes unreadable.
                    ageV=file_.MinCubeSize;
                }
                addPointToOctree(x, y, z, massV, ageV);
            }
        }
    }
}

void Scene::addPointToOctree(int x, int y, int z, double massV, double ageV) {
//    ageV=0;
//    auto startCrateGeneration = std::chrono::high_resolution_clock::now();
    std::string texture;
    if (massV>bigStarLimit_) {
        texture = textureBigStar_;
    } else {
        texture = textureSmallStar_;
    }
    if (file_.getMaxMass()==0) {
        cout<<"addPointToOctree: internal error : file_.getMaxMass()=0\n";
        exit(244);
    }
//    glm::vec4 dimensions(1,1,0.0,0.0);
    glm::vec4 dimensions(massV/file_.getMaxMass(),ageV/file_.getMaxAge(),0.0,0.0);
//    cout<<"updateOctree: (x,y,z,ageV,texture,mass,age,file_.getMaxMass()) = ("<<x<<", "<<y<<", "<<z
//       <<", "<<ageV<<","<<texture<<","<<massV/file_.getMaxMass()<<", "<<ageV/file_.getMaxAge()<<","<<
//         file_.getMaxMass()<<")\n";
//    Crate* c = new Crate(1, 1, 1, dimensions, 1, texture);
    Crate* c = new Crate(x, y, z, dimensions, ageV, texture); // Impossible de créer un nouveau Crate avec une autre thread.
                                                            //Voir Debug après avoir activé le multithreading.
    gObjects_(x, y, z) = std::shared_ptr<Crate>(c);
//    }
    // D. Log
//    auto endCrateGeneration = std::chrono::high_resolution_clock::now();
//    logger->debug(logger->get() << "Generated crate n°" << i << " at position ("
//                  << x << ", " << y << ", " << z << ") in "
//                  << chrono::duration_cast<std::chrono::milliseconds>(endCrateGeneration - startCrateGeneration).count()
//                  << " ms");
}

/**
******************************************************
 * Generate the Scene.
 * @brief Scene::initGObjects
 */
void Scene::initGObjects()
{
    if (filename_.empty()) { // no file as a parameter
        initOrigin();
    } else {
        if (randomPercentage_<100) {
            logger->info(logger->get() << "Using Random Algorythm with randomPercentage = " << randomPercentage_);
        } else {
            logger->info(logger->get() << "Using basic Algorythm.");
        }
        initGObjectsFile();
    }
}

/**
 * @brief Scene::isRandomPoint Return true in n% of the time.
 * @param n
 * @return
 */
bool Scene::isRandomPoint(int n) {
    bool res = true;

    if (n<100) {
        int a = 0, b = 100;
        int r = rand()%(b-a) +a;
        res = r<n;
    }

    return res;
}

void Scene::mainLoop()
{
    int fpsDesired = 60;
    unsigned int frameRate = 1000 / fpsDesired;
    Uint32 start (0);
    Uint32 end  (0);
    Uint32 elapsedTime (0);
    blue_=0;

    while( ! input_->isOver())
    {
        start = SDL_GetTicks();

        input_->updateEvent();

        if(input_->isKeyboardKeyDown(SDL_SCANCODE_ESCAPE))
            break;

        if(oculusRender_)
        {
            input_->oculus()->render();
        }
        else
        {
            glClearColor(0, 0, blue_, 7); // red,green,blue
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            render();
        }

        if(!oculusRender_)
        {
            SDL_GL_SwapWindow(window_);
        }

        SDL_GL_SwapWindow(window_);
        //Wait for FPS
        end = SDL_GetTicks();
        elapsedTime = end - start;

        updateFPS(elapsedTime);

        if(elapsedTime < frameRate)
        {
            SDL_Delay(frameRate - elapsedTime);
        }

        frameCount_++;
    }

    doEnd();
}

void Scene::mainLoopTest()
{
    int fpsDesired = 60;
    unsigned int frameRate = 1000 / fpsDesired;
    Uint32 start (0);
    Uint32 end  (0);
    Uint32 elapsedTime (0);
    blue_=0;
        start = SDL_GetTicks();
        input_->updateEvent();
        if(oculusRender_)
        {
            input_->oculus()->render();
        }
        else
        {
            glClearColor(0, 0, blue_, 7); // red,green,blue
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            render();
        }
        if(!oculusRender_)
        {
            SDL_GL_SwapWindow(window_);
        }
        SDL_GL_SwapWindow(window_);
        //Wait for FPS
        end = SDL_GetTicks();
        elapsedTime = end - start;
        updateFPS(elapsedTime);
        if(elapsedTime < frameRate)
        {
            SDL_Delay(frameRate - elapsedTime);
        }
        frameCount_++;
    doEnd();
}

void Scene::doEnd()
{
    logger->info(logger->get() << "Mean fps at the end; " << fps_);
}

void Scene::render()
{
    glm::mat4 projection;
    glm::mat4 modelview;

    double ratio = static_cast<double>(windowWidth_ / windowHeight_);

    projection = glm::perspective(90.0, ratio, 0.01, 100.0);
    modelview = glm::mat4(2.0);

    Scene::render(modelview, projection);
}

double norm2xyz(float x, float y, float z){
    return sqrt(x*x+y*y+z*z);
}

// 1/4 Uncomment the line to see the effect of the vision cone
// double inc=0.0;

void Scene::render(glm::mat4 & MV, glm::mat4 & proj)
{
    int sizeToRender = octantSize_ * octantsDrawnCount_;

    double e = std::numeric_limits<double>::epsilon();
    octantsDrawnCount_ = camera_->move(glm::vec3(sizeToRender + e, sizeToRender + e, sizeToRender + e) ,
                                       glm::vec3(size_ - sizeToRender -e, size_ - sizeToRender -e, size_ - sizeToRender -e), file_);
    camera_->lookAt(MV);
    glm::vec3 v = camera_->eyeTarget() - camera_->position();
    int xp = camera_->position().x;
    int yp = camera_->position().y;
    int zp = camera_->position().z;
    double cos55 = 0.5735;
    // Traiter cette partie par un autre processus ? (si sizeToRender est énorme, il faut plus de temps avant de pouvoir bouger.)
    for(int z=zp - sizeToRender; z<zp + sizeToRender; z++)
    {
        for(int y=yp - sizeToRender; y<yp + sizeToRender; y++)
        {
            for(int x=xp - sizeToRender; x<xp + sizeToRender; x++)
            {

                if(renderTypeSimplified_){

                    // Half space
                    if(v.x*x+v.y*y+v.z*z-v.x*xp-v.y*yp-v.z*zp>0) {
                        gObjects_.at(x, y, z)->draw(proj, MV);
                    }

                }
                else{

                    // Vision angles
                    // theta <= 110°/2
                    // cos(theta) >= cos(55°) with cos(55°)=0.5735
                    // norm(v)*norm(p)*cos(theta) >= norm(v)*norm(p)*0.5735 with p=(x-xp,y-yp,z-zp)
                    // dotproduct(v,p) >= norm(v)*norm(p)*0.5735

                    // 2/4 Uncomment the line to see the effect of the vision cone
                    // cos55+=inc;
                    if((x-xp)*v.x+(y-yp)*v.y+(z-zp)*v.z >= norm2xyz(x-xp,y-yp,z-zp) * norm2xyz(v.x,v.y,v.z) * cos55){
                        gObjects_.at(x, y, z)->draw(proj, MV);
                    }
                    // 3/4 Uncomment the line to see the effect of the vision cone
                    // cos55-=inc;
                }

                /* Vision angles unoptimized
                glm::vec3 m(x,y,z);
                if(glm::dot(m - camera_->position(),v) >= norm2(m - camera_->position()) * norm2(v) * (0.5735+inc)){
                    gObjects_.at(x, y, z)->draw(proj, MV);
                }
                */

            }
        }
    }
    // 4/4 Uncomment the line to see the effect of the vision cone
    // inc+=0.001;
}

/**
 * Parse the file and return a reference to file_
 * @brief Scene::parseFile
 * @return
 */
File Scene::parseFile() {
    File file(( File(filename_) ));
    file.exists_test();
    file.parseText(true); // 'error: vector::_M_default_append' is here.
    //    cout << file.getXCenterTp() << file.getYCenterTp() << file.getZCenterTp();
    file.setSizeScene(size_);
    return file;
}

SDL_Window* Scene::window() const
{
    return window_;
}

int Scene::windowWidth() const
{
    return windowWidth_;
}

void Scene::setWindowWidth(int windowWidth)
{
    windowWidth_ = windowWidth;
}
int Scene::windowHeight() const
{
    return windowHeight_;
}

void Scene::setWindowHeight(int windowHeight)
{
    windowHeight_ = windowHeight;
}

void Scene::updateFPS(int elapsedTime)
{
    //Avoid dividing by zero
    elapsedTime == 0 ? elapsedTime = 1 : elapsedTime;
    int newFps = 1000 / elapsedTime;

    //Cumulative moving average
    fps_ = fps_ + (newFps - fps_) / (frameCount_ + 1.0);

    logger->debug(logger->get() << "Current fps: " << newFps);
    logger->debug(logger->get() << "Current cumulated mean fps: " << fps_);

    std::string newTitle = windowTitle_ + " | (" + std::to_string(camera_->position().x) + ", " +
            std::to_string(camera_->position().y) + ", " +
            std::to_string(camera_->position().z) + ") | (" +
            std::to_string(newFps) + "FPS)";
    SDL_SetWindowTitle(window_, newTitle.c_str());
}


//-------------------------------------------------------
// Multithreading
//-------------------------------------------------------

void Scene::updateOctree(std::vector<double*> cube) {
    int x, y, z;
    double massV, ageV = file_.MinCubeSize;
    double* t;
//    locked_=true;
        for(std::vector<double*>::iterator it = cube.begin(); it != cube.end(); ++it) {
            t=*(it);
            x = file_.convert(t[file_.xpos]);
            y = file_.convert(t[file_.ypos]);
            z = file_.convert(t[file_.zpos]);
            massV = t[file_.mass];
            ageV = t[file_.age]; // textureBigStar_
//            cout<<"updateOctree: (x,y,z,massV,ageV) = ("<<x<<", "<<y<<", "<<z<<", "<<massV<<", "<<ageV<<")\n";
            addPointToOctree(x, y, z, massV, ageV);
        }
//     locked_=false;
//    cout << "updateOctree: Updating Octree in a new Thread.\n";
}

void Scene::removeOctree(std::vector<double*> d) {
    gObjects_.setEmptyValue(std::shared_ptr<NullGraphicObject>(new NullGraphicObject));
//    cout << "removeOctree: Removing old Octree in a new Thread.\n";
}

void Scene::startThreadEvent(MThread mt) {
    int cubePos = -1, oldCubePos=-1;
    int i=0;
    unsigned int microseconds = 100000; // update every 0.1 s

    // A. Initialize events
    mt.load(); // split data into 27 cubes
    mt.print();

    // B. Execute search if there is a new event
    while (true) { // this thread will be used for events
        i++;
        if (camera_ != NULL) {
            double x = file_.convertToFile(camera_->position().x),
                    y = file_.convertToFile(camera_->position().y),
                    z = file_.convertToFile(camera_->position().z);
            cubePos = mt.getCubeNumber(x, y, z);
            if ((cubePos != oldCubePos) && (oldCubePos != -1)) {
                blue_=1;
                mt.printCube(cubePos, i, x, y, z);
                usleep(microseconds*10);
                updateOctree(mt.getCube(cubePos));      // todo : launch this method in a new thread.
//                removeOctree(mt.getCube(oldCubePos));   // todo : launch this method in a new thread.
                blue_=0;
            }
            oldCubePos = cubePos;
        } else {
            cout <<"startThreadEvent: E: ["<<i<<"] Camera null ...\n";
        }
        usleep(microseconds); // sleeping to avoid spoofing processor.
    }
//        exit(55);
}

MThread Scene::getThread() {
    MThread res(( MThread(file_) ));
    if (isMultiThread_) { // todo : methode à part
        std::thread myThread([&]() {
            startThreadEvent(res);
        });
        myThread.detach(); // the thread will run forever
//        myThread.join(); // pauses until first finishes
    }
//    exit(88);
    return res;
}
