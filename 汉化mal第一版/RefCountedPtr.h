#ifndef INCLUDE_REFCOUNTEDPTR_H
#define INCLUDE_REFCOUNTEDPTR_H

#include "Debug.h"

#include <cstddef>

class 引用计数_过去式 {
public:
    引用计数_过去式() : 成员_引用计数_小写(0) { }
    virtual ~引用计数_过去式() { }

    const 引用计数_过去式* 获得_小写() const { 成员_引用计数_小写++; return this; }
    int 释放_小写() const { return --成员_引用计数_小写; }
    int 引用_计数_小写() const { return 成员_引用计数_小写; }

private:
    引用计数_过去式(const 引用计数_过去式&); // no copy ctor
    引用计数_过去式& operator = (const 引用计数_过去式&); // no assignments

    mutable int 成员_引用计数_小写;
};

template<class T>
class 引用计数指针_短 {
public:
    引用计数指针_短() : 成员_对象_小写(0) { }

    引用计数指针_短(T* 对象_小写) : 成员_对象_小写(0)
    { 获得_小写(对象_小写); }

    引用计数指针_短(const 引用计数指针_短& 右手边_小写缩) : 成员_对象_小写(0)
    { 获得_小写(右手边_小写缩.成员_对象_小写); }

    const 引用计数指针_短& operator = (const 引用计数指针_短& 右手边_小写缩) {
        获得_小写(右手边_小写缩.成员_对象_小写);
        return *this;
    }

    bool operator == (const 引用计数指针_短& 右手边_小写缩) const {
        return 成员_对象_小写 == 右手边_小写缩.成员_对象_小写;
    }

    bool operator != (const 引用计数指针_短& 右手边_小写缩) const {
        return 成员_对象_小写 != 右手边_小写缩.成员_对象_小写;
    }

    operator bool () const {
        return 成员_对象_小写 != NULL;
    }

    ~引用计数指针_短() {
        释放_小写();
    }

    T* operator -> () const { return 成员_对象_小写; }
    T* ptr() const { return 成员_对象_小写; }

private:
    void 获得_小写(T* 对象_小写) {
        if (对象_小写 != NULL) {
            对象_小写->获得_小写();
        }
        释放_小写();
        成员_对象_小写 = 对象_小写;
    }

    void 释放_小写() {
        if ((成员_对象_小写 != NULL) && (成员_对象_小写->释放_小写() == 0)) {
            delete 成员_对象_小写;
        }
    }

    T* 成员_对象_小写;
};

#endif // INCLUDE_REFCOUNTEDPTR_H
