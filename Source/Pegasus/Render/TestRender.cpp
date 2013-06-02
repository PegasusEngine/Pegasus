#include "Pegasus/Render/TestRender.h"

namespace Pegasus {
namespace Render {

TestRender::TestRender(int value)
:   mValue(value)
{
}

TestRender::~TestRender()
{
}

int TestRender::GetValue() const
{
    return mValue;
}

}
}
