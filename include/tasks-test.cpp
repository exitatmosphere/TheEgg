#include "tasks.hpp"
#include <cstdio>
#include <ctime>
#include <iostream>
#include <ostream>

int main() {
    SeedResult sr = perkTribVouchers(time(NULL), 0, {}, 0);

    std::cout << "Tries: " << sr.tries << std::endl;
    std::cout << "Seed: " << sr.seed.c_str() << std::endl;

    std::array<int, 4> legendariesPacks = sr.legendariesPacks;
    std::cout << "Legendaties packs: " << std::endl;
    std::cout << "------------------------------" << std::endl;
    for (int i = 0; i < legendariesPacks.size(); i += 2) {
        std::cout << "Ante: " << legendariesPacks[i]  << " Pack: " << legendariesPacks[i + 1] << std::endl;
    }
    std::cout << "------------------------------" << std::endl;

    std::vector<int> voucherAntes = sr.voucherAntes;
    std::cout << "Voucher antes:";
    for (int i = 0; i < voucherAntes.size(); i ++) {
        std::cout << " " << voucherAntes[i];
    }
    std::cout << std::endl;

    std::cout << "Time: " << sr.time << std::endl;
}