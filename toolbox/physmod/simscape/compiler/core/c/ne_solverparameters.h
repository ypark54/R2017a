#ifndef ne_solverparameters_h
#define ne_solverparameters_h
#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */
typedef struct NeSolverParametersTag NeSolverParameters; typedef enum
NeLocalSolverChoiceTag { NE_INVALID_ADVANCER_CHOICE = -1,
NE_BACKWARD_EULER_ADVANCER, NE_TRAPEZOIDAL_ADVANCER, NE_NDF2_ADVANCER,
NE_VARIABLE_STEP_ADVANCER, NE_NUM_ADVANCER_CHOICES } NeLocalSolverChoice; typedef
enum NeLinearAlgebraChoiceTag { NE_INVALID_LA_CHOICE = -1, NE_SPARSE_LA,
NE_FULL_LA, NE_NUM_LA_CHOICES } NeLinearAlgebraChoice; struct
NeSolverParametersTag { boolean_T mProfile; boolean_T mUseLocalSampling;
boolean_T mEnableSwitchedLinearOptims; boolean_T mFrequencyDomain; boolean_T
mUseCCode; real_T mRelTol; real_T mAbsTol; real_T mMinStep; boolean_T
mToWorkspace; boolean_T mRevertToSquareIcSolve; size_t mNumHomotopyIterations;
boolean_T mPhasorMode; size_t mPhasorModeNumHarmonics; boolean_T mOverrideAbsTol;
boolean_T mDoDC; real_T mResidualTolerance; boolean_T mUseLocalSolver;
NeLocalSolverChoice mLocalSolverChoice; real_T mLocalSolverSampleTime; boolean_T
mDoFixedCost; size_t mMaxNonlinIter; size_t mMaxModeIter; NeLinearAlgebraChoice
mLinearAlgebra; size_t mDelaysMemoryBudget; boolean_T mAutomaticFiltering; real_T
mFilteringTimeConstant; };
#ifdef  __cplusplus
}
#endif /* __cplusplus */
#endif /* ne_solverparameters_h */
