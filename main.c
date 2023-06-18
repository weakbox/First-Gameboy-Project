// Normal definitions.
#include <stdio.h>
#include <gb/gb.h>
#include <gbdk/font.h>

// Tilesets created by gbtd.
#include "player.c"
#include "window.c"

// Bit mask to determine only the joypad bit that relates to D-pad button input.
#define d_pad_bit(x) (x & 0x0F)

// Playable space restrictions.
#define X_MIN 0x08
#define X_MAX 0xA0
#define Y_MIN 0x10
#define Y_MAX 0x90

int16_t player_coords[2];    // Format: x, y
int8_t player_has_collided_x_min = 0;
int8_t player_has_collided_x_max = 0;
int8_t player_has_collided_y_min = 0;
int8_t player_has_collided_y_max = 0;

// Loads a font into VRAM.
void load_font(uint8_t font[])
{
    font_t min_font;                // Defines a new font variable.
    font_init();                    // Initializes the font library.
    min_font = font_load(font);     // Loads the "font_min" font into the variable we defined.
    font_set(min_font);             // Sets the font that we will be using.
}

// Initializes the Gameboy's sound.
void init_sound()
{
    // Enable sound.
    NR52_REG = 0x80;    // Turns on sound.
    NR50_REG = 0x77;    // Sets the volume for both the left and right channel to the max.
    NR51_REG = 0xFF;    // Selects which channels we want to use. In this case, all of them.
}

// Plays the wall bonk sound effect.
void play_bonk_sfx()
{
    NR10_REG = 0x1E;
    NR11_REG = 0x10;
    NR12_REG = 0xF3;
    NR13_REG = 0x00;
    NR14_REG = 0x87;
}

// Detects if the player has collided with the Gameboy screen walls.
void player_collision_detection()
{
    // Collision with the leftmost wall.
    if (player_coords[0] <= X_MIN)
    {
        if(!player_has_collided_x_min)
        {
            play_bonk_sfx();
        }
        player_has_collided_x_min = 0x01;
    }
    else
    {
        player_has_collided_x_min = 0x00;
    }

    // Collision with the rightmost wall.
    if (player_coords[0] >= X_MAX)
    {
        if(!player_has_collided_x_max)
        {
            play_bonk_sfx();
        }
        player_has_collided_x_max = 0x01;
    }
    else
    {
        player_has_collided_x_max = 0x00;
    }

    // Collision with ceiling.
    if (player_coords[1] <= Y_MIN)
    {
        if(!player_has_collided_y_min)
        {
            play_bonk_sfx();
        }
        player_has_collided_y_min = 0x01;
    }
    else
    {
        player_has_collided_y_min = 0x00;
    }

    // Collision with floor.
    if (player_coords[1] >= Y_MAX)
    {
        if(!player_has_collided_y_max)
        {
            play_bonk_sfx();
        }
        player_has_collided_y_max = 0x01;
    }
    else
    {
        player_has_collided_y_max = 0x00;
    }
}

// Moves the player sprite depending on user input. Checks for collision with the Gameboy screen walls.
void player_movement()
{
    player_collision_detection();
    switch(d_pad_bit(joypad()))
    {
        case J_UP:
            set_sprite_tile(0, 1);
            if(!player_has_collided_y_min)
            {
                scroll_sprite(0, 0, -1);
                player_coords[1]--;
            }
            break;

        case J_DOWN:
            set_sprite_tile(0, 2);
            if(!player_has_collided_y_max)
            {
                scroll_sprite(0, 0, 1);
                player_coords[1]++;
            }
            break;

        case J_LEFT:
            set_sprite_tile(0, 3);
            if(!player_has_collided_x_min)
            {
                scroll_sprite(0, -1, 0);
                player_coords[0]--;
            }
            break;

        case J_RIGHT:
            set_sprite_tile(0, 4);
            if(!player_has_collided_x_max)
            {
                scroll_sprite(0, 1, 0);
                player_coords[0]++;
            }
            break;

        case (J_UP | J_RIGHT):      // Diagonal cases are bitwise OR'd as this is what the joypad function does.
            set_sprite_tile(0, 5);
            if(!player_has_collided_y_min)
            {
                scroll_sprite(0, 0, -1);
                player_coords[1]--;
            }
            if(!player_has_collided_x_max)
            {
                scroll_sprite(0, 1, 0);
                player_coords[0]++;
            }
            break;

        case (J_RIGHT | J_DOWN):
            set_sprite_tile(0, 6);
            if(!player_has_collided_x_max)
            {
                scroll_sprite(0, 1, 0);
                player_coords[0]++;
            }
            if(!player_has_collided_y_max)
            {
                scroll_sprite(0, 0, 1);
                player_coords[1]++;
            }
            break;

        case (J_DOWN | J_LEFT):
            set_sprite_tile(0, 7);
            if(!player_has_collided_y_max)
            {
                scroll_sprite(0, 0, 1);
                player_coords[1]++;
            }
            if(!player_has_collided_x_min)
            {
                scroll_sprite(0, -1, 0);
                player_coords[0]--;
            }
            break;

        case (J_LEFT | J_UP):
            set_sprite_tile(0, 8);
            if(!player_has_collided_x_min)
            {
                scroll_sprite(0, -1, 0);
                player_coords[0]--;
            }
            if(!player_has_collided_y_min)
            {
                scroll_sprite(0, 0, -1);
                player_coords[1]--;
            }
            break;

        default:
            set_sprite_tile(0, 0);  // Return to original forward-facing sprite.
            break;
    }
}

void main()
{
    load_font(font_min);
    init_sound();

    // Setup player sprite.
    set_sprite_data(0, 9, player);
    set_sprite_tile(0, 0);
    player_coords[0] = 0x18;
    player_coords[1] = 0x20;
    move_sprite(0, player_coords[0], player_coords[1]);
    SHOW_SPRITES;

    set_win_tiles(0, 0, 19
    , 1, window);
    move_win(0x08, 0x88);
    SHOW_WIN;
    DISPLAY_ON;

    while(1)
    {
        player_movement();
        wait_vbl_done();    // Seems to frame limit to 60 fps.
    }
}
