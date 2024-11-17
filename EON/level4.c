#include <SDL.h>
#include <stdio.h>
#include "level5.h"
#include <math.h> // Include math library for sin and cos functions

#define PI 3.14159265

int checkCollision(SDL_Rect a, SDL_Rect b);
void showRestartMessage(SDL_Renderer* renderer);

int runLevel4(SDL_Renderer* renderer) {
    SDL_Delay(1000);

    // Define sizes and positions for the squares and red boxes in level 2
    float redBoxSize = 30.0f, redBoxY = 290.0, redBoxX = 340.0f;
    int blinkTimer = 0;
    int showRedBox = 1; // Visibility toggle for blinking

    float redBoxWidth2 = 256.0f, redBoxHeight2 = 30, redBoxX2 = 0.0f, redBoxY2 = 190.0f;
    float redBoxX3 = 400.0f, redBoxY3 = 290.0f, redBoxSizeWidth3 = 240.0f, redBoxSizeHeight3 = 30.0f;
    float redBoxX4 = 0.0f, redBoxY4 = 430.0, redBoxSpeed4 = 0.2f;
    int redBoxDirection = 1;

    float squareSize = 50.0f;
    float squareX = 160.0f, squareY = 100.0f; // Position for the first square
    float square2X = 460.0f, square2Y = 100.0f; // Position for the second square
    float speed = 0.1f; // Speed of movement

    int wallx = 320, wally = 0, wallwidth = 20, wallheight = 600;

    // Load the background image as a BMP file (no SDL_image needed)
    SDL_Surface* bgSurface = SDL_LoadBMP("bg.bmp");
    if (!bgSurface) {
        printf("Failed to load BMP! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return;
    }
    SDL_Texture* bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
    SDL_FreeSurface(bgSurface);
    if (!bgTexture) {
        printf("Failed to create texture from BMP! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return;
    }

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
        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

        // Move the first square (WASD keys)
        if (currentKeyStates[SDL_SCANCODE_W] && squareY > 0) {
            squareY -= speed;
        }
        if (currentKeyStates[SDL_SCANCODE_S] && squareY < 600 - squareSize) {
            squareY += speed;
        }
        if (currentKeyStates[SDL_SCANCODE_A] && squareX > 0) {
            squareX -= speed;
        }
        if (currentKeyStates[SDL_SCANCODE_D] && squareX < 260) {
            squareX += speed;
        }

        // Move the second square (Arrow keys)
        if (currentKeyStates[SDL_SCANCODE_W] && square2Y > 0) {
            square2Y -= speed;
        }
        if (currentKeyStates[SDL_SCANCODE_S] && square2Y < 600 - squareSize) {
            square2Y += speed;
        }
        if (currentKeyStates[SDL_SCANCODE_A] && square2X > 330) {
            square2X -= speed;
        }
        if (currentKeyStates[SDL_SCANCODE_D] && square2X < 640 - squareSize) {
            square2X += speed;
        }
        // Move red boxes
        redBoxX4 += redBoxDirection * redBoxSpeed4;
        if (redBoxX4 <= 0) redBoxDirection = 1;
        else if (redBoxX4 >= 260) redBoxDirection = -1;

        // Update blinking timer and toggle visibility
        blinkTimer++;
        if (blinkTimer >= 3000) { // Toggle every 3000 frames
            showRedBox = !showRedBox;
            blinkTimer = 0;
        }

        // Check for collisions
        SDL_Rect redBox = { (int)redBoxX, (int)redBoxY, (int)redBoxSize, (int)redBoxSize };
        SDL_Rect redBox2 = { (int)redBoxX2, (int)redBoxY2, (int)redBoxWidth2, (int)redBoxHeight2 };
        SDL_Rect redBox3 = { (int)redBoxX3, (int)redBoxY3, (int)redBoxSizeWidth3, (int)redBoxSizeHeight3 };
        SDL_Rect redBox4 = { (int)redBoxX4, (int)redBoxY4, (int)redBoxSize, (int)redBoxSize };
        SDL_Rect finishLineR = { 340, 482, 300, 119 };
        SDL_Rect finishLineL = { 0, 482, 320, 119 };

        SDL_Rect square = { (int)squareX, (int)squareY, (int)squareSize, (int)squareSize };
        SDL_Rect square2 = { (int)square2X, (int)square2Y, (int)squareSize, (int)squareSize };

        if (showRedBox && (checkCollision(square, redBox) || checkCollision(square2, redBox))) {
            return 1;
        }
        if (checkCollision(square, redBox2) || checkCollision(square2, redBox2) ||
            checkCollision(square, redBox3) || checkCollision(square2, redBox3)) {
            return 1;
        }
        if (checkCollision(square2, finishLineR)) {
            int level5Status = runLevel5(renderer);  // Run level 2 and check for collision
            if (level5Status == 1) {
                return 1;
            }
        }

        // Render background and game objects
        SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

        // Draw squares
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &square);
        SDL_RenderFillRect(renderer, &square2);

        // Draw red boxes
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        if (showRedBox) {
            SDL_RenderFillRect(renderer, &redBox); // Blinking red box
        }
        SDL_RenderFillRect(renderer, &redBox2);
        SDL_RenderFillRect(renderer, &redBox3);
        SDL_RenderFillRect(renderer, &redBox4);

        SDL_RenderPresent(renderer);
    }

    // Clean up resources
    SDL_DestroyTexture(bgTexture);
}