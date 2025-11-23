#ifndef __SMARTPTR_H__
#define __SMARTPTR_H__

/**
 * Smart pointer with reference counting.
 *
 * Based on Marshall Cline's C++ FAQ Lite:
 * http://www.parashift.com/c++-faq-lite/
 */
template <class T>
class SmartPtr {
public:
	SmartPtr(): m_Ptr(0) {
	}

	SmartPtr(T* p): m_Ptr(p) {
		if (m_Ptr)
            m_Ptr->IncRef();
	}

	SmartPtr(const SmartPtr<T> &p): m_Ptr(p.m_Ptr) {
		if (m_Ptr) 
            m_Ptr->IncRef();
	}

	virtual ~SmartPtr() {
        if (m_Ptr && m_Ptr->DecRef() == 0)
			delete m_Ptr;
	}

	SmartPtr<T>& operator= (const SmartPtr<T> &p) {
		/*
		 * DO NOT CHANGE THE ORDER OF THESE STATEMENTS!
		 * (This order properly handles self-assignment and other issues)
		 */

		T* const old = m_Ptr;
		m_Ptr = p.m_Ptr;
		if (m_Ptr)
            m_Ptr->IncRef();

        if (old && old->DecRef() == 0)
			delete old;

		return *this;
	}

	bool operator== (const T *p) {
		return m_Ptr == p;
	}

	bool operator== (SmartPtr<T> &p) {
		return m_Ptr == p.m_Ptr;
	}

	bool operator!= (const T *p) {
		return !(m_Ptr == p);
	}

	bool operator!= (SmartPtr<T> &p) {
		return !(m_Ptr == p.m_Ptr);
	}

	T* operator-> () {
		return m_Ptr;
	}

	T& operator* () {
		return *m_Ptr;
	}

	/* bypass, use with caution */
	T* GetPtr() {
		return m_Ptr;
	}
/* useless
	unsigned int GetRefCount() {
		return m_Ptr ? m_Ptr->refcount : 0;
	}
*/
private:
	T	*m_Ptr;
};

#endif
