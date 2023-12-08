#ifdef ENABLE_RGB_MATRIX_DUAL_BEACON
RGB_MATRIX_EFFECT(DUAL_BEACON)
#    ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

static HSV DUAL_BEACON_math(HSV hsv, int8_t sin, int8_t cos, uint8_t i, uint8_t time) {
    float beacon_effect = ((g_led_config.point[i].y - k_rgb_matrix_center.y) * cos + (g_led_config.point[i].x - k_rgb_matrix_center.x) * sin) / 128;

    hsv.h += beacon_effect;

    // Background color
    HSV background_color;
    background_color.h = 190;  // Hue for lightblue
    background_color.s = 255;  // Saturation
    background_color.v = 255;  // Brightness

    // Adjust the blending factor based on the beacon effect
    float blend_factor = 0.7;  // Adjust this value based on the desired blend

    // Blend the background color with the beacon effect
    hsv.h = (1 - blend_factor) * background_color.h + blend_factor * (hsv.h - beacon_effect);
    hsv.s = (1 - blend_factor) * background_color.s + blend_factor * hsv.s;
    hsv.v = (1 - blend_factor) * background_color.v + blend_factor * hsv.v;

    return hsv;
}

bool DUAL_BEACON(effect_params_t* params) {
    return effect_runner_sin_cos_i(params, &DUAL_BEACON_math);
}

#    endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif     // ENABLE_RGB_MATRIX_DUAL_BEACON
