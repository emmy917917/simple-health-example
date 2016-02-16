#include "main_window.h"

#define MAX_METRICS HealthMetricActiveKCalories

static Window *s_window;
static TextLayer *s_value_layer, *s_label_layer;

static HealthMetric s_metric;

static void up_click_handler(ClickRecognizerRef recognizer, void *context) { 
  s_metric -= (s_metric > 0) ? 1 : -(MAX_METRICS);
  main_window_update_ui();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) { 
  s_metric += (s_metric < MAX_METRICS) ? 1 : -(MAX_METRICS);
  main_window_update_ui();
}
  
static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_value_layer = text_layer_create(grect_inset(bounds, 
                                                GEdgeInsets(50, 0, 0, 0)));
  text_layer_set_text_alignment(s_value_layer, GTextAlignmentCenter);
  text_layer_set_text_color(s_value_layer, GColorWhite);
  text_layer_set_background_color(s_value_layer, GColorClear);
  text_layer_set_font(s_value_layer, 
                      fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_value_layer));

  s_label_layer = text_layer_create(grect_inset(bounds, 
                                                GEdgeInsets(80, 0, 0, 0)));
  text_layer_set_text_alignment(s_label_layer, GTextAlignmentCenter);
  text_layer_set_text_color(s_label_layer, GColorWhite);
  text_layer_set_background_color(s_label_layer, GColorClear);
  text_layer_set_font(s_label_layer, 
                      fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_label_layer));

#if defined(PBL_ROUND)
  text_layer_enable_screen_text_flow_and_paging(s_value_layer, 5);
  text_layer_enable_screen_text_flow_and_paging(s_label_layer, 5);
#endif

}

static void window_unload(Window *window) {
  text_layer_destroy(s_value_layer);
  text_layer_destroy(s_label_layer);

  window_destroy(s_window);
  s_window = NULL;
}

void main_window_push() {
  if(!s_window) {
    s_window = window_create();
    window_set_click_config_provider(s_window, click_config_provider);
    window_set_window_handlers(s_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload,
    });
  }
  window_stack_push(s_window, true);

  main_window_update_ui();
}

void main_window_update_ui() {
  if(health_is_available() && s_window) {
    static char s_value_buffer[8];

    snprintf(s_value_buffer, sizeof(s_value_buffer), "%d", 
                                              health_get_metric_sum(s_metric));

    switch(s_metric) {
      case HealthMetricStepCount:
        text_layer_set_text(s_label_layer, "Steps taken today");
        window_set_background_color(s_window, GColorWindsorTan);
        break;
      case HealthMetricActiveSeconds:
        text_layer_set_text(s_label_layer, "Seconds active today");
        window_set_background_color(s_window, GColorDarkGreen);
        break;
      case HealthMetricWalkedDistanceMeters:
        text_layer_set_text(s_label_layer, "Meters travelled today");
        window_set_background_color(s_window, GColorJazzberryJam);
        break;
      case HealthMetricSleepSeconds:
        text_layer_set_text(s_label_layer, "Seconds asleep today");
        window_set_background_color(s_window, GColorBlueMoon);
        break;
      case HealthMetricSleepRestfulSeconds:
        text_layer_set_text(s_label_layer, "Restful sleep today");
        window_set_background_color(s_window, GColorDukeBlue);
        break;
      case HealthMetricRestingKCalories:
        text_layer_set_text(s_label_layer, "Resting kcal today");
        window_set_background_color(s_window, GColorCadetBlue);
        break;
      case HealthMetricActiveKCalories:
        text_layer_set_text(s_label_layer, "Active kcal today");
        window_set_background_color(s_window, GColorMidnightGreen);
        break;
      default: 
        text_layer_set_text(s_label_layer, "Unknown metric!");
        window_set_background_color(s_window, GColorDarkGray);
        break;
    }

    text_layer_set_text(s_value_layer, s_value_buffer);
  } else {
    window_set_background_color(s_window, GColorDarkCandyAppleRed);
    text_layer_set_text(s_value_layer, "Health not available!");
  }
}
