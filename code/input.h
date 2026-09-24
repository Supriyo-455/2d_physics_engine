/* date = September 24th 2026 10:12 am */

#ifndef INPUT_H
#define INPUT_H

// TODO: SDL Indepent user input and output
struct mouse
{
    int XPos;
    int YPos;
    
    real32 ScrollWheel;
    
    // NOTE: For now we support 3 mouse buttons
    bool32 ButtonState[3];
};

#endif //INPUT_H
