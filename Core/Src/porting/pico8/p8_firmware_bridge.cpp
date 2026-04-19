/*
 * Firmware ABI bridge — engine-side trampolines.
 *
 * The firmware publishes a struct of function pointers at
 * (VTOR + GW_FIRMWARE_ABI_OFFSET); this file provides one thin trampoline
 * per firmware/libc symbol the engine calls, so the engine's compiled
 * calls go through a fixed-address struct instead of firmware-resident
 * code addresses.
 *
 * Naming: every trampoline uses a `p8_` prefix so it CANNOT collide with
 * the firmware's own newlib/libgcc copies. The engine's object files are
 * post-processed (arm-none-eabi-objcopy --redefine-syms) to rewrite every
 * reference to `memcpy`, `__aeabi_fadd`, etc. into its `p8_`-prefixed
 * form. Firmware continues to call plain `memcpy` which resolves to
 * newlib as always. Two separate symbol namespaces, one linked binary.
 *
 * Keep in sync with gw_firmware_abi.h (104 fields at v1) and with
 * pico8_abi_redefine_syms.txt (the objcopy rename list).
 */

extern "C" {
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
}

#include "gw_firmware_abi.h"

/* Shortcut: the ABI accessor resolves to a couple of loads. In tight
 * loops the compiler caches it into a register, so per-call overhead
 * is ~2 cycles over a direct call. */
static inline const gw_firmware_abi_t *abi(void)
{
    return gw_firmware_abi();
}

extern "C" {

/* ================================================================
 * string.h
 * ================================================================ */
void  *p8_memchr (const void *s, int c, size_t n)                      { return abi()->memchr(s, c, n); }
int    p8_memcmp (const void *a, const void *b, size_t n)              { return abi()->memcmp(a, b, n); }
void  *p8_memcpy (void *d, const void *s, size_t n)                    { return abi()->memcpy(d, s, n); }
void  *p8_memmem (const void *h, size_t hl, const void *n, size_t nl)  { return abi()->memmem(h, hl, n, nl); }
void  *p8_memmove(void *d, const void *s, size_t n)                    { return abi()->memmove(d, s, n); }
void  *p8_memset (void *d, int c, size_t n)                            { return abi()->memset(d, c, n); }
char  *p8_strchr (const char *s, int c)                                { return abi()->strchr(s, c); }
int    p8_strcmp (const char *a, const char *b)                        { return abi()->strcmp(a, b); }
int    p8_strcoll(const char *a, const char *b)                        { return abi()->strcoll(a, b); }
size_t p8_strlen (const char *s)                                       { return abi()->strlen(s); }
int    p8_strncmp(const char *a, const char *b, size_t n)              { return abi()->strncmp(a, b, n); }
char  *p8_strncpy(char *d, const char *s, size_t n)                    { return abi()->strncpy(d, s, n); }
char  *p8_strpbrk(const char *a, const char *b)                        { return abi()->strpbrk(a, b); }
char  *p8_strrchr(const char *s, int c)                                { return abi()->strrchr(s, c); }
size_t p8_strspn (const char *a, const char *b)                        { return abi()->strspn(a, b); }
char  *p8_strstr (const char *a, const char *b)                        { return abi()->strstr(a, b); }
char  *p8_strerror(int e)                                              { return abi()->strerror(e); }

/* ================================================================
 * ctype.h
 * ================================================================ */
int p8_isalnum (int c) { return abi()->isalnum(c); }
int p8_isalpha (int c) { return abi()->isalpha(c); }
int p8_iscntrl (int c) { return abi()->iscntrl(c); }
int p8_isgraph (int c) { return abi()->isgraph(c); }
int p8_islower (int c) { return abi()->islower(c); }
int p8_ispunct (int c) { return abi()->ispunct(c); }
int p8_isspace (int c) { return abi()->isspace(c); }
int p8_isupper (int c) { return abi()->isupper(c); }
int p8_isxdigit(int c) { return abi()->isxdigit(c); }
int p8_tolower (int c) { return abi()->tolower(c); }
int p8_toupper (int c) { return abi()->toupper(c); }

/* ================================================================
 * stdlib.h
 * ================================================================ */
__attribute__((noreturn)) void p8_abort(void) { abi()->abort(); while (1) {} }
void p8_qsort(void *base, size_t nmemb, size_t size,
              int (*cmp)(const void *, const void *))
{
    abi()->qsort(base, nmemb, size, cmp);
}
double p8_strtod(const char *s, char **end)              { return abi()->strtod(s, end); }
long   p8_strtol(const char *s, char **end, int base)    { return abi()->strtol(s, end, base); }

/* ================================================================
 * stdio.h
 * ================================================================ */
FILE  *p8_fopen (const char *p, const char *m)                 { return abi()->fopen(p, m); }
int    p8_fclose(FILE *s)                                      { return abi()->fclose(s); }
size_t p8_fread (void *p, size_t sz, size_t n, FILE *s)        { return abi()->fread(p, sz, n, s); }
size_t p8_fwrite(const void *p, size_t sz, size_t n, FILE *s)  { return abi()->fwrite(p, sz, n, s); }
int    p8_fseek (FILE *s, long o, int w)                       { return abi()->fseek(s, o, w); }
long   p8_ftell (FILE *s)                                      { return abi()->ftell(s); }
int    p8_feof  (FILE *s)                                      { return abi()->feof(s); }
int    p8_ferror(FILE *s)                                      { return abi()->ferror(s); }
int    p8_fgetc (FILE *s)                                      { return abi()->fgetc(s); }
int    p8_getc  (FILE *s)                                      { return abi()->fgetc(s); }
int    p8_fputc (int c, FILE *s)                               { return abi()->fputc(c, s); }
FILE  *p8_freopen(const char *p, const char *m, FILE *s)       { return abi()->freopen(p, m, s); }
int    p8_remove(const char *p)                                { return abi()->remove(p); }
int    p8_putchar(int c)                                       { return abi()->putchar(c); }
int    p8_puts  (const char *s)                                { return abi()->puts(s); }
int    p8_fflush(FILE *s)                                      { return abi()->fflush(s); }
int   *p8___errno(void)                                        { return abi()->__errno(); }

int p8_printf(const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    int r = abi()->vprintf(fmt, ap);
    va_end(ap);
    return r;
}
int p8_fprintf(FILE *s, const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    int r = abi()->vfprintf(s, fmt, ap);
    va_end(ap);
    return r;
}
int p8_snprintf(char *b, size_t n, const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    int r = abi()->vsnprintf(b, n, fmt, ap);
    va_end(ap);
    return r;
}
int p8_sprintf(char *b, const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    int r = abi()->vsprintf(b, fmt, ap);
    va_end(ap);
    return r;
}
int p8_fscanf(FILE *s, const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    int r = abi()->vfscanf(s, fmt, ap);
    va_end(ap);
    return r;
}
int p8_vprintf  (const char *fmt, va_list ap)            { return abi()->vprintf(fmt, ap); }
int p8_vfprintf (FILE *s, const char *fmt, va_list ap)   { return abi()->vfprintf(s, fmt, ap); }
int p8_vsnprintf(char *b, size_t n, const char *fmt, va_list ap) { return abi()->vsnprintf(b, n, fmt, ap); }
int p8_vsprintf (char *b, const char *fmt, va_list ap)   { return abi()->vsprintf(b, fmt, ap); }
int p8_vfscanf  (FILE *s, const char *fmt, va_list ap)   { return abi()->vfscanf(s, fmt, ap); }

/* ================================================================
 * time.h / setjmp.h / locale.h / libm
 * ================================================================ */
time_t p8_time(time_t *t)                           { return abi()->time(t); }

/* setjmp/longjmp: provided as self-contained ARM Thumb implementations
 * for any engine code that calls setjmp/longjmp (renamed to p8_setjmp/
 * p8_longjmp by objcopy). Note: ldo.c's LUAI_TRY uses ABI-direct setjmp
 * instead (gw_firmware_abi()->setjmp), which is the primary path. */
__attribute__((naked))
int p8_setjmp(jmp_buf env)
{
    asm volatile (
        "mov    r1, r0            \n"  /* r1 = jmp_buf pointer */
        "stmia  r1!, {r4-r11}    \n"  /* save callee-saved regs */
        "mov    r2, sp            \n"
        "mov    r3, lr            \n"
        "stmia  r1!, {r2, r3}    \n"  /* save sp + lr */
        "movs   r0, #0           \n"  /* return 0 */
        "bx     lr               \n"
    );
}

__attribute__((naked, noreturn))
void p8_longjmp(jmp_buf env, int val)
{
    asm volatile (
        "mov    r2, r1            \n"  /* save val */
        "mov    r1, r0            \n"  /* r1 = jmp_buf pointer */
        "ldmia  r1!, {r4-r11}    \n"  /* restore callee-saved */
        "ldmia  r1!, {r0, r3}    \n"  /* r0=sp, r3=lr */
        "mov    sp, r0            \n"
        "mov    lr, r3            \n"
        "movs   r0, r2            \n"  /* return val */
        "it     eq                \n"
        "moveq  r0, #1            \n"  /* if val==0, return 1 */
        "bx     lr               \n"
    );
}
struct lconv *p8_localeconv(void)                   { return abi()->localeconv(); }
double p8_pow(double x, double y)                   { return abi()->pow(x, y); }

/* ================================================================
 * libgcc compiler-emitted helpers
 *
 * GCC emits `bl __aeabi_fadd` implicitly when the engine performs float
 * arithmetic (struct-copy memcpy is similarly emitted). The rename step
 * rewrites those emitted calls to `bl p8___aeabi_fadd`, landing here.
 *
 * __aeabi_ldivmod is special: it returns {quot, rem} as an AAPCS
 * register pair, which can't be expressed in portable C. Hand-rolled
 * as a naked asm trampoline that calls the two ABI halves and repacks.
 * ================================================================ */
/* p8___assert_func: provided by main_pico8.c (engine's own __assert_func
 * gets renamed to p8___assert_func by objcopy and handles assertion
 * failures internally). */

int64_t p8___aeabi_d2lz (double d)       { return abi()->aeabi_d2lz(d); }
float   p8___aeabi_l2f  (int64_t v)      { return abi()->aeabi_l2f(v); }
int     p8___popcountsi2(unsigned x)     { return abi()->popcountsi2(x); }

/* p8___aeabi_ldivmod: args r0-r3 = (a_lo, a_hi, b_lo, b_hi);
   result: r0-r1 = quot, r2-r3 = rem. */
__attribute__((naked))
int64_t p8___aeabi_ldivmod(int64_t /*a*/, int64_t /*b*/)
{
    asm volatile (
        "push   {r4-r7, lr}              \n"
        "mov    r4, r0                   \n"  /* save a_lo */
        "mov    r5, r1                   \n"  /* save a_hi */
        "mov    r6, r2                   \n"  /* save b_lo */
        "mov    r7, r3                   \n"  /* save b_hi */
        "bl     p8__bridge_ldivmod_quot  \n"  /* -> r0,r1 = quot */
        "push   {r0, r1}                 \n"  /* save quot */
        "mov    r0, r4                   \n"
        "mov    r1, r5                   \n"
        "mov    r2, r6                   \n"
        "mov    r3, r7                   \n"
        "bl     p8__bridge_ldivmod_rem   \n"  /* -> r0,r1 = rem */
        "mov    r2, r0                   \n"
        "mov    r3, r1                   \n"
        "pop    {r0, r1}                 \n"  /* restore quot */
        "pop    {r4-r7, pc}              \n"
    );
}

int64_t p8__bridge_ldivmod_quot(int64_t a, int64_t b) { return abi()->ldivmod_quot(a, b); }
int64_t p8__bridge_ldivmod_rem (int64_t a, int64_t b) { return abi()->ldivmod_rem(a, b); }

/* p8___aeabi_uldivmod: unsigned 64-bit div/mod (same AAPCS pair convention). */
__attribute__((naked))
uint64_t p8___aeabi_uldivmod(uint64_t /*a*/, uint64_t /*b*/)
{
    asm volatile (
        "push   {r4-r7, lr}              \n"
        "mov    r4, r0                   \n"
        "mov    r5, r1                   \n"
        "mov    r6, r2                   \n"
        "mov    r7, r3                   \n"
        "bl     p8__bridge_uldivmod_quot \n"
        "push   {r0, r1}                 \n"
        "mov    r0, r4                   \n"
        "mov    r1, r5                   \n"
        "mov    r2, r6                   \n"
        "mov    r3, r7                   \n"
        "bl     p8__bridge_uldivmod_rem  \n"
        "mov    r2, r0                   \n"
        "mov    r3, r1                   \n"
        "pop    {r0, r1}                 \n"
        "pop    {r4-r7, pc}              \n"
    );
}
uint64_t p8__bridge_uldivmod_quot(uint64_t a, uint64_t b) { return abi()->uldivmod_quot(a, b); }
uint64_t p8__bridge_uldivmod_rem (uint64_t a, uint64_t b) { return abi()->uldivmod_rem(a, b); }

/* ================================================================
 * FatFs
 * ================================================================ */
int p8_f_opendir (void *dp, const char *path) { return abi()->f_opendir((DIR*)dp, path); }
int p8_f_closedir(void *dp)                   { return abi()->f_closedir((DIR*)dp); }
int p8_f_readdir (void *dp, void *fno)        { return abi()->f_readdir((DIR*)dp, (FILINFO*)fno); }

/* ================================================================
 * G&W hardware: LCD
 * ================================================================ */
void  p8_lcd_swap(void)                   { abi()->lcd_swap(); }
void *p8_lcd_get_active_buffer(void)      { return abi()->lcd_get_active_buffer(); }
void *p8_lcd_get_inactive_buffer(void)    { return abi()->lcd_get_inactive_buffer(); }
void *p8_lcd_clear_active_buffer(void)    { return abi()->lcd_clear_active_buffer(); }
void *p8_lcd_clear_inactive_buffer(void)  { return abi()->lcd_clear_inactive_buffer(); }

/* ================================================================
 * G&W hardware: audio
 * ================================================================ */
void     p8_audio_start_playing(uint16_t len)   { abi()->audio_start_playing(len); }
int16_t *p8_audio_get_active_buffer(void)       { return abi()->audio_get_active_buffer(); }
void     p8_audio_clear_active_buffer(void)     { abi()->audio_clear_active_buffer(); }
void     p8_audio_clear_inactive_buffer(void)   { abi()->audio_clear_inactive_buffer(); }

/* ================================================================
 * G&W hardware: allocators
 * ================================================================ */
void  *p8_itc_malloc(size_t s)                  { return abi()->itc_malloc(s); }
void  *p8_itc_calloc(size_t c, size_t s)        { return abi()->itc_calloc(c, s); }
void   p8_itc_init(void)                        { abi()->itc_init(); }
void  *p8_ram_malloc(size_t s)                  { return abi()->ram_malloc(s); }
size_t p8_ram_get_free_size(void)               { return abi()->ram_get_free_size(); }

/* ================================================================
 * G&W hardware: RTC
 * ================================================================ */
uint8_t p8_GW_GetCurrentYear(void)   { return abi()->GW_GetCurrentYear(); }
uint8_t p8_GW_GetCurrentMonth(void)  { return abi()->GW_GetCurrentMonth(); }
uint8_t p8_GW_GetCurrentDay(void)    { return abi()->GW_GetCurrentDay(); }
uint8_t p8_GW_GetCurrentHour(void)   { return abi()->GW_GetCurrentHour(); }
uint8_t p8_GW_GetCurrentMinute(void) { return abi()->GW_GetCurrentMinute(); }
uint8_t p8_GW_GetCurrentSecond(void) { return abi()->GW_GetCurrentSecond(); }

/* ================================================================
 * G&W hardware: watchdog + HAL
 * ================================================================ */
void     p8_wdog_refresh(void)      { abi()->wdog_refresh(); }
void     p8_HAL_Delay(uint32_t ms)  { abi()->HAL_Delay(ms); }
uint32_t p8_HAL_GetTick(void)       { return abi()->HAL_GetTick(); }

/* ================================================================
 * retro-go: system
 * ================================================================ */
void p8_odroid_system_init(int id, int sr)  { abi()->odroid_system_init(id, sr); }
void p8_odroid_system_emu_init(
    state_handler_t l, state_handler_t s,
    screenshot_handler_t ss, shutdown_handler_t sh,
    sleep_post_wakeup_handler_t w, sram_save_handler_t sr)
{
    abi()->odroid_system_emu_init(l, s, ss, sh, w, sr);
}
__attribute__((noreturn))
void p8_odroid_system_switch_app(int a) { abi()->odroid_system_switch_app(a); while(1){} }

/* ================================================================
 * retro-go: input / display
 * ================================================================ */
void p8_odroid_input_read_gamepad(odroid_gamepad_state_t *o) { abi()->odroid_input_read_gamepad(o); }
int  p8_odroid_display_get_scaling_mode(void)                { return (int)abi()->odroid_display_get_scaling_mode(); }
void p8_odroid_display_set_scaling_mode(int m)               { abi()->odroid_display_set_scaling_mode((odroid_display_scaling_t)m); }

/* ================================================================
 * retro-go: overlay / SD / settings
 * ================================================================ */
int      p8_odroid_overlay_draw_text(uint16_t x, uint16_t y, uint16_t w,
                                      const char *t, uint16_t c, uint16_t bg)
{
    return abi()->odroid_overlay_draw_text(x, y, w, t, c, bg);
}
uint8_t *p8_odroid_overlay_cache_file_in_flash(const char *p, uint32_t *sz, bool sw)
{
    return abi()->odroid_overlay_cache_file_in_flash(p, sz, sw);
}
int      p8_odroid_sdcard_mkdir(const char *p)                { return abi()->odroid_sdcard_mkdir(p); }
int32_t  p8_odroid_settings_app_int32_get(const char *k, int32_t d) { return abi()->odroid_settings_app_int32_get(k, d); }
void     p8_odroid_settings_app_int32_set(const char *k, int32_t v) { abi()->odroid_settings_app_int32_set(k, v); }

/* ================================================================
 * retro-go: common emulator loop
 * ================================================================ */
bool    p8_common_emu_frame_loop(void)                   { return abi()->common_emu_frame_loop(); }
void    p8_common_emu_input_loop(odroid_gamepad_state_t *j,
                                  odroid_dialog_choice_t *o,
                                  void_callback_t r)      { abi()->common_emu_input_loop(j, o, r); }
void    p8_common_emu_input_loop_handle_turbo(odroid_gamepad_state_t *j) { abi()->common_emu_input_loop_handle_turbo(j); }
uint8_t p8_common_emu_sound_get_volume(void)             { return abi()->common_emu_sound_get_volume(); }
bool    p8_common_emu_sound_loop_is_muted(void)          { return abi()->common_emu_sound_loop_is_muted(); }
void    p8_common_emu_sound_sync(bool n)                 { abi()->common_emu_sound_sync(n); }
void    p8_common_ingame_overlay(void)                   { abi()->common_ingame_overlay(); }

/* ================================================================
 * Missing libc
 * ================================================================ */
char *p8_fgets(char *s, int n, FILE *f) { return abi()->fgets(s, n, f); }
/* p8_free / p8_realloc: engine provides its own (pool allocator in
 * main_pico8.c). After objcopy rename, engine's `free`/`realloc`
 * become `p8_free`/`p8_realloc` and resolve to the pool versions. */
int   p8_ungetc(int c, FILE *f)         { return abi()->ungetc(c, f); }

/* ================================================================
 * Firmware data globals — bridge variables initialized at engine
 * init from ABI data-pointer fields.
 *
 * For pointer-type globals (ACTIVE_FILE, ROM_DATA, etc.) the bridge
 * variable holds the SAME VALUE as the firmware global (copied once).
 * For _impure_ptr, same pattern — newlib macros like `stderr` expand
 * to `_impure_ptr->_stderr`, so we need the correct pointer.
 * ================================================================ */
const unsigned char *p8_ROM_DATA = 0;
unsigned             p8_ROM_DATA_LENGTH = 0;
void                *p8_ACTIVE_FILE = 0;  /* retro_emulator_file_t* */
uint8_t             *p8_pico8_code_flash_addr = 0;
uint32_t             p8_pico8_code_flash_size = 0;
uint32_t             p8_ram_start = 0;
struct _reent       *p8__impure_ptr = 0;

/* Called from app_main_pico8 at the very top, before any engine code
 * touches these globals. Copies live values from firmware via ABI. */
void p8_firmware_bridge_init(void)
{
    const gw_firmware_abi_t *a = abi();
    p8_ROM_DATA              = (const unsigned char *)*a->ROM_DATA_ptr;
    p8_ROM_DATA_LENGTH       = *a->ROM_DATA_LENGTH_ptr;
    p8_ACTIVE_FILE           = *a->ACTIVE_FILE_ptr;
    p8_pico8_code_flash_addr = *a->pico8_code_flash_addr_ptr;
    p8_pico8_code_flash_size = *a->pico8_code_flash_size_ptr;
    p8_ram_start             = *a->ram_start_ptr;
    p8__impure_ptr           = (struct _reent *)*a->impure_ptr_ptr;
}

/* Sync writable bridge variables BACK to firmware globals via ABI.
 * Must be called after engine code modifies shared state that firmware
 * functions read (e.g. ram_start is set by engine, read by ram_malloc). */
void p8_firmware_bridge_sync(void)
{
    const gw_firmware_abi_t *a = abi();
    *a->ram_start_ptr = p8_ram_start;
}

/* Return the DTCM p8ram address from the firmware ABI.
 * Called by main_pico8.c to populate p8_platform_t.dtcm_ram. */
void *p8_firmware_bridge_get_dtcm_ram(void)
{
    const gw_firmware_abi_t *a = abi();
    if (a->version >= 1 && a->dtcm_p8ram_start)
        return a->dtcm_p8ram_start;
    return NULL;
}

} /* extern "C" */
