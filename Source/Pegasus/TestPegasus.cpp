#include "Pegasus/TestPegasus.h"

namespace Pegasus {

TestPegasus::TestPegasus(int value)
:   mValue(value)
{
}

TestPegasus::~TestPegasus()
{
}

int TestPegasus::GetValue() const
{
    return mValue;
}

}
