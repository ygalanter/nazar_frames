#pragma once
#include "pebble.h"

static const GPathInfo MINUTE_HAND_POINTS =
{
  8,
  (GPoint []) {
    { -2, 0 },  
    { 3, 0 },   
    { 3, -50 },    
    { 50, -50},    
    { 50, 50},    
    { -50, 50 },        
    { -50, -50},      
    { -2, -50 }      
  }
};

static const GPathInfo HOUR_HAND_POINTS = {
 8,
  (GPoint []) {
    { -4, -50 }, 
    { -4, 0 },    
    { 4, 0 },      
    { 4, -50 },      
    { 50, -50},      
    { 50, 50},      
    { -50, 50 },      
    { -50, -50 }      
  }
};


static const GPathInfo SECOND_HAND_POINTS = {
 7,
  (GPoint []) {
    { 0, -50 },
    { 0, 0 },    
    { 0, -50 },      
    { 50, -50},      
    { 50, 50},      
    { -50, 50 },      
    { -50, -50 }      
  }
};