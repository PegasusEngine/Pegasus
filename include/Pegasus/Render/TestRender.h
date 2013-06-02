#ifndef PEGASUS_CORE_TESTRENDER_H
#define PEGASUS_CORE_TESTRENDER_H

namespace Pegasus {
namespace Render {

class TestRender
{
public:
    TestRender(int value);
    ~TestRender();

    int GetValue() const;

private:

    int mValue;
};


}
}

#endif  // PEGASUS_CORE_TESTRENDER_H