#include <SDL.h>
#include <SDL_image.h>  // Include SDL_image
#include <stdio.h>
#include <math.h>

#define MAX_PROJECTILES 10  // Maximum number of active projectiles

typedef struct {
    float x, y;        // Position of the projectile
    float dirX, dirY;  // Direction of the projectile
    int active;        // Whether the projectile is active
} Projectile;

// Function to check collision between two SDL_Rects
int checkCollision(SDL_Rect a, SDL_Rect b);

int runel5(SDL_Renderer* renderer) {
    SDL_Delay(1000);  // Initial delay

    // Start time to track the duration
    Uint32 startTime = SDL_GetTicks();

    // Turret variables
    float turretX = 0.0f, turretY = 0.0f, turretSize = 50.0f, turretspeed = 0.1f;
    int turretdirection = 1;

    // Player square variables
    float squareSize = 50.0f;
    float squareX = 160.0f, squareY = 100.0f;
    float speed = 0.1f;

    // Projectile variables
    Projectile projectiles[MAX_PROJECTILES] = { 0 };  // Array to hold multiple projectiles
    float projectileSpeed = 0.2f;  // Speed of the projectile
    int frameCounter = 0;  // To track frame count for firing projectiles

    // Laser variables
    int laserActive = 1;  // Initially, the laser is active
    Uint32 laserStartTime = SDL_GetTicks();  // Record the time when the laser activates

    // Load textures (background, turret, bullet, laser, and character)
    SDL_Texture* bgTexture = IMG_LoadTexture(renderer, "easybg1.png");
    SDL_Texture* turretTexture = IMG_LoadTexture(renderer, "turret.png");
    SDL_Texture* bulletTexture = IMG_LoadTexture(renderer, "bullet.png");
    SDL_Texture* laserTexture = IMG_LoadTexture(renderer, "laser.png");
    SDL_Texture* characterTexture = IMG_LoadTexture(renderer, "character.png");

    // Error handling for texture loading
    if (!bgTexture || !turretTexture || !bulletTexture || !laserTexture || !characterTexture) {
        printf("Failed to load texture! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    int running = 1;
    SDL_Event event;

    while (running) {
        // Poll for events (e.g., quit)
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Calculate elapsed time since the game started
        Uint32 elapsedTime = SDL_GetTicks() - startTime;

        // Get the current state of the keyboard
        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);

        // Move the player square (WASD keys)
        if (currentKeyStates[SDL_SCANCODE_W] && squareY > 0) {
            squareY -= speed;
        }
        if (currentKeyStates[SDL_SCANCODE_S] && squareY < 600 - squareSize) {
            squareY += speed;
        }
        if (currentKeyStates[SDL_SCANCODE_A] && squareX > 0) {
            squareX -= speed;
        }
        if (currentKeyStates[SDL_SCANCODE_D] && squareX < 640 - squareSize) {
            squareX += speed;
        }

        SDL_Rect square = { (int)squareX, (int)squareY, (int)squareSize, (int)squareSize };

        // Turret shooting logic every 4000 milliseconds (4 seconds)
        frameCounter++;
        if (frameCounter >= 4000) {
            frameCounter = 0;

            // Find an inactive projectile slot
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                if (!projectiles[i].active) {
                    projectiles[i].active = 1;
                    projectiles[i].x = turretX + turretSize / 2;  // Start projectile at the turret center
                    projectiles[i].y = turretY + turretSize / 2;  // Start projectile at the turret center

                    // Calculate direction towards the player square
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

                SDL_Rect projectileRect = { (int)projectiles[i].x, (int)projectiles[i].y, 10, 10 };

                // Check if projectile collides with the player square
                if (checkCollision(square, projectileRect)) {
                    return 1;  // End level on collision
                }

                // Deactivate projectile if it goes off-screen
                if (projectiles[i].x < 0 || projectiles[i].x > 640 || projectiles[i].y < 0 || projectiles[i].y > 480) {
                    projectiles[i].active = 0;
                }
            }
        }

        // Deactivate the laser after 20 seconds
        if (laserActive && SDL_GetTicks() - laserStartTime >= 20000) {
            laserActive = 0;
        }



        // Check if the character collides with the laser
        if (laserActive) {
            SDL_Rect laserRect = { 10, 400, 640, 20 };  // Laser size and position
            if (checkCollision(square, laserRect)) {
                return 1;  // End level if the player collides with the laser
            }
        }

        // Render background and game objects
        SDL_RenderCopy(renderer, bgTexture, NULL, NULL);

        // Draw the player's square
        SDL_RenderCopy(renderer, characterTexture, NULL, &square);

        // Draw the turret
        SDL_Rect turretRect = { (int)turretX, (int)turretY, (int)turretSize, (int)turretSize };
        SDL_RenderCopy(renderer, turretTexture, NULL, &turretRect);

        // Draw active projectiles
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            if (projectiles[i].active) {
                SDL_Rect projectileRect = { (int)projectiles[i].x, (int)projectiles[i].y, 10, 10 };
                SDL_RenderCopy(renderer, bulletTexture, NULL, &projectileRect);  // Use bullet texture
            }
        }

        // Move the turret horizontally
        turretX += turretdirection * turretspeed;
        if (turretX <= 0) turretdirection = 1;
        else if (turretX >= 640 - turretSize) turretdirection = -1;
        // Render the laser if it's active
        if (laserActive) {
            SDL_Rect laserRect = { 10, 400, 640, 20 };  // Laser position and size
            SDL_RenderCopy(renderer, laserTexture, NULL, &laserRect);
        }
        // Present the rendered frame
        SDL_RenderPresent(renderer);
    }

    // Clean up resources
    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(turretTexture);
    SDL_DestroyTexture(characterTexture);
    SDL_DestroyTexture(bulletTexture);
    SDL_DestroyTexture(laserTexture);  // Clean up the laser texture
    IMG_Quit();
    SDL_Quit();
    return 0;
}
