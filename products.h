#pragma once

typedef enum prod_id_enum
{
    STOCK_PRODUCT_BAD_DOG,
    STOCK_PRODUCT_COCA_COLA,
    STOCK_PRODUCT_PEPSI,
    STOCK_PRODUCT_SPRITE,
    STOCK_PRODUCT_FANTA_ORANGE,
    STOCK_PRODUCT_MOUNTAIN_DEW,
    STOCK_PRODUCT_DR_PEPPER,
    STOCK_PRODUCT_RED_BULL,
    STOCK_PRODUCT_MONSTER_ENERGY,
    STOCK_PRODUCT_WATER_BOTTLE,
    STOCK_PRODUCT_ICED_TEA,
    STOCK_PRODUCT_ORANGE_JUICE,
    STOCK_PRODUCT_CHOCOLATE_BAR,
    STOCK_PRODUCT_POTATO_CHIPS,
    STOCK_PRODUCT_GUMMY_BEARS,
    STOCK_PRODUCT_COOKIES,
    STOCK_PRODUCT_PROTEIN_BAR,
    STOCK_PRODUCT_TRAIL_MIX,
    STOCK_PRODUCT_CHEWING_GUM,
    STOCK_PRODUCT_MINTS,
    __stock_product_count
} prod_id;

static product_t g_StockProducts[] = {
    {.prod_id = STOCK_PRODUCT_BAD_DOG, .name = "Bad Dog", .display_name = "bad_dog", .unit_cost = 85.0f, .profit_margin = .50f},
    {.prod_id = STOCK_PRODUCT_COCA_COLA, .name = "Coca Cola", .display_name = "coca_cola", .unit_cost = 65.0f, .profit_margin = .50f},
    {.prod_id = STOCK_PRODUCT_PEPSI, .name = "Pepsi", .display_name = "pepsi", .unit_cost = 63.0f, .profit_margin = .50f},
    {.prod_id = STOCK_PRODUCT_SPRITE, .name = "Sprite", .display_name = "sprite", .unit_cost = 60.0f, .profit_margin = .50f},
    {.prod_id = STOCK_PRODUCT_FANTA_ORANGE, .name = "Fanta Orange", .display_name = "fanta_orange", .unit_cost = 62.0f, .profit_margin = .50f},
    {.prod_id = STOCK_PRODUCT_MOUNTAIN_DEW, .name = "Mountain Dew", .display_name = "mountain_dew", .unit_cost = 68.0f, .profit_margin = .50f},
    {.prod_id = STOCK_PRODUCT_DR_PEPPER, .name = "Dr Pepper", .display_name = "dr_pepper", .unit_cost = 72.0f, .profit_margin = .50f},
    {.prod_id = STOCK_PRODUCT_RED_BULL, .name = "Red Bull", .display_name = "red_bull", .unit_cost = 45.0f, .profit_margin = .50f},
    {.prod_id = STOCK_PRODUCT_MONSTER_ENERGY, .name = "Monster Energy", .display_name = "monster_energy", .unit_cost = 35.0f, .profit_margin = .50f},
    {.prod_id = STOCK_PRODUCT_WATER_BOTTLE, .name = "Water Bottle", .display_name = "water_bottle", .unit_cost = 30.0f, .profit_margin = .50f},
    {.prod_id = STOCK_PRODUCT_ICED_TEA, .name = "Iced Tea", .display_name = "iced_tea", .unit_cost = 55.0f, .profit_margin = .50f},
    {.prod_id = STOCK_PRODUCT_ORANGE_JUICE, .name = "Orange Juice", .display_name = "orange_juice", .unit_cost = 80.0f, .profit_margin = .50f},
    {.prod_id = STOCK_PRODUCT_CHOCOLATE_BAR, .name = "Chocolate Bar", .display_name = "chocolate_bar", .unit_cost = 50.0f, .profit_margin = .50f},
    {.prod_id = STOCK_PRODUCT_POTATO_CHIPS, .name = "Potato Chips", .display_name = "potato_chips", .unit_cost = 48.0f, .profit_margin = .50f},
    {.prod_id = STOCK_PRODUCT_GUMMY_BEARS, .name = "Gummy Bears", .display_name = "gummy_bears", .unit_cost = 40.0f, .profit_margin = .50f},
    {.prod_id = STOCK_PRODUCT_COOKIES, .name = "Cookies", .display_name = "cookies", .unit_cost = 52.0f, .profit_margin = .50f},
    {.prod_id = STOCK_PRODUCT_PROTEIN_BAR, .name = "Protein Bar", .display_name = "protein_bar", .unit_cost = 10.0f, .profit_margin = .50f},
    {.prod_id = STOCK_PRODUCT_TRAIL_MIX, .name = "Trail Mix", .display_name = "trail_mix", .unit_cost = 95.0f, .profit_margin = .50f},
    {.prod_id = STOCK_PRODUCT_CHEWING_GUM, .name = "Chewing Gum", .display_name = "chewing_gum", .unit_cost = 18.0f, .profit_margin = .50f},
    {.prod_id = STOCK_PRODUCT_MINTS, .name = "Mints", .display_name = "mints", .unit_cost = 15.0f, .profit_margin = .50f}
};

// static_assert(sizeof(g_StockProducts) / sizeof(g_StockProducts[0]) == __stock_product_count);

#include "./slots.h"