#include <pebble.h>
#include "antialiasing.h"

/**
 * Implementation of the Xiaolin Wu's line algorithm for Pebble
 * http://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm
 */
  
#define interpol_color_(c1, c2) c1 = (int16_t)(c1 * 0x55 + ((int16_t)(c2) * 0x55 - (int16_t)(c1) * 0x55) * br) >> 6;

#ifdef PBL_COLOR 

inline void _plot(uint8_t* pixels, int16_t w, int16_t h, int16_t x, int16_t y, GColor8 color, float br)
{
	if(x<0 || x>(w-1) || y<0 || y>(h-1))
		return;

	GColor8* oc = (GColor8*)(pixels + x + w * y);
	if( br >= 1 ) {
		memcpy(oc, &color, sizeof(GColor8));
	}
	else {
		interpol_color_(oc->r, color.r)
		interpol_color_(oc->g, color.g)
		interpol_color_(oc->b, color.b)
	}
}

#define ipart_(X) ((int)(X))
#define fpart_(X) (((float)(X))-(float)ipart_(X))
#define rfpart_(X) (1.0-fpart_(X))
#define swap_(a, b) a ^= b; b ^= a; a ^= b;
#define abs_(a) (a) > 0 ? (a) : -(a);

void draw_line_antialias_(GBitmap* img, int16_t x1, int16_t y1, int16_t x2, int16_t y2, GColor8 color)
{
	uint8_t* img_pixels = gbitmap_get_data(img);
	int16_t  w 	= gbitmap_get_bounds(img).size.w;
	int16_t  h 	= gbitmap_get_bounds(img).size.h;

	float dx = abs_(x1 - x2);
	float dy = abs_(y1 - y2);
	
	bool steep = dy > dx;

	// _plot(img_pixels, x1, y1, w, h, color, 0.5);
	// _plot(img_pixels, x2, y2, w, h, color, 0.5);
	_plot(img_pixels, w, h, x1, y1, color, 1);
	_plot(img_pixels, w, h, x2, y2, color, 1);

	if(steep){
		swap_(x1, y1);
		swap_(x2, y2);
	}
	if(x1 > x2){
		swap_(x1, x2);
		swap_(y1, y2);
	}

	dx = x2 - x1;
	dy = y2 - y1;

	float gradient 	= dy / dx;
	float intery 	= y1;
 		
	int x=0;
	for(x=x1+1; x < x2; x++) {
		intery += gradient;
		if(x>=0){
			if(steep){
				_plot(img_pixels, w, h, ipart_(intery)    , x, color, rfpart_(intery));
				_plot(img_pixels, w, h, ipart_(intery) + 1, x, color,  fpart_(intery));
			}
			else {
				_plot(img_pixels, w, h, x, ipart_(intery)	 , color, rfpart_(intery));
				_plot(img_pixels, w, h, x, ipart_(intery) + 1, color,  fpart_(intery));
			}
		}
	}
}

static GColor8 graphics_context_get_stroke_color(GContext* ctx){
	return ((GColor8*)ctx)[46];
}

void graphics_draw_line_antialiased(GContext* ctx, GPoint p0, GPoint p1){
	if(p0.x == p1.x || p0.y == p1.y){
		graphics_draw_line(ctx, p0, p1);
	}
	else {
		GBitmap* bitmap = graphics_capture_frame_buffer(ctx);
		GColor8 stroke_color = graphics_context_get_stroke_color(ctx);
		draw_line_antialias_(bitmap, p0.x, p0.y, p1.x, p1.y, stroke_color);
		graphics_release_frame_buffer(ctx, bitmap);
	}
}

void gpath_draw_outline_antialiased(GContext* ctx, GPath *path){
	if(path->num_points == 0)
		return;	

	GPoint offset = path->offset;
	int32_t rotation = path->rotation;
	GBitmap* bitmap = graphics_capture_frame_buffer(ctx);
	GColor8 stroke_color = graphics_context_get_stroke_color(ctx);

  	int32_t s = sin_lookup(rotation);
  	int32_t c = cos_lookup(rotation);

	GPoint p = path->points[path->num_points-1];
	GPoint p1;
  	p1.x = (p.x * c - p.y * s) / TRIG_MAX_RATIO + offset.x;
  	p1.y = (p.x * s + p.y * c) / TRIG_MAX_RATIO + offset.y;

	for(uint32_t i=0; i<path->num_points; i++){
		GPoint p2;
		p2.x = (path->points[i].x * c - path->points[i].y * s) / TRIG_MAX_RATIO  + offset.x;
		p2.y = (path->points[i].x * s + path->points[i].y * c) / TRIG_MAX_RATIO  + offset.y;
		draw_line_antialias_(bitmap, p1.x, p1.y, p2.x, p2.y, stroke_color);
		p1 = p2;
	}

	graphics_release_frame_buffer(ctx, bitmap);
}

#endif

#undef swap_
#undef ipart_
#undef fpart_
#undef rfpart_
#undef abs_
#undef interpol_color_