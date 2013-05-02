/**
 * @file IdxList.h
 * @brief �̶��±�����<br />
 *        �����ڹ����ڴ�<br />
 * Licensed under the MIT licenses.
 *
 * @warning ע�⣺������´����Ľṹ����Ҫִ��construct������ʼ��
 * @note ���Ա����� GCC 4.7.2, VC 11.0
 *
 * @version 1.0
 * @author OWenT
 * @date 2013-2-26
 *
 * @history
 *      2013.02.28 ����const�޶�֧��
 *      2013.02.29 �Ż��������ṹ�������±���������Ȩ��
 *
 */

#ifndef __STATICIDXLIST_H_
#define __STATICIDXLIST_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include <algorithm>
#include <cstdlib>
#include <assert.h>

template<typename TObj, typename TSize>
struct StaticIdxListNode
{
	TSize iPreIdx;
	TSize iNextIdx;
	bool bIsInited;
	char stObjBin[sizeof(TObj)];
};

/**
 * �����ڹ����ڴ��C++������
 * @warning ע�⣺������´����Ľṹ����Ҫִ��construct������ʼ��,����ӹ����ڴ�ָ�������ִ��
 * @note Ŀ��ṹ������Ҫ��Ĭ�Ϲ��캯��, ���캯�������������
 * @note �ڴ�����Ϊ (sizeof(TObj) + 2 * sizeof(size_type)) * (MAX_SIZE + 1)
 */
template<typename TObj, int MAX_SIZE>
class StaticIdxList
{
public:
	typedef int size_type;
	typedef StaticIdxListNode<TObj, size_type> node_type;
	typedef TObj value_type;
	typedef StaticIdxList<TObj, MAX_SIZE> self_type;

	/**
	 * ����������
	 */
	template<typename ITObj>
	class Iterator
	{
	public:
		typedef Iterator<ITObj> self_type;
		typedef typename StaticIdxList<TObj, MAX_SIZE>::self_type list_type;

	private:
		mutable size_type iIndex;
		mutable list_type* m_pListPtr;

	public:
		Iterator(size_type index, const list_type* const pListPtr):
			iIndex(index),
			m_pListPtr(const_cast<list_type*>(pListPtr))
		{
		}

		inline size_type index() const { return iIndex; }

		self_type& operator++() const
		{
			iIndex = m_pListPtr->GetNextIdx(iIndex);

			return const_cast<Iterator&>(*this);
		}

		self_type operator++(int) const
		{
			Iterator stRet = (*this);
			++ stRet;
			return stRet;
		}

		self_type& operator--() const
		{
			iIndex = m_pListPtr->GetPreIdx(iIndex);

			return const_cast<Iterator&>(*this);
		}

		self_type operator--(int) const
		{
			Iterator stRet = (*this);
			-- stRet;
			return stRet;
		}

		ITObj* get()
		{
			return reinterpret_cast<ITObj*>(m_pListPtr->m_stData[iIndex].stObjBin);
		}

		const ITObj* get() const
		{
			return reinterpret_cast<const ITObj*>(m_pListPtr->m_stData[iIndex].stObjBin);
		}

		friend bool operator==(const Iterator& l, const Iterator& r)
		{
			return l.m_pListPtr == r.m_pListPtr && l.iIndex == r.iIndex;
		}

		friend bool operator!=(const Iterator& l, const Iterator& r)
		{
			return !(l == r);
		}

		inline ITObj* operator->()
		{
			return get();
		}

		inline const ITObj* operator->() const
		{
			return get();
		}

		inline ITObj& operator*()
		{
			return *get();
		}

		inline const ITObj& operator*() const
		{
			return *get();
		}

		void swap(const self_type& stIter) const // never throws
		{
			using std::swap;

			swap(m_pListPtr, stIter.m_pListPtr);
			swap(iIndex, stIter.iIndex);
		}

	};

	typedef Iterator<TObj> iterator;
	typedef const Iterator<TObj> const_iterator;

private:
	/**
	 * ���ݽ��������Ż�(size_type)
	 * @param [in] left
	 * @param [in] right
	 */
	inline void swap(size_type& left, size_type& right)
	{
		left ^= right ^= left ^= right;
	}

	struct IdxListHeader
	{
		size_type m_iLastUsedNode;
		size_type m_iSize;
	};

	IdxListHeader m_stHeader;
	node_type m_stData[MAX_SIZE + 1];

	size_type _create_node()
	{
		assert(m_stHeader.m_iLastUsedNode >= 0 && m_stHeader.m_iLastUsedNode <= MAX_SIZE);

		size_type iNewIdx = m_stData[m_stHeader.m_iLastUsedNode].iNextIdx;
		while (IsExists(iNewIdx))
		{
			iNewIdx = m_stData[iNewIdx].iNextIdx;
		}

		// ������������-1
		if (iNewIdx >= MAX_SIZE || IsExists(iNewIdx))
		{
			return -1;
		}

		// �ر�δ��ʼ�����š��ƶ����ڵ㡢���� +1
		m_stData[iNewIdx].bIsInited = true;
		m_stHeader.m_iLastUsedNode = iNewIdx;
		++ m_stHeader.m_iSize;

		return iNewIdx;
	}


	/**
	 * ��������ʹ��
	 */
	template<typename CObj>
	struct _destruct_obj
	{
		self_type& stSelf;

		_destruct_obj(self_type& self): stSelf(self){}

		void operator()(size_type idx, CObj& stObj)
		{
			stSelf.m_stData[idx].bIsInited = false;
			stObj.~TObj();
		}
	};

	/**
	 * ������������Ϊ��ͨ����ʱʹ��
	 */
	template<typename CObj>
	struct _count_cc_func
	{
		size_type& counter;
		bool (*fn)(size_type, CObj&);

		_count_cc_func(size_type& _counter, bool (*_fn)(size_type, CObj&)):
			counter(_counter), fn(_fn){}

		void operator()(size_type idx, CObj& stObj)
		{
			counter += (*fn)(idx, stObj)? 1: 0;
		}
	};

	/**
	 * ������������Ϊ�º���ʱʹ��
	 */
	template<typename _F, typename CObj>
	struct _count_obj_func
	{
		size_type& counter;
		_F& fn;

		_count_obj_func(size_type& _counter, _F& _fn):
			counter(_counter), fn(_fn){}

		void operator()(size_type idx, CObj& stObj)
		{
			counter += fn(idx, stObj)? 1: 0;
		}
	};

public:
	StaticIdxList()
	{
	}

	~StaticIdxList()
	{
		destruct();
	}

	/**
	 * ����ǵ�һ�δ�����������ôκ������г�ʼ��
	 */
	void construct()
	{
		m_stHeader.m_iSize = 0;
		m_stHeader.m_iLastUsedNode = MAX_SIZE; // ���һ��Ϊͷ���

		for (size_type i = 0; i <= MAX_SIZE; ++i)
		{
			m_stData[i].iPreIdx = (i - 1);
			m_stData[i].iNextIdx = (i + 1);
			m_stData[i].bIsInited = false;
		}
		m_stData[0].iPreIdx = MAX_SIZE;
		m_stData[MAX_SIZE].iNextIdx = 0;
		m_stData[MAX_SIZE].bIsInited = true;
	}

	/**
	 * �������ж������������(��ѡ)
	 */
	void destruct()
	{
		// ɾ������δ�ͷŶ���
		Foreach(_destruct_obj<TObj>(*this));
	}

	/**
	 * �ж��±�ڵ����
	 * @param [in] idx idx
	 * @return ���ڷ���true�����򷵻�false
	 */
	bool IsExists(size_type idx) const
	{
		// ��������������
		if (idx >= MAX_SIZE)
		{
			return false;
		}

		// С��0
		if (idx < 0)
		{
			return false;
		}

		// flag ���
		return m_stData[idx].bIsInited;
	}

	/**
	 * ��ȡ��һ��Ԫ�ص��±�
	 * @param [in] idx ��ǰԪ���±�
	 * @return ���ڷ�����һ��Ԫ���±꣬�����ڷ���-1
	 */
	size_type GetNextIdx(size_type idx) const
	{
		size_type iRet = -1;

		if (IsExists(idx))
		{
			iRet = m_stData[idx].iNextIdx;
		}

		iRet = IsExists(iRet)? iRet: -1;

		return iRet;

	}

	/**
	 * ��ȡ��һ��Ԫ�ص��±�
	 * @param [in] idx ��ǰԪ���±�
	 * @return ���ڷ�����һ��Ԫ���±꣬�����ڷ���-1
	 */
	size_type GetPreIdx(size_type idx) const
	{
		size_type iRet = -1;

		if (IsExists(idx))
		{
			iRet = m_stData[idx].iPreIdx;
		}

		iRet = IsExists(iRet)? iRet: -1;

		return iRet;

	}

	/**
	 * ��Idx��ȡ�ڵ�
	 * @param [in] idx
	 * @return �����򷵻����ݵ��������������򷵻�end������
	 */
	inline iterator Get(size_type idx)
	{
		return get(idx);
	}

	/**
	 * ��Idx��ȡ�ڵ�(const)
	 * @param [in] idx
	 * @return �����򷵻����ݵ��������������򷵻�end������
	 */
	inline const_iterator Get(size_type idx) const
	{
		return get(idx);
	}

	/**
	 * ��Idx��ȡ�ڵ�(const)
	 * @param [in] idx
	 * @return �����򷵻���������
	 */
	inline TObj& operator[](size_type idx) { return *get(idx); };

	/**
	 * ��Idx��ȡ�ڵ�(const)
	 * @param [in] idx
	 * @return �����򷵻����ݳ�������
	 */
	inline const TObj& operator[](size_type idx) const { return *get(idx); };

	/**
	 * �����ڵ㣬����idx
	 * @return �½ڵ��idx��ʧ�ܷ���-1
	 */
	size_type Create()
	{
		size_type ret = _create_node();
		if (ret >= 0)
		{
			new (m_stData[ret].stObjBin)TObj();
		}

		return ret;
	}

	/**
	 * �����ڵ㣬����idx
	 * @param [in] param1 ���캯������1
	 * @return �½ڵ��idx��ʧ�ܷ���-1
	 */
	template<typename _TP1>
	size_type Create(const _TP1& param1)
	{
		size_type ret = _create_node();
		if (ret >= 0)
		{
			new (m_stData[ret].stObjBin)TObj(param1);
		}

		return ret;
	}

	/**
	 * �����ڵ㣬����idx
	 * @param [in] param1 ���캯������1
	 * @param [in] param2 ���캯������2
	 * @return �½ڵ��idx��ʧ�ܷ���-1
	 */
	template<typename _TP1, typename _TP2>
	size_type Create(const _TP1& param1, const _TP2& param2)
	{
		size_type ret = _create_node();
		if (ret >= 0)
		{
			new (m_stData[ret].stObjBin)TObj(param1, param2);
		}

		return ret;
	}

	/**
	 * �����ڵ㣬����idx
	 * @param [in] param1 ���캯������1
	 * @param [in] param2 ���캯������2
	 * @param [in] param3 ���캯������3
	 * @return �½ڵ��idx��ʧ�ܷ���-1
	 */
	template<typename _TP1, typename _TP2, typename _TP3>
	size_type Create(const _TP1& param1, const _TP2& param2, const _TP3& param3)
	{
		size_type ret = _create_node();
		if (ret >= 0)
		{
			new (m_stData[ret].stObjBin)TObj(param1, param2, param3);
		}

		return ret;
	}

	/**
	 * �Ƴ�һ��Ԫ��
	 * @param [in] idx �±�
	 */
	void Remove(size_type idx)
	{
		using std::swap;

		// ������ֱ�ӷ���
		if(!IsExists(idx))
		{
			return;
		}

		size_type iPreIdx = m_stData[idx].iPreIdx;
		size_type iNextIdx = m_stData[idx].iNextIdx;
		size_type iFreeFirst = m_stData[m_stHeader.m_iLastUsedNode].iNextIdx;

		// ɾ���ڵ㣬��������
		swap(m_stData[iPreIdx].iNextIdx, m_stData[idx].iNextIdx);
		swap(m_stData[iNextIdx].iPreIdx, m_stData[idx].iPreIdx);

		// ��ֹ�Լ����Լ�����
		if (idx == m_stHeader.m_iLastUsedNode)
		{
			m_stHeader.m_iLastUsedNode = iPreIdx;
		}

		// �������ɽڵ㣬��������
		swap(m_stData[m_stHeader.m_iLastUsedNode].iNextIdx, m_stData[idx].iNextIdx);
		swap(m_stData[iFreeFirst].iPreIdx, m_stData[idx].iPreIdx);

		// �ڵ���Ϊ
		m_stData[idx].bIsInited = false;
		m_stHeader.m_iLastUsedNode = iPreIdx;

		// ִ������
		((TObj*)m_stData[idx].stObjBin)->~TObj();
		// ������һ
		-- m_stHeader.m_iSize;
	}

	/**
	 * �Ƿ�Ϊ��
	 * @return Ϊ�շ���true
	 */
	inline bool IsEmpty() const { return empty(); }

	/**
	 * �Ƿ�����
	 * @return ��������true
	 */
	inline bool IsFull() const { return size() >= max_size(); }

	// ===============================
	// =====        ������ ����                  =====
	// ===============================
	iterator get(size_type idx)
	{
		if (!IsExists(idx))
		{
			return iterator(-1, this);
		}

		return iterator(idx, this);
	}

	const_iterator get(size_type idx) const
	{
		if (!IsExists(idx))
		{
			return const_iterator(-1, this);
		}

		return const_iterator(idx, this);
	}

	iterator begin()
	{
		return get(m_stData[MAX_SIZE].iNextIdx);
	}

	const_iterator begin() const
	{
		return get(m_stData[MAX_SIZE].iNextIdx);
	}

	iterator end()
	{
		return get(-1);
	}

	const_iterator end() const
	{
		return get(-1);
	}

	size_t size() const
	{
		return static_cast<size_t>(Count());
	}

	size_t max_size() const
	{
		return MAX_SIZE;
	}

	iterator erase(iterator iter)
	{
		size_type idx = iter.index();
		++iter;

		Remove(idx);

		return iter;
	}

	inline bool empty() const
	{
		return size() == 0;
	}

	// �߼�����
	// ===============================
	// =====       Lambda ����             =====
	// ===============================


	/**
	 * foreach ����
	 * @param fn ִ�зº�������������Ϊ (size_type, TObj&)
	 */
	template<typename _F>
	void Foreach(_F fn)
	{
		size_type iIter = m_stData[MAX_SIZE].iNextIdx;
		while (IsExists(iIter))
		{
			fn(iIter, *reinterpret_cast<TObj*>(m_stData[iIter].stObjBin));
			iIter = m_stData[iIter].iNextIdx;
		}
	}

	/**
	 * const foreach ����
	 * @param fn ִ�зº�������������Ϊ (size_type, TObj&)
	 */
	template<typename _F>
	void Foreach(_F fn) const
	{
		size_type iIter = m_stData[MAX_SIZE].iNextIdx;
		while (IsExists(iIter))
		{
			fn(iIter, *reinterpret_cast<const TObj*>(m_stData[iIter].stObjBin));
			iIter = m_stData[iIter].iNextIdx;
		}
	}

	/**
	 * foreach ����
	 * @param fn ִ�к�������������Ϊ (size_type, TObj&)
	 */
	template<typename _R>
	void Foreach(_R (*fn)(size_type, TObj&))
	{
		size_type iIter = m_stData[MAX_SIZE].iNextIdx;
		while (IsExists(iIter))
		{
			(*fn)(iIter, *reinterpret_cast<TObj*>(m_stData[iIter].stObjBin));
			iIter = m_stData[iIter].iNextIdx;
		}
	};

	/**
	 * const foreach ����
	 * @param fn ִ�к�������������Ϊ (size_type, TObj&)
	 */
	template<typename _R>
	void Foreach(_R (*fn)(size_type, const TObj&)) const
	{
		size_type iIter = m_stData[MAX_SIZE].iNextIdx;
		while (IsExists(iIter))
		{
			(*fn)(iIter, *reinterpret_cast<const TObj*>(m_stData[iIter].stObjBin));
			iIter = m_stData[iIter].iNextIdx;
		}
	};

	/**
	 * ��ȡԪ�ظ���
	 * @return Ԫ�ظ���
	 */
	size_type Count() const { return m_stHeader.m_iSize; };

public:
	/**
	 * ��ȡ����������Ԫ�ظ���
	 * @param [in] fn ��������
	 * @return ����������Ԫ�ظ���
	 */
	size_type Count(bool (*fn)(size_type, const TObj&)) const
	{
		size_type iRet = 0;

		Foreach(_count_cc_func<const TObj>(iRet, fn));

		return iRet;
	};

	/**
	 * ��ȡ����������Ԫ�ظ���
	 * @param [in] fn ��������
	 * @return ����������Ԫ�ظ���
	 */
	size_type Count(bool (*fn)(size_type, TObj&))
	{
		size_type iRet = 0;

		Foreach(_count_cc_func<TObj>(iRet, fn));

		return iRet;
	};

public:
	/**
	 * ��ȡ����������Ԫ�ظ���
	 * @param [in] fn �����º���
	 * @return ����������Ԫ�ظ���
	 */
	template<typename _F>
	size_type Count(_F fn) const
	{
		size_type iRet = 0;

		Foreach(_count_obj_func<_F, const TObj>(iRet, fn));

		return iRet;
	};

	/**
	 * ��ȡ����������Ԫ�ظ���
	 * @param [in] fn �����º���
	 * @return ����������Ԫ�ظ���
	 */
	template<typename _F>
	size_type Count(_F fn)
	{
		size_type iRet = 0;

		Foreach(_count_obj_func<_F, TObj>(iRet, fn));

		return iRet;
	};

};


#endif /* IDXLIST_H_ */
