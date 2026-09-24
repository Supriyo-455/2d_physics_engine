/* date = September 24th 2026 10:16 am */

#ifndef RENDERER_H
#define RENDERER_H

// TODO: Need to implement sdl independent renderer,
// Which we can easily swap later. Like, For now we are using sdl renderer,
// in future we may want to use Opengl, Vulkan, DirectX...


#define SCREEN_WIDTH 1300
#define SCREEN_HEIGHT 800

#define PADDING_20 20

global vec4 WHITE = {
	.r=1.0f,
	.g=1.0f,
	.b=1.0f,
	.a=1.0f
};

global vec4 GRAY = {
	.r=0.5f,
	.g=0.5f,
	.b=0.5f,
	.a=1.0f
};

global vec4 BLACK = {
	.r=0.0f,
	.g=0.0f,
	.b=0.0f,
	.a=1.0f
};

global vec4 RED = {
	.r=1.0f,
	.g=0.0f,
	.b=0.0f,
	.a=1.0f
};

global vec4 YELLOW = {
	.r=1.0f,
	.g=1.0f,
	.b=0.0f,
	.a=1.0f
};

struct texture
{
    // NOTE: Actual hardware texture
    SDL_Texture* SdlTexture;
    
    uint32 Width;
    uint32 Height;
};

#endif //RENDERER_H
