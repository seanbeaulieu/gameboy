#include "../include/cpu.h"
#include "../include/bus.h"
#include "../include/ppu.h"
#include <stdio.h>
#include <string.h>

#include <SDL2/SDL.h>

// global SDL variables
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *screen_texture = NULL;

FILE *log_file = NULL;
#define MAX_CYCLES 10000000

void debug_print(cpu *gameboy) {
    fprintf(log_file, "A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X SP:%04X PC:%04X PCMEM:%02X,%02X,%02X,%02X\n",
           gameboy->registers.a,
           flags_register_to_byte(gameboy->registers.f),
           gameboy->registers.b,
           gameboy->registers.c,
           gameboy->registers.d,
           gameboy->registers.e,
           gameboy->registers.h,
           gameboy->registers.l,
           gameboy->registers.sp,
           gameboy->registers.pc,
           bus_read8(&gameboy->bus, gameboy->registers.pc),
           bus_read8(&gameboy->bus, gameboy->registers.pc + 1),
           bus_read8(&gameboy->bus, gameboy->registers.pc + 2),
           bus_read8(&gameboy->bus, gameboy->registers.pc + 3));
}

// initialize SDL and create window/renderer
int init_display() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "failed to initialize sdl2: %s\n", SDL_GetError());
        return -1;
    }

    // create window (scaled up by 4 for visibility)
    window = SDL_CreateWindow(
        "gameboy emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH * 4,
        SCREEN_HEIGHT * 4,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        fprintf(stderr, "failed to create window: %s\n", SDL_GetError());
        return -1;
    }

    // create renderer
    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer) {
        fprintf(stderr, "failed to create renderer: %s\n", SDL_GetError());
        return -1;
    }

    // set scaling quality
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    // create texture for screen buffer
    screen_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );

    if (!screen_texture) {
        fprintf(stderr, "failed to create texture: %s\n", SDL_GetError());
        return -1;
    }

    return 0;
}

// cleanup SDL resources
void cleanup_display() {
    if (screen_texture) {
        SDL_DestroyTexture(screen_texture);
    }
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

// convert gameboy colors to SDL colors
uint32_t gb_colors[4] = {
    0xE0F8D0FF,  // lightest green
    0x88C070FF,  // light green  
    0x346856FF,  // dark green
    0x081820FF   // darkest green/black
};

// callback function for PPU frame completion
void display_frame(uint8_t *buffer) {
    uint32_t pixels[SCREEN_WIDTH * SCREEN_HEIGHT];
    
    // add debug print
    // printf("Display frame called, first few pixels: %d %d %d %d\n", 
    //        buffer[0], buffer[1], buffer[2], buffer[3]);
    
    // convert gameboy palette to RGBA
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        pixels[i] = gb_colors[buffer[i]];
    }

    // update texture with new frame
    SDL_UpdateTexture(screen_texture, NULL, pixels, SCREEN_WIDTH * sizeof(uint32_t));
    
    // clear renderer and draw new frame
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, screen_texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

// event handler for main
// based on the button selected, set the corresponding bit (to 0)
void handle_input(SDL_Event *event, bus *bus) {
    switch(event->type) {
        case SDL_KEYDOWN:
            switch(event->key.keysym.sym) {
              // dpad
                case SDLK_RIGHT:
                    bus->joypad_select &= ~0x10;  // clear bit 4 for dpad
                    bus->dpad_state &= ~0x01;
                    // print_bits(bus->joypad_select, "select bits");
                    // print_bits(bus->button_state, "button state");
                    // print_bits(bus->dpad_state, "dpad state");
                    // printf("---\n");
                    break;
                case SDLK_LEFT:
                    bus->joypad_select &= ~0x10;
                    bus->dpad_state &= ~0x02;
                    // printf("left press: select=%02X dpad_state=%02X\n", bus->joypad_select, bus->dpad_state);
                    // print_bits(bus->joypad_select, "select bits");
                    // print_bits(bus->button_state, "button state");
                    // print_bits(bus->dpad_state, "dpad state");
                    // printf("---\n");
                    break;
                case SDLK_UP:
                    bus->joypad_select &= ~0x10;
                    bus->dpad_state &= ~0x04;
                    // printf("up press: select=%02X dpad_state=%02X\n", bus->joypad_select, bus->dpad_state);
                    // print_bits(bus->joypad_select, "select bits");
                    // print_bits(bus->button_state, "button state");
                    // print_bits(bus->dpad_state, "dpad state");
                    // printf("---\n");
                    break;
                case SDLK_DOWN:
                    bus->joypad_select &= ~0x10;
                    bus->dpad_state &= ~0x08;
                    // printf("down press: select=%02X dpad_state=%02X\n", bus->joypad_select, bus->dpad_state);
                    // print_bits(bus->joypad_select, "select bits");
                    // print_bits(bus->button_state, "button state");
                    // print_bits(bus->dpad_state, "dpad state");
                    // printf("---\n");
                    break;

                // buttons
                case SDLK_a:  // A button
                    bus->joypad_select &= ~0x20;  // clear bit 5 for buttons
                    bus->button_state &= ~0x01;
                    // print_bits(bus->joypad_select, "select bits");
                    // print_bits(bus->button_state, "button state");
                    // print_bits(bus->dpad_state, "dpad state");
                    // printf("---\n");
                    break;
                case SDLK_s:  // B button 
                    bus->joypad_select &= ~0x20;
                    bus->button_state &= ~0x02;
                    // printf("b press: select=%02X button_state=%02X\n", bus->joypad_select, bus->button_state);
                    // print_bits(bus->joypad_select, "select bits");
                    // print_bits(bus->button_state, "button state");
                    // print_bits(bus->dpad_state, "dpad state");
                    // printf("---\n");
                    break;
                case SDLK_q:  // select
                    bus->joypad_select &= ~0x20;
                    bus->button_state &= ~0x04;
                    // printf("select press: select=%02X button_state=%02X\n", bus->joypad_select, bus->button_state);
                    // print_bits(bus->joypad_select, "select bits");
                    // print_bits(bus->button_state, "button state");
                    // print_bits(bus->dpad_state, "dpad state");
                    // printf("---\n");
                    break;
                case SDLK_w:  // start
                    bus->joypad_select &= ~0x20;
                    bus->button_state &= ~0x08;
                    // printf("start press: select=%02X button_state=%02X\n", bus->joypad_select, bus->button_state);
                    // print_bits(bus->joypad_select, "select bits");
                    // print_bits(bus->button_state, "button state");
                    // print_bits(bus->dpad_state, "dpad state");
                    // printf("---\n");
                    break;
                }
                break;
          
      case SDL_KEYUP:
          switch(event->key.keysym.sym) {
                // dpad releases
                case SDLK_RIGHT:
                    bus->dpad_state |= 0x01;
                    //printf("right release: select=%02X dpad_state=%02X\n", bus->joypad_select, bus->dpad_state);
                    break;
                case SDLK_LEFT:
                    bus->dpad_state |= 0x02;
                    //printf("left release: select=%02X dpad_state=%02X\n", bus->joypad_select, bus->dpad_state);
                    break;
                case SDLK_UP:
                    bus->dpad_state |= 0x04;
                    //printf("up release: select=%02X dpad_state=%02X\n", bus->joypad_select, bus->dpad_state);
                    break;
                case SDLK_DOWN:
                    bus->dpad_state |= 0x08;
                    //printf("down release: select=%02X dpad_state=%02X\n", bus->joypad_select, bus->dpad_state);
                    break;

                // button releases    
                case SDLK_a:
                    bus->button_state |= 0x01;
                    //printf("a release: select=%02X button_state=%02X\n", bus->joypad_select, bus->button_state);
                    break;
                case SDLK_s:
                    bus->button_state |= 0x02;
                    //printf("b release: select=%02X button_state=%02X\n", bus->joypad_select, bus->button_state);
                    break;
                case SDLK_q:
                    bus->button_state |= 0x04;
                    //printf("select release: select=%02X button_state=%02X\n", bus->joypad_select, bus->button_state);
                    break;
                case SDLK_w:
                    bus->button_state |= 0x08;
                    //printf("start release: select=%02X button_state=%02X\n", bus->joypad_select, bus->button_state);
                    break;
          }
          
          bus->joypad_select |= 0x30;  // set bits 4-5 (nothing selected)
          break;
  }
}

int main(int argc, char *argv[]) {
    cpu gameboy;
    ppu PPU;
    memset(&gameboy, 0, sizeof(cpu));

    // init everything
    bus_init(&gameboy.bus);
    cpu_init(&gameboy, &PPU);  // pass in the PPU pointer

    // calls test init, setting to the DMG defaults for boot
    cpu_init_test(&gameboy.registers);

    // setup PPU
    ppu_set_frame_callback(&PPU, display_frame);
    ppu_init(&PPU, &gameboy.bus);
    printf("callback set: %s\n", PPU.frame_complete_callback != NULL ? "yes" : "no");
    // ppu_init(&PPU, &gameboy.bus);

    // open log file
    log_file = fopen("logfile.txt", "w");
    if (log_file == NULL) {
        fprintf(stderr, "Failed to open log file. Exiting.\n");
        return 1;
    }

    // load rom 
    if (argc < 2) {
        printf("please provide the path to the ROM file.\n");
        return 1;
    }

    const char *rom_path = argv[1];
    if (load_rom(&gameboy.bus, rom_path) == 0) {
        
    } 
    
    else {
        fprintf(stderr, "failed to load ROM. exiting.\n");
        return 1;
    }

    // initialize SDL display
    if (init_display() < 0) {
        fprintf(stderr, "display initialization failed\n");
        return 1;
    }

    // game loop
    SDL_Event event;
    int running = 1;
    
    while (running) {
        // handle SDL events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            // call handler for SDL inputs
            handle_input(&event, &gameboy.bus);
        }
        // debug_print(&gameboy);
        
        cpu_step(&gameboy);
        // ppu_step(&PPU);
        
    }

    // cleanup
    cleanup_display();
    bus_free(&gameboy.bus);
    fclose(log_file);
    return 0;
}

///////////////////////////////////////////////////////////////////

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <dirent.h>
// #include <sys/stat.h> 
// #include <unistd.h> 
// #include "../include/cpu.h"
// #include "../include/bus.h"
// #include "../include/ppu.h"
// #include "../include/jsontest.h"

// int main(int argc, char *argv[]) {
//     if (argc != 2) {
//         printf("usage: %s <test_file_or_directory>\n", argv[0]);
//         return 1;
//     }

//     // Check if argument is a directory or file
//     struct stat path_stat;
//     if (stat(argv[1], &path_stat) != 0) {
//         printf("error: cannot access %s\n", argv[1]);
//         return 1;
//     }

//     if (S_ISDIR(path_stat.st_mode)) {
//         // Handle directory
//         DIR *dir = opendir(argv[1]);
//         if (!dir) {
//             printf("failed to open directory: %s\n", argv[1]);
//             return 1;
//         }

//         struct dirent *entry;
//         char filepath[1024];
//         while ((entry = readdir(dir)) != NULL) {
//             if (strstr(entry->d_name, ".json")) {
//                 // Create new CPU instance for each test file
//                 cpu gameboy;
//                 memset(&gameboy, 0, sizeof(cpu));
//                 cpu_init(&gameboy.registers);
//                 bus_init(&gameboy.bus);

//                 snprintf(filepath, sizeof(filepath), "%s/%s", argv[1], entry->d_name);
//                 run_test_file(filepath, &gameboy);

//                 // Cleanup after each test file
//                 bus_free(&gameboy.bus);
//             }
//         }
//         closedir(dir);
//     } else {
//         // Handle single file
//         cpu gameboy;
//         memset(&gameboy, 0, sizeof(cpu));
//         cpu_init(&gameboy.registers);
//         bus_init(&gameboy.bus);
        
//         run_test_file(argv[1], &gameboy);
        
//         bus_free(&gameboy.bus);
//     }

//     return 0;
// }