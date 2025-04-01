#include <iostream>
#include "cpu/CPU.cpp"
#include <iterator>
#include <SDL2/SDL.h>
#include "interface.cpp"
#include "controller/Controller.hpp"
#if __APPLE__
    #include <sysdir.h>  // for sysdir_start_search_path_enumeration
    #include <glob.h>    // for glob needed to expand ~ to user dir
    #include <execinfo.h>
#endif

const uint32_t DPAD_DOWN = 0;
const uint32_t DPAD_UP = 1;
const uint32_t DPAD_LEFT = 2;
const uint32_t DPAD_RIGHT = 3;
const uint32_t BUTTON_A = 4;
const uint32_t BUTTON_B = 5;
const uint32_t BUTTON_START = 6;
const uint32_t BUTTON_L = 7;
const uint32_t BUTTON_R = 8;

SDL_Joystick* findController() {
    int count = SDL_NumJoysticks();

    for (int i = 0; i < count; i++) {
        SDL_Joystick* gamepad = SDL_JoystickOpen(i);

        if (gamepad != nullptr) {
            return gamepad;
        }
    }

    return nullptr;
}
#if __APPLE__
    void handler(int sig) {
        void *array[50];
        size_t size;

        // get void*'s for all entries on the stack
        size = backtrace(array, 50);

        // print out all the frames to stderr
        fprintf(stderr, "Error: signal %d:\n", sig);
        backtrace_symbols_fd(array, size, STDERR_FILENO);
        throw std::runtime_error("Runtime error");
    }
#endif

int main(int argc, char **argv) {
    #if __APPLE__
        signal(SIGSEGV, handler);
    #endif

    CPU cpu;
    SDL_Event event;

    if (argc < 1) {
        throw std::runtime_error("Please specify a filename.");
    }

    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0) {
        std::println("Error: {}", SDL_GetError());
        throw std::runtime_error("Could not initialize SDL");
    }

    SDL_Window* window = SDL_CreateWindow("N64+", 0, 0, 640, 480, SDL_WINDOW_VULKAN);

    std::println("successfully created window");

    if(window == NULL || window == nullptr) {
        std::println("window creation Error: %s\n", SDL_GetError());
        throw std::runtime_error("Could not initialize SDL");
    }

    cpu.bus.loadRom(argv[1]);
    cpu.bus.setCic();
    std::vector<std::string> saveNames = cpu.bus.getSaveNames(argv[1]);

    std::string basePath;

    // gotten from https://stackoverflow.com/questions/5123361/finding-library-application-support-from-c
    #if __APPLE__
        char path[PATH_MAX];
        auto state = sysdir_start_search_path_enumeration(SYSDIR_DIRECTORY_APPLICATION_SUPPORT,
                                                        SYSDIR_DOMAIN_MASK_USER);
        if ((state = sysdir_get_next_search_path_enumeration(state, path))) {
            glob_t globbuf;
            if (glob(path, GLOB_TILDE, nullptr, &globbuf) == 0) {
                std::string result(globbuf.gl_pathv[0]);
                globfree(&globbuf);
                basePath = result;
            } else {
                throw std::runtime_error("Unable to expand tilde");
            }
        } else {
            throw std::runtime_error("could not open directory to application support");
        }
    #elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        throw std::runtime_error("TODO: Windows support");
    #endif

    std::string separator = "/N64+/";

    std::string savesDir = basePath + separator;

    std::filesystem::create_directory(savesDir);

    for (std::string& saveName: saveNames) {
        saveName = savesDir + saveName;
    }

    cpu.bus.openSaves(saveNames);

    GFX_INFO gfxInfo;

    gfxInfo.RDRAM = &cpu.bus.rdram[0];
    gfxInfo.DMEM = &cpu.bus.rsp.dmem[0];
    gfxInfo.RDRAM_SIZE = cpu.bus.rdram.size();
    gfxInfo.DPC_CURRENT_REG = &cpu.bus.rdp.current;
    gfxInfo.DPC_START_REG = &cpu.bus.rdp.start;
    gfxInfo.DPC_END_REG = &cpu.bus.rdp.end;
    gfxInfo.DPC_STATUS_REG = &cpu.bus.rdp.status.value;
    gfxInfo.debugOn = &cpu.debugOn;


    std::println("initializing rdp");

    rdp_init(window, gfxInfo, false, false, false);

    std::println("rdp initialize success!");

    std::println("attempting to init audio");
    cpu.bus.initAudio();
    std::println("init audio success");
    std::println("finding controller...");
    cpu.bus.gamepad = findController();
    std::println("search completed");

    uint8_t xAxis = 0;
    uint8_t yAxis = 0;

    std::unordered_map<uint32_t, JoypadButton> keyboardMap = {
        {SDLK_e, BButton},
        {SDLK_RETURN, Start},
        {SDLK_LCTRL, ZButton},
        {SDLK_SPACE, AButton},
        {SDLK_UP, DpadUp},
        {SDLK_DOWN, DpadDown},
        {SDLK_LEFT, DpadLeft},
        {SDLK_RIGHT, DpadRight},
        {SDLK_1, LButton},
        {SDLK_2, RButton},
        {SDLK_i, UpCButton},
        {SDLK_j, LeftCButton},
        {SDLK_k, DownCButton},
        {SDLK_l, RightCButton}
    };
    std::unordered_map<uint32_t, JoypadButton> buttonMap = {
        {DPAD_DOWN, DpadDown},
        {DPAD_UP, DpadUp},
        {DPAD_LEFT, DpadLeft},
        {DPAD_RIGHT, DpadRight},
        {BUTTON_A, AButton},
        {BUTTON_B, BButton},
        {BUTTON_START, Start},
        {BUTTON_L, LButton},
        {BUTTON_R, RButton}
    };

    while (true) {
        while (!cpu.bus.rdp.frameFinished) {
            cpu.step();
        }

        cpu.limitFps();

        if (cpu.bus.timeSinceSaveWrite != 0) {
            auto p1 = std::chrono::system_clock::now();

            uint64_t currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                p1.time_since_epoch()).count();

            if (currentTime - cpu.bus.timeSinceSaveWrite >= 500) {
                cpu.bus.writeSave();
            }

        }

        cpu.bus.rdp.frameFinished = false;

        while(SDL_PollEvent(&event) > 0) {
            switch(event.type) {
                case SDL_QUIT:
                    cpu.bus.saveFile.close();
                    exit(0);
                    break;
                case SDL_KEYDOWN:
                    if (keyboardMap.contains(event.key.keysym.scancode)) {
                        cpu.bus.updateButton(keyboardMap[event.key.keysym.scancode], true);
                    } else {
                        switch (event.key.keysym.scancode) {
                            case SDLK_g:
                                cpu.debugOn = !cpu.debugOn;
                                break;
                            case SDLK_w:
                                yAxis = (uint8_t)MAX_AXIS_VAL;
                                break;
                            case SDLK_a:
                                xAxis = (uint8_t)-MAX_AXIS_VAL;
                                break;
                            case SDLK_s:
                                yAxis = (uint8_t)-MAX_AXIS_VAL;
                                break;
                            case SDLK_d:
                                xAxis = (uint8_t)MAX_AXIS_VAL;
                                break;
                        }
                    }
                    break;
                case SDL_KEYUP:
                    if (keyboardMap.contains(event.key.keysym.scancode)) {
                        cpu.bus.updateButton(keyboardMap[event.key.keysym.scancode], false);
                    } else {
                        switch (event.key.keysym.scancode) {
                            case SDLK_w:
                                yAxis = 0;
                                break;
                            case SDLK_a:
                                xAxis = 0;
                                break;
                            case SDLK_s:
                                yAxis = 0;
                                break;
                            case SDLK_d:
                                xAxis = 0;
                                break;
                        }
                    }
                    break;
                case SDL_JOYBUTTONDOWN:
                    std::println("you pressed button {}", event.jbutton.button);
                    if (buttonMap.contains(event.jbutton.button)) {
                        cpu.bus.updateButton(buttonMap[event.jbutton.button], true);
                    }
                    break;
                case SDL_JOYBUTTONUP:
                    if (buttonMap.contains(event.jbutton.button)) {
                        cpu.bus.updateButton(buttonMap[event.jbutton.button], false);
                    }
                    break;
                case SDL_JOYAXISMOTION:
                    if (cpu.bus.gamepad != nullptr) {
                        switch (event.jaxis.axis) {
                            case 0: {
                                int16_t xAxisL = event.jaxis.value / AXIS_DIVISOR;

                                xAxis = std::abs(xAxisL) > 10.0 ? (int8_t)(uint8_t)std::round(xAxisL) : 0;
                                break;
                            }
                            case 1: {
                                int16_t yAxisL = -event.jaxis.value / AXIS_DIVISOR;

                                yAxis = std::abs(yAxisL) > 10.0 ? (int8_t)(uint8_t)std::round(yAxisL) : 0;
                                break;
                            }
                            case 2: {
                                int16_t xAxisR = event.jaxis.value;

                                if (xAxisR > 0x7000) {
                                    cpu.bus.updateButton(RightCButton, true);
                                } else if (xAxisR < -0x7000) {
                                    cpu.bus.updateButton(LeftCButton, true);
                                } else {
                                    cpu.bus.updateButton(LeftCButton, false);
                                    cpu.bus.updateButton(RightCButton, false);
                                }
                                break;
                            }
                            case 3: {
                                int16_t yAxisR = event.jaxis.value;
                                if (yAxisR < -0x7000) {
                                    cpu.bus.updateButton(UpCButton, true);
                                } else if (yAxisR > 0x7000) {
                                    cpu.bus.updateButton(DownCButton, true);
                                } else {
                                    cpu.bus.updateButton(UpCButton, false);
                                    cpu.bus.updateButton(DownCButton, false);
                                }
                                break;
                            }
                            case 5: {
                                int16_t rightTrigger = event.jaxis.value;

                                if (rightTrigger > 0x1000) {
                                    cpu.bus.updateButton(ZButton, true);
                                } else {
                                    cpu.bus.updateButton(ZButton, false);
                                }
                                break;
                            }
                        }
                    }
                    break;
                case SDL_JOYDEVICEADDED:
                    cpu.bus.gamepad = findController();
                    break;
            }
        }

        cpu.bus.updateAxis(xAxis, yAxis);
    }

    SDL_Quit();

    return 0;
}