#include "Pegasus/Core/TestCore.h"

namespace Pegasus {
namespace Core {

TestCore::TestCore(int value)
:   mValue(value)
{
}

TestCore::~TestCore()
{
}

int TestCore::GetValue() const
{
    return mValue;
}

}
}
