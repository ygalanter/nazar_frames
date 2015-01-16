#pragma once
#include "pebble.h"

static const GPathInfo MINUTE_HAND_POINTS =
{
  8,
  (GPoint []) {
    { -50, -50},
    { -50, 50 },
    { 50, 50},
    { 50, -50},
    { 3, -50 },
    { 3, 0 },
    { -2, 0 },
    { -2, -50 } 
  }
};

static const GPathInfo HOUR_HAND_POINTS = {
 8,
  (GPoint []) {
    { -50, -50 },
    { -50, 50 },
    { 50, 50},
    { 50, -50},
    { 4, -50 },
    { 4, 0 },
    { -4, 0 },
    { -4, -50 } 
  }
};


static const GPathInfo SECOND_HAND_POINTS = {
 7,
  (GPoint []) {
    { -50, -50 },
    { -50, 50 },
    { 50, 50},
    { 50, -50},
    { 0, -50 },
    { 0, 0 },
    { 0, -50 } 
  }
};