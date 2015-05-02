#include "info.h"
#include <pebble.h>


static Window *s_window;
static GFont s_res_gothic_18_bold;
static GFont s_res_roboto_bold_subset_49;
static TextLayer *s_textlayer_date;
static TextLayer *s_textlayer_time;
static TextLayer *s_textlayer_battery;
static TextLayer *s_textlayer_bluetooth;

static void initialise_ui(void) {
  s_window = window_create();
  
  #ifdef PBL_COLOR
    window_set_background_color(s_window, GColorOxfordBlue);
  #else
    window_set_background_color(s_window, GColorBlack);
  #endif  
  
  s_res_gothic_18_bold = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  s_res_roboto_bold_subset_49 = fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49);
  // s_textlayer_date
  s_textlayer_date = text_layer_create(GRect(0, 5, 144, 20));
  text_layer_set_background_color(s_textlayer_date, GColorClear);
  text_layer_set_text_color(s_textlayer_date, GColorWhite);
  text_layer_set_text(s_textlayer_date, "BUM BAM 01 9999");
  text_layer_set_text_alignment(s_textlayer_date, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_date, s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_date);
  
  // s_textlayer_time
  s_textlayer_time = text_layer_create(GRect(0, 52, 144, 58));
  text_layer_set_background_color(s_textlayer_time, GColorClear);
  text_layer_set_text_color(s_textlayer_time, GColorWhite);
  text_layer_set_text(s_textlayer_time, "00:00");
  text_layer_set_text_alignment(s_textlayer_time, GTextAlignmentCenter);
  text_layer_set_font(s_textlayer_time, s_res_roboto_bold_subset_49);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_time);
  
  // s_textlayer_battery
  s_textlayer_battery = text_layer_create(GRect(87, 142, 56, 20));
  text_layer_set_background_color(s_textlayer_battery, GColorClear);
  text_layer_set_text_color(s_textlayer_battery, GColorWhite);
  text_layer_set_text(s_textlayer_battery, "100%");
  text_layer_set_text_alignment(s_textlayer_battery, GTextAlignmentRight);
  text_layer_set_font(s_textlayer_battery, s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_battery);
  
  // s_textlayer_bluetooth
  s_textlayer_bluetooth = text_layer_create(GRect(4, 143, 54, 20));
  text_layer_set_background_color(s_textlayer_bluetooth, GColorClear);
  text_layer_set_text_color(s_textlayer_bluetooth, GColorWhite);
  text_layer_set_text(s_textlayer_bluetooth, "BT OFF");
  text_layer_set_font(s_textlayer_bluetooth, s_res_gothic_18_bold);
  layer_add_child(window_get_root_layer(s_window), (Layer *)s_textlayer_bluetooth);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  text_layer_destroy(s_textlayer_date);
  text_layer_destroy(s_textlayer_time);
  text_layer_destroy(s_textlayer_battery);
  text_layer_destroy(s_textlayer_bluetooth);
}



char time_buffer[]="00:00";
char date_buffer[]="BUM BAM 01 9999";
char battery_buffer[]="100%";

static void display_bt_layer(bool connected){
  if (connected) {
    text_layer_set_text(s_textlayer_bluetooth, "BT ON");
  } else {
    text_layer_set_text(s_textlayer_bluetooth, "BT OFF");
  };  
}


void init_info() {
  initialise_ui();
  bluetooth_connection_service_subscribe(display_bt_layer);
}

void deinit_info() {
  bluetooth_connection_service_unsubscribe();
  destroy_ui();
}


void show_info(void) {
 
  //initial bluetooth info
  display_bt_layer(bluetooth_connection_service_peek());
  
  //initial time info
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  set_info_time(t);
  
  window_stack_push(s_window, true);
}

void hide_info(void) {
  window_stack_remove(s_window, true);
}

void set_info_time(struct tm *tick_time) {
  
   if (!clock_is_24h_style()) {
    
        if( tick_time->tm_hour > 11 ) {   // YG Jun-25-2014: 0..11 - am 12..23 - pm
            if( tick_time->tm_hour > 12 ) tick_time->tm_hour -= 12;
        } else {
            if( tick_time->tm_hour == 0 ) tick_time->tm_hour = 12;
        }        
    }
 
    //time
    strftime(time_buffer, sizeof(time_buffer), "%H:%M", tick_time);
    text_layer_set_text(s_textlayer_time, time_buffer);
  
    //date
    strftime(date_buffer, sizeof(date_buffer), "%a %b %d %Y", tick_time);
    text_layer_set_text(s_textlayer_date, date_buffer);
  
    snprintf(battery_buffer, sizeof(battery_buffer), "%d%%", battery_state_service_peek().charge_percent);
    text_layer_set_text(s_textlayer_battery, battery_buffer);
}
