
int test_scalars() {
    ASSERT_NEAR(absf(-10.5f), 10.5f, "absf negative");
    ASSERT_NEAR(absf(10.5f), 10.5f, "absf positive");
    
    ASSERT_CONDITION(RoundReal32ToUint32(0.4f) == 0, "Round 0.4 down");
    ASSERT_CONDITION(RoundReal32ToUint32(0.5f) == 1, "Round 0.5 up");
    ASSERT_CONDITION(RoundReal32ToUint32(0.6f) == 1, "Round 0.6 up");
    
    // Clamp Int
    ASSERT_CONDITION(Clamp(5, 0, 10) == 5, "Int Clamp mid");
    ASSERT_CONDITION(Clamp(-1, 0, 10) == 0, "Int Clamp min");
    ASSERT_CONDITION(Clamp(11, 0, 10) == 10, "Int Clamp max");
    ASSERT_CONDITION(Clamp(5, 10, 10) == 10, "Int Clamp equal bounds");
    
    // Clamp Float
    ASSERT_NEAR(Clamp(5.5f, 0.0f, 1.0f), 1.0f, "Float Clamp max");
    ASSERT_NEAR(LinearTosRGB(0.002f), 0.002f * 12.92f, "LinearToSRGB low branch");
    ASSERT_NEAR(LinearTosRGB(1.0f), 1.0f, "LinearToSRGB high branch");
    
    return 0;
}

int test_trigonometry() {
    // Basic Values
    ASSERT_NEAR(Sin(0.0f), 0.0f, "Sin 0");
    ASSERT_NEAR(Sin(90.0f), 1.0f, "Sin 90");
    ASSERT_NEAR(Cos(0.0f), 1.0f, "Cos 0");
    ASSERT_NEAR(Cos(90.0f), 0.0f, "Cos 90");
    
    // Wrap-around logic (> 360)
    ASSERT_NEAR(Sin(450.0f), 1.0f, "Sin 450 (90 wrap)");
    ASSERT_NEAR(Cos(720.0f), 1.0f, "Cos 720 (0 wrap)");
    
    // Negative angles
    ASSERT_NEAR(Sin(-90.0f), -1.0f, "Sin -90");
    ASSERT_NEAR(Cos(-180.0f), -1.0f, "Cos -180");
    
    // Identity: sin^2 + cos^2 = 1
    real32 angle = 37.5f;
    real32 identity = (Sin(angle) * Sin(angle)) + (Cos(angle) * Cos(angle));
    ASSERT_NEAR(identity, 1.0f, "Trig Identity sin2+cos2");
    
    return 0;
}

int test_vectors() {
    // vec2
    vec2 a2 = vec(3.0f, 4.0f);
    vec2 b2 = vec(1.0f, 0.0f);
    ASSERT_NEAR(Magnitude(a2), 5.0f, "Vec2 Magnitude");
    ASSERT_NEAR(Dot(a2, b2), 3.0f, "Vec2 Dot");
    
    vec2 n2 = Normalize(a2);
    ASSERT_NEAR(Magnitude(n2), 1.0f, "Vec2 Normalize");
    
    vec2 p1 = vec(0.0f, 0.0f);
    vec2 p2 = vec(3.0f, 4.0f);
    ASSERT_NEAR(Distance(p1, p2), 5.0f, "Distance: (0,0) to (3,4) should be 5");
    ASSERT_NEAR(Distance(p2, p1), 5.0f, "Distance: Commutative property");
    ASSERT_NEAR(Distance(p1, p1), 0.0f, "Distance: Point to itself is 0");
    
    vec2 right = vec(1.0f, 0.0f);
    vec2 up    = vec(0.0f, 1.0f);
    real32 halfPi = PI / 2.0f; // 90 degrees
    ASSERT_NEAR(Angle(right, up), halfPi, "Angle: 90 degrees between X and Y axes");
    ASSERT_NEAR(Angle(right, right), 0.0f, "Angle: 0 degrees for identical vectors");
    
    vec2 startPos = vec(1.0f, 0.0f);
    vec2 offset   = vec(5.0f, 5.0f);
    vec2 result   = Transform(startPos, offset, halfPi);
    
    ASSERT_NEAR(result.x, 5.0f, "Transform: X coordinate after rotation and translation");
    ASSERT_NEAR(result.y, 6.0f, "Transform: Y coordinate after rotation and translation");
    
    // vec3
    vec3 a3 = vec(1, 0, 0);
    vec3 b3 = vec(0, 1, 0);
    vec3 c3 = Cross(a3, b3);
    ASSERT_CONDITION(c3.z == 1.0f && c3.x == 0 && c3.y == 0, "Vec3 Cross Product");
    
    vec3 zero = vec(0, 0, 0);
    vec3 normZero = NormalizeOrZero(zero);
    ASSERT_CONDITION(normZero.x == 0 && normZero.y == 0, "NormalizeOrZero safety");
    
    // vec4 & Lerp
    vec3 vStart = vec(0, 0, 0);
    vec3 vEnd = vec(10, 10, 10);
    vec3 vMid = Lerp(vStart, 0.5f, vEnd);
    ASSERT_NEAR(vMid.x, 5.0f, "Vec3 Lerp");
    
    return 0;
}

int test_matrices() {
    mat4x4 I = Identity();
    ASSERT_CONDITION(I.E[0][0] == 1.0f && I.E[3][3] == 1.0f, "Matrix Identity diagonal");
    ASSERT_CONDITION(I.E[0][1] == 0.0f, "Matrix Identity off-diagonal");
    
    // Scaling
    vec3 s = vec(2, 3, 4);
    mat4x4 S = ScalingMat4x4(s);
    ASSERT_CONDITION(S.E[0][0] == 2.0f && S.E[1][1] == 3.0f && S.E[2][2] == 4.0f, "Scaling Matrix");
    
    // Transpose
    mat4x4 T = transpose(S);
    ASSERT_CONDITION(T.E[0][0] == S.E[0][0], "Transpose identity");
    
    return 0;
}

int test_colors() {
    vec4 color = vec(255.0f, 128.0f, 64.0f, 32.0f); // R, G, B, A
    
    // RGB Round-trip
    uint32 packedRGBA = RGBAPack4x8(color);
    vec4 unpacked = RGBAUnpack4x8(packedRGBA);
    
    ASSERT_NEAR(unpacked.r, 255.0f, "Color Unpack R");
    ASSERT_NEAR(unpacked.g, 128.0f, "Color Unpack G");
    ASSERT_NEAR(unpacked.b, 64.0f, "Color Unpack B");
    ASSERT_NEAR(unpacked.a, 32.0f, "Color Unpack A");
    
    return 0;
}
