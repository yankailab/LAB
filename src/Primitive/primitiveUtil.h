#ifndef OpenKAI_src_Primitive_primitiveUtil_H_
#define OpenKAI_src_Primitive_primitiveUtil_H_

#include <Eigen/Dense>
#include "vFloat3.h"

using namespace Eigen;

namespace kai
{

    inline Vector3f v2e(const vFloat3 &v)
    {
        return Vector3f(v.x, v.y, v.z);
    }

    inline Vector3d v2e(const vDouble3 &v)
    {
        return Vector3d(v.x, v.y, v.z);
    }


    inline vFloat3 e2v(const Vector3f &e)
    {
        return vFloat3(e[0], e[1], e[2]);
    }

    inline vDouble3 e2v(const Vector3d &e)
    {
        return vDouble3(e[0], e[1], e[2]);
    }

}
#endif
