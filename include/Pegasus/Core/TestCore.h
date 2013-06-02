#ifndef PEGASUS_CORE_TESTCORE_H
#define PEGASUS_CORE_TESTCORE_H

namespace Pegasus {
namespace Core {

class TestCore
{
public:
    TestCore(int value);
    ~TestCore();

    int GetValue() const;

private:

    int mValue;
};


}
}

#endif  // PEGASUS_CORE_TESTCORE_H