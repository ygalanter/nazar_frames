#pragma once
#include "pebble.h"
  
void show_info(void);
void hide_info(void);
void init_info();
void deinit_info();
void set_info_time(struct tm *tick_time);