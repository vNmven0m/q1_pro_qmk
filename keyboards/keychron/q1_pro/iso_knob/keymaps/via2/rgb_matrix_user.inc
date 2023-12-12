// !!! DO NOT ADD #pragma once !!! //

// Step 1.
// Declare custom effects using the RGB_MATRIX_EFFECT macro
// (note the lack of semicolon after the macro!)
RGB_MATRIX_EFFECT(diagonal_colorband)

// Step 2.
// Define effects inside the `RGB_MATRIX_CUSTOM_EFFECT_IMPLS` ifdef block
#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static uint8_t color_index = 0;

void generate_sets(uint8_t counter, uint8_t x, uint8_t limit, uint8_t result_size, uint8_t result[]) {
    uint8_t arr[result_size];
    uint8_t current_index = 0;

    for (uint8_t i = 1; i <= limit; i++) {
        for (uint8_t j = 0; j < x; j++) {
            arr[current_index++] = i;
        }
    }

    for (uint8_t i = 0; i < result_size; i++) {
        result[i] = arr[(i + counter) % result_size];
    }
}

static void diagonal_colorband_init(effect_params_t* params) {
    color_index = 0;
}

static bool diagonal_colorband_run(effect_params_t* params) {

    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    // Set colors based on the current color index
    RGB colors[3] = {
        {0xa9, 0x03, 0xfc},  // purple #a903fc
        {0x00, 0x00, 0xFF},  // Blue
        {0x04, 0xff, 0x8c},  // cyan #04ff8c   
    };
    uint8_t RGB_MATRIX_WIDTH = 15;
    uint8_t RGB_MATRIX_HEIGHT = 7;
    uint8_t h_speed_increment = 100 / (rgb_matrix_config.speed / 8);;
    static uint8_t h_increment = 0;
    uint8_t x = 7;
    uint8_t resultSize = x*2;
    uint8_t blend_leds = 1;  // Number of LEDs to blend colors over
    uint8_t result[resultSize];
    static uint8_t counter = 0;

    generate_sets(counter, x, 2, resultSize, result);

    if (++h_increment > h_speed_increment) {
        // Smoothly transition colors from left to right over multiple LEDs
        for (uint8_t row = 0; row < RGB_MATRIX_HEIGHT; row++) {
            for (uint8_t col = 0; col < RGB_MATRIX_WIDTH; col++) {
                uint8_t i = RGB_MATRIX_WIDTH * row + col;
                uint8_t color_index = (result[col] - 1);  // Cycle through the three colors in a fixed pattern
                
                // Calculate smooth color transition over multiple LEDs
                RGB blended_color = {0, 0, 0};
                for (uint8_t j = 0; j < blend_leds; j++) {
                    uint8_t next_color_index = (color_index + 1) % 3;
                    RGB current_color = colors[color_index];
                    RGB next_color = colors[next_color_index];
                    uint8_t blend_factor = (col * 255) / (RGB_MATRIX_WIDTH - 1);  // Linear interpolation factor
                    blended_color.r += (current_color.r * (255 - blend_factor) + next_color.r * blend_factor) / 255;
                    blended_color.g += (current_color.g * (255 - blend_factor) + next_color.g * blend_factor) / 255;
                    blended_color.b += (current_color.b * (255 - blend_factor) + next_color.b * blend_factor) / 255;
                    color_index = next_color_index;
                }

                rgb_matrix_set_color(i, blended_color.r / blend_leds, blended_color.g / blend_leds, blended_color.b / blend_leds);
            }
        }
        h_increment = 0;
        counter = (counter + 1) % resultSize;
    }
    return rgb_matrix_check_finished_leds(led_max);
}
static bool diagonal_colorband(effect_params_t* params) {
    if (params->init) diagonal_colorband_init(params);
    return diagonal_colorband_run(params);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS