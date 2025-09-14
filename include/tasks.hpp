#include <array>
#include <ctime>
#include <string>
#include <vector>
#include "instance.hpp"
#include "functions.hpp"
#include "items.hpp"

struct SeedResult {
    int tries;
    std::string seed;
    std::vector<int> legendariesPacks;
    std::vector<int> voucherAntes;
    std::vector<int> jokerShopIds;

    SeedResult(
        int tries, 
        std::string seed, 
        std::vector<int> legendariesPacks, 
        std::vector<int> voucherAntes, 
        std::vector<int> jokerShopIds
    ) {
        this->tries = tries;
        this->seed = seed;
        this->legendariesPacks = legendariesPacks;
        this->voucherAntes = voucherAntes;
        this->jokerShopIds = jokerShopIds;
    }
};

inline bool findJokerInFirstPack(Instance* inst, std::vector<std::string> searchedFor, bool negative, bool ankh) {
    if (ankh) {
        bool ankhFound = false;

        inst->nextPack(1);

        std::string packName = inst->nextPack(1);
        Pack pack = packInfo(packName);
        std::string packType = pack.type;
        int packSize = pack.size;

        if (packType == "Spectral Pack" && packSize == 2) {
            std::vector<std::string> cards = inst->nextSpectralPack(packSize, 1);

            for (int c = 0; c < packSize; c++) {
                std::string cardName = cards.at(c);
                if (cardName == "Ankh") {
                    ankhFound = true;
                    break;
                }
            }
        }

        if (!ankhFound) {
            return false;
        }
    }

    std::vector<std::string> toUnlock = {};

    for (int i = 0; i < 2; i++) {
        ShopItem item = inst->nextShopItem(1);

        if (item.type == "Joker") {
            std::string jokerName = item.jokerData.joker;
            inst->lock(jokerName);
            toUnlock.push_back(jokerName);
        }
    }

    std::vector<JokerData> jokers = inst->nextBuffoonPack(2, 1);
    bool jokerFound = false;

    for (int jokerIndex = 0; jokerIndex < jokers.size(); jokerIndex++) {
        JokerData joker = jokers.at(jokerIndex);
        std::string jokerName = joker.joker;
        std::string jokerEdition = joker.edition;

        if (negative && jokerEdition != "Negative") {
            continue;
        }

        if (std::find(searchedFor.begin(), searchedFor.end(), jokerName) != searchedFor.end()) {
            jokerFound = true;
            break;
        }
    }

    for (int i = 0; i < toUnlock.size(); i++) {
        inst->unlock(toUnlock.at(i));
    }

    return jokerFound;
}

inline std::vector<int> findLegendaries(Instance* inst, std::vector<std::string> searchedFor, int maxAnte, int* negatives, bool ankh) {
    int packsAmountFirstAnte = 3;
    if (ankh) {
        packsAmountFirstAnte -= 1;
    } else {
        inst->nextPack(1);
    }

    std::vector<int> legendariesPacks = {};

    for (int ante = 1; ante <= maxAnte; ante++) {
        int packsAmount = ante == 1 ? packsAmountFirstAnte : 6;

        for (int packIndex = 1; packIndex <= packsAmount; packIndex++) {
            std::string packName = inst->nextPack(ante);
            Pack pack = packInfo(packName);
            std::string packType = pack.type;
            int packSize = pack.size;

            if ((packType == "Arcana Pack" || packType == "Spectral Pack") && packSize <= 3) {
                std::vector<std::string> cards = 
                    packType == "Arcana Pack" 
                        ? inst->nextArcanaPack(packSize, ante) 
                        : inst->nextSpectralPack(packSize, ante);

                for (int c = 0; c < packSize; c++) {
                    std::string cardName = cards.at(c);

                    if (cardName == "The Soul") {
                        JokerData joker = inst->nextJoker("sou", ante, false);
                        std::string jokerName = joker.joker;
                        std::string jokerEdition = joker.edition;

                        if (std::find(searchedFor.begin(), searchedFor.end(), jokerName) != searchedFor.end()) {
                            inst->lock(jokerName);

                            if (jokerEdition == "Negative") {
                                (*negatives)++;
                            }
                            
                            legendariesPacks.push_back(ante);
                            legendariesPacks.push_back(packIndex);

                            if (legendariesPacks.size() >= searchedFor.size() * 2) {
                                return legendariesPacks;
                            }
                        }
                    }
                }
            }
        }
    }

    return legendariesPacks;
}

inline void initVoucherLocks(Instance* inst) {
    inst->lock("Overstock Plus");
    inst->lock("Liquidation");
    inst->lock("Glow Up");
    inst->lock("Reroll Glut");
    inst->lock("Omen Globe");
    inst->lock("Observatory");
    inst->lock("Nacho Tong");
    inst->lock("Recyclomancy");
    inst->lock("Tarot Tycoon");
    inst->lock("Planet Tycoon");
    inst->lock("Money Tree");
    inst->lock("Antimatter");
    inst->lock("Illusion");
    inst->lock("Petroglyph");
    inst->lock("Retcon");
    inst->lock("Palette");
}

inline std::vector<int> findVouchers(Instance* inst, std::vector<std::string> searchedFor, int maxAnte) {
    initVoucherLocks(inst);

    const std::array<std::string, 3> skipped = {"Tarot Tycoon", "Planet Merchant", "Magic Trick"};
    std::vector<int> voucherAntes = {};

    for (int ante = 2; ante <= maxAnte; ante++) {
        std::string voucher = inst->nextVoucher(ante);
        if (std::find(skipped.begin(), skipped.end(), voucher) != skipped.end()) {
            continue;
        }

        inst->activateVoucher(voucher);

        if (std::find(searchedFor.begin(), searchedFor.end(), voucher) != searchedFor.end()) {
            voucherAntes.push_back(ante);
        }

        if (voucherAntes.size() >= searchedFor.size()) {
            return voucherAntes;
        }

        if (voucher == "Hieroglyph" || voucher == "Petroglyph") {
            ante--;
        }
    }

    return voucherAntes;
}

inline std::vector<int> findJokersInShopQueue(Instance* inst, std::vector<std::string> searchedFor, int maxAnte) {
    std::vector<int> jokerShopIds = {};

    int queueLength = 12;
    int queueLengthInc = 6;

    for (int ante = 2; ante <= maxAnte; ante++) {
        for (int shopId = 1; shopId <= queueLength; shopId++) {
            ShopItem item = inst->nextShopItem(ante);

            if (item.type == "Joker") {
                std::string jokerName = item.jokerData.joker;

                auto jokerIter = std::find(searchedFor.begin(), searchedFor.end(), jokerName);
                if (jokerIter != searchedFor.end()) {
                    searchedFor.erase(jokerIter);
                    jokerShopIds.push_back(ante);
                    jokerShopIds.push_back(shopId);

                    if (searchedFor.empty()) {
                        return jokerShopIds;
                    }
                }
            }
        }
        
        queueLength += queueLengthInc;
    }

    return jokerShopIds;
}

inline SeedResult findSeed(
    int srandSeed,
    int triesBatch,
    std::vector<std::string> jokersPackToFindOneOf,
    bool jokerPackToFindNegative,
    bool jokerPackToFindAnkh,
    std::vector<std::string> legendariesToFind,
    int legendariesToFindNegatives,
    int legendariesToFindMaxAnte,
    std::vector<std::string> vouchersToFind,
    int vouchersToFindMaxAnte,
    std::vector<std::string> jokersShopToFind,
    int jokersShopToFindMaxAnte
) {
    const std::string alphabet = "ABCDEFGHIGKLMNOPQRSTUVWXYZ123456789";
    const int alphabetLen = alphabet.length();
    const int seedLen = 8;

    srand(srandSeed);

    int tries = 0;

    while (tries < triesBatch) {
        tries++;

        std::string seed = "";
        for (int i = 0; i < seedLen; i++) {
            seed.push_back(alphabet[rand() % alphabetLen]);
        }

        Instance* inst = new Instance(seed);
        inst->params.version = 10106;

        if (jokersPackToFindOneOf.size() > 0) {
            bool jokerFound = findJokerInFirstPack(inst, jokersPackToFindOneOf, jokerPackToFindNegative, jokerPackToFindAnkh);

            if (!jokerFound) {
                delete inst;
                continue;
            }
        }

        std::vector<int> legendariesPacks;
        if (legendariesToFind.size() > 0) {
            int legendariesNegatives = 0;
            legendariesPacks = findLegendaries(inst, legendariesToFind, legendariesToFindMaxAnte, &legendariesNegatives, jokerPackToFindAnkh);

            if (legendariesPacks.size() < legendariesToFind.size() * 2 || legendariesNegatives < legendariesToFindNegatives) {
                delete inst;
                continue;
            }
        } else {
            legendariesPacks = {};
        }

        std::vector<int> voucherAntes;
        if (vouchersToFind.size() > 0) {
            voucherAntes = findVouchers(inst, vouchersToFind, vouchersToFindMaxAnte);

            if (voucherAntes.size() < vouchersToFind.size()) {
                delete inst;
                continue;
            }
        } else {
            voucherAntes = {};
        }

        std::vector<int> jokerShopIds;
        if (jokersShopToFind.size() > 0) {
            jokerShopIds = findJokersInShopQueue(inst, jokersShopToFind, jokersShopToFindMaxAnte);

            if (jokerShopIds.size() < jokersShopToFind.size() * 2) {
                delete inst;
                continue;
            }
        } else {
            jokerShopIds = {};
        }

        delete inst;

        return SeedResult(tries, seed, legendariesPacks, voucherAntes, jokerShopIds);
    }

    return SeedResult(tries, "", {}, {}, {});
}