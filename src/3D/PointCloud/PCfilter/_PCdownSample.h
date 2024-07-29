/*
 * _PCdownSample.h
 *
 *  Created on: Feb 7, 2021
 *      Author: yankai
 */

#ifndef OpenKAI_src_3D_PointCloud_PCdownSample_H_
#define OpenKAI_src_3D_PointCloud_PCdownSample_H_

#include "../../_GeometryBase.h"

namespace kai
{

    class _PCdownSample : public _GeometryBase
    {
    public:
        _PCdownSample();
        virtual ~_PCdownSample();

        int init(void *pKiss);
        int start(void);
        int check(void);

    private:
        void update(void);
        void updateFilter(void);
        static void *getUpdate(void *This)
        {
            ((_PCdownSample *)This)->update();
            return NULL;
        }

    public:
        float m_rVoxel;
    };

}
#endif
