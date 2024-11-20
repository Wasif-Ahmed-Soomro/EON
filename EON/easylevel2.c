#include <SDL.h>
#include <SDL_image.h> // Include SDL_image
#include <stdio.h>
#include "easylevel3.h"

int checkCollision(SDL_Rect a, SDL_Rect b);

int runel2(SDL_Renderer* renderer) {
    SDL_Delay(1000);

    // Initialize variables for the moving gap (obi)
    float gapX = 200.0f, gapY = 274.0f, gapSpeed = 0.08f;
    int gapDirection = 1; // Moving right initially
    const float gapWidth = 60.0f, redBoxHeight = 50.0f;
    float redBoxSize = 50.0f, redBoxX = 320.0f, redBoxY = 400.0f, redBoxSpeed = 0.7f;
    int redBoxDirection = -1;

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
    SDL_Texture* bgTexture = IMG_LoadTexture(renderer, "easybg.png");
    if (!bgTexture) {
        printf("Failed to load texture! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Load the obi (gap) image
    SDL_Texture* obiTexture = IMG_LoadTexture(renderer, "obii.png");
    if (!obiTexture) {
        printf("Failed to load obi texture! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Load the moving spike image
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

    // Set up the game state for level 2
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
        if (currentKeyStates[SDL_SCANCODE_S] && squareY < 600 - squareSize) {
            squareY += speed; // Move down
        }
        if (currentKeyStates[SDL_SCANCODE_A] && squareX > 0) {
            squareX -= speed; // Move left
        }
        if (currentKeyStates[SDL_SCANCODE_D] && squareX < 640 - squareSize) {
            squareX += speed; // Move right
        }

        // Update the red box (spike) position
        redBoxX += redBoxDirection * redBoxSpeed;
        if (redBoxX <= 0) redBoxDirection = 1;
        else if (redBoxX >= 640 - redBoxSize) redBoxDirection = -1;

        // Update the gap(obi) position
        gapX += gapDirection * gapSpeed;
        if (gapX <= 0) gapDirection = 1;
        else if (gapX >= 640 - gapWidth) gapDirection = -1;

        // Define rectangles for the moving spike and lava(red box) and the gap(obi) 
        SDL_Rect finishline = { 0, 482, 640, 119 };
        SDL_Rect spike = { (int)redBoxX, (int)redBoxY, (int)redBoxSize, (int)redBoxSize };
        SDL_Rect redBox = { 0, 274, 640, (int)redBoxHeight };
        SDL_Rect gap = { (int)gapX, (int)gapY, (int)gapWidth, (int)redBoxHeight };
        SDL_Rect character = { (int)squareX, (int)squareY, (int)squareSize, (int)squareSize };

        // Check for collisions (player must not collide with the red box unless within the gap)
        if (checkCollision(character, redBox)  && !checkCollision(character, gap) || checkCollision(character,spike)) {
            return 1; // Player collided 
        }
        if (checkCollision(character, finishline)) {
            int el3status = runel3(renderer);  // Run level 3 when reache finish line
            if (el3status == 1) {
                return 1;
            }
        }

        // Render the background
        SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

        // Draw the moving spike image
        SDL_RenderCopy(renderer, spikeTexture, NULL, &spike); // Draw the moving spike

        // Draw the gap (obi image)
        SDL_RenderCopy(renderer, obiTexture, NULL, &gap); // Draw the obi image for the gap

        // Draw the player's character (replace square with character image)
        SDL_RenderCopy(renderer, characterTexture, NULL, &character); // Draw the character as a texture

        // Present the rendered frame
        SDL_RenderPresent(renderer);
    }

    // Clean up resources
    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(obiTexture); 
    SDL_DestroyTexture(spikeTexture); 
    SDL_DestroyTexture(characterTexture); 
    IMG_Quit();
    return 0;
}
