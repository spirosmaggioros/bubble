/**
* @brief Implementation of the bubble data structure, a data structure that is highly influenced
* from the fibonacci heap structure. Bubble uses an array and avl_trees to store elements and
* fastly retreieve or insert new ones. Insertion/removal time complexity is O(logn*logm) where n
* is the size of the initial array(or the input bubble size) and m is the number of nodes at the index
* that the bubble is going to search
*/

#ifndef BUBBLE_H
#define BUBBLE_H

#ifdef __cplusplus
#include <iostream>
#include <vector>
#include <optional>
#include <ranges>
#include <algorithm>
#include <utility>
#include <cassert>
#include "avl_tree.hpp"
#endif

/**
* @brief implementation of bubble<T, SIZE>
*/
template <typename T, size_t _SIZE>
class bubble {
private:
    std::vector<std::pair<T, std::optional<avl_tree<T>>>> list;
    size_t _size;

public:
    /**
    * @brief default constructor of bubble
    */
    explicit bubble() noexcept : _size(0) { }

    /**
    * @brief copy constructor of bubble
    * @param t: const& bubble<T, _NEW_SIZE>: the new bubble
    */
    template <size_t _NEW_SIZE>
    bubble(const bubble<T, _NEW_SIZE> &t) noexcept : _size(t._size) {
        try {
            if(_NEW_SIZE != _SIZE) { throw std::logic_error("Tried to copy bubbles with different sizes"); }
            this->list = t.list;
        }
        catch (std::logic_error &e){
            std::cerr << e.what() << '\n';
        }
    }

    /**
    * @brief insert function for bubble
    * @param Args: the keys you want to insert. You can insert as many as you like
    * bubble.insert(1, 2, 3, 4, ...)
    */
    template <typename... Args>
    void insert(Args ...keys);

    /**
    * @brief remove function for bubble
    * @param Args: the keys you want to remove. You can remove as many as you like
    * bubble.remove(1, 2, 3, 4, ...)
    */
    template <typename... Args>
    void remove(Args ...keys);

    /**
    * @brief search function for bubble
    * @param key: the key you want to search
    * @return true: if key exists in the bubble
    * @return false: otherwise
    */
    bool search(const T& key);

    /**
    * @brief size function for bubble
    * @return size_t: the size of the bubble
    */
    size_t size();

    /**
    * @brief empty function for bubble
    * @return true: if bubble is empty
    * @return false: otherwise
    */
    bool empty();

    /**
    * @brief operator [] for bubble
    * @param index: const& size_t, the passed index value
    * @return std::vector<T>: the elements in-order of the passed index
    */
    std::vector<T> operator[] (const size_t& index) const {
        assert(index < this->_size && index >= 0);
        if(this->list[index].second == std::nullopt) { return {this->list[index].first}; }
        return this->list[index].second.value().inorder();
    }

    /**
    * @brief operator << for bubble
    */
    friend std::ostream & operator << (std::ostream &out, const bubble<T, _SIZE> &t){
        if(t._size == 0) { return out; }
        for(auto && x : t.list) {
            out << x.first << ": {";
            if(x.second == std::nullopt){
                out << "}" << '\n';
                continue;
            }
            avl_tree<T> tmp_tree(x.second.value());
            std::vector<T> ino = tmp_tree.inorder();
            for(size_t i = 0; i<ino.size(); i++){
                if(i == ino.size() - 1) {
                    out << ino[i];
                }
                else{
                    out << ino[i] << " ";
                }
            }
            out << "}" << '\n';
        }
        return out;
    }
};

template <typename T, size_t _SIZE>
template <typename... Args>
inline void bubble<T, _SIZE>::insert(Args ...keys) {
    auto _insert = [&](const T&& key) -> void {
        if(_size < _SIZE) {
            list.push_back({key, std::nullopt});
            _size++;
            return;
        }
        if(_size == _SIZE) {
            std::ranges::sort(list, [](const std::pair<T, std::optional<avl_tree<T>>> &a, const std::pair<T, std::optional<avl_tree<T>>> &b){
                return a.first < b.first;
            });
        }

        auto it = std::lower_bound(std::ranges::begin(this->list), std::ranges::end(this->list), key, [](const std::pair<T, std::optional<avl_tree<T>>> &pair, const T &key){ return pair.first < key; });

        if(it != std::ranges::end(this->list)) {
            int idx = std::distance(std::ranges::begin(this->list), it);
            if(this->list[idx].first == key) { return; }

            if(idx - 1 < 0){
                if(this->list[0].second == std::nullopt) {
                    this->list[0].second = avl_tree<T>();
                }
                // if(this->list[0].first == key) { return; } I think we don't need this
                this->list[0].second.value().insert(key);
            }
            else{
                if(this->list[idx - 1].second == std::nullopt) {
                    this->list[idx - 1].second = avl_tree<T>();
                }
                // if(this->list[idx].first == key) { return; }
                this->list[idx - 1].second.value().insert(key);
            }
        }
        else {
            if(this->list[this->list.size() - 1].second == std::nullopt){
                this->list[this->list.size() - 1].second = avl_tree<T>();
            }
            // if(this->list[this->list.size() - 1].first == key) { return; }
            this->list[this->list.size() - 1].second.value().insert(key);
        }
        _size++;
    };
    (std::invoke(_insert, std::forward<Args>(keys)), ...);
}


template <typename T, size_t _SIZE>
template <typename... Args>
void bubble<T, _SIZE>::remove(Args ...keys) {
    auto _remove = [&](const T&& key) -> void{
        if(this->_size == 0) { return; }
        if(this->_size <= _SIZE) {
            auto [begin, end] = std::ranges::remove_if(this->list, [&](const std::pair<T, std::optional<avl_tree<T>>> &t) { return t.first == key; });
            list.erase(begin, end);
            _size--;
        }
        else{
            auto it = std::lower_bound(std::ranges::begin(this->list), std::ranges::end(this->list), key, [](const std::pair<T, std::optional<avl_tree<T>>> &pair, const T &key){ return pair.first < key; });
            if(it != std::ranges::end(this->list)) {
                size_t idx = std::ranges::distance(std::ranges::begin(this->list), it);

                if(this->list[idx].first == key && this->list[idx].second != std::nullopt) {
                    T curr_root = this->list[idx].second.value().get_root();
                    this->list[idx].second.value().remove(curr_root);
                    this->list[idx].first = curr_root;
                    return;
                }

                if(idx - 1 < 0){
                    if(this->list[0].second == std::nullopt){
                        return;
                    }
                    this->list[0].second.value().remove(key);
                }
                else{
                    if(this->list[idx - 1].second == std::nullopt) {
                        return;
                    }
                    this->list[idx - 1].second.value().remove(key);
                }
            }
            else {
                if(this->list[this->list.size() - 1].second == std::nullopt) {
                    return;
                }
                this->list[this->list.size() - 1].second.value().remove(key);
            }
            _size--;
        }
    };
    (std::invoke(_remove, std::forward<Args>(keys)), ...);
}

template <typename T, size_t _SIZE>
bool bubble<T, _SIZE>::search(const T& key) {
    if(this->_size == 0) { return false; }
    auto it = std::lower_bound(std::ranges::begin(this->list), std::ranges::end(this->list), key, [](const std::pair<T, std::optional<avl_tree<T>>> &pair, const T &key){ return pair.first < key; });
    if (it != std::ranges::end(this->list)){
        size_t idx = std::ranges::distance(std::ranges::begin(this->list), it);
        if(idx - 1 < 0){
            if(this->list[0].first == key) { return true; }
            if(this->list[0].second == std::nullopt) { return false; }
            if(this->list[0].second.value().search(key) == true) { return true; }
        }
        else {
            if(this->list[idx].first == key) { return true; }
            if(this->list[idx - 1].second == std::nullopt) { return false; }
            if(this->list[idx - 1].second.value().search(key) == true) { return true; }
        }
    }
    else {
        if(this->list[this->list.size() - 1].first == key) { return true; }
        if(this->list[this->list.size() - 1].second == std::nullopt) { return false; }
        if(this->list[this->list.size() - 1].second.value().search(key) == true) { return true; }
    }
    return false;
}

template <typename T, size_t _SIZE>
size_t bubble<T, _SIZE>::size() {
    return this->_size;
}

template <typename T, size_t _SIZE>
bool bubble<T, _SIZE>::empty() {
    return this->_size == 0;
}

#endif
