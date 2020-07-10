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

    struct Cell {
        Iterator iter;
        bool used = false, deleted = false;
    };

    std::vector<Cell> Table;

    size_t Size_of_Table; // always a power of 2

    size_t Number_of_Elements;

    Hash Big_Hash;

    size_t Mod_Hash(const KeyType& key, size_t ind) const {
        size_t hash_one = Big_Hash(key);
        size_t hash_two = hash_one;
        if (hash_two % 2 == 0) {
            hash_two += 1;
        }
        return (hash_one + ind * hash_two) % Size_of_Table;
    }

    std::list<std::pair<KeyType, ValueType>> Keys_and_Values;

    size_t find_cell_to_insert(const KeyType& key) {
        size_t ind = 0;
        while (ind != Size_of_Table && Table[Mod_Hash(key, ind)].used &&
                         !Table[Mod_Hash(key, ind)].deleted) {
            ++ind;
        }
        return Mod_Hash(key, ind);
    }

    size_t find_cell_to_erase(const KeyType& key) {
        size_t ind = 0;
        while (ind != Size_of_Table && Table[Mod_Hash(key, ind)].used) {
            if (!Table[Mod_Hash(key, ind)].deleted &&
                                Table[Mod_Hash(key, ind)].iter->first == key) {
                return Mod_Hash(key, ind);
            }
            ++ind;
        }
        throw std::out_of_range("");
    }

    void simple_insert(const std::pair<KeyType, ValueType>& key_and_value) {
        Keys_and_Values.push_front(key_and_value);
        size_t ind = find_cell_to_insert(key_and_value.first);
        Table[ind].used = true;
        Table[ind].deleted = false;
        Table[ind].iter = Keys_and_Values.begin();
        ++Number_of_Elements;
    }

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
    : Big_Hash(hash)
    , Size_of_Table(1)
    , Number_of_Elements(0)
    , Table(1) {
        while (begin != end) {
            insert(*begin++);
        }
    }

    HashMap(std::initializer_list<std::pair<KeyType, ValueType>> keys_values,
                                    Hash hash = Hash())
    : Big_Hash(hash)
    , Size_of_Table(1)
    , Number_of_Elements(0)
    , Table(1) {
        for (const auto& key_and_value : keys_values) {
            insert(key_and_value);
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
        size_t ind = 0;
        while (ind != Size_of_Table && Table[Mod_Hash(key, ind)].used) {
            if (!Table[Mod_Hash(key, ind)].deleted &&
                                Table[Mod_Hash(key, ind)].iter->first == key) {
                return Table[Mod_Hash(key, ind)].iter;
            }
            ++ind;
        }
        return Keys_and_Values.cend();
    }

    iterator find(const KeyType& key) {
        size_t ind = 0;
        while (ind != Size_of_Table && Table[Mod_Hash(key, ind)].used) {
            if (!Table[Mod_Hash(key, ind)].deleted &&
                                Table[Mod_Hash(key, ind)].iter->first == key) {
                return Table[Mod_Hash(key, ind)].iter;
            }
            ++ind;
        }
        return Keys_and_Values.end();
    }

    void insert(const std::pair<KeyType, ValueType>& key_and_value) {
        if (find(key_and_value.first) == Keys_and_Values.end()) {
            if (Number_of_Elements > Size_of_Table / 2) {
                std::list<std::pair<KeyType, ValueType>> tmp_list(std::move(Keys_and_Values));
                Keys_and_Values.clear();
                Size_of_Table = 2 * Size_of_Table;
                Table.clear(); 
                Table.resize(Size_of_Table);
                Number_of_Elements = 0;
                for (const auto& elem : tmp_list) {
                    simple_insert(elem);
                }
            }
            simple_insert(key_and_value);
        }
    }

    void erase(const KeyType& key) {
        auto it = find(key);
        if (it != Keys_and_Values.end()) {
            size_t ind = find_cell_to_erase(key);
            Table[ind].deleted = true;
            Keys_and_Values.erase(it);
            --Number_of_Elements;
            if (Number_of_Elements < Size_of_Table / 8) {
                Size_of_Table = Size_of_Table / 2;
                Table.clear(); 
                Table.resize(Size_of_Table);
                Table.shrink_to_fit();
                std::list<std::pair<KeyType, ValueType>> tmp_list(std::move(Keys_and_Values));
                Keys_and_Values.clear();
                Number_of_Elements = 0;
                for (const auto& elem : tmp_list) {
                    simple_insert(elem);
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
