#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <gewuln/resource_manager.h>
#include <gewuln/camera/camera_look_at.h>
#include <gewuln/camera/camera_fly.h>
#include <gewuln/room.h>


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
    //check keys processed and set it to true after when you want to be act only once on held.
    //you press, it stays pressed between frames, on release it resets both keys and keys pressed to false.
    //behaves as `act on press` or `just pressed`
    bool                    Keys[1024], KeysProcessed[1024];
    unsigned int            Width, Height;
    float                   dt;
    
    Room                    *start_room;
    Room                    *current_room;
    
    std::unordered_map<std::string, std::unique_ptr<Room>>      rooms;


    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void init();
    // game loop
    void update(float dt);
    void process_input();
    void process_mouse_movement(float xoffset, float yoffset);
    void process_mouse_scroll(float yoffset);
    void render();

};

#endif
