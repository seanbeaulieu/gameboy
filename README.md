<div align="center">
  <h1>Game Boy (DMG-01) Emulator</h1>
</div>

This is a project attempting to emulate the hardware of an original 1989 Nintendo Gameboy, codenamed the DMG. The DMG possessed a Sharp SM83 CPU, which was based loosely on the Zilog Z80 and Intel 8080 microprocessors. The SM83 has seven 8-bit registers, which can be combined to form four 16-bit registers. There is also the stack pointer and the program counter registers, both of which are 16 bits. Some instructions set flags after executing, which are contained in the lower bits of one of the 16-bit registers (AF).

The display of the DMG is a 160x144 pixel LCD screen. The original Gameboy only had four shades of green that could be displayed, which is in stark contrast to the 32768 available to the later Gameboy Color. The LCD is managed by a Picture Processing Unit, which runs at a 59.73 Hz refresh rate, operating alongside the CPU.

The DMG has 8 KiB of both Work RAM and Video RAM. It can play games up to 64kb, although does have functionality for cartridges that add memory via Memory Bank Controllers. 

<div align="center">
  <h2>Tetris</h2>
  <img src="https://github.com/seanbeaulieu/gameboy/blob/main/tetrisgif.gif" alt="tetris gameplay">
</div>

<div align="center">
  <h2>Pokemon Blue</h2>
  <img src="https://github.com/seanbeaulieu/gameboy/blob/main/pokemonbluegif.gif" alt="tetris gameplay">
</div>

# Components

**CPU:**

The basic control flow / one step of my CPU is:

1) If both the interrupt enable and the interrupt flag are set to 1, and the IME register (enables any interrupt) is enabled, handle interrupts.
2) Fetch the instruction currently being pointed to by the Program Counter. 
3) Increment the internal CPU counter by the amount of T-cycles the instruction takes (4 for every memory access)
4) Tick the PPU once for each instruction executed, and then tick the PPU counter for each T-cycle the instruction takes
5) Execute the instruction
6) Update timers

**PPU:**

CPU step currently takes and ticks the PPU once for each M-cycle of the instruction being executed.  

A PPU step consists of one of:
1) Reading the sprite data to display from OAM memory, storing in a sprite buffer. Grabs info for sprites on a current scanline, and determines which ones should be displayed on the current line. OAM Scan will take 80 dots.
2) Pushing pixel data from tile data to a screen buffer for a specific line, basically 'rendering' the display data. In this step, the PPU will determine what pixels to display in each of the viewports.
3) HBLANK waits a determined amount until 456 total dots (one 2^22 Hz) have passed in the scanline.
4) VBLANK pads 10 scanlines at the end of every frame and triggers an interrupt in the CPU.
Interrupts:
1) STAT interrupt can be triggered in a few different ways, in the PPU this is most commonly done with a LY=LYC comparison. LY is the internal line counter that tracks which line the PPU is currently rendering. When this is equal to the LYC register (present in address 0xFF45), an interrupt is requested.

**Cartridge Header and Memory Bank Controllers:**
1) 0x0100-0x014F cartridge header
    - Can read info about the cartridge (MBC type) at 0x014F.
2) MBC1, MBC3, no MBC
    - My initial goal with this project was to play Pokemon Red/Blue. Therefore, I elected not to implement the full array of MBCs and instead opt for just no MBC and MBC3, the MBC3 being the necessary extension for the US version of Pokemon.
3) Boot Sequence
    - Nintendo traditionally has a bootup sequence that sets the CPU values which is checked against every ROM, crashing the game if they do not match. This is accomplished by setting the values manually at PC = 0x100. 

**Input:**
1) How can we handle input? 
- I use three different joypad integers to track the bits
    - One for the dpad bits, one for the button bits, and one for the bits to select between the two
- SDL event handlers in main
    - SDL_KEYDOWN
    - SDL_KEYUP
    - These set both the d-pad or button select bit (to 0) and the relevant bit (also to 0) in either the dpad or button nibble
- The CPU constantly polls the input register (0xFF00) for input. Because we use different variables to track all components of the input register, we can instead modify the read function to return the relevant bits.

**Bus:**
1) How can we handle access to memory?
- It is commonly recommended to use a single read and write function in order to handle all of the different functionality that occurs.
- There are many times when the CPU should not have access to certain areas of memory depending on the state of other components.
- There are also certain sections of memory that cannot be accessed, or can only be accessed at certain times.

# Running the Emulator
1) Clone the Repo
``` git clone https://github.com/seanbeaulieu/gameboy.git ```
2) Dependencies
- SDL2
- gcc
- make
3) Run your ROM
``` ./gameboy-emulator your_rom.gb ```
4) Controls:
- A: a button
- B: b button
- Q: select
- W: start
- Arrow Keys: dpad

# Acknowledgements 
- The helpful community members in the Emulator Development GB discord channel [(link)](https://discordapp.com/channels/465585922579103744/465586075830845475)
- Tooling and testing by [Robert Heaton](https://robertheaton.com/gameboy-doctor/), [Matt Currie](https://github.com/mattcurrie/dmg-acid2), and Blargg. [Single Step Tests for the SM83](https://github.com/SingleStepTests/sm83) also very informative.
- Documentation from the [pandocs](https://gbdev.io/pandocs/) the best source of GB specifications. 
