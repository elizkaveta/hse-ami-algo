#include <iostream>


template<class ValueType>
class AVL {
public:
    struct Node {
        ValueType value;
        Node* left = nullptr;
        Node* right = nullptr;
        Node* parent = nullptr;
        int64_t height = 0;

        Node() {
            left = right = parent = nullptr;
            height = 0;
        }

        Node(const ValueType& val) {
            left = right = parent = nullptr;
            height = 0;
            value = val;
        }

        Node* copy() {
            Node* new_root = new Node();
            new_root->value = value;
            if (left != nullptr) {
                Node* new_left = left->copy();
                new_root->left = new_left;
                new_left->parent = new_root;
            }
            if (right != nullptr) {
                Node* new_right = right->copy();
                new_root->right = new_right;
                new_right->parent = new_root;
            }
            return new_root;
        }
    };

    AVL() {
        root = nullptr;
        size_ = 0;
    }

    AVL(const AVL& bst) {
        if (bst.root == nullptr) {
            return;
        }
        root = bst.root->copy();
        size_ = bst.size();
    }

    AVL& operator=(const AVL& bst) {
        if (this == &bst) {
            return *this;
        }
        clear_tree();
        if (bst.root == nullptr) {
            return *this;
        }
        root = bst.root->copy();
        size_ = bst.size();
        return *this;
    }

    Node* find(const ValueType& key) const {
        return find(key, root);
    }

    void insert(const ValueType& key) {
        if (find(key, root) != nullptr) {
            return;
        }
        ++size_;
        insert(key, root);
    }

    void erase(const ValueType& key) {
        if (find(key, root) == nullptr) {
            return;
        }
        --size_;
        root = erase(key, root);
    }

    size_t size() const {
        return size_;
    }

    Node* find_min_node() const {
        return find_min_node(root);
    }

    Node* find_max_node() const {
        return find_max_node(root);
    }

    void clear_tree() {
        size_ = 0;
        return clear_tree(root);
    }

    Node* lower_bound(const ValueType& key) const {
        return lower_bound(key, root);
    }

private:
    size_t size_ = 0;
    Node* root;

    void set_parent(Node* t, Node* parent) {
        if (t == nullptr) {
            return;
        }
        t->parent = parent;
    }

    void set_left_son(Node* t, Node* son) {
        if (t == nullptr) {
            return;
        }
        t->left = son;
        set_parent(son, t);
    }

    void set_right_son(Node* t, Node* son) {
        if (t == nullptr) {
            return;
        }
        t->right = son;
        set_parent(son, t);
    }

    Node* find(const ValueType& key, Node* t) const {
        if (t == nullptr) {
            return nullptr;
        }
        if (t->value < key) {
            return find(key, t->right);
        }
        if (key < t->value) {
            return find(key, t->left);
        } else {
            return t;
        }
    }

    Node* lower_bound(const ValueType& key, Node* node) const {
        if (node == nullptr) {
            return nullptr;
        }
        if (node->value < key) {
            return lower_bound(key, node->right);
        }
        Node* ans = lower_bound(key, node->left);
        if (ans != nullptr && ans->value < node->value) {
            return ans;
        }
        return node;
    }

    void clear_tree(Node*& t) {
        if (t == nullptr) {
            return;
        }
        clear_tree(t->left);
        clear_tree(t->right);
        if (t == root) {
            delete t;
            root = nullptr;
            return;
        }
        delete t;
    }

    int height(Node* node) {
        return (node != nullptr) ? node->height + 1 : 0;
    }

    Node* ll_rotate(Node*& t) {
        if (t == nullptr) {
            return nullptr;
        }
        Node* new_node = t->right;
        if (new_node != nullptr) {
            set_right_son(t, new_node->left);
            new_node->parent = t->parent;
            set_left_son(new_node, t);
        }
        t->height = std::max(height(t->left), height(t->right));
        if (new_node != nullptr) {
            new_node->height = std::max(height(new_node->left), height(new_node->right));
        }
        return new_node;
    }
    Node* rr_rotate(Node*& t) {
        if (t == nullptr) {
            return nullptr;
        }
        Node* new_node = t->left;
        if (new_node != nullptr) {
            set_left_son(t, new_node->right);
            new_node->parent = t->parent;
            set_right_son(new_node, t);

        }
        t->height = std::max(height(t->left), height(t->right));
        if (new_node != nullptr) {
            new_node->height = std::max(height(new_node->left), height(new_node->right));
        }
        return new_node;
    }

    Node* insert(const ValueType& key, Node*& t) {
        if (t == nullptr) {
            return t = new Node(key);
        }
        if (key < t->value) {
            set_left_son(t, insert(key, t->left));
        } else if (t->value < key) {
            set_right_son(t, insert(key, t->right));
        } else {
            return t;
        }
        int l_height = height(t->left);
        int r_height = height(t->right);
        t->height = std::max(l_height, r_height);
        int bal = l_height - r_height;
        if (bal > 1 && t->left != nullptr) {
            if (key < t->left->value) {
                return t = rr_rotate(t);
            } else {
                t->left = ll_rotate(t->left);
                return t = rr_rotate(t);
            }
        } else if (bal < -1 && t->right != nullptr) {
            if (key < t->right->value) {
                set_right_son(t, rr_rotate(t->right));
            }
            return t = ll_rotate(t);
        }

        return t;
    }

    Node* erase(const ValueType& key, Node*& node) {
        if (node == nullptr) {
            return nullptr;
        }
        if (key < node->value) {
            set_left_son(node, erase(key, node->left));
        } else if (node->value < key) {
            set_right_son(node, erase(key, node->right));
        } else {
            if (node->left == nullptr && node->right == nullptr) {
                delete node;
                return nullptr;
            } else if (node->left != nullptr && node->right == nullptr) {
                Node* l_tree = node->left;
                delete node;
                return l_tree;
            } else if (node->left == nullptr && node->right != nullptr) {
                Node* r_tree = node->right;
                delete node;
                return r_tree;
            } else {
                Node* max_l = find_max_node(node->left);
                node->value = max_l->value;
                set_left_son(node, erase(max_l->value, node->left));
            }
        }
        int l_height = height(node->left);
        int r_height = height(node->right);
        node->height = std::max(l_height, r_height);
        int bal_fac = l_height - r_height;
        if (bal_fac > 1) {
            if (height(node->left->left) > height(node->left->right)) {
                return node = rr_rotate(node);
            } else {
                set_left_son(node, ll_rotate(node->left));
                return node = rr_rotate(node);
            }
        } else if (bal_fac < -1) {
            if (height(node->right->left) > height(node->right->right)) {
                set_right_son(node, rr_rotate(node->right));
                return node = ll_rotate(node);
            } else {
                return node = ll_rotate(node);
            }
        }

        return node;
    }

    Node* find_max_node(Node* t) const {
        if (t == nullptr) {
            return nullptr;
        }
        while (t->right != nullptr) {
            t = t->right;
        }
        return t;
    }
    Node* find_min_node(Node* t) const {
        if (t == nullptr) {
            return nullptr;
        }
        while (t->left != nullptr) {
            t = t->left;
        }
        return t;
    }

};

template<class ValueType>
class Set {
public:
    class iterator {
    public:
        friend Set;
        typename AVL<ValueType>::Node* node_;
        bool not_end_ = false;

        iterator() {
        }

        iterator(typename AVL<ValueType>::Node* node, bool flag) {
            not_end_ = flag;
            node_ = node;
        }

        iterator operator++() {
            typename AVL<ValueType>::Node* next_node = FindNext(node_);
            if (next_node == nullptr) {
                not_end_ = false;
                return *this;
            }
            node_ = next_node;
            return *this;
        }

        iterator operator--() {
            if (!not_end_) {
                not_end_ = true;
                return *this;
            }
            typename AVL<ValueType>::Node* prev_node = FindPrev(node_);
            if (prev_node == nullptr) {
                not_end_ = false;
                return *this;
            }
            node_ = prev_node;
            return *this;
        }

        iterator operator++(int) {
            typename AVL<ValueType>::Node* next_node = FindNext(node_);
            if (next_node == nullptr) {
                not_end_ = false;
                return *this;
            }
            node_ = next_node;
            return *this;
        }

        iterator operator--(int) {
            if (!not_end_) {
                not_end_ = true;
                return *this;
            }
            typename AVL<ValueType>::Node* prev_node = FindPrev(node_);
            if (prev_node == nullptr) {
                not_end_ = false;
                return *this;
            }
            node_ = prev_node;
            return *this;
        }

        const ValueType& operator*() const {
            return node_->value;
        }

        const ValueType& operator*() {
            return node_->value;
        }

        bool operator!=(const iterator it) const {
            if (!not_end_ || !it.not_end_) {
                if (!not_end_ && !it.not_end_) {
                    return false;
                } else {
                    return true;
                }
            }
            return node_->value < it.node_->value || it.node_->value < node_->value;
        }

        bool operator==(const iterator it) const {
            if (!not_end_ || !it.not_end_) {
                if (!not_end_ && !it.not_end_) {
                    return true;
                } else {
                    return false;
                }
            }
            return !(node_->value < it.node_->value || it.node_->value < node_->value);
        }

        const ValueType* operator->() const {
            return &(node_->value);
        }

        ValueType* operator->() {
            return &(node_->value);
        }

    private:
        typename AVL<ValueType>::Node* FindNext(typename AVL<ValueType>::Node* now) {
            if (now == nullptr) {
                return nullptr;
            }
            if (now->right != nullptr) {
                now = now->right;
                while (now->left != nullptr) {
                    now = now->left;
                }
                return now;
            }
            while (now->parent != nullptr && (now->parent->right == now)) {
                now = now->parent;
            }
            return now->parent;
        }

        typename AVL<ValueType>::Node* FindPrev(typename AVL<ValueType>::Node* now) {
            if (now == nullptr) {
                return nullptr;
            }
            if (now->left != nullptr) {
                now = now->left;
                while (now->right != nullptr) {
                    now = now->right;
                }
                return now;
            }
            while (now->parent != nullptr && (now->parent->left == now)) {
                now = now->parent;
            }
            return now->parent;
        }
    };

    Set& operator=(const Set& new_set) {
        if (this == &new_set) {
            return *this;
        }
        tree_ = new_set.tree_;
        return *this;
    }

    Set(ValueType* beg, ValueType* end) {
        while (beg != end) {
            insert(*beg);
            ++beg;
        }
    }

    Set() {

    }

    Set(const std::initializer_list<ValueType>& l) {
        for (const auto& i: l) {
            tree_.insert(i);
        }
    }

    Set(const Set& new_set) {
        tree_ = new_set.tree_;
    }

    ~Set() {
        tree_.clear_tree();
    }

    void insert(const ValueType& key) {
        tree_.insert(key);
    }

    void erase(const ValueType& key) {
        tree_.erase(key);
    }

    bool empty() const {
        return tree_.size() == 0;
    }

    size_t size() const {
        return tree_.size();
    }

    iterator find(const ValueType& key) const {
        typename AVL<ValueType>::Node* node = tree_.find(key);
        if (node == nullptr) {
            return end();
        }
        return iterator(node, 1);
    }

    iterator lower_bound(const ValueType& key) const {
        typename AVL<ValueType>::Node* node = tree_.lower_bound(key);
        if (node == nullptr) {
            return end();
        }
        return iterator(node, 1);
    }

    iterator begin() const {
        if (!tree_.size()) {
            return end();
        }
        return iterator(tree_.find_min_node(), 1);
    }

    iterator end() const {
        return iterator(tree_.find_max_node(), 0);
    }

private:
    AVL<ValueType> tree_;
};
