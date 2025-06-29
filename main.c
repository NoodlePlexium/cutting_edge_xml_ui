#define _CRT_SECURE_NO_WARNINGS 

#include <math.h>
#include <SDL3/SDL.h>
#include <SDL_ttf/SDL_ttf.h>
#include "headers/parser.h"
#include "headers/layout.h"

int ProcessWindowEvents()
{
    int isRunning = 1; 

    SDL_Event event;
    while (SDL_PollEvent(&event)) 
    {
        // CLOSE WINDOW EVENT
        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)  {   
            isRunning = 0;
        }
        else if (event.type == SDL_EVENT_QUIT)               {
            isRunning = 0;
        }
        else if (event.type == SDL_EVENT_WINDOW_MOUSE_ENTER) {
            // hovered_window_ID = event.window.windowID;
        }
        else if (event.type == SDL_EVENT_WINDOW_MOUSE_LEAVE) {
            // hovered_window_ID = 0;
        }
    }

    return isRunning;
}

int main()
{
    // Create an enpty UI tree stack
    struct UI_Tree ui_tree;

    timer_start();
    start_measurement();

    // Parse xml into UI tree
    if (NU_Parse("test.xml", &ui_tree) != 0)
    {
        return -1;
    }

    end_measurement();
    timer_stop();

    // Check if SDL initialised
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }

    // Initialize SDL_ttf
    if (!TTF_Init()) {
        printf("SDL TTF could not initialize!");
        return -1;
    }

    // Load a default font
    TTF_Font* font = TTF_OpenFont("JacquesFrancois-Regular.ttf", 22);
    if (!font) printf("Could not find font file");
    ui_tree.font = font;

    // Create a vector of window pointers and store main window
    struct Vector windows;
    Vector_Reserve(&windows, sizeof(SDL_Window*), 8);

    // Create a vector of renderer pointers and store main renderer
    struct Vector renderers;
    Vector_Reserve(&renderers, sizeof(SDL_Renderer*), 8);

    printf("%s %zu\n", "node bytes:", sizeof(struct Node));

    struct NU_Watcher_Data watcher_data = {
        .ui_tree = &ui_tree,
        .windows = &windows,
        .renderers = &renderers
    };

    SDL_AddEventWatch(ResizingEventWatcher, &watcher_data);
    
    // Application loop
    int isRunning = 1;
    while (isRunning)
    {
        isRunning = ProcessWindowEvents();
        // Calculate element positions
        timer_start();
        // start_measurement();
        NU_Clear_Node_Sizes(&ui_tree);
        NU_Calculate_Text_Fit_Sizes(&ui_tree);
        NU_Calculate_Sizes(&ui_tree, &windows, &renderers);
        // NU_Calculate_Overflow(&ui_tree);
        NU_Grow_Nodes(&ui_tree);
        NU_Calculate_Positions(&ui_tree);
        NU_Render(&ui_tree, &windows, &renderers);

        // end_measurement();
        timer_stop();
    }

    // Free Memory
    NU_Free_UI_Tree_Memory(&ui_tree);
    Vector_Free(&windows);
    Vector_Free(&renderers);

    // Close SDL
    TTF_Quit();
    SDL_Quit();
}