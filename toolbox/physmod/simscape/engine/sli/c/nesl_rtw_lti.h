/******************************************************************************
 ** File: nesl_rtw_lti_copied.h
 ** Abstract:
 **    Define rectangular solver function for Lti systems.
 **
 ** Copyright 2015-2016 The MathWorks, Inc.
 ******************************************************************************/

#ifndef __nesl_rtw_lti_copied_h
#define __nesl_rtw_lti_copied_h

#include "ne_std.h"
#include "pm_default_allocator.h"
#include "_nesl_rtw_lti.h"

static bool cgSolve(const int32_T m,
                    const int32_T n,
                    int32_T *matIr,
                    int32_T *matJc,
                    const real_T *matPr,
                    const int32_T nnz,
                    real_T *xPr,
                    real_T *rhsPr)
{
    const McLinearAlgebra *la = mc_get_csparse_rect_la();
    PmAllocator *allocator = pm_default_allocator();
    PmSparsityPattern pattern;
    pattern.mJc = matJc;
    pattern.mIr = matIr;
    pattern.mNumRow = m;
    pattern.mNumCol = n;

    McLinearAlgebraData *data = la->mConstructor(allocator, &pattern);
    McLinearAlgebraStatus status = la->mSymbolic(data);
    if (status != MC_LA_OK) {
        return false;
    }
    status = la->mNumeric(data, matPr);
    if (status != MC_LA_OK) {
        la->mSymbolicDestroy(data);
        return false;
    }

    status = la->mSolve(data,
                        matPr,
                        xPr,
                        rhsPr);

    la->mNumericDestroy(data);
    la->mSymbolicDestroy(data);
    la->mDestructor(allocator, data);

    return status == MC_LA_OK;
}

#endif
