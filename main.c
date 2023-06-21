#include "header.h"

// Tilesets created by gbtd.
#include "player.c"
#include "demon.c"
#include "window.c"

int16_t player_coords[2];    // Format: x, y
uint8_t player_has_collided_x_min = 0;
uint8_t player_has_collided_x_max = 0;
uint8_t player_has_collided_y_min = 0;
uint8_t player_has_collided_y_max = 0;

// Structure to define the demon entity which will attack the player.
typedef struct _Demon
{
    int16_t pos_x;
    int16_t pos_y;
    uint8_t timer;
}   Demon;

// Pathfinding logic to allow the demon to hunt the player.
void demon_logic(Demon* demon)
{
    if(demon->timer >= 1)   // Demon pathfinds on every other frame.
    {
        // See where player is located:
        if(player_coords[0] > demon->pos_x)
        {
            demon->pos_x++;
            set_sprite_tile(1, 0x0D);
            scroll_sprite(1, 1, 0);
        }
        else if(player_coords[0] < demon->pos_x)
        {
            demon->pos_x--;
            set_sprite_tile(1, 0x0C);
            scroll_sprite(1, -1, 0);
        }
        else if(player_coords[1] > demon->pos_y)
        {   
            demon->pos_y++;
            set_sprite_tile(1, 0x0B);
            scroll_sprite(1, 0, 1);
        }
        else if(player_coords[1] < demon->pos_y)
        {
            scroll_sprite(1, 0, -1);
            set_sprite_tile(1, 0x0A);
            demon->pos_y--;
        }
        demon->timer = 0;
    }
    else
    {
        demon->timer++;
    }
}

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

// Displays the title screen. When user presses the START button the code continues to execute.
void title_screen()
{
    printf("DEMON CHASE\n\nWEAKBOX INSTUSTRIES 2023\n\n\n\n\nPRESS START\n\n\n\n");
    waitpad(J_START);
    printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
}

// Displays the death screen.
void death_screen()
{
    play_bonk_sfx();
    printf("YOU DIED\n\n\n");
}

// Checks for collisions between the player and the demon.
UBYTE check_collisions(Demon* demon)
{
    return(((player_coords[0] >= demon->pos_x && player_coords[0] <= demon->pos_x + 0x08) && (player_coords[1] >= demon->pos_y && player_coords[1] <= demon->pos_y + 0x08)) || ((demon->pos_x >= player_coords[0] && demon->pos_x <= player_coords[0] + 0x08) && (demon->pos_y >= player_coords[1] && demon->pos_y <= player_coords[1] + 0x08)));
}

void main()
{
    load_font(font_min);
    init_sound();

    UBYTE state = 0;
    Demon _demon;
    while(1)
    {
        switch(state)
        {
            case 0: // Title screen state.
                title_screen();

                // Setup player sprite.
                set_sprite_data(0, 9, player);
                set_sprite_tile(0, 0);
                player_coords[0] = 0x18;
                player_coords[1] = 0x20;
                move_sprite(0, player_coords[0], player_coords[1]);
                
                // Setup demon sprite.
                set_sprite_data(9, 9, demon);
                set_sprite_tile(1, 9);

                state++;
                break;

            case 1: // Game state.
                _demon.pos_x = 0x80; // Sets the demon's spawnpoint coordinates.
                _demon.pos_y = 0x80;
                _demon.timer = 0;
                move_sprite(1, _demon.pos_x, _demon.pos_y);   // Spawns the demon in at his spawn coordinates.

                SHOW_SPRITES;

                set_win_tiles(0, 0, 19, 1, window);
                move_win(0x08, 0x88);
                SHOW_WIN;
                DISPLAY_ON;

                init_coin(&coin, 100, 100);

                while(!check_collisions(&_demon))
                {
                    player_movement();
                    demon_logic(&_demon);
                    wait_vbl_done();    // Seems to frame limit to 60 fps.
                }

                state++;
                break;

            case 2: // Game over state.
                death_screen();
                delay(1000);
                state = 0;
                break;
        }
    }
}
