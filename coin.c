// Code that defines the coin that the player will collect.

#include "header.h"

unsigned char CoinSprite[] =
{
  0x3C,0x3C,0x7A,0x42,0xB1,0xC1,0xBB,0xC1,
  0xBF,0xC1,0x9F,0xE1,0x42,0x7E,0x3C,0x3C,
  0x18,0x18,0x3C,0x24,0x5A,0x62,0x5E,0x62,
  0x5E,0x62,0x4E,0x72,0x24,0x3C,0x18,0x18,
  0x18,0x18,0x2C,0x34,0x2C,0x34,0x2C,0x34,
  0x2C,0x34,0x2C,0x34,0x2C,0x34,0x18,0x18
};

const unsigned char hud[] = "Coins:%d";

extern int16_t player_coords[2];

Entity coin;
uint8_t frame;
uint8_t i = 0;
uint8_t coins_collected = 0;

// Initializes a coin at position (x, y).
void init_coin(Entity *coin, uint8_t x, uint8_t y)
{
    // Set the size of the coin sprite:
    coin->height = 8;
    coin->width = 8;

    // Set the coins initial position:
    coin->x = x;
    coin->y = y;

    set_sprite_data(0x12, 3, CoinSprite);   // Puts the coin's sprite in VRAM in memory location 0x12.
    set_sprite_tile(3, 0x12);               // Sets sprite 3 to the coin's sprite.
    move_sprite(3, coin->x, coin->y);       // Moves sprite 3 into the position set by the coin's initial position.
}

// Returns a random variable generated between the desired range (ChatGPT). Modulo operator is not desired for code speed.
uint8_t get_random_number_in_range(int min, int max)
{
    uint8_t range = max - min + 1;
    return ((rand() % range) + min);
}

// Moves the coin to a random location on the map.
void move_coin(Entity *coin)
{
    coin->x = get_random_number_in_range(X_MIN, X_MAX);
    coin->y = get_random_number_in_range(Y_MIN, Y_MAX);
    move_sprite(3, coin->x, coin->y);
}

// Animates the coin each frame!
void animate_coin()
{
    switch(frame)
    {
        case 0:
            set_sprite_tile(3, 0x12);
            frame++;
            break;
        case 1:
            set_sprite_tile(3, 0x13);
            frame++;
            break;
        case 2:
            set_sprite_tile(3, 0x14);
            frame++;
            break;
        case 3:
            set_sprite_tile(3, 0x13);
            frame = 0;
            break;
    }
}

// Checks for collisions between the player and an entity.
UBYTE collision_coin(Entity *entity)
{
    return(((player_coords[0] >= entity->x && player_coords[0] <= entity->x + 0x08) && (player_coords[1] >= entity->y && player_coords[1] <= entity->y + 0x08)) || ((entity->x >= player_coords[0] && entity->x <= player_coords[0] + 0x08) && (entity->y >= player_coords[1] && entity->y <= player_coords[1] + 0x08)));
}

// Checks for collisions between two entities.
UBYTE collision(Entity *ent1, Entity *ent2)
{
    return(((ent1->x >= ent2->x && ent1->x <= ent2->x + ent2->width) && (ent1->y >= ent2->y && ent1->y <= ent2->y + ent2->height)) || ((ent2->x >= ent1->x && ent2->x <= ent1->x + ent1->width) && (ent2->y >= ent1->y && ent2->y <= ent1->y + ent1->height)));
}

// Draws the HUD showing the number of coins:
void draw_hud()
{
    gotoxy(0, 0);
    printf(hud, coins_collected);
}

// Manage the coin animation and player interation.
void coin_logic()
{
    // Animate the coin using a global (can this be done better?).
    if(i > 10)
    {
        animate_coin();
        i = 0;
    }   
    i++;

    // Check if the player has interacted with the coin.
    if(collision_coin(&coin))
    {
        move_coin(&coin);
        play_coin_sfx();
        coins_collected++;
        draw_hud();
    }
}