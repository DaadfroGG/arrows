
#ifndef EVENT_HPP
#define EVENT_HPP
#include <SDL2/SDL.h>
#include <time.h>

class Events {
    private:
        SDL_Event e;
        int mouseX;
        int mouseY;
    public:
        Events() {}
        bool poll() {
            return SDL_PollEvent(&this->e);
        }
        // Check if the user wants to quit
        bool quit() {
            return this->e.type == SDL_QUIT;
        }
        // Check if the user pressed a key
        bool keyDown() {
           return this->e.type == SDL_KEYDOWN;
        }
        // Check if the user released a key
        bool keyUp() {
            return this->e.type == SDL_KEYUP;
        }

        bool keyPressed(SDL_Keycode key) {
            return this->e.key.keysym.sym == key;
        }
        // Check if the user pressed a mouse button
        bool mouseButtonDown() {
            return this->e.type == SDL_MOUSEBUTTONDOWN;
        }
        // Check if the user released a mouse button
        bool mouseButtonUp() {
            return this->e.type == SDL_MOUSEBUTTONUP;
        }
        // Check if the user pressed a mouse button
        bool mouseButtonPressed(Uint8 button) {
            return this->e.button.button == button;
        }
        // Check if the user released a mouse button
        bool mouseButtonReleased(Uint8 button) {
            return this->e.button.button == button;
        }
        // update mouse position
        void updateMousePosition() {
            SDL_GetMouseState(&this->mouseX, &this->mouseY);
        }
        // Get mouse x position
        int getMouseX() {
            return this->mouseX;
        }
        // Get mouse y position
        int getMouseY() {
            return this->mouseY;
        }
        int mouseWheelY() {
            return this->e.wheel.y;
        }
        int getType() {
            return this->e.type;
        }
};


class Time {
    private:
        Uint32 startTicks;
        Uint32 pausedTicks;
        int frameDelay = 1000 / 120;
        bool paused;
        bool started;
    public:
        Time() {
            this->startTicks = 0;
            this->pausedTicks = 0;
            this->paused = false;
            this->started = false;
        }
        void start() {
            this->started = true;
            this->paused = false;
            this->startTicks = SDL_GetTicks();
            this->pausedTicks = 0;
        }
        void stop() {
            this->started = false;
            this->paused = false;
            this->startTicks = 0;
            this->pausedTicks = 0;
        }
        void pause() {
            if (this->started && !this->paused) {
                this->paused = true;
                this->pausedTicks = SDL_GetTicks() - this->startTicks;
                this->startTicks = 0;
            }
        }
        void unpause() {
            if (this->started && this->paused) {
                this->paused = false;
                this->startTicks = SDL_GetTicks() - this->pausedTicks;
                this->pausedTicks = 0;
            }
        }
        Uint32 getTicks() {
            Uint32 time = 0;
            if (this->started) {
                if (this->paused) {
                    time = this->pausedTicks;
                } else {
                    time = SDL_GetTicks() - this->startTicks;
                }
            }
            return time;
        }
        bool isStarted() {
            return this->started;
        }
        bool isPaused() {
            return this->paused;
        }
        void limit() {
            if (this->getTicks() < this->frameDelay) {
                SDL_Delay(this->frameDelay - this->getTicks());
            }
        }
        void setFrameDelay(int delay) {
            this->frameDelay = delay;
        }
        
};

#endif