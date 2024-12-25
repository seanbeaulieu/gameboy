This is a project attempting to emulate the hardware of an original 1989 Nintendo Gameboy, codenamed the DMG. The DMG possessed a Sharp SM83 CPU, which was based loosely on the Zilog Z80 and Intel 8080 microprocessors. The SM83 has seven 8-bit registers, which can be combined to form four 16-bit registers. There is also the stack pointer and the program counter registers, both of which are 16 bits. Some instructions set flags after executing, which are contained in the lower bits of one of the 16-bit registers (AF).

The display of the DMG is a 160x144 pixel LCD screen. The original Gameboy only had four shades of green that could be displayed, which is in stark contrast to the 32768 available to the later Gameboy Color. The LCD is managed by a Pixel Processing Unit, which runs at a 59.73 Hz refresh rate, operating alongside the CPU.

The DMG has 8 KiB of both Work RAM and Video RAM. It can play games up to 64kb, although does have functionality for cartridges that add memory via Memory Bank Controllers. 

**CPU:**

The basic control flow / one step of my CPU is:

1) If both the interrupt enable and the interrupt flag are set to 1, and the IME register (enables any interrupt) is enabled, handle interrupts.
2) Fetch the instruction currently being pointed to by the Program Counter. 
3) Increment the internal CPU counter by the amount of T-cycles the instruction takes (4 for every memory access)
4) Tick the PPU once for each M-cycle the instruction takes (1 for every memory access)
5) Execute the instruction
6) Update timers

**PPU:**

CPU step currently takes and ticks the PPU once for each M-cycle of the instruction being executed.  

A PPU step consists of one of:
1) Reading the sprite data to display from OAM memory, storing in a sprite buffer. Grabs info for sprites on a current scanline, and determines which ones should be displayed on the current line. OAM Scan will take 80 dots.
2) Pushing pixel data from tile data to a screen buffer for a specific line, basically 'rendering' the display data. In this step, the PPU will determine what pixels to display in each of the viewports.
3) HBLANK waits a determined amount until 456 total dots (one 2^22 Hz) have passed in the scanline.
4) VBLANK pads 10 scanlines at the end of every frame.
Interrupts:
1) STAT interrupt can be triggered in a few different ways, in the PPU this is most commonly done with a LY=LYC comparison. LY is the internal line counter that tracks which line the PPU is currently rendering. When this is equal to the LYC register (present in address 0xFF45), an interrupt is requested.

**Cartridge Header and Memory Bank Controllers:**
1) 0x0100-0x014F cartridge header
2) MBC1
3) Boot Sequence

**Input:**
1) How can we handle input? 
- SDL event handlers in main
    - SDL_KEYDOWN
    - SDL_KEYUP
- On either of those events, update the input register (0xFF00)
- 0 for being pressed, 1 for not being pressed