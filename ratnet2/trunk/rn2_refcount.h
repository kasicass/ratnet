#ifndef KCODE_RATNET2_REFCOUNT_H
#define KCODE_RATNET2_REFCOUNT_H

namespace RN2 {

class RefCount
{
public:
    RefCount();
    virtual ~RefCount();

    void AddRef();
    void ReleaseRef();

private:
    int cnt_;
};

}

#endif
