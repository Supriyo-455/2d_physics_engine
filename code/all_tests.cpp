#include <stdio.h>
#include <math.h>
#include "defines.h"
#include "simple_math.h"
#include "physics2D.h"

#define ASSERT_CONDITION(condition, message) do { \
if (!(condition)) { \
fprintf(stderr, "[FAIL] %s:%d: %s\n", __FILE__, __LINE__, message); \
return 1; \
} \
else \
{ \
fprintf(stdout, "[PASS] %s:%d: %s\n", __FILE__, __LINE__, message); \
} \
} while(0)

#define EPSILON 0.001f

#define ASSERT_NEAR(a, b, message) do { \
real32 diff = absf((real32)(a) - (real32)(b)); \
if (diff > EPSILON) { \
fprintf(stderr, "[FAIL] %s:%d in %s: %s (Expected %f, Got %f)\n", __FILE__, __LINE__, __func__, message, (double)b, (double)a); \
return 1; \
} \
else \
{ \
fprintf(stdout, "[PASS] %s:%d in %s: %s (Expected %f, Got %f)\n", __FILE__, __LINE__, __func__, message, (double)b, (double)a); \
} \
} while(0)

// IMPORTANT: DO NOT RE-ARRANGE THESE FILES
#include "simple_math_tests.cpp"

int main(int ArgsCount, char** Args)
{
    int f = 0;
    
    printf("Running suite: Scalars...\n");     f += test_scalars();
    printf("Running suite: Trig...\n");        f += test_trigonometry();
    printf("Running suite: Vectors...\n");     f += test_vectors();
    printf("Running suite: Matrices...\n");    f += test_matrices();
    printf("Running suite: Colors...\n");      f += test_colors();
    
    printf("\n------------------------------\n");
    if (f == 0) {
        printf("RESULT: ALL TESTS PASSED\n");
    } else {
        printf("RESULT: %d TEST MODULES FAILED\n", f);
    }
    printf("------------------------------\n");
    
    return f;
}