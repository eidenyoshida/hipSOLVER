/* ************************************************************************
 * Copyright 2020-2021 Advanced Micro Devices, Inc.
 * ************************************************************************ */

//! HIP = Heterogeneous-compute Interface for Portability
//!
//! Define a extremely thin runtime layer that allows source code to be compiled unmodified
//! through either AMD HCC or NVCC.   Key features tend to be in the spirit
//! and terminology of CUDA, but with a portable path to other accelerators as well.
//!
//! This is the master include file for hipsolver, wrapping around rocsolver and cusolver
//
#ifndef HIPSOLVER_H
#define HIPSOLVER_H

#include "hipsolver-export.h"
#include "hipsolver-version.h"
#include <hip/hip_runtime_api.h>
#include <stdint.h>

/* Workaround clang bug:

   https://bugs.llvm.org/show_bug.cgi?id=35863

   This macro expands to static if clang is used; otherwise it expands empty.
   It is intended to be used in variable template specializations, where clang
   requires static in order for the specializations to have internal linkage,
   while technically, storage class specifiers besides thread_local are not
   allowed in template specializations, and static in the primary template
   definition should imply internal linkage for all specializations.

   If clang shows an error for improperly using a storage class specifier in
   a specialization, then HIPSOLVER_CLANG_STATIC should be redefined as empty,
   and perhaps removed entirely, if the above bug has been fixed.
*/
#if __clang__
#define HIPSOLVER_CLANG_STATIC static
#else
#define HIPSOLVER_CLANG_STATIC
#endif

typedef void* hipsolverHandle_t;

typedef struct hipsolverComplex
{
#ifndef __cplusplus

    float x, y;

#else

private:
    float x, y;

public:
#if __cplusplus >= 201103L
    hipsolverComplex() = default;
#else
    hipsolverComplex() {}
#endif

    hipsolverComplex(float r, float i = 0)
        : x(r)
        , y(i)
    {
    }

    float real() const
    {
        return x;
    }
    float imag() const
    {
        return y;
    }
    void real(float r)
    {
        x = r;
    }
    void imag(float i)
    {
        y = i;
    }

#endif
} hipsolverComplex;

typedef struct hipsolverDoubleComplex
{
#ifndef __cplusplus

    double x, y;

#else

private:
    double x, y;

public:

#if __cplusplus >= 201103L
    hipsolverDoubleComplex() = default;
#else
    hipsolverDoubleComplex() {}
#endif

    hipsolverDoubleComplex(double r, double i = 0)
        : x(r)
        , y(i)
    {
    }

    double real() const
    {
        return x;
    }
    double imag() const
    {
        return y;
    }
    void real(double r)
    {
        x = r;
    }
    void imag(double i)
    {
        y = i;
    }

#endif
} hipsolverDoubleComplex;

#if __cplusplus >= 201103L
#include <type_traits>
static_assert(std::is_standard_layout<hipsolverComplex>{},
              "hipsolverComplex is not a standard layout type, and thus is incompatible with C.");
static_assert(
    std::is_standard_layout<hipsolverDoubleComplex>{},
    "hipsolverDoubleComplex is not a standard layout type, and thus is incompatible with C.");
static_assert(std::is_trivial<hipsolverComplex>{},
              "hipsolverComplex is not a trivial type, and thus is incompatible with C.");
static_assert(std::is_trivial<hipsolverDoubleComplex>{},
              "hipsolverDoubleComplex is not a trivial type, and thus is incompatible with C.");
static_assert(sizeof(hipsolverComplex) == sizeof(float) * 2
                  && sizeof(hipsolverDoubleComplex) == sizeof(double) * 2
                  && sizeof(hipsolverDoubleComplex) == sizeof(hipsolverComplex) * 2,
              "Sizes of hipsolverComplex or hipsolverDoubleComplex are inconsistent");
#endif

typedef enum
{
    HIPSOLVER_STATUS_SUCCESS           = 0, // Function succeeds
    HIPSOLVER_STATUS_NOT_INITIALIZED   = 1, // hipSOLVER library not initialized
    HIPSOLVER_STATUS_ALLOC_FAILED      = 2, // resource allocation failed
    HIPSOLVER_STATUS_INVALID_VALUE     = 3, // unsupported numerical value was passed to function
    HIPSOLVER_STATUS_MAPPING_ERROR     = 4, // access to GPU memory space failed
    HIPSOLVER_STATUS_EXECUTION_FAILED  = 5, // GPU program failed to execute
    HIPSOLVER_STATUS_INTERNAL_ERROR    = 6, // an internal hipSOLVER operation failed
    HIPSOLVER_STATUS_NOT_SUPPORTED     = 7, // function not implemented
    HIPSOLVER_STATUS_ARCH_MISMATCH     = 8,
    HIPSOLVER_STATUS_HANDLE_IS_NULLPTR = 9, // hipSOLVER handle is null pointer
    HIPSOLVER_STATUS_INVALID_ENUM      = 10, // unsupported enum value was passed to function
    HIPSOLVER_STATUS_UNKNOWN           = 11, // back-end returned an unsupported status code
} hipsolverStatus_t;

// set the values of enum constants to be the same as those used in cblas
typedef enum
{
    HIPSOLVER_OP_N = 111,
    HIPSOLVER_OP_T = 112,
    HIPSOLVER_OP_C = 113,
} hipsolverOperation_t;

typedef enum
{
    HIPSOLVER_FILL_MODE_UPPER = 121,
    HIPSOLVER_FILL_MODE_LOWER = 122,
} hipsolverFillMode_t;

typedef enum
{
    HIPSOLVER_SIDE_LEFT  = 141,
    HIPSOLVER_SIDE_RIGHT = 142,
} hipsolverSideMode_t;

typedef enum
{
    HIPSOLVER_EIG_MODE_NOVECTOR = 201,
    HIPSOLVER_EIG_MODE_VECTOR   = 202,
} hipsolverEigMode_t;

typedef enum
{
    HIPSOLVER_EIG_TYPE_1 = 211,
    HIPSOLVER_EIG_TYPE_2 = 212,
    HIPSOLVER_EIG_TYPE_3 = 213,
} hipsolverEigType_t;

#ifdef __cplusplus
extern "C" {
#endif

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCreate(hipsolverHandle_t* handle);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDestroy(hipsolverHandle_t handle);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSetStream(hipsolverHandle_t handle,
                                                      hipStream_t       streamId);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverGetStream(hipsolverHandle_t handle,
                                                      hipStream_t*      streamId);

// orgbr/ungbr
HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSorgbr_bufferSize(hipsolverHandle_t   handle,
                                                              hipsolverSideMode_t side,
                                                              int                 m,
                                                              int                 n,
                                                              int                 k,
                                                              float*              A,
                                                              int                 lda,
                                                              float*              tau,
                                                              int*                lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDorgbr_bufferSize(hipsolverHandle_t   handle,
                                                              hipsolverSideMode_t side,
                                                              int                 m,
                                                              int                 n,
                                                              int                 k,
                                                              double*             A,
                                                              int                 lda,
                                                              double*             tau,
                                                              int*                lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCungbr_bufferSize(hipsolverHandle_t   handle,
                                                              hipsolverSideMode_t side,
                                                              int                 m,
                                                              int                 n,
                                                              int                 k,
                                                              hipsolverComplex*   A,
                                                              int                 lda,
                                                              hipsolverComplex*   tau,
                                                              int*                lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZungbr_bufferSize(hipsolverHandle_t       handle,
                                                              hipsolverSideMode_t     side,
                                                              int                     m,
                                                              int                     n,
                                                              int                     k,
                                                              hipsolverDoubleComplex* A,
                                                              int                     lda,
                                                              hipsolverDoubleComplex* tau,
                                                              int*                    lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSorgbr(hipsolverHandle_t   handle,
                                                   hipsolverSideMode_t side,
                                                   int                 m,
                                                   int                 n,
                                                   int                 k,
                                                   float*              A,
                                                   int                 lda,
                                                   float*              tau,
                                                   float*              work,
                                                   int                 lwork,
                                                   int*                devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDorgbr(hipsolverHandle_t   handle,
                                                   hipsolverSideMode_t side,
                                                   int                 m,
                                                   int                 n,
                                                   int                 k,
                                                   double*             A,
                                                   int                 lda,
                                                   double*             tau,
                                                   double*             work,
                                                   int                 lwork,
                                                   int*                devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCungbr(hipsolverHandle_t   handle,
                                                   hipsolverSideMode_t side,
                                                   int                 m,
                                                   int                 n,
                                                   int                 k,
                                                   hipsolverComplex*   A,
                                                   int                 lda,
                                                   hipsolverComplex*   tau,
                                                   hipsolverComplex*   work,
                                                   int                 lwork,
                                                   int*                devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZungbr(hipsolverHandle_t       handle,
                                                   hipsolverSideMode_t     side,
                                                   int                     m,
                                                   int                     n,
                                                   int                     k,
                                                   hipsolverDoubleComplex* A,
                                                   int                     lda,
                                                   hipsolverDoubleComplex* tau,
                                                   hipsolverDoubleComplex* work,
                                                   int                     lwork,
                                                   int*                    devInfo);

// orgqr/ungqr
HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSorgqr_bufferSize(
    hipsolverHandle_t handle, int m, int n, int k, float* A, int lda, float* tau, int* lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDorgqr_bufferSize(
    hipsolverHandle_t handle, int m, int n, int k, double* A, int lda, double* tau, int* lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCungqr_bufferSize(hipsolverHandle_t handle,
                                                              int               m,
                                                              int               n,
                                                              int               k,
                                                              hipsolverComplex* A,
                                                              int               lda,
                                                              hipsolverComplex* tau,
                                                              int*              lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZungqr_bufferSize(hipsolverHandle_t       handle,
                                                              int                     m,
                                                              int                     n,
                                                              int                     k,
                                                              hipsolverDoubleComplex* A,
                                                              int                     lda,
                                                              hipsolverDoubleComplex* tau,
                                                              int*                    lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSorgqr(hipsolverHandle_t handle,
                                                   int               m,
                                                   int               n,
                                                   int               k,
                                                   float*            A,
                                                   int               lda,
                                                   float*            tau,
                                                   float*            work,
                                                   int               lwork,
                                                   int*              devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDorgqr(hipsolverHandle_t handle,
                                                   int               m,
                                                   int               n,
                                                   int               k,
                                                   double*           A,
                                                   int               lda,
                                                   double*           tau,
                                                   double*           work,
                                                   int               lwork,
                                                   int*              devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCungqr(hipsolverHandle_t handle,
                                                   int               m,
                                                   int               n,
                                                   int               k,
                                                   hipsolverComplex* A,
                                                   int               lda,
                                                   hipsolverComplex* tau,
                                                   hipsolverComplex* work,
                                                   int               lwork,
                                                   int*              devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZungqr(hipsolverHandle_t       handle,
                                                   int                     m,
                                                   int                     n,
                                                   int                     k,
                                                   hipsolverDoubleComplex* A,
                                                   int                     lda,
                                                   hipsolverDoubleComplex* tau,
                                                   hipsolverDoubleComplex* work,
                                                   int                     lwork,
                                                   int*                    devInfo);

// orgtr/ungtr
HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSorgtr_bufferSize(hipsolverHandle_t   handle,
                                                              hipsolverFillMode_t uplo,
                                                              int                 n,
                                                              float*              A,
                                                              int                 lda,
                                                              float*              tau,
                                                              int*                lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDorgtr_bufferSize(hipsolverHandle_t   handle,
                                                              hipsolverFillMode_t uplo,
                                                              int                 n,
                                                              double*             A,
                                                              int                 lda,
                                                              double*             tau,
                                                              int*                lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCungtr_bufferSize(hipsolverHandle_t   handle,
                                                              hipsolverFillMode_t uplo,
                                                              int                 n,
                                                              hipsolverComplex*   A,
                                                              int                 lda,
                                                              hipsolverComplex*   tau,
                                                              int*                lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZungtr_bufferSize(hipsolverHandle_t       handle,
                                                              hipsolverFillMode_t     uplo,
                                                              int                     n,
                                                              hipsolverDoubleComplex* A,
                                                              int                     lda,
                                                              hipsolverDoubleComplex* tau,
                                                              int*                    lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSorgtr(hipsolverHandle_t   handle,
                                                   hipsolverFillMode_t uplo,
                                                   int                 n,
                                                   float*              A,
                                                   int                 lda,
                                                   float*              tau,
                                                   float*              work,
                                                   int                 lwork,
                                                   int*                devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDorgtr(hipsolverHandle_t   handle,
                                                   hipsolverFillMode_t uplo,
                                                   int                 n,
                                                   double*             A,
                                                   int                 lda,
                                                   double*             tau,
                                                   double*             work,
                                                   int                 lwork,
                                                   int*                devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCungtr(hipsolverHandle_t   handle,
                                                   hipsolverFillMode_t uplo,
                                                   int                 n,
                                                   hipsolverComplex*   A,
                                                   int                 lda,
                                                   hipsolverComplex*   tau,
                                                   hipsolverComplex*   work,
                                                   int                 lwork,
                                                   int*                devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZungtr(hipsolverHandle_t       handle,
                                                   hipsolverFillMode_t     uplo,
                                                   int                     n,
                                                   hipsolverDoubleComplex* A,
                                                   int                     lda,
                                                   hipsolverDoubleComplex* tau,
                                                   hipsolverDoubleComplex* work,
                                                   int                     lwork,
                                                   int*                    devInfo);

// ormqr/unmqr
HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSormqr_bufferSize(hipsolverHandle_t    handle,
                                                              hipsolverSideMode_t  side,
                                                              hipsolverOperation_t trans,
                                                              int                  m,
                                                              int                  n,
                                                              int                  k,
                                                              float*               A,
                                                              int                  lda,
                                                              float*               tau,
                                                              float*               C,
                                                              int                  ldc,
                                                              int*                 lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDormqr_bufferSize(hipsolverHandle_t    handle,
                                                              hipsolverSideMode_t  side,
                                                              hipsolverOperation_t trans,
                                                              int                  m,
                                                              int                  n,
                                                              int                  k,
                                                              double*              A,
                                                              int                  lda,
                                                              double*              tau,
                                                              double*              C,
                                                              int                  ldc,
                                                              int*                 lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCunmqr_bufferSize(hipsolverHandle_t    handle,
                                                              hipsolverSideMode_t  side,
                                                              hipsolverOperation_t trans,
                                                              int                  m,
                                                              int                  n,
                                                              int                  k,
                                                              hipsolverComplex*    A,
                                                              int                  lda,
                                                              hipsolverComplex*    tau,
                                                              hipsolverComplex*    C,
                                                              int                  ldc,
                                                              int*                 lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZunmqr_bufferSize(hipsolverHandle_t       handle,
                                                              hipsolverSideMode_t     side,
                                                              hipsolverOperation_t    trans,
                                                              int                     m,
                                                              int                     n,
                                                              int                     k,
                                                              hipsolverDoubleComplex* A,
                                                              int                     lda,
                                                              hipsolverDoubleComplex* tau,
                                                              hipsolverDoubleComplex* C,
                                                              int                     ldc,
                                                              int*                    lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSormqr(hipsolverHandle_t    handle,
                                                   hipsolverSideMode_t  side,
                                                   hipsolverOperation_t trans,
                                                   int                  m,
                                                   int                  n,
                                                   int                  k,
                                                   float*               A,
                                                   int                  lda,
                                                   float*               tau,
                                                   float*               C,
                                                   int                  ldc,
                                                   float*               work,
                                                   int                  lwork,
                                                   int*                 devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDormqr(hipsolverHandle_t    handle,
                                                   hipsolverSideMode_t  side,
                                                   hipsolverOperation_t trans,
                                                   int                  m,
                                                   int                  n,
                                                   int                  k,
                                                   double*              A,
                                                   int                  lda,
                                                   double*              tau,
                                                   double*              C,
                                                   int                  ldc,
                                                   double*              work,
                                                   int                  lwork,
                                                   int*                 devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCunmqr(hipsolverHandle_t    handle,
                                                   hipsolverSideMode_t  side,
                                                   hipsolverOperation_t trans,
                                                   int                  m,
                                                   int                  n,
                                                   int                  k,
                                                   hipsolverComplex*    A,
                                                   int                  lda,
                                                   hipsolverComplex*    tau,
                                                   hipsolverComplex*    C,
                                                   int                  ldc,
                                                   hipsolverComplex*    work,
                                                   int                  lwork,
                                                   int*                 devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZunmqr(hipsolverHandle_t       handle,
                                                   hipsolverSideMode_t     side,
                                                   hipsolverOperation_t    trans,
                                                   int                     m,
                                                   int                     n,
                                                   int                     k,
                                                   hipsolverDoubleComplex* A,
                                                   int                     lda,
                                                   hipsolverDoubleComplex* tau,
                                                   hipsolverDoubleComplex* C,
                                                   int                     ldc,
                                                   hipsolverDoubleComplex* work,
                                                   int                     lwork,
                                                   int*                    devInfo);

// ormtr/unmtr
HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSormtr_bufferSize(hipsolverHandle_t    handle,
                                                              hipsolverSideMode_t  side,
                                                              hipsolverFillMode_t  uplo,
                                                              hipsolverOperation_t trans,
                                                              int                  m,
                                                              int                  n,
                                                              float*               A,
                                                              int                  lda,
                                                              float*               tau,
                                                              float*               C,
                                                              int                  ldc,
                                                              int*                 lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDormtr_bufferSize(hipsolverHandle_t    handle,
                                                              hipsolverSideMode_t  side,
                                                              hipsolverFillMode_t  uplo,
                                                              hipsolverOperation_t trans,
                                                              int                  m,
                                                              int                  n,
                                                              double*              A,
                                                              int                  lda,
                                                              double*              tau,
                                                              double*              C,
                                                              int                  ldc,
                                                              int*                 lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCunmtr_bufferSize(hipsolverHandle_t    handle,
                                                              hipsolverSideMode_t  side,
                                                              hipsolverFillMode_t  uplo,
                                                              hipsolverOperation_t trans,
                                                              int                  m,
                                                              int                  n,
                                                              hipsolverComplex*    A,
                                                              int                  lda,
                                                              hipsolverComplex*    tau,
                                                              hipsolverComplex*    C,
                                                              int                  ldc,
                                                              int*                 lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZunmtr_bufferSize(hipsolverHandle_t       handle,
                                                              hipsolverSideMode_t     side,
                                                              hipsolverFillMode_t     uplo,
                                                              hipsolverOperation_t    trans,
                                                              int                     m,
                                                              int                     n,
                                                              hipsolverDoubleComplex* A,
                                                              int                     lda,
                                                              hipsolverDoubleComplex* tau,
                                                              hipsolverDoubleComplex* C,
                                                              int                     ldc,
                                                              int*                    lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSormtr(hipsolverHandle_t    handle,
                                                   hipsolverSideMode_t  side,
                                                   hipsolverFillMode_t  uplo,
                                                   hipsolverOperation_t trans,
                                                   int                  m,
                                                   int                  n,
                                                   float*               A,
                                                   int                  lda,
                                                   float*               tau,
                                                   float*               C,
                                                   int                  ldc,
                                                   float*               work,
                                                   int                  lwork,
                                                   int*                 devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDormtr(hipsolverHandle_t    handle,
                                                   hipsolverSideMode_t  side,
                                                   hipsolverFillMode_t  uplo,
                                                   hipsolverOperation_t trans,
                                                   int                  m,
                                                   int                  n,
                                                   double*              A,
                                                   int                  lda,
                                                   double*              tau,
                                                   double*              C,
                                                   int                  ldc,
                                                   double*              work,
                                                   int                  lwork,
                                                   int*                 devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCunmtr(hipsolverHandle_t    handle,
                                                   hipsolverSideMode_t  side,
                                                   hipsolverFillMode_t  uplo,
                                                   hipsolverOperation_t trans,
                                                   int                  m,
                                                   int                  n,
                                                   hipsolverComplex*    A,
                                                   int                  lda,
                                                   hipsolverComplex*    tau,
                                                   hipsolverComplex*    C,
                                                   int                  ldc,
                                                   hipsolverComplex*    work,
                                                   int                  lwork,
                                                   int*                 devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZunmtr(hipsolverHandle_t       handle,
                                                   hipsolverSideMode_t     side,
                                                   hipsolverFillMode_t     uplo,
                                                   hipsolverOperation_t    trans,
                                                   int                     m,
                                                   int                     n,
                                                   hipsolverDoubleComplex* A,
                                                   int                     lda,
                                                   hipsolverDoubleComplex* tau,
                                                   hipsolverDoubleComplex* C,
                                                   int                     ldc,
                                                   hipsolverDoubleComplex* work,
                                                   int                     lwork,
                                                   int*                    devInfo);

// gebrd
HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSgebrd_bufferSize(hipsolverHandle_t handle,
                                                              int               m,
                                                              int               n,
                                                              int*              lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDgebrd_bufferSize(hipsolverHandle_t handle,
                                                              int               m,
                                                              int               n,
                                                              int*              lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCgebrd_bufferSize(hipsolverHandle_t handle,
                                                              int               m,
                                                              int               n,
                                                              int*              lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZgebrd_bufferSize(hipsolverHandle_t handle,
                                                              int               m,
                                                              int               n,
                                                              int*              lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSgebrd(hipsolverHandle_t handle,
                                                   int               m,
                                                   int               n,
                                                   float*            A,
                                                   int               lda,
                                                   float*            D,
                                                   float*            E,
                                                   float*            tauq,
                                                   float*            taup,
                                                   float*            work,
                                                   int               lwork,
                                                   int*              devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDgebrd(hipsolverHandle_t handle,
                                                   int               m,
                                                   int               n,
                                                   double*           A,
                                                   int               lda,
                                                   double*           D,
                                                   double*           E,
                                                   double*           tauq,
                                                   double*           taup,
                                                   double*           work,
                                                   int               lwork,
                                                   int*              devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCgebrd(hipsolverHandle_t handle,
                                                   int               m,
                                                   int               n,
                                                   hipsolverComplex* A,
                                                   int               lda,
                                                   float*            D,
                                                   float*            E,
                                                   hipsolverComplex* tauq,
                                                   hipsolverComplex* taup,
                                                   hipsolverComplex* work,
                                                   int               lwork,
                                                   int*              devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZgebrd(hipsolverHandle_t       handle,
                                                   int                     m,
                                                   int                     n,
                                                   hipsolverDoubleComplex* A,
                                                   int                     lda,
                                                   double*                 D,
                                                   double*                 E,
                                                   hipsolverDoubleComplex* tauq,
                                                   hipsolverDoubleComplex* taup,
                                                   hipsolverDoubleComplex* work,
                                                   int                     lwork,
                                                   int*                    devInfo);

// geqrf
HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSgeqrf_bufferSize(
    hipsolverHandle_t handle, int m, int n, float* A, int lda, int* lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDgeqrf_bufferSize(
    hipsolverHandle_t handle, int m, int n, double* A, int lda, int* lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCgeqrf_bufferSize(
    hipsolverHandle_t handle, int m, int n, hipsolverComplex* A, int lda, int* lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZgeqrf_bufferSize(
    hipsolverHandle_t handle, int m, int n, hipsolverDoubleComplex* A, int lda, int* lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSgeqrf(hipsolverHandle_t handle,
                                                   int               m,
                                                   int               n,
                                                   float*            A,
                                                   int               lda,
                                                   float*            tau,
                                                   float*            work,
                                                   int               lwork,
                                                   int*              devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDgeqrf(hipsolverHandle_t handle,
                                                   int               m,
                                                   int               n,
                                                   double*           A,
                                                   int               lda,
                                                   double*           tau,
                                                   double*           work,
                                                   int               lwork,
                                                   int*              devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCgeqrf(hipsolverHandle_t handle,
                                                   int               m,
                                                   int               n,
                                                   hipsolverComplex* A,
                                                   int               lda,
                                                   hipsolverComplex* tau,
                                                   hipsolverComplex* work,
                                                   int               lwork,
                                                   int*              devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZgeqrf(hipsolverHandle_t       handle,
                                                   int                     m,
                                                   int                     n,
                                                   hipsolverDoubleComplex* A,
                                                   int                     lda,
                                                   hipsolverDoubleComplex* tau,
                                                   hipsolverDoubleComplex* work,
                                                   int                     lwork,
                                                   int*                    devInfo);

// gesvd
HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSgesvd_bufferSize(
    hipsolverHandle_t handle, signed char jobu, signed char jobv, int m, int n, int* lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDgesvd_bufferSize(
    hipsolverHandle_t handle, signed char jobu, signed char jobv, int m, int n, int* lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCgesvd_bufferSize(
    hipsolverHandle_t handle, signed char jobu, signed char jobv, int m, int n, int* lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZgesvd_bufferSize(
    hipsolverHandle_t handle, signed char jobu, signed char jobv, int m, int n, int* lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSgesvd(hipsolverHandle_t handle,
                                                   signed char       jobu,
                                                   signed char       jobv,
                                                   int               m,
                                                   int               n,
                                                   float*            A,
                                                   int               lda,
                                                   float*            S,
                                                   float*            U,
                                                   int               ldu,
                                                   float*            V,
                                                   int               ldv,
                                                   float*            work,
                                                   int               lwork,
                                                   float*            rwork,
                                                   int*              devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDgesvd(hipsolverHandle_t handle,
                                                   signed char       jobu,
                                                   signed char       jobv,
                                                   int               m,
                                                   int               n,
                                                   double*           A,
                                                   int               lda,
                                                   double*           S,
                                                   double*           U,
                                                   int               ldu,
                                                   double*           V,
                                                   int               ldv,
                                                   double*           work,
                                                   int               lwork,
                                                   double*           rwork,
                                                   int*              devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCgesvd(hipsolverHandle_t handle,
                                                   signed char       jobu,
                                                   signed char       jobv,
                                                   int               m,
                                                   int               n,
                                                   hipsolverComplex* A,
                                                   int               lda,
                                                   float*            S,
                                                   hipsolverComplex* U,
                                                   int               ldu,
                                                   hipsolverComplex* V,
                                                   int               ldv,
                                                   hipsolverComplex* work,
                                                   int               lwork,
                                                   float*            rwork,
                                                   int*              devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZgesvd(hipsolverHandle_t       handle,
                                                   signed char             jobu,
                                                   signed char             jobv,
                                                   int                     m,
                                                   int                     n,
                                                   hipsolverDoubleComplex* A,
                                                   int                     lda,
                                                   double*                 S,
                                                   hipsolverDoubleComplex* U,
                                                   int                     ldu,
                                                   hipsolverDoubleComplex* V,
                                                   int                     ldv,
                                                   hipsolverDoubleComplex* work,
                                                   int                     lwork,
                                                   double*                 rwork,
                                                   int*                    devInfo);

// getrf
HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSgetrf_bufferSize(
    hipsolverHandle_t handle, int m, int n, float* A, int lda, int* lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDgetrf_bufferSize(
    hipsolverHandle_t handle, int m, int n, double* A, int lda, int* lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCgetrf_bufferSize(
    hipsolverHandle_t handle, int m, int n, hipsolverComplex* A, int lda, int* lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZgetrf_bufferSize(
    hipsolverHandle_t handle, int m, int n, hipsolverDoubleComplex* A, int lda, int* lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSgetrf(hipsolverHandle_t handle,
                                                   int               m,
                                                   int               n,
                                                   float*            A,
                                                   int               lda,
                                                   float*            work,
                                                   int               lwork,
                                                   int*              devIpiv,
                                                   int*              devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDgetrf(hipsolverHandle_t handle,
                                                   int               m,
                                                   int               n,
                                                   double*           A,
                                                   int               lda,
                                                   double*           work,
                                                   int               lwork,
                                                   int*              devIpiv,
                                                   int*              devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCgetrf(hipsolverHandle_t handle,
                                                   int               m,
                                                   int               n,
                                                   hipsolverComplex* A,
                                                   int               lda,
                                                   hipsolverComplex* work,
                                                   int               lwork,
                                                   int*              devIpiv,
                                                   int*              devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZgetrf(hipsolverHandle_t       handle,
                                                   int                     m,
                                                   int                     n,
                                                   hipsolverDoubleComplex* A,
                                                   int                     lda,
                                                   hipsolverDoubleComplex* work,
                                                   int                     lwork,
                                                   int*                    devIpiv,
                                                   int*                    devInfo);

// getrs
HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSgetrs_bufferSize(hipsolverHandle_t    handle,
                                                              hipsolverOperation_t trans,
                                                              int                  n,
                                                              int                  nrhs,
                                                              float*               A,
                                                              int                  lda,
                                                              int*                 devIpiv,
                                                              float*               B,
                                                              int                  ldb,
                                                              int*                 lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDgetrs_bufferSize(hipsolverHandle_t    handle,
                                                              hipsolverOperation_t trans,
                                                              int                  n,
                                                              int                  nrhs,
                                                              double*              A,
                                                              int                  lda,
                                                              int*                 devIpiv,
                                                              double*              B,
                                                              int                  ldb,
                                                              int*                 lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCgetrs_bufferSize(hipsolverHandle_t    handle,
                                                              hipsolverOperation_t trans,
                                                              int                  n,
                                                              int                  nrhs,
                                                              hipsolverComplex*    A,
                                                              int                  lda,
                                                              int*                 devIpiv,
                                                              hipsolverComplex*    B,
                                                              int                  ldb,
                                                              int*                 lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZgetrs_bufferSize(hipsolverHandle_t       handle,
                                                              hipsolverOperation_t    trans,
                                                              int                     n,
                                                              int                     nrhs,
                                                              hipsolverDoubleComplex* A,
                                                              int                     lda,
                                                              int*                    devIpiv,
                                                              hipsolverDoubleComplex* B,
                                                              int                     ldb,
                                                              int*                    lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSgetrs(hipsolverHandle_t    handle,
                                                   hipsolverOperation_t trans,
                                                   int                  n,
                                                   int                  nrhs,
                                                   float*               A,
                                                   int                  lda,
                                                   int*                 devIpiv,
                                                   float*               B,
                                                   int                  ldb,
                                                   float*               work,
                                                   int                  lwork,
                                                   int*                 devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDgetrs(hipsolverHandle_t    handle,
                                                   hipsolverOperation_t trans,
                                                   int                  n,
                                                   int                  nrhs,
                                                   double*              A,
                                                   int                  lda,
                                                   int*                 devIpiv,
                                                   double*              B,
                                                   int                  ldb,
                                                   double*              work,
                                                   int                  lwork,
                                                   int*                 devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCgetrs(hipsolverHandle_t    handle,
                                                   hipsolverOperation_t trans,
                                                   int                  n,
                                                   int                  nrhs,
                                                   hipsolverComplex*    A,
                                                   int                  lda,
                                                   int*                 devIpiv,
                                                   hipsolverComplex*    B,
                                                   int                  ldb,
                                                   hipsolverComplex*    work,
                                                   int                  lwork,
                                                   int*                 devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZgetrs(hipsolverHandle_t       handle,
                                                   hipsolverOperation_t    trans,
                                                   int                     n,
                                                   int                     nrhs,
                                                   hipsolverDoubleComplex* A,
                                                   int                     lda,
                                                   int*                    devIpiv,
                                                   hipsolverDoubleComplex* B,
                                                   int                     ldb,
                                                   hipsolverDoubleComplex* work,
                                                   int                     lwork,
                                                   int*                    devInfo);

// potrf
HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSpotrf_bufferSize(
    hipsolverHandle_t handle, hipsolverFillMode_t uplo, int n, float* A, int lda, int* lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDpotrf_bufferSize(
    hipsolverHandle_t handle, hipsolverFillMode_t uplo, int n, double* A, int lda, int* lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCpotrf_bufferSize(hipsolverHandle_t   handle,
                                                              hipsolverFillMode_t uplo,
                                                              int                 n,
                                                              hipsolverComplex*   A,
                                                              int                 lda,
                                                              int*                lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZpotrf_bufferSize(hipsolverHandle_t       handle,
                                                              hipsolverFillMode_t     uplo,
                                                              int                     n,
                                                              hipsolverDoubleComplex* A,
                                                              int                     lda,
                                                              int*                    lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSpotrf(hipsolverHandle_t   handle,
                                                   hipsolverFillMode_t uplo,
                                                   int                 n,
                                                   float*              A,
                                                   int                 lda,
                                                   float*              work,
                                                   int                 lwork,
                                                   int*                devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDpotrf(hipsolverHandle_t   handle,
                                                   hipsolverFillMode_t uplo,
                                                   int                 n,
                                                   double*             A,
                                                   int                 lda,
                                                   double*             work,
                                                   int                 lwork,
                                                   int*                devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCpotrf(hipsolverHandle_t   handle,
                                                   hipsolverFillMode_t uplo,
                                                   int                 n,
                                                   hipsolverComplex*   A,
                                                   int                 lda,
                                                   hipsolverComplex*   work,
                                                   int                 lwork,
                                                   int*                devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZpotrf(hipsolverHandle_t       handle,
                                                   hipsolverFillMode_t     uplo,
                                                   int                     n,
                                                   hipsolverDoubleComplex* A,
                                                   int                     lda,
                                                   hipsolverDoubleComplex* work,
                                                   int                     lwork,
                                                   int*                    devInfo);

// potrf_batched
HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSpotrfBatched_bufferSize(hipsolverHandle_t   handle,
                                                                     hipsolverFillMode_t uplo,
                                                                     int                 n,
                                                                     float*              A[],
                                                                     int                 lda,
                                                                     int*                lwork,
                                                                     int batch_count);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDpotrfBatched_bufferSize(hipsolverHandle_t   handle,
                                                                     hipsolverFillMode_t uplo,
                                                                     int                 n,
                                                                     double*             A[],
                                                                     int                 lda,
                                                                     int*                lwork,
                                                                     int batch_count);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCpotrfBatched_bufferSize(hipsolverHandle_t   handle,
                                                                     hipsolverFillMode_t uplo,
                                                                     int                 n,
                                                                     hipsolverComplex*   A[],
                                                                     int                 lda,
                                                                     int*                lwork,
                                                                     int batch_count);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZpotrfBatched_bufferSize(hipsolverHandle_t       handle,
                                                                     hipsolverFillMode_t     uplo,
                                                                     int                     n,
                                                                     hipsolverDoubleComplex* A[],
                                                                     int                     lda,
                                                                     int*                    lwork,
                                                                     int batch_count);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSpotrfBatched(hipsolverHandle_t   handle,
                                                          hipsolverFillMode_t uplo,
                                                          int                 n,
                                                          float*              A[],
                                                          int                 lda,
                                                          float*              work,
                                                          int                 lwork,
                                                          int*                devInfo,
                                                          int                 batch_count);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDpotrfBatched(hipsolverHandle_t   handle,
                                                          hipsolverFillMode_t uplo,
                                                          int                 n,
                                                          double*             A[],
                                                          int                 lda,
                                                          double*             work,
                                                          int                 lwork,
                                                          int*                devInfo,
                                                          int                 batch_count);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCpotrfBatched(hipsolverHandle_t   handle,
                                                          hipsolverFillMode_t uplo,
                                                          int                 n,
                                                          hipsolverComplex*   A[],
                                                          int                 lda,
                                                          hipsolverComplex*   work,
                                                          int                 lwork,
                                                          int*                devInfo,
                                                          int                 batch_count);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZpotrfBatched(hipsolverHandle_t       handle,
                                                          hipsolverFillMode_t     uplo,
                                                          int                     n,
                                                          hipsolverDoubleComplex* A[],
                                                          int                     lda,
                                                          hipsolverDoubleComplex* work,
                                                          int                     lwork,
                                                          int*                    devInfo,
                                                          int                     batch_count);

// syevd/heevd
HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSsyevd_bufferSize(hipsolverHandle_t   handle,
                                                              hipsolverEigMode_t  jobz,
                                                              hipsolverFillMode_t uplo,
                                                              int                 n,
                                                              float*              A,
                                                              int                 lda,
                                                              float*              D,
                                                              int*                lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDsyevd_bufferSize(hipsolverHandle_t   handle,
                                                              hipsolverEigMode_t  jobz,
                                                              hipsolverFillMode_t uplo,
                                                              int                 n,
                                                              double*             A,
                                                              int                 lda,
                                                              double*             D,
                                                              int*                lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCheevd_bufferSize(hipsolverHandle_t   handle,
                                                              hipsolverEigMode_t  jobz,
                                                              hipsolverFillMode_t uplo,
                                                              int                 n,
                                                              hipsolverComplex*   A,
                                                              int                 lda,
                                                              float*              D,
                                                              int*                lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZheevd_bufferSize(hipsolverHandle_t       handle,
                                                              hipsolverEigMode_t      jobz,
                                                              hipsolverFillMode_t     uplo,
                                                              int                     n,
                                                              hipsolverDoubleComplex* A,
                                                              int                     lda,
                                                              double*                 D,
                                                              int*                    lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSsyevd(hipsolverHandle_t   handle,
                                                   hipsolverEigMode_t  jobz,
                                                   hipsolverFillMode_t uplo,
                                                   int                 n,
                                                   float*              A,
                                                   int                 lda,
                                                   float*              D,
                                                   float*              work,
                                                   int                 lwork,
                                                   int*                devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDsyevd(hipsolverHandle_t   handle,
                                                   hipsolverEigMode_t  jobz,
                                                   hipsolverFillMode_t uplo,
                                                   int                 n,
                                                   double*             A,
                                                   int                 lda,
                                                   double*             D,
                                                   double*             work,
                                                   int                 lwork,
                                                   int*                devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverCheevd(hipsolverHandle_t   handle,
                                                   hipsolverEigMode_t  jobz,
                                                   hipsolverFillMode_t uplo,
                                                   int                 n,
                                                   hipsolverComplex*   A,
                                                   int                 lda,
                                                   float*              D,
                                                   hipsolverComplex*   work,
                                                   int                 lwork,
                                                   int*                devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZheevd(hipsolverHandle_t       handle,
                                                   hipsolverEigMode_t      jobz,
                                                   hipsolverFillMode_t     uplo,
                                                   int                     n,
                                                   hipsolverDoubleComplex* A,
                                                   int                     lda,
                                                   double*                 D,
                                                   hipsolverDoubleComplex* work,
                                                   int                     lwork,
                                                   int*                    devInfo);

// sygvd/hegvd
HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSsygvd_bufferSize(hipsolverHandle_t   handle,
                                                              hipsolverEigType_t  itype,
                                                              hipsolverEigMode_t  jobz,
                                                              hipsolverFillMode_t uplo,
                                                              int                 n,
                                                              float*              A,
                                                              int                 lda,
                                                              float*              B,
                                                              int                 ldb,
                                                              float*              D,
                                                              int*                lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDsygvd_bufferSize(hipsolverHandle_t   handle,
                                                              hipsolverEigType_t  itype,
                                                              hipsolverEigMode_t  jobz,
                                                              hipsolverFillMode_t uplo,
                                                              int                 n,
                                                              double*             A,
                                                              int                 lda,
                                                              double*             B,
                                                              int                 ldb,
                                                              double*             D,
                                                              int*                lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverChegvd_bufferSize(hipsolverHandle_t   handle,
                                                              hipsolverEigType_t  itype,
                                                              hipsolverEigMode_t  jobz,
                                                              hipsolverFillMode_t uplo,
                                                              int                 n,
                                                              hipsolverComplex*   A,
                                                              int                 lda,
                                                              hipsolverComplex*   B,
                                                              int                 ldb,
                                                              float*              D,
                                                              int*                lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZhegvd_bufferSize(hipsolverHandle_t       handle,
                                                              hipsolverEigType_t      itype,
                                                              hipsolverEigMode_t      jobz,
                                                              hipsolverFillMode_t     uplo,
                                                              int                     n,
                                                              hipsolverDoubleComplex* A,
                                                              int                     lda,
                                                              hipsolverDoubleComplex* B,
                                                              int                     ldb,
                                                              double*                 D,
                                                              int*                    lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSsygvd(hipsolverHandle_t   handle,
                                                   hipsolverEigType_t  itype,
                                                   hipsolverEigMode_t  jobz,
                                                   hipsolverFillMode_t uplo,
                                                   int                 n,
                                                   float*              A,
                                                   int                 lda,
                                                   float*              B,
                                                   int                 ldb,
                                                   float*              D,
                                                   float*              work,
                                                   int                 lwork,
                                                   int*                devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDsygvd(hipsolverHandle_t   handle,
                                                   hipsolverEigType_t  itype,
                                                   hipsolverEigMode_t  jobz,
                                                   hipsolverFillMode_t uplo,
                                                   int                 n,
                                                   double*             A,
                                                   int                 lda,
                                                   double*             B,
                                                   int                 ldb,
                                                   double*             D,
                                                   double*             work,
                                                   int                 lwork,
                                                   int*                devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverChegvd(hipsolverHandle_t   handle,
                                                   hipsolverEigType_t  itype,
                                                   hipsolverEigMode_t  jobz,
                                                   hipsolverFillMode_t uplo,
                                                   int                 n,
                                                   hipsolverComplex*   A,
                                                   int                 lda,
                                                   hipsolverComplex*   B,
                                                   int                 ldb,
                                                   float*              D,
                                                   hipsolverComplex*   work,
                                                   int                 lwork,
                                                   int*                devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZhegvd(hipsolverHandle_t       handle,
                                                   hipsolverEigType_t      itype,
                                                   hipsolverEigMode_t      jobz,
                                                   hipsolverFillMode_t     uplo,
                                                   int                     n,
                                                   hipsolverDoubleComplex* A,
                                                   int                     lda,
                                                   hipsolverDoubleComplex* B,
                                                   int                     ldb,
                                                   double*                 D,
                                                   hipsolverDoubleComplex* work,
                                                   int                     lwork,
                                                   int*                    devInfo);

// sytrd/hetrd
HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSsytrd_bufferSize(hipsolverHandle_t   handle,
                                                              hipsolverFillMode_t uplo,
                                                              int                 n,
                                                              float*              A,
                                                              int                 lda,
                                                              float*              D,
                                                              float*              E,
                                                              float*              tau,
                                                              int*                lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDsytrd_bufferSize(hipsolverHandle_t   handle,
                                                              hipsolverFillMode_t uplo,
                                                              int                 n,
                                                              double*             A,
                                                              int                 lda,
                                                              double*             D,
                                                              double*             E,
                                                              double*             tau,
                                                              int*                lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverChetrd_bufferSize(hipsolverHandle_t   handle,
                                                              hipsolverFillMode_t uplo,
                                                              int                 n,
                                                              hipsolverComplex*   A,
                                                              int                 lda,
                                                              float*              D,
                                                              float*              E,
                                                              hipsolverComplex*   tau,
                                                              int*                lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZhetrd_bufferSize(hipsolverHandle_t       handle,
                                                              hipsolverFillMode_t     uplo,
                                                              int                     n,
                                                              hipsolverDoubleComplex* A,
                                                              int                     lda,
                                                              double*                 D,
                                                              double*                 E,
                                                              hipsolverDoubleComplex* tau,
                                                              int*                    lwork);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverSsytrd(hipsolverHandle_t   handle,
                                                   hipsolverFillMode_t uplo,
                                                   int                 n,
                                                   float*              A,
                                                   int                 lda,
                                                   float*              D,
                                                   float*              E,
                                                   float*              tau,
                                                   float*              work,
                                                   int                 lwork,
                                                   int*                devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverDsytrd(hipsolverHandle_t   handle,
                                                   hipsolverFillMode_t uplo,
                                                   int                 n,
                                                   double*             A,
                                                   int                 lda,
                                                   double*             D,
                                                   double*             E,
                                                   double*             tau,
                                                   double*             work,
                                                   int                 lwork,
                                                   int*                devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverChetrd(hipsolverHandle_t   handle,
                                                   hipsolverFillMode_t uplo,
                                                   int                 n,
                                                   hipsolverComplex*   A,
                                                   int                 lda,
                                                   float*              D,
                                                   float*              E,
                                                   hipsolverComplex*   tau,
                                                   hipsolverComplex*   work,
                                                   int                 lwork,
                                                   int*                devInfo);

HIPSOLVER_EXPORT hipsolverStatus_t hipsolverZhetrd(hipsolverHandle_t       handle,
                                                   hipsolverFillMode_t     uplo,
                                                   int                     n,
                                                   hipsolverDoubleComplex* A,
                                                   int                     lda,
                                                   double*                 D,
                                                   double*                 E,
                                                   hipsolverDoubleComplex* tau,
                                                   hipsolverDoubleComplex* work,
                                                   int                     lwork,
                                                   int*                    devInfo);

#ifdef __cplusplus
}
#endif

#endif
