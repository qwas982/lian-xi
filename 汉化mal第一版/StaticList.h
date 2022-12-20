#ifndef INCLUDE_STATICLIST_H
#define INCLUDE_STATICLIST_H

template<typename T>
class 静态列表
{
public:
    静态列表() : 成员_头部_小写(NULL) { }

    class 迭代器_大写;
    迭代器_大写 begin() { return 迭代器_大写(成员_头部_小写); }
    迭代器_大写 end()   { return 迭代器_大写(NULL);   }

    class 节点_大写 {
    public:
        节点_大写(静态列表<T>& 列表_小写, T 条目_小写)
        : 成员_条目_小写(条目_小写), 成员_下一个_小写(列表_小写.成员_头部_小写) {
            列表_小写.成员_头部_小写 = this;
        }

    private:
        friend class 迭代器_大写;
        T 成员_条目_小写;
        节点_大写* 成员_下一个_小写;
    };

    class 迭代器_大写 {
    public:
        迭代器_大写& operator ++ () {
            成员_节点_小写 = 成员_节点_小写->成员_下一个_小写;
            return *this;
        }

        T& operator * () { return 成员_节点_小写->成员_条目_小写; }
        bool operator != (const 迭代器_大写& that) {
            return 成员_节点_小写 != that.成员_节点_小写;
        }

    private:
        friend class 静态列表<T>;
        迭代器_大写(节点_大写* 节点_小写) : 成员_节点_小写(节点_小写) { }
        节点_大写* 成员_节点_小写;
    };

private:
    friend class 节点_大写;
    节点_大写*  成员_头部_小写;
};

#endif // INCLUDE_STATICLIST_H
