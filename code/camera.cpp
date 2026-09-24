
bool32 
IsVisible(real32 ObjectX, real32 ObjectY,
          real32 ObjectWidth, real32 ObjectHeight,
          simple_camera* Camera) {
    
    real32 ObjectLeft = ObjectX - (ObjectWidth / 2.0f); 
    real32 ObjectRight = ObjectLeft + ObjectWidth;
    real32 ObjectTop = ObjectY - (ObjectHeight / 2.0f);
    real32 ObjectBottom = ObjectTop + ObjectHeight;
    
    real32 ViewPortWidth = Camera->Width / Camera->Zoom;
    real32 ViewPortHeight = Camera->Height / Camera->Zoom;
    real32 ViewPortLeft = Camera->Position.x - (ViewPortWidth / 2.0f);
    real32 ViewPortRight = ViewPortLeft + ViewPortWidth;
    real32 ViewPortTop = Camera->Position.y - (ViewPortHeight / 2.0f);
    real32 ViewPortBottom = ViewPortTop + ViewPortHeight;
    
    return (ObjectRight > ViewPortLeft &&
            ObjectBottom > ViewPortTop &&
            ObjectLeft < ViewPortRight &&
            ObjectTop < ViewPortBottom);
}

vec2
GetRelativeWorldPosition(simple_camera* Camera, real32 NewX, real32 NewY)
{
	real32 OffsetX = NewX - (Camera->Width / 2.0f);
	real32 OffsetY = NewY - (Camera->Height / 2.0f);
	
	real32 NewWorldX = Camera->Position.x + (OffsetX / Camera->Zoom);
    real32 NewWorldY = Camera->Position.y + (OffsetY / Camera->Zoom);
    
	return vec(NewWorldX, NewWorldY);
}

void 
PanCamera(simple_camera* Camera, real32 NewX, real32 NewY)
{
	real32 NewCameraPositionX = Camera->Position.x - (NewX / Camera->Zoom);
    real32 NewCameraPositionY = Camera->Position.y - (NewY / Camera->Zoom);
	
    Camera->Position = vec(NewCameraPositionX, NewCameraPositionY);
}

void 
ZoomCamera(simple_camera* Camera, real32 ZoomXPos, real32 ZoomYPos, int32 ZoomWeight)
{
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
DeleteOutofReachPhysicsBodies(game* Game)
{
	// NOTE: Delete physics objects if they leave the zoom level 0.030 bounds
	real32 MaxViewWidth = Game->Camera.Width / Game->Camera.MinZoom;
	real32 MaxViewHeight = Game->Camera.Height / Game->Camera.MinZoom;
	real32 BoundLeft = Game->Camera.Position.x - (MaxViewWidth / 2.0f);
	real32 BoundRight = Game->Camera.Position.x + (MaxViewWidth / 2.0f);
	real32 BoundTop = Game->Camera.Position.y - (MaxViewHeight / 2.0f);
	real32 BoundBottom = Game->Camera.Position.y + (MaxViewHeight / 2.0f);
	
	for(int i = 0; 
		i < Game->World->BodyCount;)
	{
		if(!Game->World->Bodies[i].IsStatic)
		{
			AABB aabb = GetAABBFromPhysicsBody(&Game->World->Bodies[i]);
			if(aabb.Max.x < BoundLeft || aabb.Min.x > BoundRight || aabb.Max.y < BoundTop || aabb.Min.y > BoundBottom)
			{
				// TODO: Replace normal array with linkedlist for deletion
				for(uint32 j = i; j < Game->World->BodyCount - 1; j++)
				{
					Game->World->Bodies[j] = Game->World->Bodies[j+1];
					Game->BodyColors[j] = Game->BodyColors[j+1];
				}
				
				Game->World->BodyCount -= 1;
				Game->BodyColorsCount -= 1;
				
				continue;
			}
		}
		i++;
	} 
}
