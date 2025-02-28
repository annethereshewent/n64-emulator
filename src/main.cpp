#include <iostream>
#include "cpu/CPU.cpp"
#include <iterator>
#include <SDL3/SDL.h>
#include "interface.cpp"

int main(int argc, char **argv) {
    CPU cpu;
    SDL_Event event;

    if (argc < 1) {
        std::cout << "Please specify a filename.\n";
        exit(1);
    }

    if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        std::cout << "could not load sdl :-(\n";
        exit(1);
    }

    SDL_Window* window = SDL_CreateWindow("N64+", 640, 480, SDL_WINDOW_VULKAN);

    if(window == NULL || window == nullptr) {
        printf("window creation Error: %s\n", SDL_GetError());
    }

    cpu.loadRom(argv[1]);

    GFX_INFO gfxInfo;

    gfxInfo.RDRAM = &cpu.bus.rdram[0];
    gfxInfo.DMEM = &cpu.bus.rsp.dmem[0];
    gfxInfo.RDRAM_SIZE = cpu.bus.rdram.size();
    gfxInfo.DPC_CURRENT_REG = &cpu.bus.rdp.current;
    gfxInfo.DPC_START_REG = &cpu.bus.rdp.start;
    gfxInfo.DPC_END_REG = &cpu.bus.rdp.end;
    gfxInfo.DPC_STATUS_REG = &cpu.bus.rdp.status.value;
    gfxInfo.debugOn = &cpu.debugOn;

    rdp_init(window, gfxInfo, false, false, false);

    while (true) {
        while (!cpu.bus.rdp.frameFinished) {
            cpu.step();
        }

        cpu.limitFps();

        cpu.bus.rdp.frameFinished = false;

        while(SDL_PollEvent(&event) > 0) {
            switch(event.type) {
                case SDL_EVENT_QUIT:
                    exit(1);
                    break;
                case SDL_EVENT_KEY_DOWN:
                    switch (event.key.key) {
                        case SDLK_G:
                            cpu.debugOn = !cpu.debugOn;
                            break;
                    }
                    break;
            }
        }
    }

    SDL_Quit();

    return 0;
}