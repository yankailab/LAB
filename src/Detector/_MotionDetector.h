/*
 * _MotionDetector.h
 *
 *  Created on: Aug 4, 2018
 *      Author: yankai
 */

#ifndef OpenKAI_src_Detector__MotionDetector_H_
#define OpenKAI_src_Detector__MotionDetector_H_

#include "_DetectorBase.h"

namespace kai
{

	class _MotionDetector : public _DetectorBase
	{
	public:
		_MotionDetector();
		virtual ~_MotionDetector();

		int init(void *pKiss);
		int start(void);
		void draw(void *pFrame);
		int check(void);

	private:
		void detect(void);
		void update(void);
		static void *getUpdate(void *This)
		{
			((_MotionDetector *)This)->update();
			return NULL;
		}

	public:
		_VisionBase *m_pVision;
		string m_algorithm;
		cv::Ptr<cv::BackgroundSubtractor> m_pBS;
		double m_learningRate;
		Mat m_mFG;
	};

}
#endif
