#pragma once
#include "pebble.h"
  
#ifdef PBL_PLATFORM_APLITE  

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

#elif PBL_PLATFORM_BASALT
  
 static const GPathInfo MINUTE_HAND_POINTS =  
  {
  8,
  (GPoint []) {
    { -3, 0 },  
    { 3, 0 },   
    { 3, -50 },    
    { 50, -50},    
    { 50, 50},    
    { -50, 50 },        
    { -50, -50},      
    { -3, -50 }      
  }
};

static const GPathInfo HOUR_HAND_POINTS = {
 8,
  (GPoint []) {
    { -5, -50 }, 
    { -5, 0 },    
    { 5, 0 },      
    { 5, -50 },      
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

#else 
  

static const GPathInfo MINUTE_HAND_POINTS =
{
  8,
  (GPoint []) {
    { -3, 0 },  
    { 3, 0 },   
    { 3, -60 },    
    { 60, -60},    
    { 60, 60},    
    { -60, 60 },        
    { -60, -60},      
    { -3, -60 }      
  }
};

static const GPathInfo HOUR_HAND_POINTS = {
 8,
  (GPoint []) {
    { -5, -60 }, 
    { -5, 0 },    
    { 5, 0 },      
    { 5, -60 },      
    { 60, -60},      
    { 60, 60},      
    { -60, 60 },      
    { -60, -60 }      
  }
};


static const GPathInfo SECOND_HAND_POINTS = {
 7,
  (GPoint []) {
    { 0, -60 },
    { 0, 0 },    
    { 0, -60 },      
    { 60, -60},      
    { 60, 60},      
    { -60, 60 },      
    { -60, -60 }      
  }
};

#endif