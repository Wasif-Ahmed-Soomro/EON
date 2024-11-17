#include <SDL.h>
#include <SDL_image.h> // Include SDL_image
#include <stdio.h>
#include <math.h> // For sin and cos functions

int checkCollision(SDL_Rect a, SDL_Rect b);

int runel4(SDL_Renderer* renderer) {
    SDL_Delay(1000);

    int centerX = 100;
    int centerY = 400;

    // Spike variables
    float spikeRadius = 50.0; // Distance from the center
    float spikeSpeed = 0.002f; // Speed of rotation (radians per frame)
    int spikeSize = 50; // Size of each spike

    // Initial angles for each spike
    float spike1Angle = 0.0f;      // Spike 1

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

    // Load the blinking spike texture (spike.png)
    SDL_Texture* blinkSpikeTexture = IMG_LoadTexture(renderer, "spike.png");
    if (!blinkSpikeTexture) {
        printf("Failed to load blinking spike texture! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    // Blinking spike variables
    int blinkCounter = 0; // Counter for blinking
    int blinkFrequency = 1000; // Frames between visibility toggles

    // Define rectangles for blinking spikes
    SDL_Rect blinkSpike1 = { 600, 240, 30, 30 };
    SDL_Rect blinkSpike2 = { 180, 290, 30, 30 };

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
        if (currentKeyStates[SDL_SCANCODE_S] && squareY < 600 - squareSize) {
            squareY += speed; // Move down
        }
        if (currentKeyStates[SDL_SCANCODE_A] && squareX > 0) {
            squareX -= speed; // Move left
        }
        if (currentKeyStates[SDL_SCANCODE_D] && squareX < 640 - squareSize) {
            squareX += speed; // Move right
        }

        // Update spike 1's position based on circular motion
        float spike1X = centerX + spikeRadius * cos(spike1Angle) - spikeSize / 2;
        float spike1Y = centerY + spikeRadius * sin(spike1Angle) - spikeSize / 2;
        spike1Angle += spikeSpeed; // Increment angle for circular motion

        // Increment blink counter
        blinkCounter++;
        int blinkVisible = (blinkCounter / blinkFrequency) % 2; // Toggles visibility every `blinkFrequency` frames

        // Define rectangles for all spikes and finishline
        SDL_Rect finishline = { 0, 482, 640, 119 };
        SDL_Rect spike1Rect = { (int)spike1X, (int)spike1Y, spikeSize, spikeSize };

        // Define rectangles for lasers
        SDL_Rect laser1 = { 0, 250, 600, 20 };
        SDL_Rect laser2 = { 180, 340, 460, 20 };

        // Define rectangle for the player's character
        SDL_Rect character = { (int)squareX, (int)squareY, (int)squareSize, (int)squareSize };

        // Check for collisions with any spike
        if (checkCollision(character, spike1Rect) ||
            (blinkVisible && (checkCollision(character, blinkSpike1) || checkCollision(character, blinkSpike2))) ||
            checkCollision(character, laser1) || checkCollision(character, laser2)) {
            return 1; // Player collided with one of the spikes
        }

        // Render the background
        SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

        // Render the lasers
        SDL_RenderCopy(renderer, laserTexture, NULL, &laser1);
        SDL_RenderCopy(renderer, laserTexture, NULL, &laser2);

        // Render the rotating spike
        SDL_RenderCopyEx(renderer, spikeTexture, NULL, &spike1Rect, spike1Angle * 180 / M_PI, NULL, SDL_FLIP_NONE);

        // Render the blinking spikes
        if (blinkVisible) {
            SDL_RenderCopy(renderer, blinkSpikeTexture, NULL, &blinkSpike1);
            SDL_RenderCopy(renderer, blinkSpikeTexture, NULL, &blinkSpike2);
        }

        // Render the player's character
        SDL_RenderCopy(renderer, characterTexture, NULL, &character);

        // Present the rendered frame
        SDL_RenderPresent(renderer);
    }

    // Clean up resources
    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(spikeTexture);
    SDL_DestroyTexture(blinkSpikeTexture);
    SDL_DestroyTexture(characterTexture);
    SDL_DestroyTexture(laserTexture);
    IMG_Quit();
    return 0;
}
