#include "util.hpp"
#include "items.hpp"
#include "instance.hpp"
#include "functions.hpp"
#include "tasks.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
using namespace emscripten;
EMSCRIPTEN_BINDINGS(Immolate) {
    //instance.hpp
    register_vector<std::string>("VectorStr");
    register_vector<JokerData>("VectorJkr");
    register_vector<Card>("VectorCrd");
    class_<InstParams>("InstParams")
        .constructor<>()
        .constructor<std::string, std::string, bool, long>()
        .property("deck", &InstParams::deck)
        .property("stake", &InstParams::stake)
        .property("showman", &InstParams::showman)
        .property("vouchers", &InstParams::vouchers)
        .property("version", &InstParams::version);
    class_<Instance>("Instance")
        .constructor<std::string>()
        .function("get_node", &Instance::get_node)
        .function("random", &Instance::random)
        .function("randint", &Instance::randint)
        .function("randchoice", &Instance::randchoice)
        .property("params", &Instance::params)
        .property("seed", &Instance::seed)

        //functions.hpp
        .function("lock", &Instance::lock)
        .function("unlock", &Instance::unlock)
        .function("isLocked", &Instance::isLocked)
        .function("initLocks", &Instance::initLocks)
        .function("initUnlocks", &Instance::initUnlocks)
        .function("nextTarot", &Instance::nextTarot)
        .function("nextPlanet", &Instance::nextPlanet)
        .function("nextSpectral", &Instance::nextSpectral)
        .function("nextJoker", &Instance::nextJoker)
        .function("getShopInstance", &Instance::getShopInstance)
        .function("nextShopItem", &Instance::nextShopItem)
        .function("nextPack", &Instance::nextPack)
        .function("nextStandardCard", &Instance::nextStandardCard)
        .function("nextArcanaPack", &Instance::nextArcanaPack)
        .function("nextCelestialPack", &Instance::nextCelestialPack)
        .function("nextSpectralPack", &Instance::nextSpectralPack)
        .function("nextBuffoonPack", &Instance::nextBuffoonPack)
        .function("nextStandardPack", &Instance::nextStandardPack)
        .function("isVoucherActive", &Instance::isVoucherActive)
        .function("activateVoucher", &Instance::activateVoucher)
        .function("nextVoucher", &Instance::nextVoucher)
        .function("setDeck", &Instance::setDeck)
        .function("setStake", &Instance::setStake)
        .function("nextTag", &Instance::nextTag)
        .function("nextBoss", &Instance::nextBoss);
    function("packInfo", &packInfo);

    //items.hpp
    class_<ShopInstance>("ShopInstance")
        .constructor<>()
        .constructor<double, double, double, double, double>()
        .function("getTotalRate", &ShopInstance::getTotalRate)
        .property("jokerRate", &ShopInstance::jokerRate)
        .property("tarotRate", &ShopInstance::tarotRate)
        .property("planetRate", &ShopInstance::planetRate)
        .property("playingCardRate", &ShopInstance::playingCardRate)
        .property("spectralRate", &ShopInstance::spectralRate);
    class_<JokerStickers>("JokerStickers")
        .constructor<>()
        .constructor<bool, bool, bool>()
        .property("eternal", &JokerStickers::eternal)
        .property("perishable", &JokerStickers::perishable)
        .property("rental", &JokerStickers::rental);
    class_<JokerData>("JokerData")
        .constructor<>()
        .constructor<std::string, std::string, std::string, JokerStickers>()
        .property("joker", &JokerData::joker)
        .property("rarity", &JokerData::rarity)
        .property("edition", &JokerData::edition)
        .property("stickers", &JokerData::stickers);
    class_<ShopItem>("ShopItem")
        .constructor<>()
        .constructor<std::string, std::string>()
        .constructor<std::string, std::string, JokerData>()
        .property("type", &ShopItem::type)
        .property("item", &ShopItem::item)
        .property("jokerData", &ShopItem::jokerData);
    class_<WeightedItem>("WeightedItem")
        .constructor<std::string, double>()
        .property("item", &WeightedItem::item)
        .property("weight", &WeightedItem::weight);
    class_<Pack>("Pack")
        .constructor<std::string, int, int>()
        .property("type", &Pack::type)
        .property("size", &Pack::size)
        .property("choices", &Pack::choices);
    class_<Card>("Card")
        .constructor<std::string, std::string, std::string, std::string>()
        .property("base", &Card::base)
        .property("enhancement", &Card::enhancement)
        .property("edition", &Card::edition)
        .property("seal", &Card::seal);
    function("ENHANCEMENTS_GET", &ENHANCEMENTS_GET, allow_raw_pointers());
    function("CARDS_GET", &CARDS_GET, allow_raw_pointers());
    function("SUITS_GET", &SUITS_GET, allow_raw_pointers());
    function("RANKS_GET", &RANKS_GET, allow_raw_pointers());
    function("TAROTS_GET", &TAROTS_GET, allow_raw_pointers());
    function("PLANETS_GET", &PLANETS_GET, allow_raw_pointers());
    function("COMMON_JOKERS_GET", &COMMON_JOKERS_GET, allow_raw_pointers());
    function("UNCOMMON_JOKERS_GET", &UNCOMMON_JOKERS_GET, allow_raw_pointers());
    function("RARE_JOKERS_GET", &RARE_JOKERS_GET, allow_raw_pointers());
    function("LEGENDARY_JOKERS_GET", &LEGENDARY_JOKERS_GET, allow_raw_pointers());
    function("VOUCHERS_GET", &VOUCHERS_GET, allow_raw_pointers());
    function("SPECTRALS_GET", &SPECTRALS_GET, allow_raw_pointers());
    function("TAGS_GET", &TAGS_GET, allow_raw_pointers());
    function("BOSSES_GET", &BOSSES_GET, allow_raw_pointers());

    //util.hpp
    function("pseudohash", &pseudohash);
    class_<LuaRandom>("LuaRandom")
        .constructor<>()
        .constructor<double>()
        .function("random", &LuaRandom::random);

    // tasks.hpp
    register_vector<int>("VectorInt");
    class_<SeedResult>("SeedResult")
        .constructor<int, std::string, std::vector<int>, std::vector<int>, std::vector<int>>()
        .property("tries", &SeedResult::tries)
        .property("seed", &SeedResult::seed)
        .property("legendariesPacks", &SeedResult::legendariesPacks)
        .property("voucherAntes", &SeedResult::voucherAntes)
        .property("jokerShopIds", &SeedResult::jokerShopIds);
    function("findSeed", &findSeed);
}
#endif