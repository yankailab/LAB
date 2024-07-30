/*
 * _Livox.h
 *
 *  Created on: Aug 21, 2019
 *      Author: yankai
 */

#ifndef OpenKAI_src_Sensor_LiDAR__Livox_H_
#define OpenKAI_src_Sensor_LiDAR__Livox_H_

#include "../../../3D/PointCloud/_PCstream.h"
#include "LivoxLidar.h"

namespace kai
{
	struct LivoxCtrl
	{
		vFloat2 m_vRz = {0.0, 500.0};		 // z region
	};

    class _Livox : public _PCstream
    {
    public:
        _Livox();
        ~_Livox();

        virtual int init(void *pKiss);
        virtual int check(void);
        virtual int start(void);

        virtual void startStream(void);
        virtual void stopStream(void);

        void setLidarMode(LidarMode m);
        void setScanPattern(LidarScanPattern p);

    protected:
        static void CbRecvData(LivoxEthPacket *pData, void *pLivox);

        void addP(LivoxRawPoint *pLp, uint64_t &tStamp);
        void addP(LivoxExtendRawPoint *pLp, uint64_t &tStamp);
        void addDualP(LivoxDualExtendRawPoint *pLp, uint64_t &tStamp);
        void addTripleP(LivoxTripleExtendRawPoint *pLp, uint64_t &tStamp);
        void updateIMU(LivoxImuPoint *pLd);

        int open(void);
        void close(void);
        bool updateLidar(void);
        void update(void);
        static void *getUpdate(void *This)
        {
            ((_Livox *)This)->update();
            return NULL;
        }

    public:
        bool m_bOpen;
        string m_broadcastCode;
        LivoxLidar *m_pL;
        uint32_t m_iTransformed;
        const float m_ovRef = 1.0 / 255.0;
        int m_lidarMode;
        int m_scanPattern;
    };

}
#endif
