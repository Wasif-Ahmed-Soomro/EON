#include <SDL.h>
#include <SDL_image.h> // Include SDL_image
#include <stdio.h>
#include "easylevel4.h"
#include <math.h> // For sin and cos functions

int checkCollision(SDL_Rect a, SDL_Rect b);

int runel3(SDL_Renderer* renderer) {
    SDL_Delay(1000);

    // Screen center (assuming 640x600 resolution)
    int screenWidth = 640;
    int screenHeight = 600;
    int centerX = screenWidth / 2;
    int centerY = screenHeight / 2;

    // Spike variables
    float spikeRadius = 100.0f; // Distance from the center
    float spikeSpeed = 0.0002f; // Speed of rotation (radians per frame)
    int spikeSize = 50; // Size of each spike

    // Initial angles for each spike
    float spike1Angle = 0.0f;      // Spike 1
    float spike2Angle = M_PI / 2;  // Spike 2 (90 degrees offset)
    float spike3Angle = M_PI;      // Spike 3 (180 degrees offset)
    float spike4Angle = 3 * M_PI / 2; // Spike 4 (270 degrees offset)

    // Player movement speed and initial position
    float squareSize = 50.0f;
    float squareX = 160.0f, squareY = 100.0f; // Position for the player's square
    float speed = 0.1f; // Speed of movement

    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF))) {
        printf("SDL_image could not initialize! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }

    // Load the background image
    SDL_Texture* bgTexture = IMG_LoadTexture(renderer, "easybg1.png");
    if (!bgTexture) {
        printf("Failed to load texture! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Load the spike image
    SDL_Texture* spikeTexture = IMG_LoadTexture(renderer, "movingspike.png");
    if (!spikeTexture) {
        printf("Failed to load spike texture! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Load the player's character image
    SDL_Texture* characterTexture = IMG_LoadTexture(renderer, "character.png");
    if (!characterTexture) {
        printf("Failed to load character texture! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Load the laser image
    SDL_Texture* laserTexture = IMG_LoadTexture(renderer, "laser.png");
    if (!laserTexture) {
        printf("Failed to load laser texture! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Set up the game state for level 3
    int running = 1;
    SDL_Event event;

    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Get the current state of the keyboard
        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

        // Move the square (WASD keys)
        if (currentKeyStates[SDL_SCANCODE_W] && squareY > 0) {
            squareY -= speed; // Move up
        }
        if (currentKeyStates[SDL_SCANCODE_S] && squareY < screenHeight - squareSize) {
            squareY += speed; // Move down
        }
        if (currentKeyStates[SDL_SCANCODE_A] && squareX > 0) {
            squareX -= speed; // Move left
        }
        if (currentKeyStates[SDL_SCANCODE_D] && squareX < screenWidth - squareSize) {
            squareX += speed; // Move right
        }

        // Update spike 1's position based on circular motion
        float spike1X = centerX + spikeRadius * cos(spike1Angle) - spikeSize / 2;
        float spike1Y = centerY + spikeRadius * sin(spike1Angle) - spikeSize / 2;
        spike1Angle += spikeSpeed; // Increment angle for circular motion

        // Update spike 2's position based on circular motion
        float spike2X = centerX + spikeRadius * cos(spike2Angle) - spikeSize / 2;
        float spike2Y = centerY + spikeRadius * sin(spike2Angle) - spikeSize / 2;
        spike2Angle += spikeSpeed; // Increment angle for circular motion

        // Update spike 3's position based on circular motion
        float spike3X = centerX + spikeRadius * cos(spike3Angle) - spikeSize / 2;
        float spike3Y = centerY + spikeRadius * sin(spike3Angle) - spikeSize / 2;
        spike3Angle += spikeSpeed; // Increment angle for circular motion

        // Update spike 4's position based on circular motion
        float spike4X = centerX + spikeRadius * cos(spike4Angle) - spikeSize / 2;
        float spike4Y = centerY + spikeRadius * sin(spike4Angle) - spikeSize / 2;
        spike4Angle += spikeSpeed; // Increment angle for circular motion

        // Define rectangles for all spikes and finishline
        SDL_Rect finishline = { 0, 482, 640, 119 };
        SDL_Rect spike1Rect = { (int)spike1X, (int)spike1Y, spikeSize, spikeSize };
        SDL_Rect spike2Rect = { (int)spike2X, (int)spike2Y, spikeSize, spikeSize };
        SDL_Rect spike3Rect = { (int)spike3X, (int)spike3Y, spikeSize, spikeSize };
        SDL_Rect spike4Rect = { (int)spike4X, (int)spike4Y, spikeSize, spikeSize };

        // Define rectangles for lasers
        SDL_Rect laser1 = { 0, 300, 220, 20 };
        SDL_Rect laser2 = { 420, 300, 220, 20 };

        // Define rectangle for the player's character
        SDL_Rect character = { (int)squareX, (int)squareY, (int)squareSize, (int)squareSize };

        // Check for collisions with any spike
        if (checkCollision(character, spike1Rect) || checkCollision(character, spike2Rect) ||
            checkCollision(character, spike3Rect) || checkCollision(character, spike4Rect) || 
            checkCollision(character, laser1) || checkCollision(character, laser2)) {
            return 1; // Player collided with one of the spikes
        }

        if (checkCollision(character, finishline)) {
            int el4status = runel4(renderer);  // Run level 2 and check for collision
            if (el4status == 1) {
                return 1;
            }
        }



        // Render the background
        SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

        // Render the lasers
        SDL_RenderCopy(renderer, laserTexture, NULL, &laser1);
        SDL_RenderCopy(renderer, laserTexture, NULL, &laser2);

        // Render the rotating spikes
        SDL_RenderCopyEx(renderer, spikeTexture, NULL, &spike1Rect, spike1Angle * 180 / M_PI, NULL, SDL_FLIP_NONE);
        SDL_RenderCopyEx(renderer, spikeTexture, NULL, &spike2Rect, spike2Angle * 180 / M_PI, NULL, SDL_FLIP_NONE);
        SDL_RenderCopyEx(renderer, spikeTexture, NULL, &spike3Rect, spike3Angle * 180 / M_PI, NULL, SDL_FLIP_NONE);
        SDL_RenderCopyEx(renderer, spikeTexture, NULL, &spike4Rect, spike4Angle * 180 / M_PI, NULL, SDL_FLIP_NONE);

        // Render the player's character
        SDL_RenderCopy(renderer, characterTexture, NULL, &character);

        // Present the rendered frame
        SDL_RenderPresent(renderer);
    }

    // Clean up resources
    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(spikeTexture);
    SDL_DestroyTexture(characterTexture);
    SDL_DestroyTexture(laserTexture);
    IMG_Quit();
    return 0;
}
