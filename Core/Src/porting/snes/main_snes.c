#include <odroid_system.h>

#include <assert.h>
#include "gw_lcd.h"
#include "gw_linker.h"
#include "gw_buttons.h"
#include "rom_manager.h"
#include "common.h"
#include "gw_malloc.h"
#include "rg_storage.h"
#include "odroid_overlay.h"
#include "appid.h"
#include "bilinear.h"
#include "rg_i18n.h"
#include "main_snes.h"

#include "snes/ppu.h"
#include "snes/apu.h"
#include "snes/cpu.h"
#include "snes/dma.h"
#include "snes/input.h"
#include "snes/statehandler.h"

#define SNES_SAMPLE_RATE 48000
#define SNES_FPS 60
#define SNES_AUDIO_BUFFER_LENGTH (SNES_SAMPLE_RATE / SNES_FPS)
// Use DMA-friendly buffer alignment
static int8_t audioBuffer_snes[SNES_AUDIO_BUFFER_LENGTH*2] __attribute__((aligned(32))); // *2 as emulator is filling stereo buffer

static Snes* snes;

static void blit_emulator(void);

static bool LoadState(const char *savePathName) {
    return true;
}

static bool SaveState(const char *savePathName) {
    return true;
}

static void *Screenshot()
{
    lcd_wait_for_vblank();

    lcd_clear_active_buffer();
    blit_emulator();
    return lcd_get_active_buffer();
}

void snes_pcm_submit() {
    if (common_emu_sound_loop_is_muted()) {
        return;
    }

    int32_t factor = common_emu_sound_get_volume() / 2; // Divide by 2 to prevent overflow in stereo mixing
    int16_t* sound_buffer = audio_get_active_buffer();
    uint16_t sound_buffer_length = audio_get_buffer_length();

    for (int i = 0; i < sound_buffer_length; i++) {
        int32_t sample = 0;
        sound_buffer[i] = (sample * factor);
    }
}

static void blit_emulator(void)
{
    snes_setPixels(snes, lcd_get_active_buffer());
}

static void blit(void) {
    blit_emulator();
    common_ingame_overlay();
    lcd_swap();
}


void snes_input_read(odroid_gamepad_state_t *joystick) {
    if (joystick->values[ODROID_INPUT_LEFT]) {
        snes_setButtonState(snes, 1, 6, true);
    } else {
        snes_setButtonState(snes, 1, 6, false);
    }
    if (joystick->values[ODROID_INPUT_RIGHT]) {
        snes_setButtonState(snes, 1, 7, true);
    } else {
        snes_setButtonState(snes, 1, 7, false);
    }
    if (joystick->values[ODROID_INPUT_UP]) {
        snes_setButtonState(snes, 1, 4, true);
    } else {
        snes_setButtonState(snes, 1, 4, false);
    }
    if (joystick->values[ODROID_INPUT_DOWN]) {
        snes_setButtonState(snes, 1, 5, true);
    } else {
        snes_setButtonState(snes, 1, 5, false);
    }
    if (joystick->values[ODROID_INPUT_A]) {
        snes_setButtonState(snes, 1, 0, true);
    } else {
        snes_setButtonState(snes, 1, 0, false);
    }
    if (joystick->values[ODROID_INPUT_B]) {
        snes_setButtonState(snes, 1, 1, true);
    } else {
        snes_setButtonState(snes, 1, 1, false);
    }
    if (joystick->values[ODROID_INPUT_SELECT]) {
        snes_setButtonState(snes, 1, 2, true);
    } else {
        snes_setButtonState(snes, 1, 2, false);
    }
    if (joystick->values[ODROID_INPUT_START]) {
        snes_setButtonState(snes, 1, 3, true);
    } else {
        snes_setButtonState(snes, 1, 3, false);
    }
    if (joystick->values[ODROID_INPUT_Y]) {
        snes_setButtonState(snes, 1, 8, true);
    } else {
        snes_setButtonState(snes, 1, 8, false);
    }
    if (joystick->values[ODROID_INPUT_X]) {
        snes_setButtonState(snes, 1, 9, true);
    } else {
        snes_setButtonState(snes, 1, 9, false);
    }
}

static uint8_t *load_rom()
{
    uint32_t size = ACTIVE_FILE->size;
    uint8_t* buffer = NULL;

    if (size > 1 * 1024) {
        buffer = odroid_overlay_cache_file_in_flash(ACTIVE_FILE->path, &size, false);
    } else {
        buffer = ram_malloc(size);
        if (buffer) {
            odroid_overlay_cache_file_in_ram(ACTIVE_FILE->path, (uint8_t *)buffer);
        }
    }

	return buffer;
}

void app_main_snes(uint8_t load_state, uint8_t start_paused, int8_t save_slot)
{
    uint8_t fps = 60;
    odroid_gamepad_state_t joystick;
    odroid_dialog_choice_t options[] = {
        ODROID_DIALOG_CHOICE_LAST
    };

    printf("Starting SNES\n");

    snes = snes_init();

    if (load_state) {
        odroid_system_emu_load_state(save_slot);
    } else {
        lcd_clear_buffers();
    }

    uint8_t *rom = load_rom();
    if (rom) {
        snes_loadRom(snes, rom, ACTIVE_FILE->size);
    }

    if (snes->palTiming) {
        printf("PAL Timing\n");
        fps = 50;
    } else {
        printf("NTSC Timing\n");
        fps = 60;
    }

    if (start_paused) {
        common_emu_state.pause_after_frames = 2;
        odroid_audio_mute(true);
    } else {
        common_emu_state.pause_after_frames = 0;
    }
    common_emu_state.frame_time_10us = (uint16_t)(100000 / fps + 0.5f);
    lcd_set_refresh_rate(fps);

    odroid_system_init(APPID_SNES, SNES_SAMPLE_RATE);
    odroid_system_emu_init(&LoadState, &SaveState, &Screenshot, NULL);

    // Init Sound
    audio_start_playing(SNES_SAMPLE_RATE/fps);

    while(1)
    {
        wdog_refresh();

        bool drawFrame = common_emu_frame_loop();

        odroid_input_read_gamepad(&joystick);
        common_emu_input_loop(&joystick, options, &blit);
        common_emu_input_loop_handle_turbo(&joystick);

        snes_input_read(&joystick);

        snes_runFrame(snes);

//        if (drawFrame) {
            blit();
//        }

//        snes_pcm_submit();

        common_emu_sound_sync(false);
    }
}
