#ifndef RB_TREE_H
#define RB_TREE_H

#include <cstddef>

namespace sk3l
{
namespace data
{
namespace adt
{

template<typename Key,typename Type>
class red_black_tree
{
public:
   typedef Key key_type;
   typedef Type value_type;

    class red_black_tree_node
    {
        friend class red_black_tree;

    private:
        typedef red_black_tree_node node_type;
        typedef red_black_tree_node* node_ptr;

        enum color
        {
            BLACK = 0,
            RED = 1

        };

        node_ptr parent_;
        node_ptr left_;
        node_ptr right_;
        key_type 	key_;
        value_type 	val_;
        color color_;

    public:

        red_black_tree_node(const key_type & k, const value_type & v)
            : key_(k),
              val_(v),
              color_(color::BLACK)
        {}

        ~red_black_tree_node()
        {
           if (this->left_ != nullptr)
              delete this->left_;

           if (this->right_ != nullptr)
             delete this->right_;
        }

        value_type & get() const {return this->val_;}
        void set(const value_type & v) {this->val_ = v;}
    };

    typedef red_black_tree_node node_type;
    typedef red_black_tree_node* node_ptr;
    
    class iterator
    {
      typedef iterator self_type;
      typedef Type value_type;
      typedef Type& reference_type;
      typedef Type* pointer_type;
      typedef int differenc_t;

       public:
      iterator(node_ptr p)
         : ptr_(p)
      {}

      iterator(const iterator & rhs)
      : ptr_(rhs.ptr_)
      {}

      iterator operator=(const iterator & rhs)
      {
         this->ptr_ = rhs.ptr_;
         return *this;
      };

      iterator operator++()
      {
         this->ptr_ = this->next();  

         return *this;
      } 

      iterator operator++(int ignore)
      {
         iterator tmp(*this);
         this->ptr = this->next();
         return tmp;
      }

      reference_type operator*()
      {
        return this->ptr_->val_;
      }

      pointer_type operator->()
      {
         return &(this->ptr_->val_);
      } 

      bool operator==(const iterator & rhs)
      {
         return this->ptr_ == rhs.ptr_;
      }

      bool operator!=(const iterator & rhs)
      {
         return !(*this == rhs);
      }

       private:

      node_ptr next()
      {
        node_ptr nxt = nullptr;

        if (this->ptr_->right_ != nullptr)
        {
           nxt = this->ptr_->right_;
           while (nxt->left_ != nullptr)
              nxt = nxt->left_;
        }
        else if (this->ptr_->parent_ != nullptr)
        {
           if (this->ptr_->parent_->left_ == this->ptr_)
              nxt = this->ptr_->parent_;
           else
           {
              node_ptr parent = this->ptr_->parent_;
              nxt = parent->parent_;

              while (nxt != nullptr && parent == nxt->right_)
              {
                 parent = nxt;
                 nxt = nxt->parent_;
              }              
           } 
        }
        
        return nxt; 
      }

         node_ptr ptr_;  
    };

    typedef red_black_tree<Key,Type> tree_type;
private:

    node_ptr root_;
    std::size_t count_;

    node_ptr search_internal(node_ptr root, const key_type & k)
    {
        node_ptr node = nullptr;
        if (root)
        {
            if (k == root->key_)
                node = root;
            else if (k < root->key_)
                node = this->search_internal(root->left_, k);
            else
                node = this->search_internal(root->right_, k);
        }

        return node;
    }

    void insert_internal(node_ptr newNode, node_ptr & root)
    {
        if (root == nullptr)
           root = newNode;
        else if (newNode->key_ < root->key_)
        {
           newNode->parent_ = root;
           this->insert_internal(newNode, root->left_);
        }
        else
        {
           newNode->parent_ = root;
           this->insert_internal(newNode, root->right_);
        }
    }

    void rotate_left(node_ptr node, node_ptr & root)
    {
        if (node != nullptr && node->right_ != nullptr)
        {
            node_ptr right = node->right_;
            node->right_ = right->left_;
            if (right->left_ != nullptr)
                right->left_->parent_ = node;
            right->parent_ = node->parent_;

            if (node->parent_ == nullptr)
               root = right;
            else if (node == node->parent_->left_)
               node->parent_->left_ = right;
            else
               node->parent_->right_ = right;

            right->left_ = node;
            node->parent_ = right;
        }
    }

    void rotate_right(node_ptr node, node_ptr & root)
    {
        if (node != nullptr && node->left_ != nullptr)
        {
            node_ptr left = node->left_;
            node->left_ = left->right_;
            if (left->right_ != nullptr)
                left->right_->parent_ = node;
            left->parent_ = node->parent_;

            if (node->parent_ == nullptr)
               root = left;
            else if (node == node->parent_->left_) 
               node->parent_->left_ = left;
            else
               node->parent_->right_ = left;

            left->right_ = node;
            node->parent_ = left;
        }

    }

    node_ptr find_predecessor(node_ptr node)
    {
       node_ptr pred = nullptr;
       if (node != nullptr && node->left_)
       {
            pred = node->left_;
            while (pred->right_ != nullptr)
            {
                pred = pred->right_;
            }
       }

       return pred;
    }

    node_ptr find_successor(node_ptr node)
    {
        node_ptr succ = nullptr;
        if (node != nullptr && node->right_)
        {
            succ = node->right_;
            while (succ->left_ != nullptr)
            {
                succ = succ->left_;
            }
        }

        return succ;
    }

    void fixup_delete(node_ptr node, node_ptr nodeParent)
    {
        while (node != this->root_ && (
                   node == nullptr ||
                   node->color == node_type::color::BLACK))
        {
            if (node == nodeParent->left_)
            {
                node_ptr rhs = nodeParent->right_;
                if (rhs->color == node_type::color::RED)
                {
                    rhs->parent_->color_ = node_type::color::RED;
                    this->rotate_left(rhs->parent_, this->root_);
                    rhs = nodeParent->right_;
                }

                if (rhs->left_->color == node_type::color::BLACK &&
                    rhs->right_->color == node_type::color::BLACK)
                {
                    rhs->color_ = node_type::color::RED;
                    node = nodeParent;
                }
                else
                {
                    if (rhs->right_->color == node_type::color::BLACK)
                    {
                        rhs->left_->color_ = node_type::color::BLACK;
                        rhs->color_ = node_type::color::RED;
                        this->rotate_right(rhs, this->root_);
                        rhs = nodeParent->right_;
                    }

                    rhs->color_ = nodeParent->color;
                    nodeParent->color_ = node_type::color::BLACK;
                    rhs->right_->color_ = node_type::color::BLACK;
                    this->rotate_left(nodeParent, this->root_);
                }
            }
           else
            {
                node_ptr lhs = nodeParent->left;
                if (lhs->color == node_type::color::RED)
                {
                    lhs->parent_->color_ = node_type::color::RED;
                    this->rotate_left(lhs->parent_, this->root_);
                    lhs = nodeParent->left_;
                }

                if (lhs->left_->color == node_type::color::BLACK &&
                    lhs->right_->color == node_type::color::BLACK)
                {
                    lhs->color_ = node_type::color::RED;
                    node = nodeParent;
                }
                else
                {
                    if (lhs->left_->color == node_type::color::BLACK)
                    {
                        lhs->right_t->color_ = node_type::color::BLACK;
                        lhs->color_ = node_type::color::RED;
                        this->rotate_left(lhs, this->root_);
                        lhs = nodeParent->left_;
                    }

                    lhs->color_ = nodeParent->color;
                    nodeParent->color_ = node_type::color::BLACK;
                    lhs->left_->color_ = node_type::color::BLACK;
                    this->rotate_right(nodeParent, this->root_);
                }
            }
        }

        if (node)
            node->color_ = node_type::color::BLACK;
    }

public:
   red_black_tree()
    : root_(nullptr),
      count_(0)
    {}

    ~red_black_tree()
    {
      if (this->root_ != nullptr)
        delete this->root_;
    } 

    node_ptr find(const key_type & k)
    {
        return this->search_internal(this->root_, k);
    }

    void insert(const key_type & key, const value_type & val)
    {
        node_ptr newNode = new node_type(key, val);

        this->insert_internal(newNode, this->root_);
        newNode->color_ = node_type::color::RED;

        node_ptr n = newNode;
        while (n != this->root_ &&
               n->parent_ != nullptr &&
               n->parent_->color_ == node_type::color::RED)
        {
            node_ptr grandParent = n->parent_->parent_;

            if (grandParent == nullptr)
                break;

            if (n->parent_ == grandParent->left_)
            {
                node_ptr right = grandParent->right_;
                if (right != nullptr &&
                    right->color_ == node_type::color::RED)
                {
                    right->color_ = node_type::color::BLACK;
                    n->parent_->color_ = node_type::color::BLACK;
                    grandParent->color_ = node_type::color::RED;
                    n = grandParent;
                }
                else
                {
                    if (n == n->parent_->right_)
                    {
                        n = n->parent_;
                        this->rotate_left(n, this->root_);
                    }
                    n->parent_->color_ = node_type::color::BLACK;
                    grandParent->color_ = node_type::color::RED;
                    this->rotate_right(grandParent, this->root_);
                }

            }
            else
            {
                node_ptr left = grandParent->left_;
                if (left != nullptr &&
                    left->color_ == node_type::color::RED)
                {
                    left->color_ = node_type::color::BLACK;
                    n->parent_->color_ = node_type::color::BLACK;
                    grandParent->color_ = node_type::color::RED;
                    n = grandParent;
                }
                else
                {
                    if (n == n->parent_->left_)
                    {
                        n = n->parent_;
                        this->rotate_right(n, this->root_);
                    }
                    n->parent_->color_ = node_type::color::BLACK;
                    grandParent->color_ = node_type::color::RED;
                    this->rotate_left(grandParent, this->root_);
                }

            }

        }

        this->root_->color_ = node_type::color::BLACK;
        ++this->count_;
    }

    void remove(node_ptr node)
    {
        if (node != nullptr)
        {
            node_ptr del = nullptr,	// node to remove
                     sub = nullptr; // node to substitute

            if (node->left_ == nullptr || node->right_ == nullptr)
                del = node;
            else
                del = this->find_successor(node);

            if (del->left_ != nullptr)
                 sub = del->left_;
            else
                sub = del->right_;

            if (sub != nullptr)
                sub->parent_ = del->parent_;

            if (del->parent_ == nullptr)
                this->root_ = sub;
            else if (del == del->parent_->left_)
                del->parent_->left_ = sub;
            else
                del->parent_->right_ = sub;

            if (del != node)
            {
                node->key_ = del->key_;
                node->val_ = del->val_;
            }

            if (del->color_ == node_type::color::BLACK)
                this->fixup_delete(del, del->parent_);

            delete del;
            --this->count_;
        }
    }

   iterator begin()
   {
      node_ptr ptr = this->root_;
      while (ptr != nullptr && ptr->left_ != nullptr)
         ptr = ptr->left_;

      return iterator(ptr);
   }

   iterator end()
   {
      return iterator(nullptr);
   }
};

}
}
}
#endif // RB_TREE_H

