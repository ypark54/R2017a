#ifndef _nesl_rtw_lti_h
#define _nesl_rtw_lti_h
#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */
int_T pm_create_sparsity_pattern_fields(PmSparsityPattern *patternPtr, size_t
nNzMax, size_t nRow, size_t nCol, PmAllocator *allocator); PmSparsityPattern
*pm_create_sparsity_pattern(size_t nNzMax, size_t nRow, size_t nCol, PmAllocator
*allocatorPtr); void pm_sp_equals_sp(PmSparsityPattern *Ap, const
PmSparsityPattern *Bp); boolean_T pm_sp_equalequals_sp(const PmSparsityPattern
*Ap, const PmSparsityPattern *Bp); PmSparsityPattern
*pm_copy_sparsity_pattern(const PmSparsityPattern *input_pattern, PmAllocator
*allocatorPtr); void pm_destroy_sparsity_pattern_fields(PmSparsityPattern
*patternPtr, PmAllocator *allocator); void
pm_destroy_sparsity_pattern(PmSparsityPattern *patternPtr, PmAllocator
*allocatorPtr); PmSparsityPattern *pm_create_full_sparsity_pattern(size_t nrows,
size_t ncols, PmAllocator *allocatorPtr); PmSparsityPattern
*pm_create_empty_sparsity_pattern(size_t nrows, size_t ncols, PmAllocator
*allocatorPtr); PmSparsityPattern
*pm_create_partial_identity_sparsity_pattern(size_t p, size_t n, PmAllocator
*allocatorPtr); PmSparsityPattern *pm_create_identity_sparsity_pattern(size_t n,
PmAllocator *allocatorPtr); typedef enum McLinearAlgebraStatusTag { MC_LA_INVALID
= -1, MC_LA_ERROR, MC_LA_OK }McLinearAlgebraStatus; typedef struct
McLinearAlgebraDataTag McLinearAlgebraData; typedef McLinearAlgebraData*
(*McLinearAlgebraConstructor) (PmAllocator*, const PmSparsityPattern*); typedef
McLinearAlgebraStatus (*McLinearAlgebraSymbolic) (McLinearAlgebraData*); typedef
McLinearAlgebraStatus (*McLinearAlgebraNumeric) (McLinearAlgebraData*, const
real_T*); typedef McLinearAlgebraStatus (*McLinearAlgebraSolve)
(McLinearAlgebraData*, const real_T*, real_T*, const real_T*); typedef void
(*McLinearAlgebraNumericDestroy) (McLinearAlgebraData*); typedef void
(*McLinearAlgebraSymbolicDestroy) (McLinearAlgebraData*); typedef void
(*McLinearAlgebraDestructor) (PmAllocator*, McLinearAlgebraData*); struct
McLinearAlgebraTag{ McLinearAlgebraConstructor mConstructor;
McLinearAlgebraSymbolic mSymbolic; McLinearAlgebraNumeric mNumeric;
McLinearAlgebraSolve mSolve; McLinearAlgebraNumericDestroy mNumericDestroy;
McLinearAlgebraSymbolicDestroy mSymbolicDestroy; McLinearAlgebraDestructor
mDestructor; }; const McLinearAlgebra *mc_get_csparse_rect_la(void); real_T
mc_csparse_rect_la_cond(const McLinearAlgebraData* mcLaData);
#ifdef  __cplusplus
}
#endif /* __cplusplus */
#endif /* _nesl_rtw_lti_h */
