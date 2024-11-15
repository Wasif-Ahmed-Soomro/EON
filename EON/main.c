#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include "level2.h" // Make sure this file declares runLevel2

int checkCollision(SDL_Rect a, SDL_Rect b) {
    return SDL_HasIntersection(&a, &b);
}

void showRestartMessage(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect white = { 150, 190, 340, 100 };
    SDL_RenderFillRect(renderer, &white);

    SDL_RenderPresent(renderer);
}


void showMainMenu(SDL_Renderer* renderer, int* play) {
    SDL_Event event;
    int menuRunning = 1;

    SDL_Texture* bgTexture = IMG_LoadTexture(renderer, "mainmenubg2.png");
    SDL_Texture* playTexture = IMG_LoadTexture(renderer, "play.png");
    SDL_Texture* quitTexture = IMG_LoadTexture(renderer, "quit.png");

    if (!bgTexture || !playTexture || !quitTexture) {
        printf("Failed to load textures! IMG_Error: %s\n", IMG_GetError());
        *play = 0;
        if (bgTexture) SDL_DestroyTexture(bgTexture);
        if (playTexture) SDL_DestroyTexture(playTexture);
        if (quitTexture) SDL_DestroyTexture(quitTexture);
        return;
    }

    SDL_Rect playButton = { 220, 180, 250, 100 };
    SDL_Rect quitButton = { 220, 320, 250, 100 };
    SDL_Rect bgRect = { 0, 0, 640, 600 };

    while (menuRunning) {
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bgTexture, NULL, &bgRect);
        SDL_RenderCopy(renderer, playTexture, NULL, &playButton);
        SDL_RenderCopy(renderer, quitTexture, NULL, &quitButton);
        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                *play = 0;
                menuRunning = 0;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                if (x >= playButton.x && x <= playButton.x + playButton.w &&
                    y >= playButton.y && y <= playButton.y + playButton.h) {
                    showModeSelectionMenu(renderer, play);  // Show mode selection instead of starting game
                    menuRunning = 0;
                }
                else if (x >= quitButton.x && x <= quitButton.x + quitButton.w &&
                    y >= quitButton.y && y <= quitButton.y + quitButton.h) {
                    *play = 0;
                    menuRunning = 0;
                }
            }
        }
    }

    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(playTexture);
    SDL_DestroyTexture(quitTexture);
}

int showModeSelectionMenu(SDL_Renderer* renderer, int* startGame) {
    SDL_Event event;
    int modeMenuRunning = 1;

    SDL_Texture* bgTexture = IMG_LoadTexture(renderer, "mainmenubg2.png");
    SDL_Texture* easyModeTexture = IMG_LoadTexture(renderer, "easy.png"); // Placeholder for easy mode button
    SDL_Texture* startGameTexture = IMG_LoadTexture(renderer, "hard.png"); // Placeholder for start game button

    if (!bgTexture || !easyModeTexture || !startGameTexture) {
        printf("Failed to load textures! IMG_Error: %s\n", IMG_GetError());
        *startGame = 0;
        if (bgTexture) SDL_DestroyTexture(bgTexture);
        if (easyModeTexture) SDL_DestroyTexture(easyModeTexture);
        if (startGameTexture) SDL_DestroyTexture(startGameTexture);
        return;
    }

    SDL_Rect easyModeButton = { 220, 180, 250, 100 };
    SDL_Rect startGameButton = { 220, 320, 250, 100 };
    SDL_Rect bgRect = { 0, 0, 640, 600 };

    while (modeMenuRunning) {
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bgTexture, NULL, &bgRect);
        SDL_RenderCopy(renderer, easyModeTexture, NULL, &easyModeButton);
        SDL_RenderCopy(renderer, startGameTexture, NULL, &startGameButton);
        SDL_RenderPresent(renderer);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                *startGame = 0;
                modeMenuRunning = 0;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                if (x >= easyModeButton.x && x <= easyModeButton.x + easyModeButton.w &&
                    y >= easyModeButton.y && y <= easyModeButton.y + easyModeButton.h) {
                    easyMode(); // Call easy mode function
                    modeMenuRunning = 0;
                }
                else if (x >= startGameButton.x && x <= startGameButton.x + startGameButton.w &&
                    y >= startGameButton.y && y <= startGameButton.y + startGameButton.h) {
                    *startGame = 1;
                    modeMenuRunning = 0;
                }
            }
        }
    }

    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(easyModeTexture);
    SDL_DestroyTexture(startGameTexture);
}

int easyMode() {
    printf("Easy mode selected!\n");
    // Implement easy mode game logic here
}


int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("EON", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }



    // Load the background image
    SDL_Texture* bgTexture = IMG_LoadTexture(renderer, "bg.bmp");
    if (!bgTexture) {
        printf("Failed to load texture! IMG_Error: %s\n", IMG_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    int play = 0;
    showMainMenu(renderer, &play);  // Display the main menu
    if (!play) {
        // Exit if "Quit" was selected
        SDL_DestroyTexture(bgTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return 0;
    }

    //int level = 1;
    float squareSize = 50.0f, squareX = 160.0f, squareY = 100.0f, speed = 0.1f;
    float square2Size = 50.0f, square2X = 460.0f, square2Y = 100.0f, speed2 = 0.1f;

    // Initialize red boxes
    //2nd moving
    float redBoxSize = 30.0f, redBoxX = 0.0f, redBoxY = 430.0f, redBoxSpeed = 0.1f;
    int redBoxDirection = -1;
    //first moving
    float redBoxSize2 = 30.0f, redBoxX2 = 200.0f, redBoxY2 = 200.0f, redBoxSpeed2 = 0.1f;
    int redBoxDirection2 = 1;
    //last not moving
    float redBoxSizer = 30.0f, redBoxXr = 510.0f, redBoxYr = 420.0f, redBoxSpeedr = 0.1f;
    int redBoxDirectionr = -1;
    //first non moving
    float redBoxSize2r = 30.0f, redBoxX2r = 390.0f, redBoxY2r = 280.0f, redBoxSpeed2r = 0.1f;
    int redBoxDirection2r = 1;

    int wallx = 320, wally = 0, wallwidth = 20, wallheight = 600;

    int running = 1, gameOver = 0;
    SDL_Event event;

    // Define finish line rectangles
    SDL_Rect finishLineR = { 340, 482, 300, 119 };
    SDL_Rect finishLineL = { 0, 482, 320, 119 };

    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            if (gameOver && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
                // Reset positions and resume the game when 'R' is pressed after game over
                squareX = 160.0f;
                squareY = 100.0f;
                square2X = 460.0f;
                square2Y = 100.0f;
                redBoxX = 320.0f;
                redBoxX2 = 200.0f;
                redBoxXr = 510.0f;
                redBoxX2r = 390.0f;
                gameOver = 0;
            }
        }

        if (!gameOver) {
            // Handle keyboard input for square 1 (WASD keys)
            const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
            SDL_Rect square = { (int)squareX, (int)squareY, (int)squareSize, (int)squareSize };
            SDL_Rect square2 = { (int)square2X, (int)square2Y, (int)square2Size, (int)square2Size };
            SDL_Rect wall = { wallx, wally, wallwidth, wallheight };

            // Move the first square (WASD keys)
            if (currentKeyStates[SDL_SCANCODE_W] && squareY > 0) {
                squareY -= speed;  // Move up
            }
            if (currentKeyStates[SDL_SCANCODE_S] && squareY < 600) {
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
            if (currentKeyStates[SDL_SCANCODE_S] && square2Y < 600) {
                square2Y += speed;  // Move down
            }
            if (currentKeyStates[SDL_SCANCODE_A] && square2X > 330) {
                square2X -= speed;  // Move left
            }
            if (currentKeyStates[SDL_SCANCODE_D] && square2X < 640 - squareSize) {
                square2X += speed;  // Move right
            }

            // Move the red boxes horizontally for left wall
            //last box
            redBoxX += redBoxDirection * redBoxSpeed;
            if (redBoxX <= 0) redBoxDirection = 1;
            else if (redBoxX >= 320 - redBoxSize) redBoxDirection = -1;

            //first box
            redBoxX2 += redBoxDirection2 * redBoxSpeed2;
            if (redBoxX2 <= 0) redBoxDirection2 = 1;
            else if (redBoxX2 >= 320 - redBoxSize2) redBoxDirection2 = -1;

            // Check for collisions
            SDL_Rect redBox = { (int)redBoxX, (int)redBoxY, (int)redBoxSize, (int)redBoxSize };
            SDL_Rect redBox2 = { (int)redBoxX2, (int)redBoxY2, (int)redBoxSize2, (int)redBoxSize2 };
            SDL_Rect redBoxr = { (int)redBoxXr, (int)redBoxYr, (int)redBoxSizer, (int)redBoxSizer };
            SDL_Rect redBox2r = { (int)redBoxX2r, (int)redBoxY2r, (int)redBoxSize2r, (int)redBoxSize2r };

            if (checkCollision(square, redBox) || checkCollision(square, redBox2) || checkCollision(square2, redBoxr) || checkCollision(square2, redBox2r)) {
                gameOver = 1;
            }


            if (checkCollision(square2, finishLineR)) {
                int level2status = runLevel2(renderer);
                if (level2status == 1) {
                    gameOver = 1;
                    continue;
                }
                break;
            }

            // Clear the window and render the background
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, bgTexture, NULL, NULL); // Render the background texture



            // Draw the squares
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderFillRect(renderer, &square);
            SDL_RenderFillRect(renderer, &square2);

            // Draw the red boxes
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &redBox);
            SDL_RenderFillRect(renderer, &redBox2);
            SDL_RenderFillRect(renderer, &redBoxr);
            SDL_RenderFillRect(renderer, &redBox2r);


            SDL_RenderPresent(renderer);
        }
        else {
            // Show restart message
            showRestartMessage(renderer);
        }
    }

    // Cleanup
    SDL_DestroyTexture(bgTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit(); // Quit SDL_image
    SDL_Quit();
    return 0;
}
