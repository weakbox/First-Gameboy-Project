#pragma once

// Normal definitions.
#include <stdio.h>
#include <gb/gb.h>
#include <gbdk/font.h>
#include <rand.h>
#include <gbdk/console.h>

// Bit mask to determine only the joypad bit that relates to D-pad button input.
#define d_pad_bit(x) (x & 0x0F)

// Playable space restrictions.
#define X_MIN 0x08
#define X_MAX 0xA0
#define Y_MIN 0x10
#define Y_MAX 0x90

// Structure to define a generic game entity.
typedef struct _Entity
{
    uint8_t x;
    uint8_t y;
    uint8_t width;
    uint8_t height;
}   Entity;

extern Entity coin;

extern void play_bonk_sfx();
extern void play_coin_sfx();

extern void init_coin(Entity *coin, uint8_t x, uint8_t y);
extern uint8_t get_random_number_in_range(int min, int max);
extern void move_coin(Entity *coin);
extern void animate_coin();
extern void coin_logic();
extern void draw_hud();
