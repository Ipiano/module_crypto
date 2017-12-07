#include <string>
#include <unordered_map>

namespace adfgx
{
    class transformer
    {
        std::string _key;

        static const std::unordered_map<char, std::unordered_map<char, char>> DECRYPT_TABLE;
        static const std::unordered_map<char, std::string> ENCRYPT_TABLE;

    public:
        transformer(const std::string& key);
        std::string encrypt(const std::string& message);
        std::string decrypt(const std::string& cipher);
    };
}