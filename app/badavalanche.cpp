#include <iostream>
#include <cstdint>
#include <cmath>


using namespace std;



uint64_t hash_thomas_wang(uint64_t key, uint64_t mask)
{
	key = (~key + (key << 21)) & mask; // key = (key << 21) - key - 1;
	key = key ^ key >> 24;
	key = ((key + (key << 3)) + (key << 8)) & mask; // key * 265
	key = key ^ key >> 14;
	key = ((key + (key << 2)) + (key << 4)) & mask; // key * 21
	key = key ^ key >> 28;
	key = (key + (key << 31)) & mask;
	return key;
}


uint64_t hash_best_known(uint64_t key)
{
    key ^= key >> 16;
    key = (key * 0x21f0aaad) & 0xFFFFFFFF;
    key ^= key >> 15;
    key = (key * 0xd35a2d97) & 0xFFFFFFFF;
    key ^= key >> 15;

    return key;
}





int main()
{
    uint64_t matrix[32][32];
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            matrix[i][j] = 0;
        }
    }
    // 32 bits mask
    uint64_t mask = 0xFFFFFFFF;
    uint64_t total_tests = 0;

    // Compte the matrix for the cascading criterion
    for (uint64_t val = 0; val < 0xFFFFFFFF; val++)
    {
        uint64_t hash = hash_best_known(val);

        // Mutate the i value
        for (int mut_idx = 0; mut_idx < 32; mut_idx++)
        {
            uint64_t mut_val = val ^ (1 << mut_idx);
            uint64_t mut_hash = hash_best_known(mut_val); 
            
            // Compute the difference
            uint64_t diff = hash ^ mut_hash;
            for (int bit_idx = 0; bit_idx < 32; bit_idx++)
            {
                if (diff & (1 << bit_idx))
                {
                    matrix[mut_idx][bit_idx]++;
                }
            }
            total_tests++;
        }

        if (val % 1000000 == 0)
        {
            cout << (100.0 * static_cast<double>(val+1) / static_cast<double>(0xFFFFFFFF)) << "% completed" << endl;
        }
    }

    // Xi² average test
    double mean {0};
    double const expected_diff {static_cast<double>(total_tests) / static_cast<double>(32 * 2.0)};
    for (size_t i{0} ; i<32 ; i++)
    {
        for (size_t j{0} ; j<32 ; j++)
        {
            std::cout << "[" << matrix[i][j] << "]";
            double const diff {(static_cast<double>(matrix[i][j]) - expected_diff) / expected_diff};
            mean += (diff * diff) / static_cast<double>(32 * 32);
        }
        std::cout << std::endl;
    }

    // Return the percentage of bits that changed
    cout << "cascading criterion: " <<  sqrt(mean) << endl;

    return 0;
}