/* ************************************************************************
 * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
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

#include "testing_getrf.hpp"
#include "testing_getrf_npvt.hpp"
#include "utility.h"
#include <math.h>
#include <stdexcept>
#include <vector>

using std::vector;
using ::testing::Combine;
using ::testing::TestWithParam;
using ::testing::Values;
using ::testing::ValuesIn;

typedef std::tuple<vector<int>, double, int, bool> getrf_tuple;

const vector<vector<int>> matrix_size_range = {{-1, -1, 1, 1},
                                               {10, 10, 10, 10},
                                               {10, 10, 20, 100},
                                               {600, 500, 600, 600},
                                               {1024, 1024, 1024, 1024}};

const vector<double> stride_scale_range = {2.5};

const vector<int> batch_count_range = {1};

const vector<bool> is_fortran = {false, true};

Arguments setup_getrf_arguments(getrf_tuple tup)
{
    vector<int> matrix_size  = std::get<0>(tup);
    double      stride_scale = std::get<1>(tup);
    int         batch_count  = std::get<2>(tup);
    bool        fortran      = std::get<3>(tup);

    Arguments arg;

    arg.M   = matrix_size[0];
    arg.N   = matrix_size[1];
    arg.lda = matrix_size[2];
    //arg.ldb = matrix_size[3];

    arg.stride_scale = stride_scale;
    arg.batch_count  = batch_count;

    arg.fortran = fortran;

    return arg;
}

class getrf_gtest : public ::TestWithParam<getrf_tuple>
{
protected:
    getrf_gtest() {}
    virtual ~getrf_gtest() {}
    virtual void SetUp() {}
    virtual void TearDown() {}
};

#ifndef __HIP_PLATFORM_NVCC__

TEST_P(getrf_gtest, getrf_gtest_float)
{
    // GetParam returns a tuple. The setup routine unpacks the tuple
    // and initializes arg(Arguments), which will be passed to testing routine.

    Arguments arg = setup_getrf_arguments(GetParam());

    hipblasStatus_t status = testing_getrf<float>(arg);

    if(status != HIPBLAS_STATUS_SUCCESS)
    {
        if(arg.N < 0 || arg.lda < arg.N)
        {
            EXPECT_EQ(HIPBLAS_STATUS_INVALID_VALUE, status);
        }
        else
        {
            EXPECT_EQ(HIPBLAS_STATUS_SUCCESS, status); // fail
        }
    }
}

TEST_P(getrf_gtest, getrf_gtest_double)
{
    // GetParam returns a tuple. The setup routine unpacks the tuple
    // and initializes arg(Arguments), which will be passed to testing routine.

    Arguments arg = setup_getrf_arguments(GetParam());

    hipblasStatus_t status = testing_getrf<double>(arg);

    if(status != HIPBLAS_STATUS_SUCCESS)
    {
        if(arg.N < 0 || arg.lda < arg.N)
        {
            EXPECT_EQ(HIPBLAS_STATUS_INVALID_VALUE, status);
        }
        else
        {
            EXPECT_EQ(HIPBLAS_STATUS_SUCCESS, status); // fail
        }
    }
}

TEST_P(getrf_gtest, getrf_gtest_float_complex)
{
    // GetParam returns a tuple. The setup routine unpacks the tuple
    // and initializes arg(Arguments), which will be passed to testing routine.

    Arguments arg = setup_getrf_arguments(GetParam());

    hipblasStatus_t status = testing_getrf<hipblasComplex>(arg);

    if(status != HIPBLAS_STATUS_SUCCESS)
    {
        if(arg.N < 0 || arg.lda < arg.N)
        {
            EXPECT_EQ(HIPBLAS_STATUS_INVALID_VALUE, status);
        }
        else
        {
            EXPECT_EQ(HIPBLAS_STATUS_SUCCESS, status); // fail
        }
    }
}

TEST_P(getrf_gtest, getrf_gtest_double_complex)
{
    // GetParam returns a tuple. The setup routine unpacks the tuple
    // and initializes arg(Arguments), which will be passed to testing routine.

    Arguments arg = setup_getrf_arguments(GetParam());

    hipblasStatus_t status = testing_getrf<hipblasDoubleComplex>(arg);

    if(status != HIPBLAS_STATUS_SUCCESS)
    {
        if(arg.N < 0 || arg.lda < arg.N)
        {
            EXPECT_EQ(HIPBLAS_STATUS_INVALID_VALUE, status);
        }
        else
        {
            EXPECT_EQ(HIPBLAS_STATUS_SUCCESS, status); // fail
        }
    }
}

TEST_P(getrf_gtest, getrf_npvt_gtest_float)
{
    // GetParam returns a tuple. The setup routine unpacks the tuple
    // and initializes arg(Arguments), which will be passed to testing routine.

    Arguments arg = setup_getrf_arguments(GetParam());

    hipblasStatus_t status = testing_getrf_npvt<float>(arg);

    if(status != HIPBLAS_STATUS_SUCCESS)
    {
        if(arg.N < 0 || arg.lda < arg.N)
        {
            EXPECT_EQ(HIPBLAS_STATUS_INVALID_VALUE, status);
        }
        else
        {
            EXPECT_EQ(HIPBLAS_STATUS_SUCCESS, status); // fail
        }
    }
}

TEST_P(getrf_gtest, getrf_npvt_gtest_double)
{
    // GetParam returns a tuple. The setup routine unpacks the tuple
    // and initializes arg(Arguments), which will be passed to testing routine.

    Arguments arg = setup_getrf_arguments(GetParam());

    hipblasStatus_t status = testing_getrf_npvt<double>(arg);

    if(status != HIPBLAS_STATUS_SUCCESS)
    {
        if(arg.N < 0 || arg.lda < arg.N)
        {
            EXPECT_EQ(HIPBLAS_STATUS_INVALID_VALUE, status);
        }
        else
        {
            EXPECT_EQ(HIPBLAS_STATUS_SUCCESS, status); // fail
        }
    }
}

TEST_P(getrf_gtest, getrf_npvt_gtest_float_complex)
{
    // GetParam returns a tuple. The setup routine unpacks the tuple
    // and initializes arg(Arguments), which will be passed to testing routine.

    Arguments arg = setup_getrf_arguments(GetParam());

    hipblasStatus_t status = testing_getrf_npvt<hipblasComplex>(arg);

    if(status != HIPBLAS_STATUS_SUCCESS)
    {
        if(arg.N < 0 || arg.lda < arg.N)
        {
            EXPECT_EQ(HIPBLAS_STATUS_INVALID_VALUE, status);
        }
        else
        {
            EXPECT_EQ(HIPBLAS_STATUS_SUCCESS, status); // fail
        }
    }
}

TEST_P(getrf_gtest, getrf_npvt_gtest_double_complex)
{
    // GetParam returns a tuple. The setup routine unpacks the tuple
    // and initializes arg(Arguments), which will be passed to testing routine.

    Arguments arg = setup_getrf_arguments(GetParam());

    hipblasStatus_t status = testing_getrf_npvt<hipblasDoubleComplex>(arg);

    if(status != HIPBLAS_STATUS_SUCCESS)
    {
        if(arg.N < 0 || arg.lda < arg.N)
        {
            EXPECT_EQ(HIPBLAS_STATUS_INVALID_VALUE, status);
        }
        else
        {
            EXPECT_EQ(HIPBLAS_STATUS_SUCCESS, status); // fail
        }
    }
}

// notice we are using vector of vector
// so each elment in xxx_range is a vector,
// ValuesIn takes each element (a vector), combines them, and feeds them to test_p
// The combinations are  { {M, N, lda, ldb}, stride_scale, batch_count }

INSTANTIATE_TEST_SUITE_P(hipblasGetrf,
                         getrf_gtest,
                         Combine(ValuesIn(matrix_size_range),
                                 ValuesIn(stride_scale_range),
                                 ValuesIn(batch_count_range),
                                 ValuesIn(is_fortran)));

#endif
