#include <SDL.h>
#include <SDL_image.h> // Include SDL_image
#include <stdio.h>

int checkCollision(SDL_Rect a, SDL_Rect b);

int runLevel3(SDL_Renderer* renderer) {
    SDL_Delay(1000);
    // Define sizes for the squares in level 3

        // Initialize red boxes
    float redBoxSize = 30.0f, redBoxX = 320.0f, redBoxY = 430.0f, redBoxSpeed = 0.1f;
    int redBoxDirection = -1;



    float redboxsize4 = 30.0f, redboxX4 = 0.0f, redboxY4 = 0.0f;
    float redboxorbitspeed = 0.001f, redboxorbitradius = 100.0f, redboxorbitangle = 0.0f;


    float redBoxwidth2 = 256.0f, redBoxheight2 = 30.0f, redBoxX2 = 0.0f, redBoxY2 = 180.0f, redBoxSpeed2 = 0.1f;
    int redBoxDirection2 = 1;
    float redBoxX3 = 400.0f, redBoxY3 = 290.0f, redBoxSpeed3 = 0.3f, width3 = 300.0, height3 = 100.0;
    int redBoxDirection3 = 1;
    float squareSize = 50.0f;
    float squareX = 160.0f, squareY = 100.0f; // Position for the first square
    float square2X = 460.0f, square2Y = 100.0f; // Position for the second square
    float speed = 0.1f; // Speed of movement
    int wallx = 320, wally = 0, wallwidth = 20, wallheight = 600;

    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF))) {
        printf("SDL_image could not initialize! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return; // No need to return 1
    }

    // Load the background image
    SDL_Texture* bgTexture = IMG_LoadTexture(renderer, "bg.bmp");
    if (!bgTexture) {
        printf("Failed to load texture! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        IMG_Quit();
        SDL_Quit();
        return; // No need to return 1
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

        SDL_Rect wall = { wallx, wally, wallwidth, wallheight };
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
        if (currentKeyStates[SDL_SCANCODE_D] && squareX < 260) {
            squareX += speed;  // Move right
        }

        // Move the second square (Arrow keys)
        if (currentKeyStates[SDL_SCANCODE_W] && square2Y > 0) {
            square2Y -= speed;  // Move up
        }
        if (currentKeyStates[SDL_SCANCODE_S] && square2Y < 600 - squareSize) {
            square2Y += speed;  // Move down
        }
        if (currentKeyStates[SDL_SCANCODE_A] && square2X > 330) {
            square2X -= speed;  // Move left
        }
        if (currentKeyStates[SDL_SCANCODE_D] && square2X < 640 - squareSize) {
            square2X += speed;  // Move right
        }



        redBoxX += redBoxDirection * redBoxSpeed;
        if (redBoxX <= 0) redBoxDirection = 1;
        else if (redBoxX >= 640 - redBoxSize) redBoxDirection = -1;

        redboxorbitangle += redboxorbitspeed;
        redboxX4 = 140.0 + redboxorbitradius * cos(redboxorbitangle);
        redboxY4 = 335.0 + redboxorbitradius * sin(redboxorbitangle);




        // Check for collisions
        SDL_Rect redBox = { (int)redBoxX, (int)redBoxY, (int)redBoxSize, (int)redBoxSize };
        SDL_Rect redBox2 = { (int)redBoxX2, (int)redBoxY2, (int)redBoxwidth2, (int)redBoxheight2 };
        SDL_Rect redBox3 = { (int)redBoxX3, (int)redBoxY3, (int)width3, (int)height3 };
        SDL_Rect redBox4 = { (int)redboxX4, (int)redboxY4, (int)redBoxSize, (int)redBoxSize };

        // Define finish line rectangles
        SDL_Rect finishLineR = { 340, 482, 300, 119 };
        SDL_Rect finishLineL = { 0, 482, 320, 119 };

        // Check for collisions
        SDL_Rect square = { (int)squareX, (int)squareY, (int)squareSize, (int)squareSize };
        SDL_Rect square2 = { (int)square2X, (int)square2Y, (int)squareSize, (int)squareSize };

        // Check for collisions
        if (checkCollision(square, redBox) || checkCollision(square2, redBox) ||
            checkCollision(square, redBox2) || checkCollision(square2, redBox2) ||
            checkCollision(square, redBox3) || checkCollision(square2, redBox3) ||
            checkCollision(square, redBox4) || checkCollision(square2, redBox4)) {
            return 1; // Stop the game if there's a collision
        }

        if (checkCollision(square2, finishLineR)) {
            int level3status = runLevel4(renderer);
            if (level3status == 1) {
                return 1;
            }
        }


        SDL_RenderCopy(renderer, bgTexture, NULL, NULL);



        // Draw squares
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for squares
        SDL_RenderFillRect(renderer, &square);
        SDL_RenderFillRect(renderer, &square2);

        // Draw the red boxes
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &redBox);
        SDL_RenderFillRect(renderer, &redBox2);
        SDL_RenderFillRect(renderer, &redBox3);
        SDL_RenderFillRect(renderer, &redBox4);




        // Add additional logic for level 2 (obstacles, goals, etc.) here

        SDL_RenderPresent(renderer);

    }

    // Clean up resources
    SDL_DestroyTexture(bgTexture);
    IMG_Quit();
}