/*
 * _SLAMbase.h
 *
 *  Created on: Nov 12, 2024
 *      Author: yankai
 */

#ifndef OpenKAI_src_SLAM__SLAMbase_H_
#define OpenKAI_src_SLAM__SLAMbase_H_

#include "../Vision/_VisionBase.h"
#include "../Navigation/_NavBase.h"

namespace kai
{
	class _SLAMbase : public _NavBase
	{
	public:
		_SLAMbase();
		virtual ~_SLAMbase();

		virtual int init(void *pKiss);
		virtual int link(void);
		virtual int start(void);
		virtual int check(void);
		virtual void console(void *pConsole);

		bool bTracking(void);

	private:
		void detect(void);
		void update(void);
		static void *getUpdate(void *This)
		{
			((_SLAMbase *)This)->update();
			return NULL;
		}

	protected:
		_VisionBase *m_pV;
		vInt2 m_vSize;
		bool m_bTracking;

	};

}
#endif
