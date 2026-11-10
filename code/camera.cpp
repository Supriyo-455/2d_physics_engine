
inline bool32 
IsVisible(real32 ObjectX, real32 ObjectY,
          real32 ObjectWidth, real32 ObjectHeight,
          simple_camera* Camera) {
    
	TIMED_FUNCTION;
	
    real32 ObjectLeft = ObjectX - (ObjectWidth / 2.0f); 
    real32 ObjectRight = ObjectLeft + ObjectWidth;
    real32 ObjectTop = ObjectY - (ObjectHeight / 2.0f);
    real32 ObjectBottom = ObjectTop + ObjectHeight;
    
    real32 ViewPortWidth = Camera->Width / Camera->MinZoom;
    real32 ViewPortHeight = Camera->Height / Camera->MinZoom;
    real32 ViewPortLeft = Camera->Position.x - (ViewPortWidth / 2.0f);
    real32 ViewPortRight = ViewPortLeft + ViewPortWidth;
    real32 ViewPortTop = Camera->Position.y - (ViewPortHeight / 2.0f);
    real32 ViewPortBottom = ViewPortTop + ViewPortHeight;
    
    return (ObjectRight > ViewPortLeft &&
            ObjectBottom > ViewPortTop &&
            ObjectLeft < ViewPortRight &&
            ObjectTop < ViewPortBottom);
}

inline vec2 
WorldToScreen(simple_camera* Cam, vec2 P) 
{
    TIMED_FUNCTION;
	
	real32 X = (P.x - Cam->Position.x) * Cam->Zoom + (Cam->Width * 0.5f);
    real32 Y = (P.y - Cam->Position.y) * Cam->Zoom + (Cam->Height * 0.5f);
    
    return vec(X, Y);
}

inline vec2
GetRelativeWorldPosition(simple_camera* Camera, real32 NewX, real32 NewY)
{
	TIMED_FUNCTION;
	
	real32 OffsetX = NewX - (Camera->Width / 2.0f);
	real32 OffsetY = NewY - (Camera->Height / 2.0f);
	
	real32 NewWorldX = Camera->Position.x + (OffsetX / Camera->Zoom);
    real32 NewWorldY = Camera->Position.y + (OffsetY / Camera->Zoom);
    
	return vec(NewWorldX, NewWorldY);
}

inline void 
PanCamera(simple_camera* Camera, real32 NewX, real32 NewY)
{
	TIMED_FUNCTION;
	
	real32 NewCameraPositionX = Camera->Position.x - (NewX / Camera->Zoom);
    real32 NewCameraPositionY = Camera->Position.y - (NewY / Camera->Zoom);
	
    Camera->Position = vec(NewCameraPositionX, NewCameraPositionY);
}

inline void 
ZoomCamera(simple_camera* Camera, real32 ZoomXPos, real32 ZoomYPos, int32 ZoomWeight)
{
    TIMED_FUNCTION;
	
	real32 OffsetX = ZoomXPos - (Camera->Width / 2.0f);
    real32 OffsetY = ZoomYPos - (Camera->Height / 2.0f);
    
    real32 WorldXBefore = Camera->Position.x + (OffsetX / Camera->Zoom);
    real32 WorldYBefore = Camera->Position.y + (OffsetY / Camera->Zoom);
    
	if(ZoomWeight > 0)
        Camera->Zoom *= 1.1f;
    else
        Camera->Zoom /= 1.1f;
    
    // NOTE: Clamp zoom level to the requested bounds
    if(Camera->Zoom < Camera->MinZoom) Camera->Zoom = Camera->MinZoom;
    if(Camera->Zoom > Camera->MaxZoom)  Camera->Zoom = Camera->MaxZoom;
    
    Camera->Position.x = WorldXBefore - (OffsetX / Camera->Zoom);
    Camera->Position.y = WorldYBefore - (OffsetY / Camera->Zoom);
}

// TODO: Pass only camera and bodies array? Or maybe entities 
void
DeleteOutofReachPhysicsBodies(game_state* GameState)
{
	TIMED_FUNCTION;
	
	// NOTE: Delete physics objects if they leave the zoom level 0.030 bounds
	real32 MaxViewWidth = GameState->Camera.Width / GameState->Camera.MinZoom;
	real32 MaxViewHeight = GameState->Camera.Height / GameState->Camera.MinZoom;
	real32 BoundLeft = GameState->Camera.Position.x - (MaxViewWidth / 2.0f);
	real32 BoundRight = GameState->Camera.Position.x + (MaxViewWidth / 2.0f);
	real32 BoundTop = GameState->Camera.Position.y - (MaxViewHeight / 2.0f);
	real32 BoundBottom = GameState->Camera.Position.y + (MaxViewHeight / 2.0f);
	
	for(uint32 i = 0; 
		i < GameState->World->BodyCount;)
	{
		if(!GameState->World->Bodies[i].IsStatic)
		{
			AABB aabb = GetAABBFromPhysicsBody(&GameState->World->Bodies[i]);
			if(aabb.Max.x < BoundLeft || aabb.Min.x > BoundRight || aabb.Max.y < BoundTop || aabb.Min.y > BoundBottom)
			{
				// TODO: Replace normal array with linkedlist for deletion
				for(uint32 j = i; j < GameState->World->BodyCount - 1; j++)
				{
					GameState->World->Bodies[j] = GameState->World->Bodies[j+1];
					GameState->BodyColors[j] = GameState->BodyColors[j+1];
				}
				
				GameState->World->BodyCount -= 1;
				GameState->BodyColorsCount -= 1;
				
				continue;
			}
		}
		i++;
	} 
}
