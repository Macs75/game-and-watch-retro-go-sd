#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * PICO-8 emulator entry point.
 *
 * The actual engine (pico8.bin) is a separately-distributed third-party
 * overlay loaded from /cores/pico8.bin on the SD card at runtime. This
 * stub firmware does not contain the engine itself; selecting a PICO-8
 * cart in the launcher invokes app_main_pico8(), which displays an
 * install-prompt screen and returns to the main menu when the user
 * presses any button.
 *
 * Replace /cores/pico8.bin on the SD card with the real engine binary
 * to enable PICO-8 cartridge playback.
 */
void app_main_pico8(uint8_t load_state, uint8_t start_paused, int8_t save_slot);

#ifdef __cplusplus
}
#endif
