#!/bin/bash

cd build/release/clients/staging/
N=1024
./hipblas-bench --a_type bf16_r --b_type bf16_r --c_type bf16_r --d_type bf16_r --precision bf16_r --compute_type f32_r --function gemm_ex \
	-m $N -n $N -k $N --lda $N --ldb $N --ldc $N --ldd $N

