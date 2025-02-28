#include <iostream>
#include "cpu/CPU.cpp"
#include <iterator>
#include <SDL3/SDL.h>
#include "interface.cpp"
#include "controller/Controller.hpp"

SDL_Gamepad* findController() {
    int count;
    SDL_JoystickID* ids = SDL_GetGamepads(&count);

    for (int i = 0; i < count; i++) {
        SDL_Gamepad* gamepad = SDL_OpenGamepad(ids[i]);

        if (gamepad != nullptr) {
            return gamepad;
        }
    }

    return nullptr;
}

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

    cpu.bus.initAudio();

    SDL_Gamepad* gamepad = findController();

    uint8_t xAxis = 0;
    uint8_t yAxis = 0;

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
                        case SDLK_W:
                            yAxis = (uint8_t)MAX_AXIS_VAL;
                            break;
                        case SDLK_A:
                            xAxis = (uint8_t)-MAX_AXIS_VAL;
                            break;
                        case SDLK_S:
                            yAxis = (uint8_t)-MAX_AXIS_VAL;
                            break;
                        case SDLK_D:
                            xAxis = (uint8_t)MAX_AXIS_VAL;
                            break;
                        case SDLK_E:
                            cpu.bus.updateButton(BButton, true);
                            break;
                        case SDLK_RETURN:
                            cpu.bus.updateButton(Start, true);
                            break;
                        case SDLK_TAB:
                            break;
                        case SDLK_SPACE:
                            cpu.bus.updateButton(AButton, true);
                            break;
                    }
                    break;
                case SDL_EVENT_KEY_UP:
                    switch (event.key.key) {
                        case SDLK_G:
                            cpu.debugOn = !cpu.debugOn;
                            break;
                        case SDLK_W:
                            yAxis = 0;

                            break;
                        case SDLK_A:
                            xAxis = 0;
                            break;
                        case SDLK_S:
                            yAxis = 0;
                            break;
                        case SDLK_D:
                            xAxis = 0;
                            break;
                        case SDLK_E:
                            cpu.bus.updateButton(BButton, false);
                            break;
                        case SDLK_RETURN:
                            cpu.bus.updateButton(Start, false);
                            break;
                        case SDLK_TAB:
                            break;
                        case SDLK_SPACE:
                            cpu.bus.updateButton(AButton, false);
                            break;
                    }
                    break;
                case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                    switch (event.gbutton.button) {
                        case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
                            cpu.bus.updateButton(DpadDown, true);
                            break;
                        case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
                            cpu.bus.updateButton(DpadLeft, true);
                            break;
                        case SDL_GAMEPAD_BUTTON_DPAD_UP:
                            cpu.bus.updateButton(DpadUp, true);
                            break;
                        case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
                            cpu.bus.updateButton(DpadRight, true);
                            break;
                        case SDL_GAMEPAD_BUTTON_EAST:
                            cpu.bus.updateButton(AButton, true);
                            break;
                        case SDL_GAMEPAD_BUTTON_SOUTH:
                            cpu.bus.updateButton(BButton, true);
                            break;
                        case SDL_GAMEPAD_BUTTON_START:
                            cpu.bus.updateButton(Start, true);
                            break;
                        case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:
                            cpu.bus.updateButton(LButton, true);
                            break;
                        case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
                            cpu.bus.updateButton(RButton, true);
                            break;
                    }
                    break;
                case SDL_EVENT_GAMEPAD_BUTTON_UP:
                    switch (event.gbutton.button) {
                        case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
                            cpu.bus.updateButton(DpadDown, false);
                            break;
                        case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
                            cpu.bus.updateButton(DpadLeft, false);
                            break;
                        case SDL_GAMEPAD_BUTTON_DPAD_UP:
                            cpu.bus.updateButton(DpadUp, false);
                            break;
                        case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
                            cpu.bus.updateButton(DpadRight, false);
                            break;
                        case SDL_GAMEPAD_BUTTON_EAST:
                            cpu.bus.updateButton(AButton, false);
                            break;
                        case SDL_GAMEPAD_BUTTON_SOUTH:
                            cpu.bus.updateButton(BButton, false);
                            break;
                        case SDL_GAMEPAD_BUTTON_START:
                            cpu.bus.updateButton(Start, false);
                            break;
                        case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:
                            cpu.bus.updateButton(LButton, false);
                            break;
                        case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
                            cpu.bus.updateButton(RButton, false);
                            break;
                    }
                    break;
                case SDL_EVENT_GAMEPAD_ADDED:
                    gamepad = findController();
                    break;
            }
        }

        if (gamepad != nullptr) {
            double xAxisD = (double)SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX) / AXIS_DIVISOR;
            double yAxisD = (double)-SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY) / AXIS_DIVISOR;
            double rightTrigger = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_RIGHT_TRIGGER);

            xAxis = std::abs(xAxisD) > 10.0 ? (int8_t)(uint8_t)std::round(xAxisD) : 0;
            yAxis = std::abs(yAxisD) > 10.0 ? (int8_t)(uint8_t)std::round(yAxisD) : 0;

            if (rightTrigger > 0x1000) {
                cpu.bus.updateButton(ZButton, true);
            } else {
                cpu.bus.updateButton(ZButton, false);
            }
        }

        cpu.bus.updateAxis(xAxis, yAxis);
    }

    SDL_Quit();

    return 0;
}