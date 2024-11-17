#include <SDL.h>
#include <SDL_image.h> // Include SDL_image
#include <stdio.h>
#include "easylevel2.h"

int checkCollision(SDL_Rect a, SDL_Rect b);
void showRestartMessage(SDL_Renderer* renderer);

int easyMode(SDL_Renderer* renderer) {
    // Initialize SDL_image BEFORE loading images
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF))) {
        printf("SDL_image could not initialize! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* bgTexture = IMG_LoadTexture(renderer, "easybg1.png");
    if (!bgTexture) {
        printf("Failed to load background image! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }

    // Load the spike texture
    SDL_Texture* spikeTexture = IMG_LoadTexture(renderer, "movingspike.png");
    if (!spikeTexture) {
        printf("Failed to load spike image! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }

    // Load character image (single static image)
    SDL_Texture* characterTexture = IMG_LoadTexture(renderer, "character.png");
    if (!characterTexture) {
        printf("Failed to load character image! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }

    // Initialize movement variables for spikes
    float spikeSize = 50.0f, spikeX = 320.0f, spikeY = 430.0f, spikeSpeed = 0.2f;
    int spikeDirection = -1;

    // First spike
    float spikeX2 = 200.0f, spikeY2 = 200.0f, spikeSpeed2 = 0.1f;
    int spikeDirection2 = 1;

    // Second spike
    float spikeX3 = 250.0f, spikeY3 = 280.0f, spikeSpeed3 = 0.1f;
    int spikeDirection3 = -1;

    float squareSize = 50.0f;
    float squareX = 160.0f, squareY = 100.0f;  // Position for the first square
    float speed = 0.1f;  // Speed of movement

    int gameOver = 0;  // Game over flag
    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            if (gameOver && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
                // Reset the game when 'R' is pressed
                squareX = 160.0f;
                squareY = 100.0f;
                spikeX = 320.0f;
                spikeX2 = 200.0f;
                spikeX3 = 280.0f;
                gameOver = 0;
            }
        }

        if (!gameOver) {
            // Get the current state of the keyboard
            const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

            // Move the first square (WASD keys)
            int isMoving = 0; // Flag to track if the player is moving

            if (currentKeyStates[SDL_SCANCODE_W] && squareY > 0) {
                squareY -= speed;  // Move up
                isMoving = 1;
            }
            if (currentKeyStates[SDL_SCANCODE_S] && squareY < 600 - squareSize) {
                squareY += speed;  // Move down
                isMoving = 1;
            }
            if (currentKeyStates[SDL_SCANCODE_A] && squareX > 0) {
                squareX -= speed;  // Move left
                isMoving = 1;
            }
            if (currentKeyStates[SDL_SCANCODE_D] && squareX < 640 - squareSize) {
                squareX += speed;  // Move right
                isMoving = 1;
            }

            // Update the spike positions
            spikeX += spikeDirection * spikeSpeed;
            if (spikeX <= 0) spikeDirection = 1;
            else if (spikeX >= 640 - spikeSize) spikeDirection = -1;

            spikeX3 += spikeDirection3 * spikeSpeed3;
            if (spikeX3 <= 0) spikeDirection3 = 1;
            else if (spikeX3 >= 640 - spikeSize) spikeDirection3 = -1;

            spikeX2 += spikeDirection2 * spikeSpeed2;
            if (spikeX2 <= 0) spikeDirection2 = 1;
            else if (spikeX2 >= 640 - spikeSize) spikeDirection2 = -1;

            // Define the squares and spikes as SDL_Rect objects
            SDL_Rect finishLine = { 0, 482, 640, 119 };
            SDL_Rect spike = { (int)spikeX, (int)spikeY, (int)spikeSize, (int)spikeSize };
            SDL_Rect spike2 = { (int)spikeX2, (int)spikeY2, (int)spikeSize, (int)spikeSize };
            SDL_Rect spike3 = { (int)spikeX3, (int)spikeY3, (int)spikeSize, (int)spikeSize };
            SDL_Rect character = { (int)squareX, (int)squareY, (int)squareSize, (int)squareSize };

            // Check for collisions
            if (checkCollision(character, spike) || checkCollision(character, spike2) || checkCollision(character, spike3)) {
                gameOver = 1;
            }
            if (checkCollision(character, finishLine)) {
                int el2status = runel5(renderer);
                if (el2status == 1) {
                    gameOver = 1;
                    continue;
                }
                break;
            }

            // Clear the window and render the background
            SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

            // Draw the character (use the single static character image)
            SDL_RenderCopy(renderer, characterTexture, NULL, &character);

            // Draw the spikes (using textures)
            SDL_RenderCopy(renderer, spikeTexture, NULL, &spike);
            SDL_RenderCopy(renderer, spikeTexture, NULL, &spike2);
            SDL_RenderCopy(renderer, spikeTexture, NULL, &spike3);

            // Present the rendered frame
            SDL_RenderPresent(renderer);
        }
        else {
            // Show the restart message
            showRestartMessage(renderer);
        }
    }

    // Clean up resources
    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(spikeTexture);
    SDL_DestroyTexture(characterTexture);  // Destroy the character texture
    IMG_Quit();
    return 0;  // Ensure the return type matches the function signature
}
