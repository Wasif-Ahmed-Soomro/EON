
#include <SDL.h>
#include <stdio.h>
#include <SDL_image.h> 
#include <math.h>

#define MAX_PROJECTILES 10 // Maximum number of active projectiles

typedef struct {
    float x, y;           // Position of the projectile
    float dirX, dirY;     // Direction of the projectile
    int active;           // Whether the projectile is active
} Projectile;

int checkCollision(SDL_Rect a, SDL_Rect b);
void showRestartMessage(SDL_Renderer* renderer);


int runLevel5(SDL_Renderer* renderer) {
    SDL_Delay(1000);
    playbullethard(renderer);

    // Start time to track the duration
    Uint32 startTime = SDL_GetTicks();

    // Red box position and size
    float redBoxSize = 50.0f;
    float redBoxX = 0.0f, redBoxY = 0.0f, redboxspeed = 0.1f;
    int redboxdirection = 1;

    // Red box position and size
    float redBoxX2 = 0.0f, redBoxY2 = 430.0f, redboxwidth2 = 640, redboxheight2 = 30;
    int redbox2Visible = 1;  // Flag to control visibility of redbox2

    // Player square variables
    float squareSize = 50.0f;
    float squareX = 160.0f, squareY = 100.0f;
    float square2X = 460.0f, square2Y = 100.0f;
    float speed = 0.1f;

    int wallx = 320, wally = 0, wallwidth = 20, wallheight = 600;

    // Projectile variables
    Projectile projectiles[MAX_PROJECTILES] = { 0 }; // Array to hold multiple projectiles
    float projectileSpeed = 0.1f;
    int frameCounter = 0;

    // Load the background image
    SDL_Surface* bgSurface = SDL_LoadBMP("bg.bmp");
    if (!bgSurface) {
        printf("Failed to load BMP! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return -1;
    }
    SDL_Texture* bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
    SDL_FreeSurface(bgSurface);
    if (!bgTexture) {
        printf("Failed to create texture from BMP! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return -1;
    }
    // Load character image (single static image)
    SDL_Texture* characterTexture = IMG_LoadTexture(renderer, "character.png");
    if (!characterTexture) {
        printf("Failed to load character image! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }

    // Load the turret texture
    SDL_Texture* turret = IMG_LoadTexture(renderer, "turret.png");
    if (!turret) {
        printf("Failed to load spike image! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* bulletTexture = IMG_LoadTexture(renderer, "bullet.png");
    SDL_Texture* laser = IMG_LoadTexture(renderer, "laser.png");

    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Calculate elapsed time
        Uint32 elapsedTime = SDL_GetTicks() - startTime;

        // Hide redbox2 after 10 seconds (10000 milliseconds)
        if (elapsedTime >= 10000) {
            redbox2Visible = 0;
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
        SDL_Rect square = { (int)squareX, (int)squareY, (int)squareSize, (int)squareSize };
        SDL_Rect square2 = { (int)square2X, (int)square2Y, (int)squareSize, (int)squareSize };

        // Projectile launching logic every 3000 frames
        frameCounter++;
        if (frameCounter >= 3000) {
            frameCounter = 0;

            // Find an inactive projectile slot
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                if (!projectiles[i].active) {
                    projectiles[i].active = 1;
                    projectiles[i].x = redBoxX + redBoxSize / 2;
                    projectiles[i].y = redBoxY + redBoxSize / 2;

                    // Calculate direction towards the first square
                    float deltaX = squareX - projectiles[i].x;
                    float deltaY = squareY - projectiles[i].y;
                    float distance = sqrt(deltaX * deltaX + deltaY * deltaY);

                    projectiles[i].dirX = deltaX / distance;
                    projectiles[i].dirY = deltaY / distance;
                    break;
                }
            }
        }

        // Update projectile positions
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            if (projectiles[i].active) {
                projectiles[i].x += projectiles[i].dirX * projectileSpeed;
                projectiles[i].y += projectiles[i].dirY * projectileSpeed;

                SDL_Rect projectileRect = { (int)projectiles[i].x, (int)projectiles[i].y, 20, 20 };

                // Check if projectile collides with either square
                if (checkCollision(square, projectileRect) || checkCollision(square2, projectileRect)) {
                    return 1;  // End level on collision
                }

                // Deactivate projectile if it goes off-screen
                if (projectiles[i].x < 0 || projectiles[i].x > 640 || projectiles[i].y < 0 || projectiles[i].y > 480) {
                    projectiles[i].active = 0;
                }
            }
        }

        SDL_Rect redbox2 = { (int)redBoxX2, (int)redBoxY2, (int)redboxwidth2, (int)redboxheight2 };
        SDL_Rect redBox = { (int)redBoxX, (int)redBoxY, (int)redBoxSize, (int)redBoxSize };
        SDL_Rect finishLineR = { 340, 482, 300, 119 };
        SDL_Rect finishLineL = { 0, 482, 320, 119 };

        // Collision detection with visible redbox2
        if (redbox2Visible && (checkCollision(square, redbox2) || checkCollision(square2, redbox2))) {
            return 1;
        }

        // Move red boxe
        redBoxX += redboxdirection * redboxspeed;
        if (redBoxX <= 0) redboxdirection = 1;
        else if (redBoxX >= 640 - redBoxSize) redboxdirection = -1;

        if (checkCollision(square2, finishLineR)) {
            playending(renderer);
            SDL_Quit(); 
            exit(0);
        }

        // Render background and game objects
        SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

        // Draw the player's square
        SDL_RenderCopy(renderer, characterTexture, NULL, &square);
        SDL_RenderCopy(renderer, characterTexture, NULL, &square2);

        SDL_RenderCopy(renderer, turret, NULL, &redBox);



        // Draw redbox2 if still visible
        if (redbox2Visible) {
            SDL_RenderCopy(renderer, laser, NULL, &redbox2);
        }

        // Draw active projectiles
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            if (projectiles[i].active) {
                SDL_Rect projectileRect = { (int)projectiles[i].x, (int)projectiles[i].y, 13, 13 };
                SDL_RenderCopy(renderer, bulletTexture, NULL, &projectileRect);
            }
        }

        SDL_RenderPresent(renderer);
    }

    // Clean up resources
    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(turret);
    SDL_DestroyTexture(characterTexture);
    SDL_DestroyTexture(bulletTexture);
    SDL_DestroyTexture(laser);
    IMG_Quit();
}