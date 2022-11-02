// this code based on code by David Harvey.
// http://web.ftech.net/~honeyg/articles/smartp.htm
// Sir, I salute you and give thanks for your most excellent code.

#ifndef REFCOUNTPTR_H
#define REFCOUNTPTR_H

/* For ANSI-challenged compilers, you may want to #define
 * NO_MEMBER_TEMPLATES or explicit */

template <class X> class CRefCountPtr
{
public:
    typedef X element_type;

    explicit CRefCountPtr(X* p = NULL) // allocate a new counter
        : itsCounter(NULL) {if (p) itsCounter = new counter(p);}
    virtual ~CRefCountPtr()
        {Release();}
    CRefCountPtr(const CRefCountPtr& r) throw()
        {Acquire(r.itsCounter);}
    CRefCountPtr& operator=(const CRefCountPtr& r)
    {
        if (this != &r) {
            Release();
            Acquire(r.itsCounter);
        }
        return *this;
    }

/*
#ifndef NO_MEMBER_TEMPLATES
    template <class Y> friend class CRefCountPtr<Y>;
    template <class Y> CRefCountPtr(const CRefCountPtr<Y>& r) throw()
        {Acquire(r.itsCounter);}
    template <class Y> CRefCountPtr& operator=(const CRefCountPtr<Y>& r)
    {
        if (this != &r) {
            Release();
            Acquire(r.itsCounter);
        }
        return *this;
    }
#endif // NO_MEMBER_TEMPLATES
*/

    X& operator*()  const throw()   {return *itsCounter->ptr;}
    X* operator->() const throw()   {return itsCounter->ptr;}
    X* Get()        const throw()   {return itsCounter ? itsCounter->ptr : 0;}
    bool Unique()   const throw()
        {return (itsCounter ? itsCounter->count == 1 : true);}
    bool Valid() const throw() { return(itsCounter != 0); }
protected:

    struct counter {
        counter(X* p = 0, unsigned c = 1) : ptr(p), count(c) {}
        X*          ptr;
        unsigned    count;
    }* itsCounter;

    void Acquire(counter* c) throw()
    { // increment the count
        itsCounter = c;
        if (c) ++c->count;
    }

    virtual void Release()
    { // decrement the count, delete if it is 0
        if (itsCounter) {
            if (--itsCounter->count == 0) {
                delete itsCounter->ptr;
                delete itsCounter;
            }
            itsCounter = 0;
        }
    }
};

#endif // RefCountPtr_H

