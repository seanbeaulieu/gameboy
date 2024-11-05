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
    0xFFFFFFFF,  // white
    0xAAAAAFFF,  // light gray
    0x555555FF,  // dark gray
    0x000000FF   // black
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

int main(int argc, char *argv[]) {
    cpu gameboy;
    ppu PPU;
    memset(&gameboy, 0, sizeof(cpu));
    // init
    cpu_init(&gameboy.registers);
    cpu_init_test(&gameboy.registers);
    bus_init(&gameboy.bus);
    // set up PPU frame callback
    ppu_set_frame_callback(&PPU, display_frame);
    printf("callback set: %s\n", PPU.frame_complete_callback != NULL ? "yes" : "no");
    ppu_init(&PPU, &gameboy.bus);

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
        }
        // debug_print(&gameboy);
        // existing emulation code
        cpu_step(&gameboy);
        ppu_step(&PPU);
        
    }

    // cleanup
    cleanup_display();
    bus_free(&gameboy.bus);
    fclose(log_file);
    return 0;
}