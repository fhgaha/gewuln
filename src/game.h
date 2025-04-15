#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <gewuln/resource_manager.h>
#include <gewuln/camera_look_at.h>
#include <gewuln/camera_fly.h>

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
public:
    // game state
    GameState               State;
    bool                    Keys[1024];
    unsigned int            Width, Height;
    float                   dt;

    CameraFly               free_look_camera;
    CameraLookAt            look_at_camera_corridor;
    CameraLookAt            look_at_camera_kitchen_start;
    CameraLookAt            look_at_camera_kitchen_end;
    CameraFly            *active_cam;


    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void Update(float dt);
    void ProcessInput();
    void ProcessMouseMovement(float xoffset, float yoffset);
    void ProcessMouseScroll(float yoffset);
    void Render();

    void PlayCameraThing();
};

#endif
