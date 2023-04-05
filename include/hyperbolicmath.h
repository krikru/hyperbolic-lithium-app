#pragma once


////////////////////////////////////////////////////////////////////////////////
// INCLUDES
////////////////////////////////////////////////////////////////////////////////


//#include <cmath>

#include "metamath.h"


////////////////////////////////////////////////////////////////////////////////
// CLASSES
////////////////////////////////////////////////////////////////////////////////


template <typename T, int D, T k>
class HyperboloidGyroVector
{
public:
    HyperboloidGyroVector()
    : _carrierVector()
    {}

    // Constructor for D = 3
    template<typename std::enable_if<D == 3, int>::type = 0>
    HyperboloidGyroVector(const T& x, const T& y, const T& z)
        : _carrierVector(x, y, z, T(1))
    {
        normalize();
    }

    // Constructor for D = 3
    template<typename std::enable_if<D == 3, int>::type = 0>
    HyperboloidGyroVector(const T& x, const T& y, const T& z, const T& t)
        : _carrierVector(x, y, z, t)
    {
    }

    HyperboloidGyroVector(glm::vec<D+1, T, glm::highp> carrierVector)
    : _carrierVector(carrierVector)
    {}

    void normalize()
    {
        _carrierVector *= _R / getProperTime();
    }

    static glm::mat<D+1, D+1, T, glm::highp> getTranslationMatrix(glm::vec<D, T, glm::highp> direction, T length)
    {
        static const auto timeDir = glm::vec<D+1, T, glm::highp>(glm::vec<D, T, glm::highp>(T(0)), T(1));
        static const auto timeOuter = glm::outerProduct(timeDir, timeDir);
        static const auto identity = glm::mat<D+1, D+1, T, glm::highp>(T(1));

        glm::vec<D+1, T, glm::highp> transDir{direction, T(0)};
        auto transOuter = glm::outerProduct(transDir, transDir);

        T sLength = length * _RInv;

        return (
            (timeOuter + transOuter) * (cosh(sLength) - 1)
            - (_RInv * glm::outerProduct(timeDir, transDir) + _R * glm::outerProduct(transDir, timeDir)) * sinh(sLength)
            + identity);
    }

private:
    T getSquaredProperTime() const
    {
        T sum = _carrierVector[D]*_R;
        sum = sum * sum;
        for (size_t idx = 0; idx < D; idx++) {
            sum -= _carrierVector[idx]*_carrierVector[idx];
        }
        return sum;
    }

    T getProperTime() const
    {
        return sqrt(getSquaredProperTime());
    }

    static const T _R;  // Can't be initialied here since it's not an int type
    static const T _RInv;  // Can't be initialied here since it's not an int type
    static const T _targetProperTime;  // Can't be initialied here since it's not an int type
    glm::vec<D+1, T, glm::highp> _carrierVector;
};
template <typename T, int D, T k>
const T HyperboloidGyroVector<T, D, k>::_R = metaSqrt(metaAbs(k));
template <typename T, int D, T k>
const T HyperboloidGyroVector<T, D, k>::_RInv = T(1) / HyperboloidGyroVector<T, D, k>::_R;

template <typename T, T k>
using HyperboloidGyroVector2D = HyperboloidGyroVector<T, 2, k>;

template <typename T, T k>
using HyperboloidGyroVector3D = HyperboloidGyroVector<T, 3, k>;
