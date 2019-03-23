#include "Pegasus/UnitTests/RenderTests.h"
#include "Pegasus/UnitTests/UnitTests.h"

namespace Pegasus
{
namespace Tests
{

class RenderHarness : public TestHarness
{
public:
    virtual void Initialize() {}
    virtual void Destroy() {}
    virtual const TestFunc* GetTests(int& funcSz) const;
};

const TestFunc* RenderHarness::GetTests(int& funcSz) const
{
    funcSz = 0;
    return nullptr;
}

TestHarness* createRenderHarness()
{
    return new RenderHarness();
}
}
}
