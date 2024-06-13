#ifndef OpenKAI_src_Utility_utilCV_H_
#define OpenKAI_src_Utility_utilCV_H_

#ifdef USE_OPENCV
#include "../Base/cv.h"
#include "../Base/platform.h"
#include "../Base/macro.h"
#include "../Base/constant.h"
#include "utilFile.h"

namespace kai
{

	inline bool readCamMatrices(const string &fCalib, Mat *pC, Mat *pD)
	{
		NULL_F(pC);
		NULL_F(pD);

		string s;
		if(!readFile(fCalib, &s))
		{
            return false;
		}

 		Kiss *pKf = new Kiss();
		if (!pKf->parse(s))
        {
            DEL(pKf);
            return false;
        }

		Kiss *pK = pKf->child("calib");
		IF_d__(pK->empty(), DEL(pKf), false);

		Mat mC = Mat::zeros(3, 3, CV_64FC1);
		pK->v("Fx", &mC.at<double>(0, 0));
		pK->v("Fy", &mC.at<double>(1, 1));
		pK->v("Cx", &mC.at<double>(0, 2));
		pK->v("Cy", &mC.at<double>(1, 2));
		mC.at<double>(2, 2) = 1.0;

		Mat mD = Mat::zeros(1, 5, CV_64FC1);
		pK->v("k1", &mD.at<double>(0, 0));
		pK->v("k2", &mD.at<double>(0, 1));
		pK->v("p1", &mD.at<double>(0, 2));
		pK->v("p2", &mD.at<double>(0, 3));
		pK->v("k3", &mD.at<double>(0, 4));

		*pC = mC;
		*pD = mD;

		DEL(pKf);
		return true;
	}

	inline bool scaleCamMatrices(const cv::Size &s, const Mat &mC, const Mat &mD, Mat *pCs)
	{
		NULL_F(pCs);
		IF_F(mC.empty() || mD.empty());

		Mat mCs;
		mC.copyTo(mCs);
		mCs.at<double>(0, 0) *= (double)s.width;  // Fx
		mCs.at<double>(1, 1) *= (double)s.height; // Fy
		mCs.at<double>(0, 2) *= (double)s.width;  // Cx
		mCs.at<double>(1, 2) *= (double)s.height; // Cy

		*pCs = getOptimalNewCameraMatrix(mCs, mD, s, 1, s, 0);
		return true;
	}

	inline float hist(Mat m, float rFrom, float rTo, int nLevel, float nMin)
	{
		IF__(m.empty(), -1.0);

		vector<int> vHistLev = {nLevel};
		vector<float> vRange = {rFrom, rTo};
		vector<int> vChannel = {0};

		vector<Mat> vRoi = {m};
		Mat mHist;
		cv::calcHist(vRoi, vChannel, Mat(),
					 mHist, vHistLev, vRange,
					 true // accumulate
		);

		int nMinHist = nMin * m.cols * m.rows;
		int i;
		for (i = 0; i < nLevel; i++)
		{
			if (mHist.at<float>(i) >= (float)nMinHist)
				break;
		}

		return rFrom + (((float)i) / (float)nLevel) * (rTo - rFrom);
	}

	template <typename T>
	inline T rect2BB(cv::Rect r)
	{
		T v;
		v.x = r.x;
		v.y = r.y;
		v.z = r.x + r.width;
		v.w = r.y + r.height;

		return v;
	}

	template <typename T>
	inline cv::Rect bb2Rect(T v)
	{
		cv::Rect r;
		r.x = v.x;
		r.y = v.y;
		r.width = v.z - v.x;
		r.height = v.w - v.y;

		return r;
	}

	inline float IoU(Rect2f &r1, Rect2f &r2)
	{
		Rect2f rOR = r1 | r2;
		Rect2f rAND = r1 & r2;
		return rAND.area() / rOR.area();
	}

	inline float IoU(vFloat4 &bb1, vFloat4 &bb2)
	{
		Rect2f r1 = bb2Rect(bb1);
		Rect2f r2 = bb2Rect(bb2);
		return IoU(r1, r2);
	}

	inline float nIoU(Rect2f &r1, Rect2f &r2)
	{
		Rect2f rAND = r1 & r2;
		return rAND.area() / small(r1.area(), r2.area());
	}

	inline float nIoU(vFloat4 &bb1, vFloat4 &bb2)
	{
		Rect2f r1 = bb2Rect(bb1);
		Rect2f r2 = bb2Rect(bb2);
		return nIoU(r1, r2);
	}

}
#endif
#endif
