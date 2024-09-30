/*
 * Predict.h
 *
 *  Created on: Mar 18, 2021
 *      Author: yankai
 */

#ifndef OpenKAI_src_Filter_Predict_H_
#define OpenKAI_src_Filter_Predict_H_

#include "FilterBase.h"

namespace kai
{

	template <class T>
	class Predict : public FilterBase<T>
	{
	public:
		Predict<T>()
		{
			FilterBase<T>::m_nW = 2;
		}
		virtual ~Predict()
		{
		}

		bool init(int nW)
		{
			IF_F(nW < 2);

			FilterBase<T>::m_nW = nW;
			FilterBase<T>::reset();
			return true;
		}

		T update(T v, float dT)
		{
			FilterBase<T>::add(v);

			if (FilterBase<T>::m_qV.size() < FilterBase<T>::m_nW)
			{
				FilterBase<T>::m_v = v;
				return FilterBase<T>::m_v;
			}

			int s = FilterBase<T>::m_qV.size();
			T p = FilterBase<T>::m_qV.at(s - 2);
			T q = FilterBase<T>::m_qV.at(s - 1);
				
			FilterBase<T>::m_v = q + (q - p) * (T)dT;
			return FilterBase<T>::m_v;
		}
	};

}
#endif
