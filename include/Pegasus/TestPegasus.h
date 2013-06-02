#ifndef PEGASUS_TESTPEGASUS_H
#define PEGASUS_TESTPEGASUS_H

namespace Pegasus {

class TestPegasus
{
public:
    TestPegasus(int value);
    ~TestPegasus();

    int GetValue() const;

private:

    int mValue;
};


}

#endif  // PEGASUS_TESTPEGASUS_H