// keys coming from JS
#define KEY_DIGITAL_WATCH 1
#define KEY_SHOW_SECOND_HAND 2

// store whether digital screen currentlt displayed  
#define KEY_IS_DIGITAL_WATCH_DISPLAYED 0  
  
// Types of digital screen display/persitance
#define KEY_DIGITAL_WATCH_NORMAL 0
#define KEY_DIGITAL_WATCH_STICKY 1
#define KEY_DIGITAL_WATCH_DISABLED 2  
  
#include "simple_analog.h"
#include "info.h"  

#include "pebble.h"

#include "string.h"
#include "stdlib.h"
#include "antialiasing.h"  

Layer * hands_layer;

static GPath *minute_arrow;
static GPath *hour_arrow;
static GPath *second_arrow;
Window *window;

static AppTimer *app_timer;


static bool bInfoDisplayed = false;
static int8_t iDigitalWatch = KEY_DIGITAL_WATCH_NORMAL;  
static bool bShowSecondHand = false; 


static void hands_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  gpath_rotate_to(minute_arrow, TRIG_MAX_ANGLE * t->tm_min / 60);
  gpath_rotate_to(hour_arrow, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6));

  #ifdef PBL_COLOR
    gpath_draw_outline_antialiased(ctx, minute_arrow, GColorYellow);
    //gpath_draw_filled_antialiased(ctx, minute_arrow, GColorRed);
    gpath_draw_outline_antialiased(ctx, hour_arrow, GColorYellow);
  #else
    graphics_context_set_stroke_color(ctx, GColorWhite);
    gpath_draw_outline(ctx, minute_arrow);
    gpath_draw_outline(ctx, hour_arrow);
  #endif
    
  graphics_context_set_compositing_mode(ctx, GCompOpSet);

  if (bShowSecondHand) {
       gpath_rotate_to(second_arrow, TRIG_MAX_ANGLE * t->tm_sec / 60);
    
       #ifdef PBL_COLOR
         gpath_draw_outline_antialiased(ctx, second_arrow, GColorYellow);
       #else
         gpath_draw_outline(ctx, second_arrow);  
       #endif
       
  }
  
  // dot in the middle 
  #ifdef PBL_COLOR
    graphics_context_set_fill_color(ctx, GColorBulgarianRose);
  #else
    graphics_context_set_fill_color(ctx, GColorWhite);
  #endif   
  graphics_fill_circle(ctx, GPoint(bounds.size.w / 2, bounds.size.h / 2 - 1) , 4);
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(hands_layer);
  
  if (units_changed & MINUTE_UNIT) {
    set_info_time(tick_time);
  }  
}



static void in_recv_handler(DictionaryIterator *iterator, void *context) {
  Tuple *t = dict_read_first(iterator);

  while (t)  {
    
    switch(t->key)    {

      case KEY_SHOW_SECOND_HAND:
        tick_timer_service_unsubscribe();
      
        if (t->value->int8 == 1) {
           tick_timer_service_subscribe(SECOND_UNIT, handle_minute_tick);  
           persist_write_bool(KEY_SHOW_SECOND_HAND, true);
           bShowSecondHand = true;
        } else {
           tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);  
           persist_write_bool(KEY_SHOW_SECOND_HAND, false);
           bShowSecondHand = false;
        }
        break;
      
      case KEY_DIGITAL_WATCH:
      
         iDigitalWatch = t->value->int8;
         persist_write_int(KEY_DIGITAL_WATCH, iDigitalWatch);
        
         if ((iDigitalWatch == KEY_DIGITAL_WATCH_NORMAL || iDigitalWatch == KEY_DIGITAL_WATCH_DISABLED) && bInfoDisplayed) {
            hide_info();
            bInfoDisplayed = false;
         }
      
        break;
   
    }    
    
    t = dict_read_next(iterator);
  
  }
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
  
  if (iDigitalWatch == KEY_DIGITAL_WATCH_DISABLED) return; // if digital watch disabled - dont do anything on tap
  
  if (iDigitalWatch == KEY_DIGITAL_WATCH_STICKY) { // if we need to stick digital time until another shake
    
    if (bInfoDisplayed) {
      hide_info();
      bInfoDisplayed = false;
    } else {
      show_info();
      bInfoDisplayed = true;
    }
    
  } else { // if digital time need to autoclose
  
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
  
  #ifdef PBL_COLOR
    window_set_background_color(window, GColorOxfordBlue);
  #else
    window_set_background_color(window, GColorBlack);
  #endif  
  
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });


  // init hand paths
  minute_arrow = gpath_create(&MINUTE_HAND_POINTS);
  hour_arrow = gpath_create(&HOUR_HAND_POINTS);
  second_arrow = gpath_create(&SECOND_HAND_POINTS);
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  GPoint center = grect_center_point(&bounds);
  
  //correction
  center.y += 6;
    
  gpath_move_to(minute_arrow, center);
  gpath_move_to(hour_arrow, center);
  gpath_move_to(second_arrow, center);
  
  app_message_register_inbox_received((AppMessageInboxReceived) in_recv_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
 
  bShowSecondHand = persist_read_bool(KEY_SHOW_SECOND_HAND);
  if (bShowSecondHand) {
    tick_timer_service_subscribe(SECOND_UNIT, handle_minute_tick);
  } else {
    tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
  }
  
  accel_tap_service_subscribe(&accel_tap_handler);
  
  // Push the window onto the stack
  window_stack_push(window, true);
  
  // restore state we were in before watch unloaded (only in "sticky digital" mode)
  iDigitalWatch = persist_read_bool(KEY_DIGITAL_WATCH);
  bInfoDisplayed = persist_read_bool(KEY_IS_DIGITAL_WATCH_DISPLAYED);
  if (iDigitalWatch == KEY_DIGITAL_WATCH_STICKY && bInfoDisplayed) {
     show_info();
  }
   
}

static void deinit(void) {
  
  app_message_deregister_callbacks();
  
  // remember what state we were in when watch unloaded (only in "sticky digital" mode)
  if (iDigitalWatch == KEY_DIGITAL_WATCH_STICKY) {
    persist_write_bool(KEY_IS_DIGITAL_WATCH_DISPLAYED, bInfoDisplayed);
  }
  
  if (app_timer != NULL) {
		app_timer_cancel(app_timer);
	}  
  
  gpath_destroy(minute_arrow);
  gpath_destroy(hour_arrow);
  gpath_destroy(second_arrow);
  
  tick_timer_service_unsubscribe();
  accel_tap_service_unsubscribe();
  window_destroy(window);
}

int main(void) {
  init_info();
  init();
  app_event_loop();
  deinit();
  deinit_info();
}
