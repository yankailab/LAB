/*
 * _Livox2.h
 *
 *  Created on: Jan 19, 2024
 *      Author: yankai
 */

#ifndef OpenKAI_src_Sensor_LiDAR__Livox2_H_
#define OpenKAI_src_Sensor_LiDAR__Livox2_H_

#include "../../../3D/PointCloud/_PCstream.h"
#include "../../../Dependencies/SensorFusion/SensorFusion.h"
#include "LivoxLidar2.h"

namespace kai
{
    struct Livox2Ctrl
    {
        vFloat2 m_vRz = {0.0, 500.0}; // z region
    };

    class _Livox2 : public _PCstream
    {
    public:
        _Livox2();
        ~_Livox2();

        virtual bool init(void *pKiss);
		virtual bool link(void);
        virtual int check(void);
        virtual bool start(void);
        virtual void console(void *pConsole);

        virtual void startStream(void);
        virtual void stopStream(void);

        void setLidarMode(LivoxLidarWorkMode m);


        // Callbacks
        static void sCbPointCloud(LivoxLidarEthernetPacket *data, void *client_data)
        {
            NULL_(client_data);
            ((_Livox2 *)client_data)->CbPointCloud(data);
        }

        static void sCbIMU(LivoxLidarEthernetPacket *data, void *client_data)
        {
            NULL_(client_data);
            ((_Livox2 *)client_data)->CbIMU(data);
        }

        static void sCbWorkMode(livox_status status, LivoxLidarAsyncControlResponse *response, void *client_data)
        {
            NULL_(client_data);
            ((_Livox2 *)client_data)->CbWorkMode(status, response);
        }

        static void sCbLidarInfoChange(const LivoxLidarInfo *info, void *client_data)
        {
            NULL_(client_data);
            ((_Livox2 *)client_data)->CbLidarInfoChange(info);
        }

    protected:
        void CbPointCloud(LivoxLidarEthernetPacket *pD);
        void CbIMU(LivoxLidarEthernetPacket *pD);
        void CbWorkMode(livox_status status, LivoxLidarAsyncControlResponse *pR);
        void CbLidarInfoChange(const LivoxLidarInfo *pI);

        bool open(void);
        void close(void);

    private:
        void updateLidar(void);
        void update(void);
        static void *getUpdate(void *This)
        {
            ((_Livox2 *)This)->update();
            return NULL;
        }

    protected:
        LivoxLidar2* m_pLv;
        string m_SN;
        uint32_t m_handle;
        bool m_bOpen;
        LivoxLidarWorkMode m_workMode;

        SF m_SF;
        uint64_t m_tIMU;
        bool m_bEnableIMU;

    };

}
#endif
