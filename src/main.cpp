#include <iostream>
#include "cpu/CPU.cpp"
#include <iterator>
#include <SDL2/SDL.h>

int main(int argc, char **argv) {
    CPU cpu;
    SDL_Event event;

    if (argc < 1) {
        std::cout << "Please specify a filename.\n";
        exit(1);
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
        std::cout << "could not load sdl :-(\n";
        exit(1);
    }

    SDL_Window* win = SDL_CreateWindow("N64+", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);

    if(win == NULL) {
        printf("window creation Error: %s\n", SDL_GetError());
    }

    cpu.loadRom(argv[1]);

    while (true) {
        while (!cpu.bus.rdp.frameFinished) {
            cpu.step();
        }

        cpu.bus.rdp.frameFinished = false;

        while(SDL_PollEvent(&event) > 0) {
            switch(event.type) {
              case SDL_QUIT:
                exit(1);
                break;
            }
        }
    }

    SDL_Quit();

    return 0;
}