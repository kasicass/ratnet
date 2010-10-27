#include "rn2_refcount.h"

namespace RN2 {

RefCount::RefCount() : cnt_(0)
{
}

RefCount::~RefCount()
{
}

void RefCount::AddRef()
{
    cnt_++;
}

void RefCount::ReleaseRef()
{
    cnt_--;
    if ( cnt_ < 1 )
        delete this;
}

}

