/* ************************************************************************
 * Copyright (C) 2016-2023 Advanced Micro Devices, Inc. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ************************************************************************ */

#include "testing_symm.hpp"
#include "testing_symm_batched.hpp"
#include "testing_symm_strided_batched.hpp"
#include "utility.h"
#include <math.h>
#include <stdexcept>
#include <vector>

using std::vector;
using ::testing::Combine;
using ::testing::TestWithParam;
using ::testing::Values;
using ::testing::ValuesIn;

// only GCC/VS 2010 comes with std::tr1::tuple, but it is unnecessary,  std::tuple is good enough;

typedef std::tuple<vector<int>, vector<double>, vector<char>, double, int, bool> symm_tuple;

/* =====================================================================
README: This file contains testers to verify the correctness of
        BLAS routines with google test

        It is supposed to be played/used by advance / expert users
        Normal users only need to get the library routines without testers
     =================================================================== */

/* =====================================================================
Advance users only: BrainStorm the parameters but do not make artificial one which invalidates the
matrix.
like lda pairs with M, and "lda must >= M". case "lda < M" will be guarded by argument-checkers
inside API of course.
Yet, the goal of this file is to verify result correctness not argument-checkers.

Representative sampling is sufficient, endless brute-force sampling is not necessary
=================================================================== */

// vector of vector, each vector is a {M, N, lda, ldb, ldc};
// add/delete as a group
const vector<vector<int>> matrix_size_range = {
    {-1, -1, 1, 1, 1},
    {600, 500, 600, 600, 700},
};

const vector<vector<double>> alpha_beta_range = {{-5.0, 2.0, 3.0, -2.0}};

// vector of vector, each pair is a {side, uplo};
// side has two option "Lefe (L), Right (R)"
// uplo has two "Lower (L), Upper (U)"
// Each letter is capitalizied, e.g. do not use 'l', but use 'L' instead.

const vector<vector<char>> side_uplo_range = {
    {'L', 'L'},
    {'R', 'L'},
    {'L', 'U'},
    {'R', 'U'},
};

const vector<double> stride_scale_range = {2};

const vector<int> batch_count_range = {2};

const bool is_fortran[] = {false, true};

/* ===============Google Unit Test==================================================== */

/* =====================================================================
     BLAS-3 symm:
=================================================================== */

/* ============================Setup Arguments======================================= */

// Please use "class Arguments" (see utility.hpp) to pass parameters to templated testers;
// Some routines may not touch/use certain "members" of objects "arg".
// like BLAS-1 Scal does not have lda, BLAS-2 GEMV does not have ldb, ldc;
// That is fine. These testers & routines will leave untouched members alone.
// Do not use std::tuple to directly pass parameters to testers
// by std:tuple, you have unpack it with extreme care for each one by like "std::get<0>" which is
// not intuitive and error-prone

Arguments setup_symm_arguments(symm_tuple tup)
{

    vector<int>    matrix_size  = std::get<0>(tup);
    vector<double> alpha_beta   = std::get<1>(tup);
    vector<char>   side_uplo    = std::get<2>(tup);
    double         stride_scale = std::get<3>(tup);
    int            batch_count  = std::get<4>(tup);
    bool           fortran      = std::get<5>(tup);

    Arguments arg;

    // see the comments about matrix_size_range above
    arg.M   = matrix_size[0];
    arg.N   = matrix_size[1];
    arg.lda = matrix_size[2];
    arg.ldb = matrix_size[3];
    arg.ldc = matrix_size[4];

    arg.alpha  = alpha_beta[0];
    arg.alphai = alpha_beta[1];
    arg.beta   = alpha_beta[2];
    arg.betai  = alpha_beta[3];

    arg.side = side_uplo[0];
    arg.uplo = side_uplo[1];

    arg.timing = 0;

    arg.stride_scale = stride_scale;
    arg.batch_count  = batch_count;

    arg.fortran = fortran;

    return arg;
}

class symm_gtest : public ::TestWithParam<symm_tuple>
{
protected:
    symm_gtest() {}
    virtual ~symm_gtest() {}
    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_P(symm_gtest, symm_gtest_float)
{
    // GetParam return a tuple. Tee setup routine unpack the tuple
    // and initializes arg(Arguments) which will be passed to testing routine
    // The Arguments data struture have physical meaning associated.
    // while the tuple is non-intuitive.

    Arguments arg = setup_symm_arguments(GetParam());

    hipblasStatus_t status = testing_symm<float>(arg);

    // if not success, then the input argument is problematic, so detect the error message
    if(status != HIPBLAS_STATUS_SUCCESS)
    {

        if(arg.M < 0 || arg.N < 0 || arg.ldc < arg.M || arg.ldb < arg.M
           || (arg.side == 'L' ? arg.lda < arg.M : arg.lda < arg.N))
        {
            EXPECT_EQ(HIPBLAS_STATUS_INVALID_VALUE, status);
        }
        else
        {
            EXPECT_EQ(HIPBLAS_STATUS_SUCCESS, status); // fail
        }
    }
}

TEST_P(symm_gtest, symm_gtest_float_complex)
{
    // GetParam return a tuple. Tee setup routine unpack the tuple
    // and initializes arg(Arguments) which will be passed to testing routine
    // The Arguments data struture have physical meaning associated.
    // while the tuple is non-intuitive.

    Arguments arg = setup_symm_arguments(GetParam());

    hipblasStatus_t status = testing_symm<hipblasComplex>(arg);

    // if not success, then the input argument is problematic, so detect the error message
    if(status != HIPBLAS_STATUS_SUCCESS)
    {

        if(arg.M < 0 || arg.N < 0 || arg.ldc < arg.M || arg.ldb < arg.M
           || (arg.side == 'L' ? arg.lda < arg.M : arg.lda < arg.N))
        {
            EXPECT_EQ(HIPBLAS_STATUS_INVALID_VALUE, status);
        }
        else
        {
            EXPECT_EQ(HIPBLAS_STATUS_SUCCESS, status); // fail
        }
    }
}

TEST_P(symm_gtest, symm_gtest_double_complex)
{
    // GetParam return a tuple. Tee setup routine unpack the tuple
    // and initializes arg(Arguments) which will be passed to testing routine
    // The Arguments data struture have physical meaning associated.
    // while the tuple is non-intuitive.

    Arguments arg = setup_symm_arguments(GetParam());

    hipblasStatus_t status = testing_symm<hipblasDoubleComplex>(arg);

    // if not success, then the input argument is problematic, so detect the error message
    if(status != HIPBLAS_STATUS_SUCCESS)
    {

        if(arg.M < 0 || arg.N < 0 || arg.ldc < arg.M || arg.ldb < arg.M
           || (arg.side == 'L' ? arg.lda < arg.M : arg.lda < arg.N))
        {
            EXPECT_EQ(HIPBLAS_STATUS_INVALID_VALUE, status);
        }
        else
        {
            EXPECT_EQ(HIPBLAS_STATUS_SUCCESS, status); // fail
        }
    }
}

#ifndef __HIP_PLATFORM_NVCC__

TEST_P(symm_gtest, symm_batched_gtest_float)
{
    // GetParam return a tuple. Tee setup routine unpack the tuple
    // and initializes arg(Arguments) which will be passed to testing routine
    // The Arguments data struture have physical meaning associated.
    // while the tuple is non-intuitive.

    Arguments arg = setup_symm_arguments(GetParam());

    hipblasStatus_t status = testing_symm_batched<float>(arg);

    // if not success, then the input argument is problematic, so detect the error message
    if(status != HIPBLAS_STATUS_SUCCESS)
    {

        if(arg.M < 0 || arg.N < 0 || arg.ldc < arg.M || arg.ldb < arg.M
           || (arg.side == 'L' ? arg.lda < arg.M : arg.lda < arg.N) || arg.batch_count < 0)
        {
            EXPECT_EQ(HIPBLAS_STATUS_INVALID_VALUE, status);
        }
        else
        {
            EXPECT_EQ(HIPBLAS_STATUS_SUCCESS, status); // fail
        }
    }
}

TEST_P(symm_gtest, symm_batched_gtest_float_complex)
{
    // GetParam return a tuple. Tee setup routine unpack the tuple
    // and initializes arg(Arguments) which will be passed to testing routine
    // The Arguments data struture have physical meaning associated.
    // while the tuple is non-intuitive.

    Arguments arg = setup_symm_arguments(GetParam());

    hipblasStatus_t status = testing_symm_batched<hipblasComplex>(arg);

    // if not success, then the input argument is problematic, so detect the error message
    if(status != HIPBLAS_STATUS_SUCCESS)
    {

        if(arg.M < 0 || arg.N < 0 || arg.ldc < arg.M || arg.ldb < arg.M
           || (arg.side == 'L' ? arg.lda < arg.M : arg.lda < arg.N) || arg.batch_count < 0)
        {
            EXPECT_EQ(HIPBLAS_STATUS_INVALID_VALUE, status);
        }
        else
        {
            EXPECT_EQ(HIPBLAS_STATUS_SUCCESS, status); // fail
        }
    }
}

TEST_P(symm_gtest, symm_batched_gtest_double_complex)
{
    // GetParam return a tuple. Tee setup routine unpack the tuple
    // and initializes arg(Arguments) which will be passed to testing routine
    // The Arguments data struture have physical meaning associated.
    // while the tuple is non-intuitive.

    Arguments arg = setup_symm_arguments(GetParam());

    hipblasStatus_t status = testing_symm_batched<hipblasDoubleComplex>(arg);

    // if not success, then the input argument is problematic, so detect the error message
    if(status != HIPBLAS_STATUS_SUCCESS)
    {

        if(arg.M < 0 || arg.N < 0 || arg.ldc < arg.M || arg.ldb < arg.M
           || (arg.side == 'L' ? arg.lda < arg.M : arg.lda < arg.N) || arg.batch_count < 0)
        {
            EXPECT_EQ(HIPBLAS_STATUS_INVALID_VALUE, status);
        }
        else
        {
            EXPECT_EQ(HIPBLAS_STATUS_SUCCESS, status); // fail
        }
    }
}

TEST_P(symm_gtest, symm_strided_batched_gtest_float)
{
    // GetParam return a tuple. Tee setup routine unpack the tuple
    // and initializes arg(Arguments) which will be passed to testing routine
    // The Arguments data struture have physical meaning associated.
    // while the tuple is non-intuitive.

    Arguments arg = setup_symm_arguments(GetParam());

    hipblasStatus_t status = testing_symm_strided_batched<float>(arg);

    // if not success, then the input argument is problematic, so detect the error message
    if(status != HIPBLAS_STATUS_SUCCESS)
    {

        if(arg.M < 0 || arg.N < 0 || arg.ldc < arg.M || arg.ldb < arg.M
           || (arg.side == 'L' ? arg.lda < arg.M : arg.lda < arg.N) || arg.batch_count < 0)
        {
            EXPECT_EQ(HIPBLAS_STATUS_INVALID_VALUE, status);
        }
        else
        {
            EXPECT_EQ(HIPBLAS_STATUS_SUCCESS, status); // fail
        }
    }
}

TEST_P(symm_gtest, symm_strided_batched_gtest_float_complex)
{
    // GetParam return a tuple. Tee setup routine unpack the tuple
    // and initializes arg(Arguments) which will be passed to testing routine
    // The Arguments data struture have physical meaning associated.
    // while the tuple is non-intuitive.

    Arguments arg = setup_symm_arguments(GetParam());

    hipblasStatus_t status = testing_symm_strided_batched<hipblasComplex>(arg);

    // if not success, then the input argument is problematic, so detect the error message
    if(status != HIPBLAS_STATUS_SUCCESS)
    {

        if(arg.M < 0 || arg.N < 0 || arg.ldc < arg.M || arg.ldb < arg.M
           || (arg.side == 'L' ? arg.lda < arg.M : arg.lda < arg.N) || arg.batch_count < 0)
        {
            EXPECT_EQ(HIPBLAS_STATUS_INVALID_VALUE, status);
        }
        else
        {
            EXPECT_EQ(HIPBLAS_STATUS_SUCCESS, status); // fail
        }
    }
}

TEST_P(symm_gtest, symm_strided_batched_gtest_double_complex)
{
    // GetParam return a tuple. Tee setup routine unpack the tuple
    // and initializes arg(Arguments) which will be passed to testing routine
    // The Arguments data struture have physical meaning associated.
    // while the tuple is non-intuitive.

    Arguments arg = setup_symm_arguments(GetParam());

    hipblasStatus_t status = testing_symm_strided_batched<hipblasDoubleComplex>(arg);

    // if not success, then the input argument is problematic, so detect the error message
    if(status != HIPBLAS_STATUS_SUCCESS)
    {

        if(arg.M < 0 || arg.N < 0 || arg.ldc < arg.M || arg.ldb < arg.M
           || (arg.side == 'L' ? arg.lda < arg.M : arg.lda < arg.N) || arg.batch_count < 0)
        {
            EXPECT_EQ(HIPBLAS_STATUS_INVALID_VALUE, status);
        }
        else
        {
            EXPECT_EQ(HIPBLAS_STATUS_SUCCESS, status); // fail
        }
    }
}

#endif

// notice we are using vector of vector
// so each elment in xxx_range is a avector,
// ValuesIn take each element (a vector) and combine them and feed them to test_p
// The combinations are  { {M, N, lda, ldb}, alpha, {side, diag} }
INSTANTIATE_TEST_SUITE_P(hipblassymm_matrix_size,
                         symm_gtest,
                         Combine(ValuesIn(matrix_size_range),
                                 ValuesIn(alpha_beta_range),
                                 ValuesIn(side_uplo_range),
                                 ValuesIn(stride_scale_range),
                                 ValuesIn(batch_count_range),
                                 ValuesIn(is_fortran)));
