#ifndef INCLUDE_STATICLIST_H
#define INCLUDE_STATICLIST_H

template<typename T>
class 静态列表
{
public:
    静态列表() : m_head(NULL) { }

    class Iterator;
    Iterator begin() { return Iterator(m_head); }
    Iterator end()   { return Iterator(NULL);   }

    class Node {
    public:
        Node(静态列表<T>& list, T item)
        : m_item(item), m_next(list.m_head) {
            list.m_head = this;
        }

    private:
        friend class Iterator;
        T m_item;
        Node* m_next;
    };

    class Iterator {
    public:
        Iterator& operator ++ () {
            m_node = m_node->m_next;
            return *this;
        }

        T& operator * () { return m_node->m_item; }
        bool operator != (const Iterator& that) {
            return m_node != that.m_node;
        }

    private:
        friend class 静态列表<T>;
        Iterator(Node* node) : m_node(node) { }
        Node* m_node;
    };

private:
    friend class Node;
    Node*  m_head;
};

#endif // INCLUDE_STATICLIST_H
