// Step 1.
// Declare custom effects using the RGB_MATRIX_EFFECT macro
// (note the lack of semicolon after the macro!)
RGB_MATRIX_EFFECT(diagonal_colorband)
RGB_MATRIX_EFFECT(diagonal_colorband_splash)

// Step 2.
// Define effects inside the `RGB_MATRIX_CUSTOM_EFFECT_IMPLS` ifdef block
#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

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

uint8_t map(uint8_t x, uint8_t in_min, uint8_t in_max, uint8_t out_min, uint8_t out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static bool diagonal_colorband_run(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    RGB colors[3] = {
        {0xa9, 0x03, 0xfc}, // purple #a903fc
        {0x00, 0x00, 0xFF}, // Blue
        {0x04, 0xff, 0x8c}, // cyan #04ff8c
    };

    uint8_t color_width = 7;
    uint8_t blend_leds  = 2;

    uint8_t        shift             = 0;
    uint8_t        h_speed_increment = 100 / (rgb_matrix_config.speed / 8);
    static uint8_t h_increment       = 0;
    uint8_t        effectSize        = color_width * 3;
    uint8_t        result[effectSize];
    static uint8_t counter = 0;

    generate_sets(counter, color_width, 3, effectSize, result);

    if (h_increment > h_speed_increment) {
        // Set colors from left to right
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            for (uint8_t col = 0; col < MATRIX_COLS; col++) {
                uint8_t i = g_led_config.matrix_co[row][(col + shift) % MATRIX_COLS];
                if (i != NO_LED) {
                    uint8_t color_index      = (result[col % effectSize] - 1);
                    uint8_t next_color_index = (result[(col + blend_leds) % effectSize] - 1);
                    if (color_index == next_color_index) {
                        rgb_matrix_set_color(i, colors[color_index].r, colors[color_index].g, colors[color_index].b);
                        continue;
                    }
                    uint8_t interpolation = 0.5;

                    if (blend_leds > 1) {
                        uint8_t distance = blend_leds;
                        for (uint8_t n = 1; n < blend_leds; n++) {
                            if (color_index != (result[(col + n) % effectSize] - 1)) {
                                distance = n;
                                break;
                            }
                        }
                        interpolation = distance / (blend_leds + 1);
                    }

                    RGB blended_color = {0, 0, 0};
                    RGB current_color = colors[color_index];
                    RGB next_color    = colors[next_color_index];
                    blended_color.r   = current_color.r + ((next_color.r - current_color.r) * interpolation);
                    blended_color.g   = current_color.g + ((next_color.g - current_color.g) * interpolation);
                    blended_color.b   = current_color.b + ((next_color.b - current_color.b) * interpolation);
                    rgb_matrix_set_color(i, blended_color.r, blended_color.g, blended_color.b);
                }
            }
            shift++;
        }
        h_increment = 0;
        counter     = (counter + 1) % effectSize;
    }
    if ((rgb_matrix_config.speed / 8) > 0.1) {
        h_increment++;
    }
    return rgb_matrix_check_finished_leds(led_max);
}

static bool diagonal_colorband_splash_run(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    const uint8_t color_count = 2;
    const uint8_t color_width = 7;
    const uint8_t blend_leds  = 3;
    float       interpolation = 0.5;

    HSV colors[2] = {
        {140, 255, 255},
        {180, 255, 255},
    };

    uint8_t        shift             = 0;
    uint8_t        h_speed_increment = 100 / (rgb_matrix_config.speed / 8);
    static uint8_t h_increment       = 0;
    uint8_t        effectSize        = color_width * color_count;
    uint8_t        result[effectSize];
    static uint8_t counter = 0;

    generate_sets(counter, color_width, color_count, effectSize, result);

    // Set colors from left to right
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint8_t i = g_led_config.matrix_co[row][(col + shift) % MATRIX_COLS];
            if (i != NO_LED) {
                uint8_t color_index      = result[col % effectSize] - 1;
                uint8_t next_color_index = result[(col + blend_leds) % effectSize] - 1;
                HSV current_color = colors[color_index];

                if (color_index == next_color_index) {
                    RGB rgb = rgb_matrix_hsv_to_rgb(current_color);
                    rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
                    continue;
                }

                HSV next_color    = colors[next_color_index];
                HSV blended_color = {255, 255, 255};

                blended_color.h   = (uint8_t)(current_color.h + (((float)next_color.h - current_color.h) * interpolation));
                blended_color.s   = (uint8_t)(current_color.s + (((float)next_color.s - current_color.s) * interpolation));
                
                RGB rgb = rgb_matrix_hsv_to_rgb(blended_color);
                rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
            }
        }
        // shift++;
    }
    if (h_increment > h_speed_increment) {
        h_increment = 0;
        counter     = (counter + 1) % effectSize;
    }
    if ((rgb_matrix_config.speed / 8) > 0.1) {
        h_increment++;
    }
    return rgb_matrix_check_finished_leds(led_max);
}

static bool diagonal_colorband(effect_params_t* params) {
    return diagonal_colorband_run(params);
}

static bool diagonal_colorband_splash(effect_params_t* params) {
    return diagonal_colorband_splash_run(params);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
