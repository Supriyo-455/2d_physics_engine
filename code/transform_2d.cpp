transform2D
CreateTransform2D(vec2 Pos, real32 Angle)
{
    transform2D Result = {};
    
    Result.Cos = Cos(Angle);
    Result.Sin = Sin(Angle);
    Result.PosX = Pos.x;
    Result.PosY = Pos.y;
    
    return Result;
}

transform2D
CreateTransform2D(real32 PosX, real32 PosY, real32 Angle)
{
    transform2D Result = {};
    
    Result.Cos = Cos(Angle);
    Result.Sin = Sin(Angle);
    Result.PosX = PosX;
    Result.PosY = PosY;
    
    return Result;
}

vec2
Transform(vec2 V, transform2D Transform)
{
    vec2 Result = {};
    
    Result.x = Transform.Cos * V.x - Transform.Sin * V.y + Transform.PosX;
    Result.y = Transform.Sin * V.x + Transform.Cos * V.y + Transform.PosY;
    
    return Result;
}

vec2
Transform(vec2 V, vec2 Pos, real32 Angle)
{
    transform2D T = CreateTransform2D(Pos, Angle);
    return Transform(V, T);
}
