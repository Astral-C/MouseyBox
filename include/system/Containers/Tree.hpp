#ifndef __MB_TREE_H__
#define __MB_TREE_H__
#include <cstdint>
#include <map>
#include <tuple>
#include <string>
#include <deque>
#include <memory>

namespace mb {
    
    template<typename T>
    class TreeNode : public std::enable_shared_from_this<TreeNode<T>> {
        T mData;
        std::weak_ptr<TreeNode<T>> mParent;
        std::deque<std::shared_ptr<TreeNode<T>>> mChildren;
    public:
        T* data() { return &mData; }

        void SetParent(std::shared_ptr<TreeNode<T>> parent) { mParent = parent; }
        std::shared_ptr<TreeNode<T>> GetChild(int idx) { return mChildren[idx]; }
        std::deque<std::shared_ptr<TreeNode<T>>>* GetChildren() { return &mChildren; }

        std::shared_ptr<TreeNode<T>> AddNode(std::shared_ptr<TreeNode<T>> subroot){
            subroot->SetParent(std::enable_shared_from_this<TreeNode<T>>::shared_from_this());
            mChildren.push_back(subroot);
            return mChildren.back();
        }

        TreeNode(T item){ mData = item; }
        TreeNode(){}
        ~TreeNode(){}
    };

    template<typename T>
    class Tree {
        std::shared_ptr<TreeNode<T>> mRoot;
    public:

        void SetRoot(T item) { mRoot = std::make_shared<TreeNode<T>>(item); }
        std::shared_ptr<TreeNode<T>> GetRoot() { return mRoot; }

        Tree() {}
        ~Tree() {}
    };

    template<typename T>
    class AVLNode {
        T mData;
        AVLNode<T>* mParent { nullptr };
        AVLNode<T>* mLeft { nullptr };
        AVLNode<T>* mRight { nullptr };
    public:
        T* data() { return &mData; }

        AVLNode<T>* Left() { return mLeft; }
        AVLNode<T>* Right() { return mRight; }
        
        T* begin() { return (mLeft != nullptr ? mLeft->begin() : &this); }
        T* end() { return (mRight != nullptr ? mRight->begin() : &this); }

        AVLNode(){}
        ~AVLNode(){}
    };

    //AVL Binary Tree
    template<typename T>
    class AVLTree {
        std::vector<AVLNode<T>> mNodes;
    public:

        T Insert(T item){}
        
        T* Find(T item) {}

        AVLTree() {}
        ~AVLTree() {}
    };
}
#endif