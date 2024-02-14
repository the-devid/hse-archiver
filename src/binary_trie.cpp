//
// Created by denis on 21.10.22.
//

#include "binary_trie.h"

bool archiver::BinaryTrie::Iterator::operator==(const archiver::BinaryTrie::Iterator& oth) const {
    return vertex_ == oth.vertex_;
}
archiver::BinaryTrie::Iterator::Iterator(Node* vertex) : vertex_(vertex) {
}
archiver::BinaryTrie::Iterator archiver::BinaryTrie::Iterator::Next(int bit) const {
    return Iterator(vertex_->children_[bit].get());
}
archiver::BinaryTrie::Iterator archiver::BinaryTrie::Iterator::Prev() const {
    return archiver::BinaryTrie::Iterator(vertex_->parent_.lock().get());
}
int archiver::BinaryTrie::Iterator::GetPrevBit() const {
    return vertex_->parent_.lock().get()->children_[1].get() == vertex_;
}
archiver::ExtChar archiver::BinaryTrie::Iterator::Chr() const {
    return vertex_->chr_;
}
bool archiver::BinaryTrie::Iterator::IsNull() const {
    return vertex_ == nullptr;
}
archiver::BinaryTrie::BinaryTrie(archiver::ExtChar chr) : root_(std::make_shared<Node>()) {
    root_->chr_ = chr;
}
archiver::BinaryTrie archiver::BinaryTrie::NewInternal(archiver::BinaryTrie& zeroth_child,
                                                       archiver::BinaryTrie& first_child) {
    BinaryTrie result;
    result.root_->children_[0] = zeroth_child.root_;
    result.root_->children_[1] = first_child.root_;
    if (zeroth_child.root_) {
        zeroth_child.root_->parent_ = result.root_;
    }
    if (first_child.root_) {
        first_child.root_->parent_ = result.root_;
    }
    return result;
}
archiver::BinaryTrie::Iterator archiver::BinaryTrie::Root() {
    return Iterator(root_.get());
}
std::vector<bool> archiver::BinaryTrie::PathFromRoot(archiver::BinaryTrie::Iterator iterator) {
    std::vector<bool> result;
    while (iterator != Root()) {
        int prev_bit = iterator.GetPrevBit();
        result.push_back(prev_bit);
        iterator = iterator.Prev();
    }
    return result;
}
size_t archiver::BinaryTrie::LengthOfPathFromRoot(archiver::BinaryTrie::Iterator iterator) {
    size_t result = 0;
    while (iterator != Root()) {
        ++result;
        iterator = iterator.Prev();
    }
    return result;
}
void archiver::BinaryTrie::AddChild(archiver::BinaryTrie::Iterator iterator, int bit) {
    if (iterator.vertex_->children_[bit]) {
        return;
    }
    iterator.vertex_->children_[bit] = std::make_shared<Node>();
    iterator.vertex_->children_[bit]->parent_ = iterator.vertex_->children_[bit];
}
void archiver::BinaryTrie::SetTerminalChar(archiver::BinaryTrie::Iterator iterator, archiver::ExtChar chr) {
    iterator.vertex_->chr_ = chr;
}
