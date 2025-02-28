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

    std::unordered_map<uint32_t, JoypadButton> keyboardMap = {
        {SDLK_E, BButton},
        {SDLK_RETURN, Start},
        {SDLK_LCTRL, ZButton},
        {SDLK_SPACE, AButton},
        {SDLK_UP, DpadUp},
        {SDLK_DOWN, DpadDown},
        {SDLK_LEFT, DpadLeft},
        {SDLK_RIGHT, DpadRight},
        {SDLK_1, LButton},
        {SDLK_2, RButton},
        {SDLK_I, UpCButton},
        {SDLK_J, LeftCButton},
        {SDLK_K, DownCButton},
        {SDLK_L, RightCButton}
    };
    std::unordered_map<uint32_t, JoypadButton> buttonMap = {
        {SDL_GAMEPAD_BUTTON_DPAD_DOWN, DpadDown},
        {SDL_GAMEPAD_BUTTON_DPAD_UP, DpadUp},
        {SDL_GAMEPAD_BUTTON_DPAD_LEFT, DpadLeft},
        {SDL_GAMEPAD_BUTTON_DPAD_RIGHT, DpadRight},
        {SDL_GAMEPAD_BUTTON_EAST, AButton},
        {SDL_GAMEPAD_BUTTON_SOUTH, BButton},
        {SDL_GAMEPAD_BUTTON_START, Start},
        {SDL_GAMEPAD_BUTTON_LEFT_SHOULDER, LButton},
        {SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER, RButton}
    };

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
                    if (keyboardMap.contains(event.key.key)) {
                        cpu.bus.updateButton(keyboardMap[event.key.key], true);
                    } else {
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
                        }
                    }
                    break;
                case SDL_EVENT_KEY_UP:
                    if (keyboardMap.contains(event.key.key)) {
                        cpu.bus.updateButton(keyboardMap[event.key.key], false);
                    } else {
                        switch (event.key.key) {
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
                        }
                    }
                    break;
                case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
                    if (buttonMap.contains(event.gbutton.button)) {
                        cpu.bus.updateButton(buttonMap[event.gbutton.button], true);
                    }
                    break;
                case SDL_EVENT_GAMEPAD_BUTTON_UP:
                    if (buttonMap.contains(event.gbutton.button)) {
                        cpu.bus.updateButton(buttonMap[event.gbutton.button], true);
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