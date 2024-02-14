#pragma once

#include <vector>
#include <functional>

namespace archiver {
template <typename T, class Comparator = std::less<T>>
class MinHeap {
public:
    void Push(const T& val);
    const T& Top() const;
    void Pop();
    size_t Size() const;

private:
    void SiftUp(size_t pos);
    void SiftDown(size_t pos);
    std::vector<T> data_ = std::vector<T>(1);
    Comparator cmp_ = Comparator();
};
template <typename T, class Comparator>
void MinHeap<T, Comparator>::SiftUp(size_t pos) {
    while (pos > 1 && cmp_(data_[pos], data_[pos / 2])) {
        std::swap(data_[pos / 2], data_[pos]);
        pos /= 2;
    }
}
template <typename T, class Comparator>
void MinHeap<T, Comparator>::SiftDown(size_t pos) {
    while (pos * 2 < data_.size()) {
        size_t smaller_son = pos * 2;
        size_t bigger_son = pos * 2 + 1;
        if (bigger_son < data_.size() && cmp_(data_[bigger_son], data_[smaller_son])) {
            std::swap(smaller_son, bigger_son);
        }
        if (cmp_(data_[pos], data_[smaller_son])) {
            break;
        }
        std::swap(data_[pos], data_[smaller_son]);
        pos = smaller_son;
    }
}
template <typename T, class Comparator>
void MinHeap<T, Comparator>::Push(const T& val) {
    data_.template emplace_back(val);
    SiftUp(data_.size() - 1);
}
template <typename T, class Comparator>
const T& MinHeap<T, Comparator>::Top() const {
    return data_[1];
}
template <typename T, class Comparator>
void MinHeap<T, Comparator>::Pop() {
    std::swap(data_[1], data_.back());
    data_.pop_back();
    SiftDown(1);
}
template <typename T, class Comparator>
size_t MinHeap<T, Comparator>::Size() const {
    return data_.size() - 1;
}
}  // namespace archiver
