#include<functional>
#include<utility>
#include<vector>
#include<list>
#include<stdexcept>


template<typename KeyType, typename ValueType, typename Hash = std::hash<KeyType>>
class HashMap {
private:
    using  Iterator = typename std::list<std::pair<KeyType, ValueType>>::iterator;
    using  cIterator = typename std::list<std::pair<KeyType, ValueType>>::const_iterator;
    std::vector<std::list<Iterator>> Table;
    size_t Size_of_Table, Number_of_Elements;
    Hash Big_Hash;
    size_t Mod_Hash(const KeyType& key) const {
        return Big_Hash(key) % Size_of_Table;
    }
    std::list<std::pair<KeyType, ValueType>> Keys_and_Values;

public:
    typedef typename std::list<std::pair<KeyType, ValueType>>::iterator iterator;
    typedef typename std::list<std::pair<KeyType, ValueType>>::const_iterator const_iterator;
    HashMap(Hash hash = Hash())
    : Big_Hash(hash)
    , Size_of_Table(1)
    , Number_of_Elements(0)
    , Table(1) {}

    

    template<typename Iter>
    HashMap(Iter begin, Iter end, Hash hash = Hash())
    : Big_Hash(hash) {
        std::vector<std::pair<KeyType, ValueType>> keys_values(begin, end);
        Number_of_Elements = keys_values.size();
        Size_of_Table = 2 * Number_of_Elements;
        Table.resize(Size_of_Table);
        for (const auto& key_and_value : keys_values) {
            Keys_and_Values.push_front(key_and_value);
            Table[Mod_Hash(key_and_value.first)].push_back(Keys_and_Values.begin());
        }
    }

    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> keys_values,
                                    Hash hash = Hash())
    : Big_Hash(hash) {
        Number_of_Elements = keys_values.size();
        Size_of_Table = 2 * Number_of_Elements;
        Table.resize(Size_of_Table);
        for (const auto& key_and_value : keys_values) {
            Keys_and_Values.push_front(key_and_value);
            Table[Mod_Hash(key_and_value.first)].push_back(Keys_and_Values.begin());
        }
    }

    size_t size() const {
        return Number_of_Elements;
    }

    bool empty() const {
        return Number_of_Elements == 0;
    }

    Hash hash_function() const {
        return Big_Hash;
    }

    const_iterator find(const KeyType& key) const {
        for (const auto& it : Table[Mod_Hash(key)]) {
            if (it->first == key) {
                return it;
            }
        }
        return Keys_and_Values.cend();
    }

    iterator find(const KeyType& key) {
        for (const auto& it : Table[Mod_Hash(key)]) {
            if (it->first == key) {
                return it;
            }
        }
        return Keys_and_Values.end();
    }

    void insert(const std::pair<KeyType, ValueType>& key_and_value) {
        if (find(key_and_value.first) == Keys_and_Values.end()) {
            if (Number_of_Elements > Size_of_Table - 1) {
                std::list<std::pair<KeyType, ValueType>> tmp_list(std::move(Keys_and_Values));
                Keys_and_Values.clear();
                Size_of_Table = 2 * Size_of_Table;
                Table.clear(); 
                Table.resize(Size_of_Table);
                for (const auto& elem : tmp_list) {
                    Keys_and_Values.push_front(elem);
                    Table[Mod_Hash(elem.first)].push_back(Keys_and_Values.begin());
                }
            }
            Keys_and_Values.push_front(key_and_value);
            Table[Mod_Hash(key_and_value.first)].push_back(Keys_and_Values.begin());
            ++Number_of_Elements;
        }
    }

    void erase(const KeyType& key) {
        auto it = find(key);
        if (it != Keys_and_Values.end()) {
            Table[Mod_Hash(key)].remove(it);
            Keys_and_Values.erase(it);
            --Number_of_Elements;
            if (Number_of_Elements < Size_of_Table / 4) {
                Size_of_Table = Size_of_Table / 2;
                Table.clear(); 
                Table.resize(Size_of_Table);
                Table.shrink_to_fit();
                std::list<std::pair<KeyType, ValueType>> tmp_list(std::move(Keys_and_Values));
                Keys_and_Values.clear();
                for (const auto& elem : tmp_list) {
                    Keys_and_Values.push_front(elem);
                    Table[Mod_Hash(elem.first)].push_back(Keys_and_Values.begin());
                }
            }
        }
    }

    iterator begin() {
         return Keys_and_Values.begin();
    }

    const_iterator begin() const {
         return Keys_and_Values.cbegin();
    }

    iterator end() {
         return Keys_and_Values.end();
    }

    const_iterator end() const {
         return Keys_and_Values.cend();
    }

    ValueType& operator[](const KeyType& key) {
        iterator it = find(key);
        if (it == Keys_and_Values.end()) {
            insert(std::pair<KeyType, ValueType>(key, ValueType()));
            it = find(key);
        }
        return it->second;
    }

    const ValueType& at(const KeyType& key) const {
        const_iterator it = find(key);
        if (it == Keys_and_Values.cend()) {
            throw std::out_of_range("");
        }
        return it->second;
    }

    void clear() {
        Table.clear();
        Table.resize(1);
        Table.shrink_to_fit();
        Keys_and_Values.clear();
        Number_of_Elements = 0;
        Size_of_Table = 1;
    }
};
