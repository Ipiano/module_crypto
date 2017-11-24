#include <array>
#include <functional>
#include <utility>

namespace des4
{
    namespace _internal
    {
        typedef std::array<uint8_t, 16> S_BOX;

        const std::array<S_BOX, 2> S_BOXES
        {{
            {{0b101, 0b010, 0b001, 0b110, 0b011, 0b100, 0b111, 0b000, 
              0b001, 0b100, 0b110, 0b010, 0b000, 0b111, 0b101, 0b011}},

            {{0b100, 0b000, 0b110, 0b101, 0b111, 0b001, 0b011, 0b010,
              0b101, 0b011, 0b000, 0b111, 0b110, 0b010, 0b001, 0b100}}
        }};

        uint8_t expand(const uint8_t& n);
        uint8_t next_key(uint16_t& key);
        uint8_t prev_key(uint16_t& key);        
        uint8_t F(uint8_t r, const uint8_t& key);
        uint16_t round(const uint16_t& block, const uint8_t& key);
    }

    uint16_t encrypt(uint16_t block, uint16_t key, const uint64_t& rounds=4);
    uint16_t decrypt(uint16_t block, uint16_t key, const uint64_t& rounds=4);

    uint16_t crack3(std::function<uint16_t(uint16_t)> des3);
    uint16_t crack4(std::function<uint16_t(uint16_t)> des4);    
}