/* date = September 24th 2026 10:16 am */

#ifndef RENDERER_H
#define RENDERER_H

// TODO: Need to implement sdl independent renderer,
// Which we can easily swap later. Like, For now we are using sdl renderer,
// in future we may want to use Opengl, Vulkan, DirectX...

struct texture
{
    // NOTE: Actual hardware texture
    SDL_Texture* SdlTexture;
    
    uint32 Width;
    uint32 Height;
};

#endif //RENDERER_H
