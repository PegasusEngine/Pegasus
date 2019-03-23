#pragma once

namespace Pegasus
{
namespace Tests
{
class TestHarness;

struct TestContext
{
    int successCount = 0;
    int totals = 0;
};

typedef bool(*TestFuncFn)(TestHarness* harness);

struct TestFunc
{
    TestFuncFn fn;
    const char* name;
};

#define DECLARE_TEST(TEST_FUNC_SYMBOL) TestFunc{ run##TEST_FUNC_SYMBOL, #TEST_FUNC_SYMBOL }

class TestHarness
{
public:
    virtual void Initialize() = 0;
    virtual void Destroy() = 0;
    virtual const TestFunc* GetTests(int& testCounts) const = 0;
};



}
}
