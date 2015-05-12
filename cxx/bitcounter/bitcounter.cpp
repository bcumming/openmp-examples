#include <chrono>
#include <iostream>
#include <random>

#include <omp.h>

using word = uint16_t;
using byte = unsigned char;

using clock_type    = std::chrono::high_resolution_clock;
using duration_type = std::chrono::duration<double>;

byte count_bits_in_word(word value) {
    word mask = 1;
    byte count = 0;

    while(mask) {
        count += (mask & value) ? 1 : 0;
        mask <<= 1;
    }

    return count;
}

size_t count_naiive(std::vector<word> &v) {
    size_t count = 0;
    const auto n = v.size();

    #pragma omp parallel for reduction(+:count)
    for(size_t i=0; i<n; ++i) {
        count += count_bits_in_word(v[i]);
    }

    return count;
}

size_t count_lookup_16(std::vector<word> const& v, std::vector<byte> const& table) {
    size_t count = 0;
    const auto n = v.size();

    #pragma omp parallel for reduction(+:count)
    for(size_t i=0; i<n; ++i) {
        count += table[v[i]];
    }

    return count;
}

size_t count_lookup_8(std::vector<word> const& v, std::vector<byte> const& table) {
    size_t count = 0;
    size_t size = v.size()*2;
    const byte* as_bytes = reinterpret_cast<const byte*>(&v[0]);

    #pragma omp parallel for reduction(+:count)
    for(size_t i=0; i<size; ++i) {
        count += table[as_bytes[i]];
    }

    return count;
}

// generate lookup table for bit counting
// generates bit counts for indexes in the range [0, size]
std::vector<byte> gen_table(word size) {
    std::vector<byte> table(size+1);

    table[0] = 0;
    size_t pos = 1;
    for(word lim = 1; lim<size && lim; lim<<=1) {
        for(auto i=0; i<lim; ++i) {
            table[pos] = table[i] + 1;
            pos++;
        }
    }

    return table;
}

std::vector<word> generate_random_sample(size_t n) {
    std::random_device rd;
    std::mt19937 generator(rd());
    auto max_value = std::numeric_limits<word>::max();
    std::uniform_int_distribution<word> distribution(0,max_value);

    std::vector<word> values(n);
    for(auto &v : values) {
        v = distribution(generator);
    }

    return values;
}

template <class T>
struct result_type {
    double time_taken;
    T result;
};

template <typename F, class ...Args>
auto time_call(F f, Args... args) -> result_type<decltype(f(args...))> {
    using return_type = decltype(f(args...));
    auto start_ = clock_type::now();
    auto result = f(args...);
    auto time_taken = duration_type(clock_type::now() - start_).count();
    std::cout << time_taken << " seconds" << std::endl;
    return result_type<return_type>{time_taken, result};
}

int main(int argc, char** argv) {

    auto shift = 12;
    if(argc>1) {
        shift = std::stoi(argv[1]);
    }
    // set problem size
    const size_t n = 1<<shift;
    std::cout << "count bits in list of " << n << " 16-bit integers" << std::endl;

    auto max_threads = omp_get_max_threads();

    // generate random input sample
    auto values = generate_random_sample(n);

    // generate the lookup tables
    auto table_16 = gen_table(std::numeric_limits<word>::max());
    auto table_8  = gen_table(std::numeric_limits<byte>::max());

    int threads[] = {1, 2, 4, 8};
    for(auto num_threads : threads) {
        omp_set_num_threads(num_threads);
        std::cout << "==== " << num_threads << " threads" << std::endl;

        std::cout << "naiive           : ";
        auto result_naiive = time_call(count_naiive, values);

        std::cout << "16 bit table     : ";
        auto result_16     = time_call(count_lookup_16,     values, table_16);

        std::cout << " 8 bit table     : ";
        auto result_8      = time_call(count_lookup_8,      values, table_8);
    }

    return 0;
}
