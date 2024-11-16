#include <SDL.h>
#include <SDL_image.h> // Include SDL_image
#include <stdio.h>

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

    SDL_Texture* bgTexture = IMG_LoadTexture(renderer, "easybg.png");
    if (!bgTexture) {
        printf("Failed to load background image! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }

    // Initialize red boxes
    float redBoxSize = 30.0f, redBoxX = 320.0f, redBoxY = 430.0f, redBoxSpeed = 0.2f;
    int redBoxDirection = -1;

    // First box
    float redBoxSize2 = 30.0f, redBoxX2 = 200.0f, redBoxY2 = 200.0f, redBoxSpeed2 = 0.1f;
    int redBoxDirection2 = 1;

    // Second box
    float redBoxX3 = 250.0f, redBoxY3 = 280.0f, redBoxSpeed3 = 0.1f;
    int redBoxDirection3 = -1;

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
                redBoxX = 320.0f;
                redBoxX2 = 200.0f;
                redBoxX3 = 280.0f;
                gameOver = 0;
            }
        }

        if (!gameOver) {
            // Get the current state of the keyboard
            const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

            // Move the first square (WASD keys)
            if (currentKeyStates[SDL_SCANCODE_W] && squareY > 0) {
                squareY -= speed;  // Move up
            }
            if (currentKeyStates[SDL_SCANCODE_S] && squareY < 600 - squareSize) {
                squareY += speed;  // Move down
            }
            if (currentKeyStates[SDL_SCANCODE_A] && squareX > 0) {
                squareX -= speed;  // Move left
            }
            if (currentKeyStates[SDL_SCANCODE_D] && squareX < 640 - squareSize) {
                squareX += speed;  // Move right
            }

            // Update the red boxes (movement)
            redBoxX += redBoxDirection * redBoxSpeed;
            if (redBoxX <= 0) redBoxDirection = 1;
            else if (redBoxX >= 640 - redBoxSize) redBoxDirection = -1;

            redBoxX3 += redBoxDirection3 * redBoxSpeed3;
            if (redBoxX3 <= 0) redBoxDirection3 = 1;
            else if (redBoxX3 >= 640 - redBoxSize) redBoxDirection3 = -1;

            redBoxX2 += redBoxDirection2 * redBoxSpeed2;
            if (redBoxX2 <= 0) redBoxDirection2 = 1;
            else if (redBoxX2 >= 640 - redBoxSize2) redBoxDirection2 = -1;

            // Define the squares and red boxes as SDL_Rect objects
            SDL_Rect redBox = { (int)redBoxX, (int)redBoxY, (int)redBoxSize, (int)redBoxSize };
            SDL_Rect redBox2 = { (int)redBoxX2, (int)redBoxY2, (int)redBoxSize2, (int)redBoxSize2 };
            SDL_Rect redBox3 = { (int)redBoxX3, (int)redBoxY3, (int)redBoxSize, (int)redBoxSize };
            SDL_Rect square = { (int)squareX, (int)squareY, (int)squareSize, (int)squareSize };

            // Check for collisions
            if (checkCollision(square, redBox) || checkCollision(square, redBox2) || checkCollision(square, redBox3)) {
                gameOver = 1;
            }

            // Clear the window and render the background
            SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

            // Draw squares
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for squares
            SDL_RenderFillRect(renderer, &square);

            // Draw the red boxes
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color for boxes
            SDL_RenderFillRect(renderer, &redBox);
            SDL_RenderFillRect(renderer, &redBox2);
            SDL_RenderFillRect(renderer, &redBox3);

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
    IMG_Quit();
    return 0;  // Ensure the return type matches the function signature
}
