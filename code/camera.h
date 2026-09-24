/* date = September 24th 2026 10:09 am */

#ifndef CAMERA_H
#define CAMERA_H

struct simple_camera
{
    real32 Pan;
    real32 Zoom;
    
    real32 Width;
    real32 Height;
    
    vec2 Position;
	
	real32 MaxZoom;
	real32 MinZoom;
};

#endif //CAMERA_H
