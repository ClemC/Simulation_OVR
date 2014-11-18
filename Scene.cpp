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

#include <numeric>
#include <random>
#include <chrono>

std::unique_ptr<Logger> logger(new Logger("log.log", "log.err", true, Severity::error | Severity::info));
std::unique_ptr<NullOculus> nullOculus(new NullOculus);

Scene::Scene(std::string windowTitle, int windowWidth, int windowHeight, bool oculusRender, bool fullscreen,
             std::string textureName, unsigned long objectsCount, int size, int octantSize,
             int  octantsDrawnCount, std::string filename, int randomPercentage):
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
    randomPercentage_ {randomPercentage}
{
    logger->trace(logger->get() << "Scene constructor");

    input_ = std::unique_ptr<Input>(new Input(this));
    input_->showCursor(false);
    input_->capturePointer(true);

    camera_ = std::unique_ptr<Camera>(new Camera(
                                          glm::vec3(size_/2, size_/2, size_/2),
                                          glm::vec3(0, 0, 0), glm::vec3(0, 1, 0),
                                          0.5,
                                          0.2,
                                          *(input_.get()))
                                      );

    gObjects_.setEmptyValue(std::shared_ptr<NullGraphicObject>(new NullGraphicObject));

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
        gObjects_(x, y, z) = std::shared_ptr<Crate>(new Crate(x, y, z, 1.0, textureName_));
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
 */
void Scene::initGObjectsFile() {
    // A. Parse file
    File file(( File(filename_) ));
    file.exists_test();
    file.parseText(); // 'error: vector::_M_default_append' is here.
    file_=file;

    // B. Plot star.txt
    int i=0, n=file_.getTotalLines();
    srand ( time(NULL) ); // seed
    for (i=0; i<=n - 1; i++) {
        if (isRandomPoint(randomPercentage_)) { // if randomPercentage==100, always true.
            int max = size_ - 1;
            double zoom = 1; // only way to see more data at the same time ? double zoom = 1; // should be 1.
            int x = (file_.getData()[i][file_.xpos])*(max)*(zoom);
            int y = (file_.getData()[i][file_.ypos])*(max)*(zoom);
            int z = (file_.getData()[i][file_.zpos])*(max)*(zoom);
            double massV = file_.getData()[i][file_.mass],
                    ageV = file_.getData()[i][file_.age]; // textureBigStar_
            if (n>=10000) {
                ageV=0.1;
                zoom=1;
            }

            // C. Increase size according to age.
            auto startCrateGeneration = std::chrono::high_resolution_clock::now();
            std::string texture;
            if (massV>bigStarLimit_) {
                texture = textureBigStar_;
            } else {
                texture = textureSmallStar_;
            }
            gObjects_(x, y, z) = std::shared_ptr<Crate>(new Crate(x, y, z, ageV, texture)); // textureName_
            // D. Log
            auto endCrateGeneration = std::chrono::high_resolution_clock::now();
            logger->debug(logger->get() << "Generated crate n°" << i << " at position ("
                          << x << ", " << y << ", " << z << ") in "
                          << chrono::duration_cast<std::chrono::milliseconds>(endCrateGeneration - startCrateGeneration).count()
                          << " ms");
        }
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

/**
******************************************************
 * Generate the Scene.
 * Use file star.txt
 * (x, y, z) are in [0, (128*zoom)].
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

void Scene::mainLoop()
{
    int fpsDesired = 60;
    unsigned int frameRate = 1000 / fpsDesired;
    Uint32 start (0);
    Uint32 end  (0);
    Uint32 elapsedTime (0);

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
            glClearColor(0, 0, 0, 1);
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

void Scene::render(glm::mat4 & MV, glm::mat4 & proj)
{
    int sizeToRender = octantSize_ * octantsDrawnCount_;

    double e = std::numeric_limits<double>::epsilon();
    camera_->move(glm::vec3(sizeToRender + e, sizeToRender + e, sizeToRender + e) , glm::vec3(size_ - sizeToRender -e, size_ - sizeToRender -e, size_ - sizeToRender -e));
    camera_->lookAt(MV);

    for(int z=camera_->position().z - sizeToRender; z<camera_->position().z + sizeToRender; z++)
    {
        for(int y=camera_->position().y - sizeToRender; y<camera_->position().y + sizeToRender; y++)
        {
            for(int x=camera_->position().x - sizeToRender; x<camera_->position().x + sizeToRender; x++)
            {
                gObjects_.at(x, y, z)->draw(proj, MV);
            }
        }
    }
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
