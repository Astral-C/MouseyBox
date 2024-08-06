#ifndef __MB_TREE_H__
#define __MB_TREE_H__
#include <cstdint>
#include <map>
#include <tuple>
#include <string>
#include <vector>

namespace mb {
    
    template<typename T>
    class TreeNode {
        T mData;
        std::vector<TreeNode<T>> mChildren;
    public:
        T* data() { return &mData; }

        T GetChild(int idx) { return mChildren[idx]; }
        std::vector<TreeNode<T>>* GetChildren() { return &mChildren; }
        TreeNode<T>* AddChild(T item){
            TreeNode<T> node;
            node.mData = item;
            mChildren.push_back(node);
            return &mChildren.back();
        }

        T* begin() { return mChildren.begin(); }
        T* end() { return mChildren.end(); }

        TreeNode(T item){ mData = item; }
        TreeNode(){}
        ~TreeNode(){}
    };

    template<typename T>
    class Tree {
        TreeNode<T> mRoot;
    public:

        void SetRoot(T item) { mRoot = TreeNode<T>(item); }
        TreeNode<T>* GetRoot() { return &mRoot; }

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