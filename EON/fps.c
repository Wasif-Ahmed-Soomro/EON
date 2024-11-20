
#include <SDL.h>
#include "fps.h"

void capFPS(Uint32 frameStart, int frameDelay) {
    Uint32 frameTime = SDL_GetTicks() - frameStart;
    if (frameDelay > frameTime) {
        SDL_Delay(frameDelay - frameTime);
    }
}
