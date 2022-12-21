#ifndef INCLUDE_REFCOUNTEDPTR_H
#define INCLUDE_REFCOUNTEDPTR_H

#include "Debug.h"

#include <cstddef>

class 已被引用计数的 {
public:
    已被引用计数的() : m_refCount(0) { }
    virtual ~已被引用计数的() { }

    const 已被引用计数的* acquire() const { m_refCount++; return this; }
    int release() const { return --m_refCount; }
    int refCount() const { return m_refCount; }

private:
    已被引用计数的(const 已被引用计数的&); // no copy ctor
    已被引用计数的& operator = (const 已被引用计数的&); // no assignments

    mutable int m_refCount;
};

template<class T>
class 已被引用计数的指针 {
public:
    已被引用计数的指针() : m_object(0) { }

    已被引用计数的指针(T* object) : m_object(0)
    { acquire(object); }

    已被引用计数的指针(const 已被引用计数的指针& rhs) : m_object(0)
    { acquire(rhs.m_object); }

    const 已被引用计数的指针& operator = (const 已被引用计数的指针& rhs) {
        acquire(rhs.m_object);
        return *this;
    }

    bool operator == (const 已被引用计数的指针& rhs) const {
        return m_object == rhs.m_object;
    }

    bool operator != (const 已被引用计数的指针& rhs) const {
        return m_object != rhs.m_object;
    }

    operator bool () const {
        return m_object != NULL;
    }

    ~已被引用计数的指针() {
        release();
    }

    T* operator -> () const { return m_object; }
    T* ptr() const { return m_object; }

private:
    void acquire(T* object) {
        if (object != NULL) {
            object->acquire();
        }
        release();
        m_object = object;
    }

    void release() {
        if ((m_object != NULL) && (m_object->release() == 0)) {
            delete m_object;
        }
    }

    T* m_object;
};

#endif // INCLUDE_REFCOUNTEDPTR_H
