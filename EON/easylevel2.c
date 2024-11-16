#include <SDL.h>
#include <SDL_image.h> // Include SDL_image
#include <stdio.h>

int checkCollision(SDL_Rect a, SDL_Rect b);

int runel2(SDL_Renderer* renderer) {
    SDL_Delay(1000);

    // Initialize variables for the moving gap
    float gapX = 200.0f, gapY = 250.0f, gapSpeed = 0.08f;
    int gapDirection = 1; // Moving right initially
    const float gapWidth = 60.0f, redBoxHeight = 30.0f;
    float redBoxSize = 30.0f, redBoxX = 320.0f, redBoxY = 400.0f, redBoxSpeed = 0.7f;
    int redBoxDirection = -1;

    float squareSize = 50.0f;
    float squareX = 160.0f, squareY = 100.0f; // Position for the first square
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

        // Move the first square (WASD keys)
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
        redBoxX += redBoxDirection * redBoxSpeed;
        if (redBoxX <= 0) redBoxDirection = 1;
        else if (redBoxX >= 640 - redBoxSize) redBoxDirection = -1;
        // Update the gap position
        gapX += gapDirection * gapSpeed;
        if (gapX <= 0) gapDirection = 1;
        else if (gapX >= 640 - gapWidth) gapDirection = -1;

        // Define rectangles for the red box and the gap
        SDL_Rect redBox1 = { (int)redBoxX, (int)redBoxY, (int)redBoxSize, (int)redBoxSize };
        SDL_Rect redBox = { 0, (int)gapY, 640, (int)redBoxHeight };
        SDL_Rect gap = { (int)gapX, (int)gapY, (int)gapWidth, (int)redBoxHeight };
        SDL_Rect square = { (int)squareX, (int)squareY, (int)squareSize, (int)squareSize };

        // Check for collisions (player must not collide with the red box unless within the gap)
        if (checkCollision(square, redBox) && !checkCollision(square, gap)) {
            return 1; // Player collided with the red box
        }

        // Render the background
        SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

        // Draw the red box
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color
        SDL_RenderFillRect(renderer, &redBox);
        SDL_RenderFillRect(renderer, &redBox1);

        // Draw the gap (overwrite red box with background color)
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Black color (gap is transparent)
        SDL_RenderFillRect(renderer, &gap);

        // Draw the player's square
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color
        SDL_RenderFillRect(renderer, &square);

        SDL_RenderPresent(renderer);
    }

    // Clean up resources
    SDL_DestroyTexture(bgTexture);
    IMG_Quit();
    return 0;
}
