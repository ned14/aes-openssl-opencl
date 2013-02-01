#!/bin/sh

benchmark -b aes-gpu -k 128 -n aes_vector_unroll_constant_local_roundkeys_local_tables_if --cache -s 1K   -o results/perf-128-opencl-K1.txt
benchmark -b aes-gpu -k 128 -n aes_vector_unroll_constant_local_roundkeys_local_tables_if --cache -s 2K   -o results/perf-128-opencl-K2.txt
benchmark -b aes-gpu -k 128 -n aes_vector_unroll_constant_local_roundkeys_local_tables_if --cache -s 4K   -o results/perf-128-opencl-K4.txt
benchmark -b aes-gpu -k 128 -n aes_vector_unroll_constant_local_roundkeys_local_tables_if --cache -s 8K   -o results/perf-128-opencl-K8.txt
benchmark -b aes-gpu -k 128 -n aes_vector_unroll_constant_local_roundkeys_local_tables_if --cache -s 16K  -o results/perf-128-opencl-K16.txt
benchmark -b aes-gpu -k 128 -n aes_vector_unroll_constant_local_roundkeys_local_tables_if --cache -s 32K  -o results/perf-128-opencl-K32.txt
benchmark -b aes-gpu -k 128 -n aes_vector_unroll_constant_local_roundkeys_local_tables_if --cache -s 64K  -o results/perf-128-opencl-K64.txt
benchmark -b aes-gpu -k 128 -n aes_vector_unroll_constant_local_roundkeys_local_tables_if --cache -s 128K -o results/perf-128-opencl-K128.txt
benchmark -b aes-gpu -k 128 -n aes_vector_unroll_constant_local_roundkeys_local_tables_if --cache -s 256K -o results/perf-128-opencl-K256.txt
benchmark -b aes-gpu -k 128 -n aes_vector_unroll_constant_local_roundkeys_local_tables_if --cache -s 512K -o results/perf-128-opencl-K512.txt
benchmark -b aes-gpu -k 128 -n aes_vector_unroll_constant_local_roundkeys_local_tables_if --cache -s 1M   -o results/perf-128-opencl-M1.txt
benchmark -b aes-gpu -k 128 -n aes_vector_unroll_constant_local_roundkeys_local_tables_if --cache -s 2M   -o results/perf-128-opencl-M2.txt
benchmark -b aes-gpu -k 128 -n aes_vector_unroll_constant_local_roundkeys_local_tables_if --cache -s 4M   -o results/perf-128-opencl-M4.txt
benchmark -b aes-gpu -k 128 -n aes_vector_unroll_constant_local_roundkeys_local_tables_if --cache -s 8M   -o results/perf-128-opencl-M8.txt
benchmark -b aes-gpu -k 128 -n aes_vector_unroll_constant_local_roundkeys_local_tables_if --cache -s 16M  -o results/perf-128-opencl-M16.txt
benchmark -b aes-gpu -k 128 -n aes_vector_unroll_constant_local_roundkeys_local_tables_if --cache -s 32M  -o results/perf-128-opencl-M32.txt
benchmark -b aes-gpu -k 128 -n aes_vector_unroll_constant_local_roundkeys_local_tables_if --cache -s 64M  -o results/perf-128-opencl-M64.txt
benchmark -b aes-gpu -k 128 -n aes_vector_unroll_constant_local_roundkeys_local_tables_if --cache -s 127M -o results/perf-128-opencl-M127.txt

benchmark -b aes-cpu -k 128 -s 1K   -o results/perf-128-openssl-K1.txt
benchmark -b aes-cpu -k 128 -s 2K   -o results/perf-128-openssl-K2.txt
benchmark -b aes-cpu -k 128 -s 4K   -o results/perf-128-openssl-K4.txt
benchmark -b aes-cpu -k 128 -s 8K   -o results/perf-128-openssl-K8.txt
benchmark -b aes-cpu -k 128 -s 16K  -o results/perf-128-openssl-K16.txt
benchmark -b aes-cpu -k 128 -s 32K  -o results/perf-128-openssl-K32.txt
benchmark -b aes-cpu -k 128 -s 64K  -o results/perf-128-openssl-K64.txt
benchmark -b aes-cpu -k 128 -s 128K -o results/perf-128-openssl-K128.txt
benchmark -b aes-cpu -k 128 -s 256K -o results/perf-128-openssl-K256.txt
benchmark -b aes-cpu -k 128 -s 512K -o results/perf-128-openssl-K512.txt
benchmark -b aes-cpu -k 128 -s 1M   -o results/perf-128-openssl-M1.txt
benchmark -b aes-cpu -k 128 -s 2M   -o results/perf-128-openssl-M2.txt
benchmark -b aes-cpu -k 128 -s 4M   -o results/perf-128-openssl-M4.txt
benchmark -b aes-cpu -k 128 -s 8M   -o results/perf-128-openssl-M8.txt
benchmark -b aes-cpu -k 128 -s 16M  -o results/perf-128-openssl-M16.txt
benchmark -b aes-cpu -k 128 -s 32M  -o results/perf-128-openssl-M32.txt
benchmark -b aes-cpu -k 128 -s 64M  -o results/perf-128-openssl-M64.txt
benchmark -b aes-cpu -k 128 -s 127M -o results/perf-128-openssl-M127.txt