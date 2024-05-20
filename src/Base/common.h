#ifndef OpenKAI_src_Base_common_H_
#define OpenKAI_src_Base_common_H_

#ifdef USE_OPEN3D
#include "open3d.h"
#endif

#ifdef USE_OPENCV
#include "cv.h"
#endif

#ifdef USE_MATHGL
#include <mgl2/mgl.h>
#include <mgl2/fltk.h>
#endif

#ifdef USE_GLOG
#include <glog/logging.h>
#endif

#include <Eigen/Dense>

#include "platform.h"
#include "macro.h"
#include "constant.h"
#include "../Primitive/vInt2.h"
#include "../Primitive/vInt3.h"
#include "../Primitive/vInt4.h"
#include "../Primitive/vFloat2.h"
#include "../Primitive/vFloat3.h"
#include "../Primitive/vFloat4.h"
#include "../Primitive/vDouble2.h"
#include "../Primitive/vDouble3.h"
#include "../Primitive/vDouble4.h"
#include "../Primitive/primitiveUtil.h"
#include "../Primitive/tSwap.h"
#include "../Utility/util.h"
#include "../Utility/utilTime.h"
#include "../Utility/utilEvent.h"
#include "../Utility/utilStr.h"

#endif

