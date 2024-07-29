/*
 * _Invert.h
 *
 *  Created on: March 14, 2019
 *      Author: yankai
 */

#ifndef OpenKAI_src_Vision__Invert_H_
#define OpenKAI_src_Vision__Invert_H_

#include "../_VisionBase.h"

namespace kai
{

	class _Invert : public _VisionBase
	{
	public:
		_Invert();
		virtual ~_Invert();

		int init(void *pKiss);
		int start(void);
		bool open(void);
		void close(void);

	private:
		void filter(void);
		void update(void);
		static void *getUpdate(void *This)
		{
			((_Invert *)This)->update();
			return NULL;
		}

	public:
		_VisionBase *m_pV;
	};

}
#endif
