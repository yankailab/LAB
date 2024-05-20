/*
 * PCstream.h
 *
 *  Created on: May 24, 2020
 *      Author: yankai
 */

#ifndef OpenKAI_src_3D_PointCloud__PCstream_H_
#define OpenKAI_src_3D_PointCloud__PCstream_H_

#include "../_GeometryBase.h"

namespace kai
{
    inline int iInc(int i, int n)
    {
        i++;
        if (i >= n)
            i = 0;

        return i;
    }

    class _PCstream : public _GeometryBase
    {
    public:
        _PCstream();
        virtual ~_PCstream();

        // BASE
        virtual bool init(void *pKiss);
        virtual bool start(void);
        virtual int check(void);

        // _GeometryBase
		virtual bool initBuffer(void);
        virtual void clear(void);
        virtual void getPCstream(void* p, const uint64_t& tExpire);

        virtual void writeSharedMem(void);
        virtual void readSharedMem(void);

        // _PCstream
   		virtual void copyTo(PointCloud *pPC, const uint64_t& tExpire);
        virtual void add(const Vector3d &vP, const Vector3f &vC, const uint64_t& tStamp);
        virtual GEOMETRY_POINT* get(int i);
        virtual int nP(void);
        virtual int iP(void);

    private:
        void updatePCstream(void);
        void update(void);
        static void *getUpdate(void *This)
        {
            ((_PCstream *)This)->update();
            return NULL;
        }

    protected:
        //ring buf
        GEOMETRY_POINT *m_pP;
        int m_nP;
        int m_iP;
    };

}
#endif
