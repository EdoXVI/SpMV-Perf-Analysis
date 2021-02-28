#include <chrono>
#include <cstring>
#include <iostream>
#include <vector>

#include "functions.hpp"
#include "options.hpp"
#include "utils.hpp"

using clock_type = std::chrono::high_resolution_clock;

/////////////////////////////
/////////////////////////////

// a is N x K;
// b is K x M;
// c is N x M;

// 2020-10-18, Version 1;
template <typename I, typename T>
void spmv_csr1(std::vector<I> &ptr, std::vector<I> &idx, std::vector<T> &val, std::vector<T> &vec, std::vector<T> &res) {
    for (int i = 0; i < ptr.size(); i++) {
        T result = 0;
        for (int j = ptr[i]; j < ptr[i + 1]; j++) {
            result += vec[idx[j]] * val[j];
        }
        res[i] = result;
    }
}

// 2020-10-18, Version 2;
#define PARTITIONS 16
#define INNER_PARTITIONS 2
template <typename I, typename T>
void spmv_inner(std::vector<I> &ptr, std::vector<I> &idx, std::vector<T> &val, std::vector<T> &vec, std::vector<T> &res, int start, int end) {
    for (int i = start; i < end; i++) {
        T result = 0;
        for (int j = ptr[i]; j < ptr[i + 1]; j++) {
            result += vec[idx[j]] * val[j];
        }
        res[i] = result;
    }
}

template <typename I, typename T>
void spmv_csr2(std::vector<I> &ptr, std::vector<I> &idx, std::vector<T> &val, std::vector<T> &vec, std::vector<T> &res) {
    std::thread threads[PARTITIONS];
    int N = ptr.size();
    for (int i = 0; i < PARTITIONS; i++) {
        int start = i * (N + PARTITIONS - 1) / PARTITIONS;
        int end = (i + 1) * (N + PARTITIONS - 1) / PARTITIONS;
        threads[i] = std::thread(spmv_inner<I, T>, std::ref(ptr), std::ref(idx), std::ref(val), std::ref(vec), std::ref(res), start, end);
    }
    for (int i = 0; i < PARTITIONS; i++) {
        threads[i].join();
    }
}

// 2020-10-18, Version 1;
template <typename I, typename T>
void spmv_coo1(std::vector<I> &x, std::vector<I> &y, std::vector<T> &val, std::vector<T> &vec, std::vector<T> &res) {
    I x_old = 0;
    T result = 0;
    for (int i = 0; i < x.size(); i++) {
        I x_curr = x[i];
        T res_tmp = val[i] * vec[y[i]];
        if (x_curr != x_old) {
            res[x_old] = result;
            x_old = x_curr;
            result = 0;
        }
        result += res_tmp;
    }
    // Final value;
    res[x_old] = result;
}

// 2021-02-08, Version 2;
template <typename I, typename T>
void spmv_coo2(std::vector<I> &x, std::vector<I> &y, std::vector<T> &val, std::vector<T> &vec, std::vector<T> &res) {
    for (int i = 0; i < x.size(); i++) {
        res[x[i]] += val[i] * vec[y[i]];
    }
}

/////////////////////////////
/////////////////////////////

int main(int argc, char *argv[]) {
    srand(12);

    Options options(argc, argv);
    int n = 0;
    int nnz = 0;
    int num_iter = options.num_iter;
    bool debug = options.debug;
    std::string dataPath = "./data/" + options.dataset + ".mtx";
    int isDirected = options.directed;
    int algorithm = options.algorithm;

    std::vector<int> x;
    std::vector<int> y;
    std::vector<float> val;
    std::vector<long> times(num_iter);

    readMtx<int, float>(dataPath.c_str(), &x, &y, &val, &n, &n, &nnz, &isDirected, false, options.debug, false, true);
    
    std::vector<float> vec(n);
    std::vector<float> res(n);

    std::vector<int> ptr(n);
    std::vector<int> idx(x.size());
    coo2csr(ptr.data(), idx.data(), val.data(), x.data(), y.data(), val.data(), (int)n, (int)n, (int)x.size());

    for (uint i = 0; i < num_iter; i++) {
        if (i == 0) { initialize_random_array(vec); }

        auto start = clock_type::now();
        if(isDirected == true)
            spmv_csr1(ptr, idx, val, vec, res);
        else
            spmv_coo2(x, y, val, vec, res);
        auto end = clock_type::now();
        times[i] = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        if(debug) { std::cout << "time=" << (float) times[i] / 1000 << " ms" << std::endl; }
    }

    float meanTime = mean<long, float>(times.data(), times.size());
    
    /*int old_precision = std::cout.precision();
    std::cout.precision(2);
    std::cout << "\n----------------" << std::endl;
    std::cout << options.dataset << std::endl;
    std::cout << ((isDirected == 1)? "Directed" : "Undirected") << std::endl;
    std::cout << "# of nnz = " << nnz << std::endl;
    std::cout << ((algorithm == 1)? "CSR" : "COO") << " mean execution time = " << meanTime / 1000.0 << "±" << st_dev<long, float>(times.data(), times.size()) / 1000 << " ms" << std::endl;
    std::cout << ((algorithm == 1)? "CSR" : "COO") << " performance = " << (int) (nnz/meanTime) << " Mnnz/s" << std::endl;
    std::cout << "----------------\n" << std::endl;
    std::cout.precision(old_precision);*/
    int old_precision = std::cout.precision();
    std::cout.precision(5);
    std::cout << meanTime / 1000.0 << " " << ((float)nnz)/meanTime << std::endl;
    std::cout.precision(old_precision);
}