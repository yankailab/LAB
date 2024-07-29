/*
 * _ActuatorBase.h
 *
 *  Created on: May 16, 2019
 *      Author: yankai
 */

#ifndef OpenKAI_src_Actuator__ActuatorBase_H_
#define OpenKAI_src_Actuator__ActuatorBase_H_

#include "../Base/_ModuleBase.h"
#include "../UI/_Console.h"
#include "../Utility/BitFlag.h"

namespace kai
{

#define ACT_BF_STOP 0

	struct ACTUATOR_AXIS_PARAM
	{
		float m_v;
		float m_vTarget;
		float m_vErr;
		vFloat2 m_vRange;

		void init(void)
		{
			m_v = 0.0;
			m_vTarget = 0.0;
			m_vErr = 0.0;
			m_vRange.set(-FLT_MAX, FLT_MAX);
		}

		bool bComplete(void)
		{
			return EQUAL(m_v, m_vTarget, m_vErr);
		}

		bool bInRange(float v)
		{
			IF_F(v < m_vRange.x);
			IF_F(v > m_vRange.y);

			return true;
		}

		void setTarget(float v)
		{
			m_vTarget = m_vRange.constrain(v);
		}

		void setNormalizedTarget(float v)
		{
			m_vTarget = m_vRange.x + constrain<float>(v, 0, 1) * m_vRange.d();
		}
	};

	struct ACTUATOR_AXIS
	{
		string m_name;

		float m_pOrigin;
		ACTUATOR_AXIS_PARAM m_p; // pos
		ACTUATOR_AXIS_PARAM m_s; // speed
		ACTUATOR_AXIS_PARAM m_a; // accel
		ACTUATOR_AXIS_PARAM m_b; // brake
		ACTUATOR_AXIS_PARAM m_c; // current

		void init(void)
		{
			m_name = "";

			m_pOrigin = 0.0;
			m_p.init();
			m_s.init();
			m_a.init();
			m_b.init();
			m_c.init();
		}

		void gotoOrigin(void)
		{
			m_p.setTarget(m_pOrigin);
		}
	};

	enum ACTUATOR_CMD_TYPE
	{
		actCmd_standby,
		actCmd_pos,
		actCmd_spd,
		actCmd_accel,
		actCmd_brake,
	};

	class _ActuatorBase : public _ModuleBase
	{
	public:
		_ActuatorBase();
		~_ActuatorBase();

		virtual int init(void *pKiss);
		virtual int link(void);
		virtual int start(void);
		virtual void console(void *pConsole);

		virtual void atomicFrom(void);
		virtual void atomicTo(void);
		virtual void setPtarget(int i, float p, bool bNormalized = false);
		virtual void setStarget(int i, float s, bool bNormalized = false);
		virtual void setAtarget(int i, float a, bool bNormalized = false);
		virtual void setBtarget(int i, float a, bool bNormalized = false);
		virtual void gotoOrigin(void);
		virtual bool bComplete(void);
		virtual bool bComplete(int i);

		virtual float getP(int i);
		virtual float getS(int i);
		virtual float getPtarget(int i);
		virtual float getStarget(int i);

		virtual bool power(bool bON);
		virtual void setStop(void);

	protected:
		virtual bool bCmdTimeout(void);
		virtual bool open(void);
		virtual void update(void);
		static void *getUpdate(void *This)
		{
			((_ActuatorBase *)This)->update();
			return NULL;
		}

	protected:
		vector<ACTUATOR_AXIS> m_vAxis;
		int m_nMinAxis;

		bool m_bPower;
		bool m_bReady;
		bool m_bFeedback;
		bool m_bMoving;

		pthread_mutex_t m_mutex;

		ACTUATOR_CMD_TYPE m_lastCmdType;
		uint64_t m_tLastCmd;
		uint64_t m_tCmdTimeout;

		_ActuatorBase *m_pParent;

		BIT_FLAG m_bf;
	};

}
#endif
