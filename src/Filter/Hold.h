/*
 * Hold.h
 *
 *  Created on: Mar 18, 2021
 *      Author: yankai
 */

#ifndef OpenKAI_src_Filter_Hold_H_
#define OpenKAI_src_Filter_Hold_H_

#include "FilterBase.h"

namespace kai
{

	template <class T>
	class Hold : public FilterBase<T>
	{
	public:
		Hold<T>()
		{
		}
		virtual ~Hold()
		{
		}

		bool init(float dThold)
		{
			m_dThold = dThold;
			reset();
			return true;
		}

		T *update(T *pV, float dT)
		{
			if (pV)
			{
				FilterBase<T>::m_v = *pV;
				m_dT = 0;
				return FilterBase<T>::m_pV;
			}

			m_dT += dT;
			IF_N(m_dT >= m_dThold);

			return FilterBase<T>::m_pV;
		}

		void reset(void)
		{
			FilterBase<T>::reset();
			m_dT = 0;
		}

	private:
		float m_dThold;
		float m_dT;
	};

}
#endif
