#include <stdio.h>
#include <math.h>
#include "defines.h"
#include "simple_math.h"
#include "physics_2d.h"
#include "transform_2d.h"
#include "transform_2d.cpp"

global uint32 GlobalTotalTests = 0;
global uint32 GlobalPassedTests = 0;
global uint32 GlobalFailedTests = 0;

#define ASSERT_CONDITION(condition, message) do { \
GlobalTotalTests++; \
if (!(condition)) { \
GlobalFailedTests++; \
fprintf(stderr, "[FAIL] %s:%d: %s\n", __FILE__, __LINE__, message); \
return 1; \
} \
else \
{ \
GlobalPassedTests++; \
fprintf(stdout, "[PASS] %s:%d: %s\n", __FILE__, __LINE__, message); \
} \
} while(0)

#define EPSILON 0.001f

#define ASSERT_NEAR(a, b, message) do { \
GlobalTotalTests++; \
real32 _macro_diff_ = Abs((real32)(a) - (real32)(b)); \
if (_macro_diff_ > EPSILON) { \
GlobalFailedTests++; \
fprintf(stderr, "[FAIL] %s:%d in %s: %s (Expected %f, Got %f)\n", __FILE__, __LINE__, __func__, message, (double)(b), (double)(a)); \
return 1; \
} \
else \
{ \
GlobalPassedTests++; \
fprintf(stdout, "[PASS] %s:%d in %s: %s (Expected %f, Got %f)\n", __FILE__, __LINE__, __func__, message, (double)(b), (double)(a)); \
} \
} while(0)

#define ASSERT_NEAR_EPS(a, b, eps, message) do { \
GlobalTotalTests++; \
real32 _macro_diff_ = Abs((real32)(a) - (real32)(b)); \
if (_macro_diff_ > (eps)) { \
GlobalFailedTests++; \
fprintf(stderr, "[FAIL] %s:%d in %s: %s (Expected %f, Got %f, Diff %f > %f)\n", __FILE__, __LINE__, __func__, message, (double)(b), (double)(a), (double)_macro_diff_, (double)(eps)); \
return 1; \
} \
else \
{ \
GlobalPassedTests++; \
fprintf(stdout, "[PASS] %s:%d in %s: %s (Expected %f, Got %f)\n", __FILE__, __LINE__, __func__, message, (double)(b), (double)(a)); \
} \
} while(0)

// IMPORTANT: DO NOT RE-ARRANGE THESE FILES
#include "simple_math_tests.cpp"

typedef int (*test_suite_func)();

struct test_suite_entry
{
    const char* Name;
    test_suite_func Func;
};

struct test_suite_result
{
    const char* Name;
    uint32 TotalTests;
    uint32 PassedTests;
    uint32 FailedTests;
    bool32 Success;
};

int main(int ArgsCount, char** Args)
{
    (void)ArgsCount;
    (void)Args;

    test_suite_entry Suites[] = {
        { "Scalars", test_scalars },
        { "Trig", test_trigonometry },
        { "Vectors", test_vectors },
        { "Matrices", test_matrices },
        { "Colors", test_colors },
        { "Transforms", test_transforms },
        { "Random", test_random },
    };
    uint32 SuiteCount = (uint32)ARRAY_COUNT(Suites);
    test_suite_result Results[ARRAY_COUNT(Suites)] = {};
    uint32 PassedSuites = 0;
    uint32 FailedSuites = 0;

    for (uint32 i = 0; i < SuiteCount; i++)
    {
        printf("\nRunning suite: %s...\n", Suites[i].Name);
        uint32 startTotal = GlobalTotalTests;
        uint32 startPassed = GlobalPassedTests;
        uint32 startFailed = GlobalFailedTests;

        int status = Suites[i].Func();

        Results[i].Name = Suites[i].Name;
        Results[i].TotalTests = GlobalTotalTests - startTotal;
        Results[i].PassedTests = GlobalPassedTests - startPassed;
        Results[i].FailedTests = GlobalFailedTests - startFailed;
        Results[i].Success = (status == 0 && Results[i].FailedTests == 0);

        if (Results[i].Success)
        {
            PassedSuites++;
        }
        else
        {
            FailedSuites++;
        }
    }

    real32 totalTestPassPct = GlobalTotalTests > 0 
        ? (((real32)GlobalPassedTests / (real32)GlobalTotalTests) * 100.0f) 
        : 100.0f;
    real32 totalSuitePassPct = SuiteCount > 0 
        ? (((real32)PassedSuites / (real32)SuiteCount) * 100.0f) 
        : 100.0f;

    printf("\n");
    printf("================================================================================\n");
    printf("                               TEST RUN SUMMARY                                 \n");
    printf("================================================================================\n");
    printf(" %-22s %10s %10s %10s %12s\n", "Suite / Module", "Passed", "Failed", "Total", "Pass %");
    printf("--------------------------------------------------------------------------------\n");

    for (uint32 i = 0; i < SuiteCount; i++)
    {
        real32 pct = Results[i].TotalTests > 0 
            ? (((real32)Results[i].PassedTests / (real32)Results[i].TotalTests) * 100.0f) 
            : 100.0f;
        printf(" %-22s %10u %10u %10u %11.2f%%\n",
               Results[i].Name,
               Results[i].PassedTests,
               Results[i].FailedTests,
               Results[i].TotalTests,
               (double)pct);
    }

    printf("--------------------------------------------------------------------------------\n");
    printf(" Tests Pass Percentage:                                             %10.2f%%\n", (double)totalTestPassPct);
    printf(" Total Tests:               %u Passed, %u Failed, %u Total\n",
           GlobalPassedTests, GlobalFailedTests, GlobalTotalTests);
    printf(" Suites Pass Percentage:                                            %10.2f%%\n", (double)totalSuitePassPct);
    printf(" Total Suites:              %u Passed, %u Failed, %u Total\n",
           PassedSuites, FailedSuites, SuiteCount);
    printf("================================================================================\n");

    if (FailedSuites == 0 && GlobalFailedTests == 0)
    {
        printf(" RESULT: ALL TESTS PASSED\n");
    }
    else
    {
        printf(" RESULT: %u TEST MODULE(S) FAILED (%u individual test(s) failed)\n",
               FailedSuites, GlobalFailedTests);
    }
    printf("================================================================================\n");

    return (FailedSuites > 0 || GlobalFailedTests > 0) ? 1 : 0;
}
