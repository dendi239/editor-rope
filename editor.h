#pragma once

#include <memory>
#include <random>
#include <string>

template <class T>
struct Node {
    using NodePtr = std::shared_ptr<const Node>;
    static std::mt19937 rnd;

    const T data;
    const size_t size = 1;
    const std::uint_fast32_t priority;
    const NodePtr left, right;

    explicit Node(T data) : data(data), size(1), priority(rnd()), left(nullptr), right(nullptr) {
    }

    Node(T data, std::uint_fast32_t priority, NodePtr left, NodePtr right)
        : data(data),
          size(Size(left.get()) + 1 + Size(right.get())),
          priority(priority),
          left(left),
          right(right) {
        // no-op
    }

    auto WithLeft(NodePtr new_left) const -> NodePtr {
        return std::make_shared<Node>(data, priority, new_left, right);
    }

    auto WithRight(NodePtr new_right) const -> NodePtr {
        return std::make_shared<Node>(data, priority, left, new_right);
    }

    static auto Size(const Node *node) -> size_t {
        return node != nullptr ? node->size : 0;
    }

    static auto Split(NodePtr node, size_t size) -> std::pair<NodePtr, NodePtr> {
        if (size == 0) {
            return {nullptr, node};
        }
        if (size == Size(node.get())) {
            return {node, nullptr};
        }

        if (auto left_size = Size(node->left.get()); left_size >= size) {
            auto [lhs, rhs] = Split(node->left, size);
            return {lhs, node->WithLeft(rhs)};
        } else {
            auto [lhs, rhs] = Split(node->right, size - left_size - 1);
            return {node->WithRight(lhs), rhs};
        }
    }

    static auto Merge(NodePtr left, NodePtr right) -> NodePtr {
        if (left == nullptr) {
            return right;
        }
        if (right == nullptr) {
            return left;
        }
        if (left->priority < right->priority) {
            return left->WithRight(Merge(left->right, right));
        } else {
            return right->WithLeft(Merge(left, right->left));
        }
    }

    template <class It>
    static auto PrintTo(const Node *node, It write) -> It {
        if (node == nullptr) {
            return write;
        }

        write = PrintTo(node->left.get(), write);
        *write++ = node->data;
        return PrintTo(node->right.get(), write);
    }
};

template <class T>
std::mt19937 Node<T>::rnd;

class Editor {
    size_t pos_ = 0;
    std::vector<Node<char>::NodePtr> states_{nullptr};
    std::vector<size_t> cursors_{0};

public:
    std::string GetText() const {
        std::string result;
        Node<char>::PrintTo(states_[pos_].get(), std::back_inserter(result));
        return result;
    }

    void Type(char symbol) {
        states_.resize(pos_ + 2);
        cursors_.resize(pos_ + 2);

        auto [before, after] = Node<char>::Split(states_[pos_], cursors_[pos_]);
        auto symbol_node = std::make_shared<Node<char>>(symbol);

        states_[pos_ + 1] = Node<char>::Merge(before, Node<char>::Merge(symbol_node, after));
        cursors_[pos_ + 1] = cursors_[pos_] + 1;

        ++pos_;
    }

    void ShiftLeft() {
        if (Node<char>::Size(states_[pos_].get()) == 0) {
            return;
        }

        states_.resize(pos_ + 2);
        cursors_.resize(pos_ + 2);

        states_[pos_ + 1] = states_[pos_];
        cursors_[pos_ + 1] = std::max<size_t>(cursors_[pos_], 1) - 1;

        ++pos_;
    }

    void ShiftRight() {
        if (Node<char>::Size(states_[pos_].get()) == cursors_[pos_]) {
            return;
        }

        states_.resize(pos_ + 2);
        cursors_.resize(pos_ + 2);

        states_[pos_ + 1] = states_[pos_];
        cursors_[pos_ + 1] = cursors_[pos_] + 1;

        ++pos_;
    }

    void Backspace() {
        states_.resize(pos_ + 2);
        cursors_.resize(pos_ + 2);

        if (cursors_[pos_] == 0) {
            states_[pos_ + 1] = states_[pos_];
            cursors_[++pos_] = 0;
            return;
        }

        auto [before, mid_after] = Node<char>::Split(states_[pos_], cursors_[pos_] - 1);
        auto [mid, after] = Node<char>::Split(mid_after, 1);

        states_[pos_ + 1] = Node<char>::Merge(before, after);
        cursors_[pos_ + 1] = cursors_[pos_] - 1;

        ++pos_;
    }

    void Undo() {
        if (pos_ != 0) {
            --pos_;
        }
    }

    void Redo() {
        if (pos_ + 1 < states_.size()) {
            ++pos_;
        }
    }
};
