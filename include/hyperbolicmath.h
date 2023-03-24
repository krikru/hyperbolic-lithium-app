#pragma once


////////////////////////////////////////////////////////////////////////////////
// INCLUDES
////////////////////////////////////////////////////////////////////////////////


#include "metamath.h"


////////////////////////////////////////////////////////////////////////////////
// CLASSES
////////////////////////////////////////////////////////////////////////////////


template <typename T, int D, T k>
class PoincareBallGyroVector
{
public:
    template<bool CHECK_VALIDITY=true>
    PoincareBallGyroVector(const T carrierVector[])
    : _carrierVector{carrierVector}
    {
        if (CHECK_VALIDITY) {
            assert(-k * getCarrierVectorSquareLength() < 1);
        }
    }

    T getCarrierVectorSquareLength()
    {
        T sum = 0;
        for (int idx = 0; idx < D; idx++) {
            sum += _carrierVector[idx]*_carrierVector[idx];
        }
        return sum;
    }

    T _carrierVector[D];
};


template <typename T, T k>
using PoincareBallGyroVector2D = PoincareBallGyroVector<T, 2, k>;

template <typename T, T k>
using PoincareBallGyroVector3D = PoincareBallGyroVector<T, 3, k>;

