#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <map>
#include "libretro.h"

#define LOG(fmt, ...) if (libretro::log) libretro::log(RETRO_LOG_INFO, "[Hack] " fmt "\n", ##__VA_ARGS__); else fprintf(stderr, fmt "\n", ##__VA_ARGS__);


#define CPU_RATE 2000000
#define VIDEO_FRAME_RATE 60
#define AUDIO_SAMPLE_RATE 0


#define ROM_BUFFER_SIZE 0x8000
#define RAM_BUFFER_SIZE 0x8000
#define SCREEN_BUFFER_OFFSET 0x4000
#define SCREEN_BUFFER_SIZE 0x2000
#define KEYBOARD_BUFFER_OFFSET 0x6000
#define KEYBOARD_BUFFER_SIZE 0x0001

//==============================================================================
// Machine state
//==============================================================================
static uint16_t rom[ROM_BUFFER_SIZE];
static uint16_t ram[RAM_BUFFER_SIZE];
static uint16_t framebuffer[512*256];
static uint16_t A, D, PC;


//==============================================================================
// Libretro Callbacks
//==============================================================================
namespace libretro {
    static retro_environment_t environment = nullptr;
    static retro_video_refresh_t video_refresh = nullptr;
    static retro_audio_sample_t audio_sample = nullptr;
    static retro_audio_sample_batch_t audio_sample_batch = nullptr;
    static retro_input_poll_t input_poll = nullptr;
    static retro_input_state_t input_state = nullptr;

    static retro_log_printf_t log = nullptr;
}

RETRO_API void retro_set_environment(retro_environment_t cb) {
    libretro::environment = cb;
}
RETRO_API void retro_set_video_refresh(retro_video_refresh_t cb) {
    libretro::video_refresh = cb;
}
RETRO_API void retro_set_audio_sample(retro_audio_sample_t cb) {
    libretro::audio_sample = cb;
}
RETRO_API void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) {
    libretro::audio_sample_batch = cb;
}
RETRO_API void retro_set_input_poll(retro_input_poll_t cb) {
    libretro::input_poll = cb;
}
RETRO_API void retro_set_input_state(retro_input_state_t cb) {
    libretro::input_state = cb;
}


//==============================================================================
// Core Info
//==============================================================================
void retro_get_system_info(struct retro_system_info *info) {
    memset(info, 0, sizeof(*info));
    info->library_name = "Hack";
    info->library_version = "0.1.0";
    info->need_fullpath = false;
    info->valid_extensions = "hack";
}

void retro_get_system_av_info(struct retro_system_av_info *info) {
    memset(info, 0, sizeof(*info));
    info->timing.fps            = VIDEO_FRAME_RATE,
    info->timing.sample_rate    = AUDIO_SAMPLE_RATE,
    info->geometry.base_width   = 512;
    info->geometry.base_height  = 256;
    info->geometry.max_width    = 512;
    info->geometry.max_height   = 256;
    info->geometry.aspect_ratio = 2;

    enum retro_pixel_format pixel_format = RETRO_PIXEL_FORMAT_RGB565;
    libretro::environment(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &pixel_format);
}


//==============================================================================
// Initialization
//==============================================================================
unsigned retro_api_version(void) {
  return RETRO_API_VERSION;
}

void retro_init() {
    /* set up some logging */
    struct retro_log_callback log;

    if (libretro::environment(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log)) {
        libretro::log = log.log;
    }

    // the performance level is guide to frontend to give an idea of how intensive this core is to run
    unsigned performanceLevel = 1;
    libretro::environment(RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL, &performanceLevel);
}

// End of retrolib
void retro_deinit() {
}


//==============================================================================
// Load/Unload cartridge
//==============================================================================
bool retro_load_game(const struct retro_game_info *info) {
    if (info && info->data) { // ensure there is ROM data
        memset(rom, 0, sizeof(rom));
        memset(ram, 0, sizeof(ram));
        uint16_t rom_size = 0;

        char* buffer = (char*)malloc(info->size + 1);
        memcpy(buffer, info->data, info->size);
        buffer[info->size] = '\0';

        char *cursor = buffer;
        while(true) {
            char *endptr = nullptr;
            errno = 0;
            long cmd = strtol(cursor, &endptr, 2);
            if (errno) {
                LOG("retro_load_game: Failed to parse file");
                free(buffer);
                return false;
            }
            if (endptr == cursor) {
                free(buffer);
                return true;
            }

            rom[rom_size++] = cmd;
            cursor = endptr;
        }
    }

    return false;
}

bool retro_load_game_special(unsigned game_type, const struct retro_game_info *info, size_t num_info) {
    return false;
}

// Unload the cartridge
void retro_unload_game() {
}


//==============================================================================
// Input Mapping
//==============================================================================
static std::map<unsigned, uint16_t> joypad_bindings = {
    {RETRO_DEVICE_ID_JOYPAD_LEFT,  130},
    {RETRO_DEVICE_ID_JOYPAD_UP,    131},
    {RETRO_DEVICE_ID_JOYPAD_RIGHT, 132},
    {RETRO_DEVICE_ID_JOYPAD_DOWN,  133},
    {RETRO_DEVICE_ID_JOYPAD_START,  128},
    {RETRO_DEVICE_ID_JOYPAD_SELECT, 129},

    {RETRO_DEVICE_ID_JOYPAD_X,  136},
    {RETRO_DEVICE_ID_JOYPAD_B,  137},
    {RETRO_DEVICE_ID_JOYPAD_Y,  134},
    {RETRO_DEVICE_ID_JOYPAD_A,  135},

    {RETRO_DEVICE_ID_JOYPAD_L,  141},
    {RETRO_DEVICE_ID_JOYPAD_R,  142},
    {RETRO_DEVICE_ID_JOYPAD_L2, 143},
    {RETRO_DEVICE_ID_JOYPAD_R2, 144},
    {RETRO_DEVICE_ID_JOYPAD_L3, 145},
    {RETRO_DEVICE_ID_JOYPAD_R3, 146},
};

static uint16_t read_hack_keyboard() {
    libretro::input_poll();
    for (auto input_it = joypad_bindings.begin(); input_it != joypad_bindings.end(); input_it++) {
        if (libretro::input_state(0, RETRO_DEVICE_JOYPAD, 0, input_it->first)) {
            return input_it->second;
        }
    }
    return 0;
}

void retro_set_controller_port_device(unsigned port, unsigned device) {
    LOG("retro_set_controller_port_device: %d, %d", port, device);
}


//==============================================================================
// Emulate CPU
//==============================================================================
void retro_reset() {
    memset(ram, 0, sizeof(ram));
    A = D = PC = 0;
}

void retro_tick() {
    uint16_t opcode = rom[PC & 0x7FFF];
    PC++;

    if (opcode & (1<<15)) {
        // Type C instruction

        //Calculate ALU output
        uint16_t x = D;
        uint16_t y = A;
        if (opcode & (1<<12))
            y = ram[y & 0x7FFF];
        if (opcode & (1<<11))
            x = 0;
        if (opcode & (1<<10))
            x = ~x;
        if (opcode & (1<< 9))
            y = 0;
        if (opcode & (1<< 8))
            y = ~y;

        uint16_t ret = opcode & (1<< 7) ? x + y : x & y;
        if (opcode & (1<< 6))
            ret = ~ret;

        //Perform jumps
        if (ret & 0x8000) {
            if (opcode & (1<< 2)) { // jump less
                PC = A;
            }
        } else if (ret == 0) {
            if (opcode & (1<< 1)) { // jump equal
                PC = A;
            }
        } else {
            if (opcode & (1<< 0)) { // jump greater
                PC = A;
            }
        }

        // Write output to registers
        if (opcode & (1<< 3)) {
            ram[A & 0x7FFF] = ret;
        }
        if (opcode & (1<< 4)) {
            D = ret;
        }
        if (opcode & (1<< 5)) {
            A = ret;
        }
    } else {
        // Type A instruction
        A = opcode & 0x7FFF;
    }
}

void retro_run() {
    // Read input
    ram[KEYBOARD_BUFFER_OFFSET] = read_hack_keyboard();

    // Run CPU for a few cycles
    for (int i=0; i<CPU_RATE/VIDEO_FRAME_RATE; i++) {
        retro_tick();
    }

    // Refresh Screen
    uint16_t *fb = ram + SCREEN_BUFFER_OFFSET;
    for (int i=0; i<512*256; i++) {
        framebuffer[i] = (fb [i/16] & (1<<(i%16)) ) ? 0x0000 : 0xFFFF;
    }
    libretro::video_refresh(framebuffer, 512, 256, sizeof(unsigned short) * 512);
}


//==============================================================================
// Save/Restore state
//==============================================================================
size_t retro_serialize_size() {
    return sizeof(A) + sizeof(D) + sizeof(PC) + sizeof(ram);
}

/* Serializes internal state. If failed, or size is lower than
 * retro_serialize_size(), it should return false, true otherwise. */
bool retro_serialize(void *data, size_t size) {
    if (data == nullptr || size < retro_serialize_size()) {
        return false;
    }
    char* blob = (char*)data;
    memcpy(blob, &A, sizeof(A));
    blob += sizeof(A);
    memcpy(blob, &D, sizeof(D));
    blob += sizeof(D);
    memcpy(blob, &PC, sizeof(PC));
    blob += sizeof(PC);
    memcpy(blob, &ram, sizeof(ram));
    blob += sizeof(ram);

    return true;
}

bool retro_unserialize(const void *data, size_t size) {
    if (data == nullptr || size < retro_serialize_size()) {
        return false;
    }
    char* blob = (char*)data;
    memcpy(&A, blob, sizeof(A));
    blob += sizeof(A);
    memcpy(&D, blob, sizeof(D));
    blob += sizeof(D);
    memcpy(&PC, blob, sizeof(PC));
    blob += sizeof(PC);
    memcpy(&ram, blob, sizeof(ram));
    blob += sizeof(ram);

    return true;
}


//==============================================================================
// Cheats
//==============================================================================
void retro_cheat_reset() {
    LOG("retro_cheat_reset");
}
void retro_cheat_set(unsigned index, bool enabled, const char *code) {
    LOG("retro_cheat_set");
}


//==============================================================================
// Region
//==============================================================================
unsigned retro_get_region(void) {
    return RETRO_REGION_NTSC;
}


//==============================================================================
// Memory Inspection
//==============================================================================
void *retro_get_memory_data(unsigned id) {
    switch (id) {
        case RETRO_MEMORY_SYSTEM_RAM:
            return ram;
        default:
            return 0;
    }
}
size_t retro_get_memory_size(unsigned id) {
    switch (id) {
        case RETRO_MEMORY_SYSTEM_RAM:
            return sizeof(ram);
        default:
            return 0;
    }
}
