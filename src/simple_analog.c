#include "simple_analog.h"
#include "info.h"  

#include "pebble.h"

#include "string.h"
#include "stdlib.h"

Layer * hands_layer;

static GPath *minute_arrow;
static GPath *hour_arrow;
Window *window;

static AppTimer *app_timer;
static bool bInfoDisplayed = false;

static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  graphics_context_set_stroke_color(ctx, GColorWhite);

  gpath_rotate_to(minute_arrow, TRIG_MAX_ANGLE * t->tm_min / 60);
  gpath_draw_outline(ctx, minute_arrow);

  gpath_rotate_to(hour_arrow, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));
  gpath_draw_outline(ctx, hour_arrow);
  
  // dot in the middle 
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, GPoint(bounds.size.w / 2, bounds.size.h / 2 - 1) , 4);
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(hands_layer);
}

// when info window is autoclosing: reset timer and close window
static void  auto_close_info_window(void *data){
    app_timer = NULL;
    hide_info();
    bInfoDisplayed = false;
}


// when watch is shaken or tapped
static void accel_tap_handler(AccelAxisType axis, int32_t direction)
{
  
  // if info already being displayed - don't do anything
  if (bInfoDisplayed) return;
  
  // stop existing timer if its running
	if (app_timer != NULL) {
		app_timer_cancel(app_timer);
	}
	
  // show info window and start timer to hide it
  bInfoDisplayed = true;
	show_info();
	app_timer = app_timer_register(3000, auto_close_info_window, NULL);
}



static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // init hands
  hands_layer = layer_create(bounds);
  layer_set_update_proc(hands_layer, hands_update_proc);
  layer_add_child(window_layer, hands_layer);
}

static void window_unload(Window *window) {
  layer_destroy(hands_layer);
}

static void init(void) {
  setlocale(LC_ALL, "");
  window = window_create();
  window_set_background_color(window, GColorBlack);
  
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });


  // init hand paths
  minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  hour_arrow = gpath_create(&HOUR_HAND_POINTS);
 
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  GPoint center = grect_center_point(&bounds);
  
  //correction
  center.y += 6;
    
  gpath_move_to(minute_arrow, center);
  gpath_move_to(hour_arrow, center);
 
  // Push the window onto the stack
  window_stack_push(window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
  accel_tap_service_subscribe(&accel_tap_handler);
}

static void deinit(void) {
  if (app_timer != NULL) {
		app_timer_cancel(app_timer);
	}  
  
  gpath_destroy(minute_arrow);
  gpath_destroy(hour_arrow);
  
  tick_timer_service_unsubscribe();
  accel_tap_service_unsubscribe();
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
