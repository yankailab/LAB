/*
 * _Mask.h
 *
 *  Created on: July 2, 2020
 *      Author: yankai
 */

#ifndef OpenKAI_src_Vision__Mask_H_
#define OpenKAI_src_Vision__Mask_H_

#include "../_VisionBase.h"

namespace kai
{

	class _Mask : public _VisionBase
	{
	public:
		_Mask();
		virtual ~_Mask();

		int init(void *pKiss);
		int link(void);
		int start(void);
		bool open(void);
		void close(void);

	private:
		void filter(void);
		void update(void);
		static void *getUpdate(void *This)
		{
			((_Mask *)This)->update();
			return NULL;
		}

	public:
		_VisionBase *m_pV;
		_VisionBase *m_pVmask;

		Frame m_fIn;
		Frame m_fMask;
	};

}
#endif
