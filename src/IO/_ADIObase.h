/*
 * _ADIObase.h
 *
 *  Created on: Jan 6, 2024
 *      Author: yankai
 */

#ifndef OpenKAI_src_IO_ADIObase_H_
#define OpenKAI_src_IO_ADIObase_H_

#include "../Base/_ModuleBase.h"
#include "../UI/_Console.h"

#define ADIO_MAX_PORT 128

namespace kai
{

	enum ADIO_STATUS
	{
		adio_unknown,
		adio_closed,
		adio_opened
	};

	struct ADIO_PORT
	{
		bool m_bDigital;

		float m_vW; // value to be written
		float m_vR; // value read

		void clear(void)
		{
			m_bDigital = true;
			m_vW = 0;
			m_vR = 0;
		}

		bool writeA(float v)
		{
			IF_F(m_bDigital);

			m_vW = v;
			return true;
		}

		bool writeD(bool v)
		{
			m_vW = (v > 0)?1:0;
			return true;
		}

		float readA(void)
		{
			return m_vR;
		}

		bool readD(void)
		{
			return (m_vR > 0);
		}
	};

	class _ADIObase : public _ModuleBase
	{
	public:
		_ADIObase();
		virtual ~_ADIObase();

		virtual bool init(void *pKiss);
		virtual bool link(void);
		virtual void console(void *pConsole);

		virtual bool open(void);
		virtual bool bOpen(void);
		virtual void close(void);

		virtual bool writeD(int iPort, bool b);
		virtual bool readD(int iPort);
		virtual bool writeA(int iPort, float v);
		virtual float readA(int iPort);

	protected:
		virtual void updateW(void);
		virtual void updateR(void);

	protected:
		ADIO_STATUS m_status;
		int m_nPort;
		vector<ADIO_PORT> m_vPort;



	};

}
#endif
