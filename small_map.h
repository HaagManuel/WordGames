#pragma once

#include <vector>

// keeps element sorted by key
template <typename Key, typename Value>
struct SmallSortedMap
{
    SmallSortedMap() {}

    // inserts element into map if key is not present, otherwise does nothing
    // returns if value was inserted and the current value in the map
    std::pair<Value, bool> insert_if_not_present(Key key, Value value)
    {
        auto it = arr.begin();
        for (; it != arr.end(); it++)
        {
            auto [k, v] = *it;
            if (k == key)
            {
                return {v, false};
            }
            else if (k > key)
            {
                break;
            }
        }
        arr.insert(it, {key, value});
        return {value, true};
    }

    // returns {value, true} if key is present, {default, false} otherwise
    std::pair<Value, bool> get_value_if_key_present(Key key) const
    {
        for (auto [k, v] : arr)
        {
            if (k == key)
            {
                return {v, true};
            }
        }
        return {Value(), false};
    }

    inline int size() const { return arr.size(); }
    std::vector<std::pair<Key, Value>> arr;
};

template <typename Key, typename Value>
struct SmallUnsortedMap
{
    SmallUnsortedMap() {}

    // inserts element into map if key is not present, otherwise does nothing
    // returns if value was inserted and the current value in the map
    std::pair<Value, bool> insert_if_not_present(Key key, Value value)
    {
        for (auto [k, v] : arr)
        {
            if (k == key)
            {
                return {v, false};
            }
        }
        arr.emplace_back(key, value);
        return {value, true};
    }

    // returns {value, true} if key is present, {default, false} otherwise
    std::pair<Value, bool> get_value_if_key_present(Key key) const
    {
        for (auto [k, v] : arr)
        {
            if (k == key)
            {
                return {v, true};
            }
        }
        return {Value(), false};
    }

    inline int size() const { return arr.size(); }

    std::vector<std::pair<Key, Value>> arr;
};