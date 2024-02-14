#pragma once

#include <cstddef>
#include <vector>
#include <memory>
#include "archiver_constants.h"

namespace archiver {
class BinaryTrie {
    struct Node;

public:
    class Iterator {
    public:
        friend class BinaryTrie;
        Iterator() = default;
        explicit Iterator(Node* vertex);
        Iterator Next(int bit) const;
        Iterator Prev() const;
        bool operator==(const Iterator& oth) const;
        int GetPrevBit() const;
        ExtChar Chr() const;
        bool IsNull() const;

    private:
        Node* vertex_;
    };
    explicit BinaryTrie(ExtChar chr = -1);
    static BinaryTrie NewInternal(BinaryTrie& zeroth_child, BinaryTrie& first_child);
    Iterator Root();
    std::vector<bool> PathFromRoot(Iterator iterator);
    size_t LengthOfPathFromRoot(Iterator iterator);
    void AddChild(Iterator iterator, int bit);
    void SetTerminalChar(Iterator iterator, ExtChar chr);

private:
    struct Node {
        std::shared_ptr<Node> children_[2] = {nullptr, nullptr};
        std::weak_ptr<Node> parent_;
        ExtChar chr_ = -1;
    };
    std::shared_ptr<Node> root_ = std::make_shared<Node>();
};
}  // namespace archiver
