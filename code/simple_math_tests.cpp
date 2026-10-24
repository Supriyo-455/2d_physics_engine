
// =============================================================================
// SCALAR TESTS
// =============================================================================

int test_scalars_abs() {
    ASSERT_NEAR(Abs(-10.5f), 10.5f, "Abs negative float");
    ASSERT_NEAR(Abs(10.5f), 10.5f, "Abs positive float");
    ASSERT_NEAR(Abs(0.0f), 0.0f, "Abs zero");
    ASSERT_NEAR(Abs(-0.0f), 0.0f, "Abs negative zero");
    ASSERT_NEAR(Abs(-0.0001f), 0.0001f, "Abs small negative");
    ASSERT_NEAR(Abs(123456.0f), 123456.0f, "Abs large positive");
    ASSERT_NEAR(Abs(-123456.0f), 123456.0f, "Abs large negative");

    // Backward compatibility alias absf
    ASSERT_NEAR(absf(-10.5f), 10.5f, "absf alias negative");
    ASSERT_NEAR(absf(10.5f), 10.5f, "absf alias positive");
    ASSERT_NEAR(absf(0.0f), 0.0f, "absf alias zero");

    return 0;
}

int test_scalars_round() {
    ASSERT_CONDITION(RoundReal32ToUint32(0.0f) == 0, "Round 0.0");
    ASSERT_CONDITION(RoundReal32ToUint32(0.4f) == 0, "Round 0.4 down");
    ASSERT_CONDITION(RoundReal32ToUint32(0.4999f) == 0, "Round 0.4999 down");
    ASSERT_CONDITION(RoundReal32ToUint32(0.5f) == 1, "Round 0.5 up");
    ASSERT_CONDITION(RoundReal32ToUint32(0.5001f) == 1, "Round 0.5001 up");
    ASSERT_CONDITION(RoundReal32ToUint32(0.6f) == 1, "Round 0.6 up");
    ASSERT_CONDITION(RoundReal32ToUint32(1.0f) == 1, "Round 1.0");
    ASSERT_CONDITION(RoundReal32ToUint32(1.4f) == 1, "Round 1.4 down");
    ASSERT_CONDITION(RoundReal32ToUint32(1.5f) == 2, "Round 1.5 up");
    ASSERT_CONDITION(RoundReal32ToUint32(99.9f) == 100, "Round 99.9 up");
    ASSERT_CONDITION(RoundReal32ToUint32(255.0f) == 255, "Round 255.0");

    return 0;
}

int test_scalars_floor() {
    ASSERT_CONDITION(Floor(0.0f) == 0, "Floor 0.0");
    ASSERT_CONDITION(Floor(0.4f) == 0, "Floor 0.4");
    ASSERT_CONDITION(Floor(0.99f) == 0, "Floor 0.99");
    ASSERT_CONDITION(Floor(1.0f) == 1, "Floor 1.0");
    ASSERT_CONDITION(Floor(1.01f) == 1, "Floor 1.01");
    ASSERT_CONDITION(Floor(1.99f) == 1, "Floor 1.99");
    ASSERT_CONDITION(Floor(5.7f) == 5, "Floor 5.7");
    ASSERT_CONDITION(Floor(100.0f) == 100, "Floor 100.0");
    ASSERT_CONDITION(Floor(-0.1f) == -1, "Floor -0.1");
    ASSERT_CONDITION(Floor(-0.9f) == -1, "Floor -0.9");
    ASSERT_CONDITION(Floor(-1.0f) == -1, "Floor -1.0");
    ASSERT_CONDITION(Floor(-1.5f) == -2, "Floor -1.5");
    ASSERT_CONDITION(Floor(-5.0f) == -5, "Floor -5.0");

    return 0;
}

int test_scalars_equals() {
    ASSERT_CONDITION(Equals(0.0f, 0.0f), "Equals exact zero");
    ASSERT_CONDITION(Equals(1.0f, 1.0f), "Equals exact positive");
    ASSERT_CONDITION(Equals(-5.0f, -5.0f), "Equals exact negative");
    ASSERT_CONDITION(Equals(1.0f, 1.00005f), "Equals within epsilon positive");
    ASSERT_CONDITION(Equals(1.0f, 0.99995f), "Equals within epsilon negative");
    ASSERT_CONDITION(Equals(-3.1415f, -3.14155f), "Equals negative within epsilon");
    ASSERT_CONDITION(!Equals(1.0f, 1.0002f), "Equals outside epsilon positive");
    ASSERT_CONDITION(!Equals(1.0f, 0.9998f), "Equals outside epsilon negative");
    ASSERT_CONDITION(!Equals(-1.0f, 1.0f), "Equals different signs");

    return 0;
}

int test_scalars_clamp() {
    // Clamp Int
    ASSERT_CONDITION(Clamp(5, 0, 10) == 5, "Int Clamp mid");
    ASSERT_CONDITION(Clamp(-1, 0, 10) == 0, "Int Clamp below min");
    ASSERT_CONDITION(Clamp(15, 0, 10) == 10, "Int Clamp above max");
    ASSERT_CONDITION(Clamp(0, 0, 10) == 0, "Int Clamp at min");
    ASSERT_CONDITION(Clamp(10, 0, 10) == 10, "Int Clamp at max");
    ASSERT_CONDITION(Clamp(5, 10, 10) == 10, "Int Clamp equal bounds");
    ASSERT_CONDITION(Clamp(-5, -10, -2) == -5, "Int Clamp negative range mid");
    ASSERT_CONDITION(Clamp(-15, -10, -2) == -10, "Int Clamp negative range min");
    ASSERT_CONDITION(Clamp(0, -10, -2) == -2, "Int Clamp negative range max");
    ASSERT_CONDITION(Clamp(5, 10, 0) == 0, "Int Clamp invalid Min > Max returns 0");

    // Clamp Float
    ASSERT_NEAR(Clamp(0.5f, 0.0f, 1.0f), 0.5f, "Float Clamp mid");
    ASSERT_NEAR(Clamp(-0.5f, 0.0f, 1.0f), 0.0f, "Float Clamp below min");
    ASSERT_NEAR(Clamp(5.5f, 0.0f, 1.0f), 1.0f, "Float Clamp above max");
    ASSERT_NEAR(Clamp(0.0f, 0.0f, 1.0f), 0.0f, "Float Clamp at min");
    ASSERT_NEAR(Clamp(1.0f, 0.0f, 1.0f), 1.0f, "Float Clamp at max");
    ASSERT_NEAR(Clamp(5.5f, 2.0f, 2.0f), 2.0f, "Float Clamp equal bounds");
    ASSERT_NEAR(Clamp(-5.5f, -10.0f, -2.0f), -5.5f, "Float Clamp negative mid");
    ASSERT_NEAR(Clamp(5.0f, 10.0f, 2.0f), 0.0f, "Float Clamp invalid Min > Max returns 0");

    return 0;
}

int test_scalars_srgb() {
    // Clamping boundaries
    ASSERT_NEAR(LinearTosRGB(0.0f), 0.0f, "LinearToSRGB zero");
    ASSERT_NEAR(LinearTosRGB(-1.0f), 0.0f, "LinearToSRGB negative clamped to 0");
    ASSERT_NEAR(LinearTosRGB(0.00005f), 0.0f, "LinearToSRGB < 0.0001 clamped to 0");
    ASSERT_NEAR(LinearTosRGB(1.0f), 1.0f, "LinearToSRGB 1.0");
    ASSERT_NEAR(LinearTosRGB(2.5f), 1.0f, "LinearToSRGB > 1.0 clamped to 1");

    // Linear region (0.0001 <= L <= 0.0031308)
    ASSERT_NEAR(LinearTosRGB(0.001f), 0.001f * 12.92f, "LinearToSRGB low branch 0.001");
    ASSERT_NEAR(LinearTosRGB(0.002f), 0.002f * 12.92f, "LinearToSRGB low branch 0.002");
    ASSERT_NEAR(LinearTosRGB(0.0031308f), 0.0031308f * 12.92f, "LinearToSRGB threshold exact");

    // Gamma curve region (L > 0.0031308)
    real32 expectedMid = (1.055f * powf(0.5f, 1.0f / 2.4f)) - 0.055f;
    ASSERT_NEAR(LinearTosRGB(0.5f), expectedMid, "LinearToSRGB mid branch 0.5");
    real32 expectedQuarter = (1.055f * powf(0.25f, 1.0f / 2.4f)) - 0.055f;
    ASSERT_NEAR(LinearTosRGB(0.25f), expectedQuarter, "LinearToSRGB mid branch 0.25");

    return 0;
}

int test_scalars() {
    int f = 0;
    f += test_scalars_abs();
    f += test_scalars_round();
    f += test_scalars_floor();
    f += test_scalars_equals();
    f += test_scalars_clamp();
    f += test_scalars_srgb();
    return f;
}


// =============================================================================
// TRIGONOMETRY TESTS (Radians)
// =============================================================================

int test_trigonometry() {
    // Basic angles
    ASSERT_NEAR(Sin(0.0f), 0.0f, "Sin(0)");
    ASSERT_NEAR(Cos(0.0f), 1.0f, "Cos(0)");

    ASSERT_NEAR(Sin(PI / 6.0f), 0.5f, "Sin(PI / 6) = 0.5");
    ASSERT_NEAR(Cos(PI / 6.0f), 0.866025f, "Cos(PI / 6) = sqrt(3)/2");

    ASSERT_NEAR(Sin(PI / 4.0f), 0.707107f, "Sin(PI / 4) = sqrt(2)/2");
    ASSERT_NEAR(Cos(PI / 4.0f), 0.707107f, "Cos(PI / 4) = sqrt(2)/2");

    ASSERT_NEAR(Sin(PI / 3.0f), 0.866025f, "Sin(PI / 3) = sqrt(3)/2");
    ASSERT_NEAR(Cos(PI / 3.0f), 0.5f, "Cos(PI / 3) = 0.5");

    ASSERT_NEAR(Sin(PI / 2.0f), 1.0f, "Sin(PI / 2) = 1");
    ASSERT_NEAR(Cos(PI / 2.0f), 0.0f, "Cos(PI / 2) = 0");

    ASSERT_NEAR(Sin(PI), 0.0f, "Sin(PI) = 0");
    ASSERT_NEAR(Cos(PI), -1.0f, "Cos(PI) = -1");

    ASSERT_NEAR(Sin(1.5f * PI), -1.0f, "Sin(3*PI / 2) = -1");
    ASSERT_NEAR(Cos(1.5f * PI), 0.0f, "Cos(3*PI / 2) = 0");

    ASSERT_NEAR(Sin(2.0f * PI), 0.0f, "Sin(2*PI) = 0");
    ASSERT_NEAR(Cos(2.0f * PI), 1.0f, "Cos(2*PI) = 1");

    // Negative angles
    ASSERT_NEAR(Sin(-PI / 2.0f), -1.0f, "Sin(-PI / 2) = -1");
    ASSERT_NEAR(Cos(-PI / 2.0f), 0.0f, "Cos(-PI / 2) = 0");
    ASSERT_NEAR(Sin(-PI), 0.0f, "Sin(-PI) = 0");
    ASSERT_NEAR(Cos(-PI), -1.0f, "Cos(-PI) = -1");

    // Periodic wrap-around
    ASSERT_NEAR(Sin(2.0f * PI + PI / 2.0f), 1.0f, "Sin(2*PI + PI / 2) wrap");
    ASSERT_NEAR(Cos(2.0f * PI + PI / 2.0f), 0.0f, "Cos(2*PI + PI / 2) wrap");
    ASSERT_NEAR(Cos(4.0f * PI), 1.0f, "Cos(4*PI) wrap");
    ASSERT_NEAR(Sin(4.0f * PI), 0.0f, "Sin(4*PI) wrap");

    // Pythagorean identity: sin^2(x) + cos^2(x) = 1
    real32 testAngles[] = { 0.0f, 0.35f, 0.785f, 1.2f, 2.5f, -0.785f, 10.0f };
    for (int i = 0; i < (int)ARRAY_COUNT(testAngles); i++) {
        real32 a = testAngles[i];
        real32 id = (Sin(a) * Sin(a)) + (Cos(a) * Cos(a));
        ASSERT_NEAR(id, 1.0f, "Trig Identity sin2+cos2 = 1");
    }

    return 0;
}


// =============================================================================
// VECTOR TESTS
// =============================================================================

int test_vectors_vec2() {
    // Construction & component access
    vec2 v = vec(3.0f, -4.0f);
    ASSERT_NEAR(v.x, 3.0f, "vec2.x access");
    ASSERT_NEAR(v.y, -4.0f, "vec2.y access");
    ASSERT_NEAR(v.E[0], 3.0f, "vec2.E[0] access");
    ASSERT_NEAR(v.E[1], -4.0f, "vec2.E[1] access");

    // Unary negation
    vec2 neg = -v;
    ASSERT_NEAR(neg.x, -3.0f, "vec2 unary minus x");
    ASSERT_NEAR(neg.y, 4.0f, "vec2 unary minus y");

    // Addition & subtraction
    vec2 a = vec(1.0f, 2.0f);
    vec2 b = vec(3.0f, 4.0f);
    vec2 sum = a + b;
    ASSERT_NEAR(sum.x, 4.0f, "vec2 operator+ x");
    ASSERT_NEAR(sum.y, 6.0f, "vec2 operator+ y");

    vec2 diffVec = b - a;
    ASSERT_NEAR(diffVec.x, 2.0f, "vec2 operator- x");
    ASSERT_NEAR(diffVec.y, 2.0f, "vec2 operator- y");

    // Scalar multiplication
    vec2 mul1 = a * 3.0f;
    ASSERT_NEAR(mul1.x, 3.0f, "vec2 * scalar x");
    ASSERT_NEAR(mul1.y, 6.0f, "vec2 * scalar y");

    vec2 mul2 = 3.0f * a;
    ASSERT_NEAR(mul2.x, 3.0f, "scalar * vec2 x");
    ASSERT_NEAR(mul2.y, 6.0f, "scalar * vec2 y");

    vec2 mulAssign = a;
    mulAssign *= 3.0f;
    ASSERT_NEAR(mulAssign.x, 3.0f, "vec2 *= scalar x");
    ASSERT_NEAR(mulAssign.y, 6.0f, "vec2 *= scalar y");

    // Scalar division
    vec2 div1 = vec(8.0f, 12.0f) / 4.0f;
    ASSERT_NEAR(div1.x, 2.0f, "vec2 / scalar x");
    ASSERT_NEAR(div1.y, 3.0f, "vec2 / scalar y");

    vec2 div2 = 12.0f / vec(3.0f, 4.0f);
    ASSERT_NEAR(div2.x, 4.0f, "scalar / vec2 x");
    ASSERT_NEAR(div2.y, 3.0f, "scalar / vec2 y");

    // Dot product
    ASSERT_NEAR(Dot(vec(1, 0), vec(0, 1)), 0.0f, "vec2 Dot orthogonal");
    ASSERT_NEAR(Dot(vec(3, 4), vec(3, 4)), 25.0f, "vec2 Dot parallel");
    ASSERT_NEAR(Dot(vec(1, 0), vec(-1, 0)), -1.0f, "vec2 Dot opposite");
    ASSERT_NEAR(Dot(vec(2, 3), vec(4, -5)), -7.0f, "vec2 Dot general");

    // 2D Cross product
    ASSERT_NEAR(Cross(vec(1, 0), vec(0, 1)), 1.0f, "vec2 Cross right x up");
    ASSERT_NEAR(Cross(vec(0, 1), vec(1, 0)), -1.0f, "vec2 Cross up x right");
    ASSERT_NEAR(Cross(vec(2, 3), vec(4, 6)), 0.0f, "vec2 Cross parallel");
    ASSERT_NEAR(Cross(vec(2, 5), vec(3, 7)), -1.0f, "vec2 Cross general (2*7 - 5*3)");

    // Magnitude & Normalize
    ASSERT_NEAR(Magnitude(vec(0, 0)), 0.0f, "vec2 Magnitude zero");
    ASSERT_NEAR(Magnitude(vec(1, 0)), 1.0f, "vec2 Magnitude unit x");
    ASSERT_NEAR(Magnitude(vec(0, -1)), 1.0f, "vec2 Magnitude unit -y");
    ASSERT_NEAR(Magnitude(vec(3, 4)), 5.0f, "vec2 Magnitude 3-4-5");

    vec2 norm = Normalize(vec(3, 4));
    ASSERT_NEAR(norm.x, 0.6f, "vec2 Normalize x");
    ASSERT_NEAR(norm.y, 0.8f, "vec2 Normalize y");
    ASSERT_NEAR(Magnitude(norm), 1.0f, "vec2 Normalize magnitude is 1");

    // Distance & DistanceSquared
    vec2 p1 = vec(1.0f, 2.0f);
    vec2 p2 = vec(4.0f, 6.0f);
    ASSERT_NEAR(Distance(p1, p1), 0.0f, "vec2 Distance self is 0");
    ASSERT_NEAR(DistanceSquared(p1, p1), 0.0f, "vec2 DistanceSquared self is 0");
    ASSERT_NEAR(Distance(p1, p2), 5.0f, "vec2 Distance 3-4-5");
    ASSERT_NEAR(DistanceSquared(p1, p2), 25.0f, "vec2 DistanceSquared 3-4-5");
    ASSERT_NEAR(Distance(p1, p2), Distance(p2, p1), "vec2 Distance commutative");

    // Angle
    ASSERT_NEAR(Angle(vec(1, 0), vec(1, 0)), 0.0f, "vec2 Angle identical");
    ASSERT_NEAR(Angle(vec(1, 0), vec(0, 1)), PI / 2.0f, "vec2 Angle perpendicular");
    ASSERT_NEAR(Angle(vec(1, 0), vec(-1, 0)), PI, "vec2 Angle opposite");
    ASSERT_NEAR(Angle(vec(1, 0), vec(1, 1)), PI / 4.0f, "vec2 Angle 45 deg");

    // Equals
    ASSERT_CONDITION(Equals(vec(1.0f, 2.0f), vec(1.0f, 2.0f)), "vec2 Equals exact");
    ASSERT_CONDITION(Equals(vec(1.0f, 2.0f), vec(1.00005f, 1.99995f)), "vec2 Equals within eps");
    ASSERT_CONDITION(!Equals(vec(1.0f, 2.0f), vec(1.0002f, 2.0f)), "vec2 Equals outside eps x");
    ASSERT_CONDITION(!Equals(vec(1.0f, 2.0f), vec(1.0f, 2.0002f)), "vec2 Equals outside eps y");

    return 0;
}

int test_vectors_vec3() {
    // Construction & component access
    vec3 v = vec(1.0f, -2.0f, 3.0f);
    ASSERT_NEAR(v.x, 1.0f, "vec3.x");
    ASSERT_NEAR(v.y, -2.0f, "vec3.y");
    ASSERT_NEAR(v.z, 3.0f, "vec3.z");
    ASSERT_NEAR(v.E[0], 1.0f, "vec3.E[0]");
    ASSERT_NEAR(v.E[1], -2.0f, "vec3.E[1]");
    ASSERT_NEAR(v.E[2], 3.0f, "vec3.E[2]");

    // Unary negation
    vec3 neg = -v;
    ASSERT_NEAR(neg.x, -1.0f, "vec3 -v x");
    ASSERT_NEAR(neg.y, 2.0f, "vec3 -v y");
    ASSERT_NEAR(neg.z, -3.0f, "vec3 -v z");

    // Addition & Subtraction
    vec3 a = vec(1.0f, 2.0f, 3.0f);
    vec3 b = vec(4.0f, 5.0f, 6.0f);
    vec3 sum = a + b;
    ASSERT_NEAR(sum.x, 5.0f, "vec3 a+b x");
    ASSERT_NEAR(sum.y, 7.0f, "vec3 a+b y");
    ASSERT_NEAR(sum.z, 9.0f, "vec3 a+b z");

    vec3 diffVec = b - a;
    ASSERT_NEAR(diffVec.x, 3.0f, "vec3 b-a x");
    ASSERT_NEAR(diffVec.y, 3.0f, "vec3 b-a y");
    ASSERT_NEAR(diffVec.z, 3.0f, "vec3 b-a z");

    // Scalar multiplication & division
    vec3 mul = a * 2.0f;
    ASSERT_NEAR(mul.x, 2.0f, "vec3 * scalar x");
    ASSERT_NEAR(mul.y, 4.0f, "vec3 * scalar y");
    ASSERT_NEAR(mul.z, 6.0f, "vec3 * scalar z");

    vec3 div = mul / 2.0f;
    ASSERT_NEAR(div.x, 1.0f, "vec3 / scalar x");
    ASSERT_NEAR(div.y, 2.0f, "vec3 / scalar y");
    ASSERT_NEAR(div.z, 3.0f, "vec3 / scalar z");

    // Inner (dot) product
    ASSERT_NEAR(Inner(vec(1, 0, 0), vec(0, 1, 0)), 0.0f, "vec3 Inner orthogonal");
    ASSERT_NEAR(Inner(vec(1, 2, 3), vec(4, -5, 6)), 12.0f, "vec3 Inner general (4 - 10 + 18)");

    // Cross product
    vec3 iHat = vec(1, 0, 0);
    vec3 jHat = vec(0, 1, 0);
    vec3 kHat = vec(0, 0, 1);

    vec3 c1 = Cross(iHat, jHat); // i x j = k
    ASSERT_NEAR(c1.x, 0.0f, "Cross(i, j).x");
    ASSERT_NEAR(c1.y, 0.0f, "Cross(i, j).y");
    ASSERT_NEAR(c1.z, 1.0f, "Cross(i, j).z");

    vec3 c2 = Cross(jHat, kHat); // j x k = i
    ASSERT_NEAR(c2.x, 1.0f, "Cross(j, k).x");
    ASSERT_NEAR(c2.y, 0.0f, "Cross(j, k).y");
    ASSERT_NEAR(c2.z, 0.0f, "Cross(j, k).z");

    vec3 c3 = Cross(kHat, iHat); // k x i = j
    ASSERT_NEAR(c3.x, 0.0f, "Cross(k, i).x");
    ASSERT_NEAR(c3.y, 1.0f, "Cross(k, i).y");
    ASSERT_NEAR(c3.z, 0.0f, "Cross(k, i).z");

    // Cross anti-commutativity: a x b = -(b x a)
    vec3 cAnti = Cross(jHat, iHat);
    ASSERT_NEAR(cAnti.z, -1.0f, "Cross anti-commutativity");

    // Cross self is zero
    vec3 cSelf = Cross(a, a);
    ASSERT_NEAR(cSelf.x, 0.0f, "Cross self x");
    ASSERT_NEAR(cSelf.y, 0.0f, "Cross self y");
    ASSERT_NEAR(cSelf.z, 0.0f, "Cross self z");

    // Cross orthogonality: Inner(Cross(A, B), A) == 0
    vec3 cGen = Cross(a, b);
    ASSERT_NEAR(Inner(cGen, a), 0.0f, "Cross orthogonality with A");
    ASSERT_NEAR(Inner(cGen, b), 0.0f, "Cross orthogonality with B");

    // Hadamard product
    vec3 had = Hadamard(vec(2, 3, 4), vec(5, 6, 7));
    ASSERT_NEAR(had.x, 10.0f, "Hadamard x");
    ASSERT_NEAR(had.y, 18.0f, "Hadamard y");
    ASSERT_NEAR(had.z, 28.0f, "Hadamard z");

    // Length & LengthSq
    vec3 v3 = vec(2.0f, 3.0f, 6.0f);
    ASSERT_NEAR(LengthSq(v3), 49.0f, "vec3 LengthSq 2-3-6");
    ASSERT_NEAR(Length(v3), 7.0f, "vec3 Length 2-3-6");

    // Normalize
    vec3 vNorm = Normalize(vec(0.0f, 3.0f, 4.0f));
    ASSERT_NEAR(vNorm.x, 0.0f, "vec3 Normalize x");
    ASSERT_NEAR(vNorm.y, 0.6f, "vec3 Normalize y");
    ASSERT_NEAR(vNorm.z, 0.8f, "vec3 Normalize z");
    ASSERT_NEAR(Length(vNorm), 1.0f, "vec3 Normalize length is 1");

    // NormalizeOrZero
    vec3 zeroNorm = NormalizeOrZero(vec(0, 0, 0));
    ASSERT_NEAR(zeroNorm.x, 0.0f, "NormalizeOrZero zero vector x");
    ASSERT_NEAR(zeroNorm.y, 0.0f, "NormalizeOrZero zero vector y");
    ASSERT_NEAR(zeroNorm.z, 0.0f, "NormalizeOrZero zero vector z");

    vec3 tinyNorm = NormalizeOrZero(vec(0.00001f, 0, 0));
    ASSERT_NEAR(tinyNorm.x, 0.0f, "NormalizeOrZero tiny vector x");
    ASSERT_NEAR(tinyNorm.y, 0.0f, "NormalizeOrZero tiny vector y");
    ASSERT_NEAR(tinyNorm.z, 0.0f, "NormalizeOrZero tiny vector z");

    vec3 normalNorm = NormalizeOrZero(vec(0, 0, 5));
    ASSERT_NEAR(normalNorm.x, 0.0f, "NormalizeOrZero normal vector x");
    ASSERT_NEAR(normalNorm.y, 0.0f, "NormalizeOrZero normal vector y");
    ASSERT_NEAR(normalNorm.z, 1.0f, "NormalizeOrZero normal vector z");

    // Lerp
    vec3 start = vec(0, 10, 20);
    vec3 end   = vec(10, 20, 40);
    vec3 l0    = Lerp(start, 0.0f, end);
    ASSERT_NEAR(l0.x, start.x, "Lerp t=0 x");
    ASSERT_NEAR(l0.y, start.y, "Lerp t=0 y");
    ASSERT_NEAR(l0.z, start.z, "Lerp t=0 z");

    vec3 l1    = Lerp(start, 1.0f, end);
    ASSERT_NEAR(l1.x, end.x, "Lerp t=1 x");
    ASSERT_NEAR(l1.y, end.y, "Lerp t=1 y");
    ASSERT_NEAR(l1.z, end.z, "Lerp t=1 z");

    vec3 lHalf = Lerp(start, 0.5f, end);
    ASSERT_NEAR(lHalf.x, 5.0f, "Lerp t=0.5 x");
    ASSERT_NEAR(lHalf.y, 15.0f, "Lerp t=0.5 y");
    ASSERT_NEAR(lHalf.z, 30.0f, "Lerp t=0.5 z");

    return 0;
}

int test_vectors_vec4() {
    // Construction & component access
    vec4 v4 = vec(1.0f, 2.0f, 3.0f, 4.0f);
    ASSERT_NEAR(v4.x, 1.0f, "vec4.x");
    ASSERT_NEAR(v4.y, 2.0f, "vec4.y");
    ASSERT_NEAR(v4.z, 3.0f, "vec4.z");
    ASSERT_NEAR(v4.w, 4.0f, "vec4.w");
    ASSERT_NEAR(v4.r, 1.0f, "vec4.r");
    ASSERT_NEAR(v4.g, 2.0f, "vec4.g");
    ASSERT_NEAR(v4.b, 3.0f, "vec4.b");
    ASSERT_NEAR(v4.a, 4.0f, "vec4.a");
    ASSERT_NEAR(v4.E[0], 1.0f, "vec4.E[0]");
    ASSERT_NEAR(v4.E[3], 4.0f, "vec4.E[3]");

    // Construct vec4 from vec3 and w
    vec4 vFrom3 = vec(vec(5.0f, 6.0f, 7.0f), 8.0f);
    ASSERT_NEAR(vFrom3.x, 5.0f, "vec4 from vec3 x");
    ASSERT_NEAR(vFrom3.y, 6.0f, "vec4 from vec3 y");
    ASSERT_NEAR(vFrom3.z, 7.0f, "vec4 from vec3 z");
    ASSERT_NEAR(vFrom3.w, 8.0f, "vec4 from vec3 w");

    // Scalar multiplication & division
    vec4 mul = v4 * 2.0f;
    ASSERT_NEAR(mul.x, 2.0f, "vec4 * scalar x");
    ASSERT_NEAR(mul.y, 4.0f, "vec4 * scalar y");
    ASSERT_NEAR(mul.z, 6.0f, "vec4 * scalar z");
    ASSERT_NEAR(mul.w, 8.0f, "vec4 * scalar w");

    vec4 div = mul / 2.0f;
    ASSERT_NEAR(div.x, 1.0f, "vec4 / scalar x");
    ASSERT_NEAR(div.y, 2.0f, "vec4 / scalar y");
    ASSERT_NEAR(div.z, 3.0f, "vec4 / scalar z");
    ASSERT_NEAR(div.w, 4.0f, "vec4 / scalar w");

    // Inner product
    ASSERT_NEAR(Inner(vec(1, 0, 0, 0), vec(0, 1, 0, 0)), 0.0f, "vec4 Inner orthogonal");
    ASSERT_NEAR(Inner(vec(1, 2, 3, 4), vec(2, -1, 4, -2)), 4.0f, "vec4 Inner general (2 - 2 + 12 - 8)");

    // Length & LengthSq
    vec4 vLen = vec(1.0f, 2.0f, 2.0f, 4.0f);
    ASSERT_NEAR(LengthSq(vLen), 25.0f, "vec4 LengthSq (1 + 4 + 4 + 16)");
    ASSERT_NEAR(Length(vLen), 5.0f, "vec4 Length");

    // Normalize
    vec4 norm = Normalize(vec(0.0f, 0.0f, 3.0f, 4.0f));
    ASSERT_NEAR(norm.x, 0.0f, "vec4 Normalize x");
    ASSERT_NEAR(norm.y, 0.0f, "vec4 Normalize y");
    ASSERT_NEAR(norm.z, 0.6f, "vec4 Normalize z");
    ASSERT_NEAR(norm.w, 0.8f, "vec4 Normalize w");
    ASSERT_NEAR(Length(norm), 1.0f, "vec4 Normalize length is 1");

    return 0;
}

int test_vectors() {
    int f = 0;
    f += test_vectors_vec2();
    f += test_vectors_vec3();
    f += test_vectors_vec4();
    return f;
}


// =============================================================================
// MATRIX TESTS
// =============================================================================

int test_matrices_identity() {
    mat4x4 I = Identity();

    // Check diagonal elements are 1
    for (int i = 0; i < 4; i++) {
        ASSERT_NEAR(I.E[i][i], 1.0f, "Identity diagonal element is 1");
    }

    // Check off-diagonal elements are 0
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (r != c) {
                ASSERT_NEAR(I.E[r][c], 0.0f, "Identity off-diagonal element is 0");
            }
        }
    }

    return 0;
}

int test_matrices_scaling_and_translation() {
    // Scaling matrix
    vec3 scale = vec(2.0f, 3.0f, 4.0f);
    mat4x4 S = ScalingMat4x4(scale);
    ASSERT_NEAR(S.E[0][0], 2.0f, "Scaling S.E[0][0]");
    ASSERT_NEAR(S.E[1][1], 3.0f, "Scaling S.E[1][1]");
    ASSERT_NEAR(S.E[2][2], 4.0f, "Scaling S.E[2][2]");
    ASSERT_NEAR(S.E[3][3], 1.0f, "Scaling S.E[3][3]");
    ASSERT_NEAR(S.E[0][1], 0.0f, "Scaling off-diagonal");

    // Translation matrix
    vec3 trans = vec(10.0f, 20.0f, 30.0f);
    mat4x4 T = transMat4x4(Identity(), trans);
    ASSERT_NEAR(T.E[0][3], 10.0f, "Translation T.E[0][3]");
    ASSERT_NEAR(T.E[1][3], 20.0f, "Translation T.E[1][3]");
    ASSERT_NEAR(T.E[2][3], 30.0f, "Translation T.E[2][3]");
    ASSERT_NEAR(T.E[0][0], 1.0f, "Translation diagonal T.E[0][0]");
    ASSERT_NEAR(T.E[3][3], 1.0f, "Translation diagonal T.E[3][3]");

    return 0;
}

int test_matrices_operations() {
    mat4x4 I = Identity();

    // Matrix addition
    mat4x4 sum = I + I;
    for (int i = 0; i < 4; i++) {
        ASSERT_NEAR(sum.E[i][i], 2.0f, "Matrix addition diagonal");
    }
    ASSERT_NEAR(sum.E[0][1], 0.0f, "Matrix addition off-diagonal");

    // Matrix scalar multiplication
    mat4x4 mulScalar = I * 5.0f;
    for (int i = 0; i < 4; i++) {
        ASSERT_NEAR(mulScalar.E[i][i], 5.0f, "Matrix scalar mul diagonal");
    }
    ASSERT_NEAR(mulScalar.E[0][1], 0.0f, "Matrix scalar mul off-diagonal");

    // Matrix-vector multiplication
    vec4 v = vec(1.0f, 2.0f, 3.0f, 1.0f);
    vec4 vIdent = I * v;
    ASSERT_NEAR(vIdent.x, v.x, "Identity * vec4 x");
    ASSERT_NEAR(vIdent.y, v.y, "Identity * vec4 y");
    ASSERT_NEAR(vIdent.z, v.z, "Identity * vec4 z");
    ASSERT_NEAR(vIdent.w, v.w, "Identity * vec4 w");

    // Scaling * vec4
    mat4x4 S = ScalingMat4x4(vec(2.0f, 3.0f, 4.0f));
    vec4 vScaled = S * v;
    ASSERT_NEAR(vScaled.x, 2.0f, "Scaling * vec4 x");
    ASSERT_NEAR(vScaled.y, 6.0f, "Scaling * vec4 y");
    ASSERT_NEAR(vScaled.z, 12.0f, "Scaling * vec4 z");
    ASSERT_NEAR(vScaled.w, 1.0f, "Scaling * vec4 w");

    // Translation * vec4 (position with w=1)
    mat4x4 T = transMat4x4(Identity(), vec(10.0f, 20.0f, 30.0f));
    vec4 vTranslated = T * v;
    ASSERT_NEAR(vTranslated.x, 11.0f, "Translation * vec4 pos x");
    ASSERT_NEAR(vTranslated.y, 22.0f, "Translation * vec4 pos y");
    ASSERT_NEAR(vTranslated.z, 33.0f, "Translation * vec4 pos z");
    ASSERT_NEAR(vTranslated.w, 1.0f, "Translation * vec4 pos w");

    // Translation * vec4 (direction with w=0) -> unchanged
    vec4 dir = vec(1.0f, 2.0f, 3.0f, 0.0f);
    vec4 dirTranslated = T * dir;
    ASSERT_NEAR(dirTranslated.x, dir.x, "Translation * direction x");
    ASSERT_NEAR(dirTranslated.y, dir.y, "Translation * direction y");
    ASSERT_NEAR(dirTranslated.z, dir.z, "Translation * direction z");
    ASSERT_NEAR(dirTranslated.w, 0.0f, "Translation * direction w");

    // Matrix-Matrix multiplication
    mat4x4 II = I * I;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            ASSERT_NEAR(II.E[r][c], I.E[r][c], "Identity * Identity is Identity");
        }
    }

    // Compound transformation: M = T * S
    mat4x4 M = T * S;
    vec4 vCombined = M * v;
    ASSERT_NEAR(vCombined.x, 12.0f, "Compound (T * S) * v x (2 + 10)");
    ASSERT_NEAR(vCombined.y, 26.0f, "Compound (T * S) * v y (6 + 20)");
    ASSERT_NEAR(vCombined.z, 42.0f, "Compound (T * S) * v z (12 + 30)");
    ASSERT_NEAR(vCombined.w, 1.0f, "Compound (T * S) * v w");

    // Transpose
    mat4x4 transT = transpose(T);
    ASSERT_NEAR(transT.E[3][0], 10.0f, "Transpose row/col swap [3][0]");
    ASSERT_NEAR(transT.E[3][1], 20.0f, "Transpose row/col swap [3][1]");
    ASSERT_NEAR(transT.E[3][2], 30.0f, "Transpose row/col swap [3][2]");
    ASSERT_NEAR(transT.E[0][3], 0.0f, "Transpose original [0][3] now 0");

    mat4x4 doubleTrans = transpose(transpose(T));
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            ASSERT_NEAR(doubleTrans.E[r][c], T.E[r][c], "transpose(transpose(M)) == M");
        }
    }

    return 0;
}

int test_matrices_rotations_and_projection() {
    // rotateZ
    mat4x4 rz0 = rotateZ(0.0f);
    ASSERT_NEAR(rz0.E[0][0], 1.0f, "rotateZ(0) diagonal [0][0]");
    ASSERT_NEAR(rz0.E[1][1], 1.0f, "rotateZ(0) diagonal [1][1]");

    mat4x4 rz90 = rotateZ(PI / 2.0f);
    vec4 xPoint = vec(1.0f, 0.0f, 0.0f, 1.0f);
    vec4 rotZPoint = rz90 * xPoint;
    ASSERT_NEAR(rotZPoint.x, 0.0f, "rotateZ(PI/2) * (1,0,0,1) x");
    ASSERT_NEAR(rotZPoint.y, 1.0f, "rotateZ(PI/2) * (1,0,0,1) y");
    ASSERT_NEAR(rotZPoint.z, 0.0f, "rotateZ(PI/2) * (1,0,0,1) z");
    ASSERT_NEAR(rotZPoint.w, 1.0f, "rotateZ(PI/2) * (1,0,0,1) w");

    // rotateX
    mat4x4 rx90 = rotateX(PI / 2.0f);
    vec4 yPoint = vec(0.0f, 1.0f, 0.0f, 1.0f);
    vec4 rotXPoint = rx90 * yPoint;
    ASSERT_NEAR(rotXPoint.x, 0.0f, "rotateX(PI/2) * (0,1,0,1) x");
    ASSERT_NEAR(rotXPoint.y, 0.0f, "rotateX(PI/2) * (0,1,0,1) y");
    ASSERT_NEAR(rotXPoint.z, 1.0f, "rotateX(PI/2) * (0,1,0,1) z");
    ASSERT_NEAR(rotXPoint.w, 1.0f, "rotateX(PI/2) * (0,1,0,1) w");

    // projection
    mat4x4 proj = projection(1.777f, 2.0f);
    ASSERT_NEAR(proj.E[0][0], 1.0f, "projection E[0][0]");
    ASSERT_NEAR(proj.E[1][1], 1.777f, "projection E[1][1] (aspect)");
    ASSERT_NEAR(proj.E[2][2], 1.0f, "projection E[2][2]");
    ASSERT_NEAR(proj.E[3][2], 2.0f, "projection E[3][2] (focal length)");
    ASSERT_NEAR(proj.E[0][1], 0.0f, "projection off-diagonal");

    return 0;
}

int test_matrices() {
    int f = 0;
    f += test_matrices_identity();
    f += test_matrices_scaling_and_translation();
    f += test_matrices_operations();
    f += test_matrices_rotations_and_projection();
    return f;
}


// =============================================================================
// COLOR TESTS
// =============================================================================

int test_colors() {
    // Component aliasing
    vec4 color = vec(255.0f, 128.0f, 64.0f, 32.0f); // R, G, B, A
    ASSERT_NEAR(color.r, 255.0f, "Color r alias");
    ASSERT_NEAR(color.g, 128.0f, "Color g alias");
    ASSERT_NEAR(color.b, 64.0f, "Color b alias");
    ASSERT_NEAR(color.a, 32.0f, "Color a alias");

    // RGBAPack4x8 & RGBAUnpack4x8 round-trip
    uint32 packedRGBA = RGBAPack4x8(color);
    uint32 expectedPacked = (32u << 24) | (64u << 16) | (128u << 8) | 255u;
    ASSERT_CONDITION(packedRGBA == expectedPacked, "RGBAPack4x8 exact bit pattern");

    vec4 unpacked = RGBAUnpack4x8(packedRGBA);
    ASSERT_NEAR(unpacked.r, 255.0f, "Color Unpack R");
    ASSERT_NEAR(unpacked.g, 128.0f, "Color Unpack G");
    ASSERT_NEAR(unpacked.b, 64.0f, "Color Unpack B");
    ASSERT_NEAR(unpacked.a, 32.0f, "Color Unpack A");

    // Black & White round-trips
    vec4 black = vec(0.0f, 0.0f, 0.0f, 0.0f);
    uint32 packedBlack = RGBAPack4x8(black);
    ASSERT_CONDITION(packedBlack == 0x00000000u, "Black packed value");
    vec4 unpackedBlack = RGBAUnpack4x8(packedBlack);
    ASSERT_NEAR(unpackedBlack.r, 0.0f, "Unpack Black R");
    ASSERT_NEAR(unpackedBlack.a, 0.0f, "Unpack Black A");

    vec4 white = vec(255.0f, 255.0f, 255.0f, 255.0f);
    uint32 packedWhite = RGBAPack4x8(white);
    ASSERT_CONDITION(packedWhite == 0xFFFFFFFFu, "White packed value");
    vec4 unpackedWhite = RGBAUnpack4x8(packedWhite);
    ASSERT_NEAR(unpackedWhite.r, 255.0f, "Unpack White R");
    ASSERT_NEAR(unpackedWhite.a, 255.0f, "Unpack White A");

    // Primary Colors
    vec4 red = vec(255.0f, 0.0f, 0.0f, 255.0f);
    ASSERT_CONDITION(RGBAPack4x8(red) == 0xFF0000FFu, "Red packed value");

    vec4 green = vec(0.0f, 255.0f, 0.0f, 255.0f);
    ASSERT_CONDITION(RGBAPack4x8(green) == 0xFF00FF00u, "Green packed value");

    vec4 blue = vec(0.0f, 0.0f, 255.0f, 255.0f);
    ASSERT_CONDITION(RGBAPack4x8(blue) == 0xFFFF0000u, "Blue packed value");

    // Floating-point rounding on pack
    vec4 floatColor = vec(0.4f, 0.6f, 127.8f, 254.3f);
    vec4 unpackedFloat = RGBAUnpack4x8(RGBAPack4x8(floatColor));
    ASSERT_NEAR(unpackedFloat.r, 0.0f, "Unpack rounded float 0.4 -> 0");
    ASSERT_NEAR(unpackedFloat.g, 1.0f, "Unpack rounded float 0.6 -> 1");
    ASSERT_NEAR(unpackedFloat.b, 128.0f, "Unpack rounded float 127.8 -> 128");
    ASSERT_NEAR(unpackedFloat.a, 254.0f, "Unpack rounded float 254.3 -> 254");

    // BGRAPack4x8
    vec4 bgraTest = vec(255.0f, 0.0f, 0.0f, 32.0f); // Red with A=32
    // BGRAPack4x8 packs X (255) into bits 24..31, Y (0) into 16..23, Z (0) into 8..15, W (32) into 0..7
    uint32 packedBGRA = BGRAPack4x8(bgraTest);
    uint32 expectedBGRA = (255u << 24) | (0u << 16) | (0u << 8) | 32u;
    ASSERT_CONDITION(packedBGRA == expectedBGRA, "BGRAPack4x8 byte order verification");

    return 0;
}


// =============================================================================
// 2D TRANSFORM TESTS
// =============================================================================

int test_transforms() {
    // CreateTransform2D (vec2 Pos, real32 Angle)
    vec2 pos = vec(10.0f, 20.0f);
    transform2D t1 = CreateTransform2D(pos, PI / 2.0f);
    ASSERT_NEAR(t1.PosX, 10.0f, "CreateTransform2D PosX");
    ASSERT_NEAR(t1.PosY, 20.0f, "CreateTransform2D PosY");
    ASSERT_NEAR(t1.Sin, 1.0f, "CreateTransform2D Sin(PI/2)");
    ASSERT_NEAR(t1.Cos, 0.0f, "CreateTransform2D Cos(PI/2)");

    // CreateTransform2D (real32 PosX, real32 PosY, real32 Angle)
    transform2D t2 = CreateTransform2D(5.0f, -3.0f, 0.0f);
    ASSERT_NEAR(t2.PosX, 5.0f, "CreateTransform2D scalar PosX");
    ASSERT_NEAR(t2.PosY, -3.0f, "CreateTransform2D scalar PosY");
    ASSERT_NEAR(t2.Sin, 0.0f, "CreateTransform2D scalar Sin(0)");
    ASSERT_NEAR(t2.Cos, 1.0f, "CreateTransform2D scalar Cos(0)");

    // Transform: Identity (no rotation, no translation)
    vec2 v = vec(3.0f, 4.0f);
    vec2 resIdent = Transform(v, vec(0.0f, 0.0f), 0.0f);
    ASSERT_NEAR(resIdent.x, 3.0f, "Transform identity x");
    ASSERT_NEAR(resIdent.y, 4.0f, "Transform identity y");

    // Transform: Pure translation
    vec2 resTrans = Transform(v, vec(5.0f, -2.0f), 0.0f);
    ASSERT_NEAR(resTrans.x, 8.0f, "Transform translation x");
    ASSERT_NEAR(resTrans.y, 2.0f, "Transform translation y");

    // Transform: Pure rotation 90 deg (PI / 2)
    vec2 xUnit = vec(1.0f, 0.0f);
    vec2 resRot90 = Transform(xUnit, vec(0.0f, 0.0f), PI / 2.0f);
    ASSERT_NEAR(resRot90.x, 0.0f, "Transform rotate 90 x");
    ASSERT_NEAR(resRot90.y, 1.0f, "Transform rotate 90 y");

    // Transform: Pure rotation 180 deg (PI)
    vec2 resRot180 = Transform(xUnit, vec(0.0f, 0.0f), PI);
    ASSERT_NEAR(resRot180.x, -1.0f, "Transform rotate 180 x");
    ASSERT_NEAR(resRot180.y, 0.0f, "Transform rotate 180 y");

    // Transform: Pure rotation 270 deg (1.5 * PI)
    vec2 resRot270 = Transform(xUnit, vec(0.0f, 0.0f), 1.5f * PI);
    ASSERT_NEAR(resRot270.x, 0.0f, "Transform rotate 270 x");
    ASSERT_NEAR(resRot270.y, -1.0f, "Transform rotate 270 y");

    // Transform: Combined rotation (90 deg) and translation (5, 5)
    vec2 startPos = vec(1.0f, 0.0f);
    vec2 offset   = vec(5.0f, 5.0f);
    vec2 result   = Transform(startPos, offset, PI / 2.0f);
    ASSERT_NEAR(result.x, 5.0f, "Transform: X coordinate after 90 deg rotation and translation");
    ASSERT_NEAR(result.y, 6.0f, "Transform: Y coordinate after 90 deg rotation and translation");

    // Transform using cached transform2D struct
    transform2D savedT = CreateTransform2D(offset, PI / 2.0f);
    vec2 resultStruct = Transform(startPos, savedT);
    ASSERT_NEAR(resultStruct.x, 5.0f, "Transform struct: X coord");
    ASSERT_NEAR(resultStruct.y, 6.0f, "Transform struct: Y coord");

    // Transform with 180 deg rotation
    vec2 resCombined180 = Transform(vec(2.0f, 3.0f), vec(-1.0f, 4.0f), PI);
    ASSERT_NEAR(resCombined180.x, -3.0f, "Transform 180 + translation x (-2 - 1)");
    ASSERT_NEAR(resCombined180.y, 1.0f, "Transform 180 + translation y (-3 + 4)");

    return 0;
}


// =============================================================================
// RANDOM NUMBER GENERATOR TESTS
// =============================================================================

int test_random() {
    InitializeRandomNumbers();

    // RandomUnilateral: values must be in [0.0, 1.0]
    real32 minUni = 100.0f;
    real32 maxUni = -100.0f;
    for (int i = 0; i < 100; i++) {
        real32 r = RandomUnilateral();
        ASSERT_CONDITION(r >= 0.0f && r <= 1.0f, "RandomUnilateral value in [0, 1]");
        if (r < minUni) minUni = r;
        if (r > maxUni) maxUni = r;
    }
    // Verify it is not returning a constant number
    ASSERT_CONDITION(minUni < 0.45f, "RandomUnilateral distribution min spread");
    ASSERT_CONDITION(maxUni > 0.55f, "RandomUnilateral distribution max spread");

    // RandomBilateral: values must be in [-1.0, 1.0]
    real32 minBi = 100.0f;
    real32 maxBi = -100.0f;
    for (int i = 0; i < 100; i++) {
        real32 r = RandomBilateral();
        ASSERT_CONDITION(r >= -1.0f && r <= 1.0f, "RandomBilateral value in [-1, 1]");
        if (r < minBi) minBi = r;
        if (r > maxBi) maxBi = r;
    }
    // Verify it produces both negative and positive numbers
    ASSERT_CONDITION(minBi < -0.2f, "RandomBilateral negative spread");
    ASSERT_CONDITION(maxBi > 0.2f, "RandomBilateral positive spread");

    return 0;
}
