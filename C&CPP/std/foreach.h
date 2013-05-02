/**
 * @file foreach.hpp
 * @brief foreach ����
 *        ֧��VC++��GCC
 *        foreach��֧
 *        1. �ڱ�����֧�ֲ�������C++11��׼�ǻ�ʹ��C++11�� range-based-loop
 *        2. �ϰ汾��VC��֧��for each(var obj in arr)ʱ��ʹ��VC��������for each�﷨
 *        3. �����Ŀʹ����BOOST�⣬��ʹ��BOOST_FOREACH
 *        4. �����������������㣬���ʹ���Լ�ʵ�ֵļ�foreach [���� GCC 4.1.2 4.4.5 4.7.1 �� VC 11.0 RC �²��Թ�]
 *
 *
 * Licensed under the MIT licenses.
 *
 * @version 1.0
 * @author OWenT, owt5008137@live.com
 * @date 2012.06.27
 * @example
 *       //����
 *       const int arr[] = {1, 7, 3, 9, 5, 6, 2, 8, 4};
 *       owent_foreach(const int& v, arr) {
 *           printf("%d\n", v);
 *       }
 *
 *       //std::vector
 *       std::vector<int> vec;
 *       //...
 *       owent_foreach(const int& v, vec) {
 *           printf("%d\n", v);
 *       }
 *
 *       //std::map
 *       std::map<int, int> mp;
 *       //...
 *       typedef std::pair<const int, int> map_pair; // ����foreach�Ǻ궨�壬��������������ŵĻ�������ô�����������������Ϊ��������ǲ����ָ���
 *       owent_foreach(map_pair& pr, mp) {
 *           pr.second = 0;
 *       }
 *
 * @history
 *   2012.07.19 ���Ӷ���const������stl������֧��
 *
 */
 
#ifndef owent_foreach
 
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif
 
// ============================================================
// ����������
// foreach��ʵ��ԭ������boost��foreach
// �Զ����ñ������ṩ��foreach����
// ============================================================
  
/**
 * foreach����
 * �����G++�Ұ汾����4.6�ҿ�����c++0x��c++11, ������֧��C++11��VC++
 * �������C++11��׼��range-based-forѭ��
 *
 * �����VC++�Ұ汾����9.0 SP1
 * �������VC++�� for each (object var in collection_to_loop)
 *
 * ���������BOOST�� (���Ӻ궨�� FOREACH_WITH_BOOST_HPP )
 * ���ʹ��BOOST_FOREACH
 *
 * �����Զ���foreach����
 */
  
// VC11.0 SP1���Ϸ�֧�ж�
#if defined(_MSC_VER) && (_MSC_VER > 1500 || (_MSC_VER == 1500 && defined (_HAS_TR1)))
    #if _MSC_VER >= 1700
        // ���� VC ��range-based-forѭ��
        #define owent_foreach(VAR, COL) for(VAR : COL)
    #else
        // ���� VC �� for each (object var in collection_to_loop)
        #define owent_foreach(VAR, COL) for each (VAR in COL)
    #endif
#elif defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)) && defined(__GXX_EXPERIMENTAL_CXX0X__)
    // ����G++ ��range-based-forѭ��
    #define owent_foreach(VAR, COL) for(VAR : COL)
#elif defined(FOREACH_WITH_BOOST_HPP) || defined(BOOST_FOREACH)
    #include <boost/foreach.hpp>
    #define owent_foreach(VAR, COL) BOOST_FOREACH(VAR, COL)
#else
    #include <cstddef>
    // �������޵�foreach����
    namespace foreach_detail{
        ///////////////////////////////////////////////////////////////////////////////
        // �Զ�����
        ///////////////////////////////////////////////////////////////////////////////
        struct auto_any_base
        {
            // ���ں궨���еĳ�ʼ���ж�
            operator bool() const
            {
                return false;
            }
        };
 
        template<typename T>
        struct auto_any : auto_any_base
        {
            explicit auto_any(T const &t)
              : item(t)
            {
            }
 
            // ����Ϊ���ÿɱ�
            mutable T item;
        };
 
        typedef auto_any_base const& auto_any_t;
        #define OWENT_FOREACH_ANY_TYPE(x) auto_any<x>
        #define OWENT_FOREACH_ARRAY_DECL(x, y, z) x (&y)[z]
        #define OWENT_FOREACH_ALLOC_DECL(x, y) x& y
        #if defined(_MSC_VER) && _MSC_VER <= 1500
        // ĳЩ�汾��VC,����������������������(����VC 6)
        // ��Ҫʹ��__LINE__������Ψһ��ʶ��������BOOST_FOREACH��
            #define OWENT_FOREACH_CAT(x, y) x ## y
            #define OWENT_FOREACH_ID(x) OWENT_FOREACH_CAT(x, __LINE__)
        #else
            #define OWENT_FOREACH_ID(x) x
        #endif
 
        template<typename _Ty> inline
        _Ty& auto_any_cast(auto_any_t a)
        {
            return static_cast<auto_any<_Ty> const &>(a).item;
        }
 
        /////////////////////////////////////////////////////////////////////////////
        // ��ȡ��ʼλ��
        /////////////////////////////////////////////////////////////////////////////
     
        // ģ���������
        template<typename _Ty> inline
        OWENT_FOREACH_ANY_TYPE(typename _Ty::const_iterator) begin(OWENT_FOREACH_ALLOC_DECL(const _Ty, c))
        {   // ��ȡ������ʼ������
            return OWENT_FOREACH_ANY_TYPE(typename _Ty::const_iterator)(c.begin());
        }
 
        template<typename _Ty> inline
        OWENT_FOREACH_ANY_TYPE(typename _Ty::iterator) begin(OWENT_FOREACH_ALLOC_DECL(_Ty, c))
        {   // ��ȡ������ʼ������
            return OWENT_FOREACH_ANY_TYPE(typename _Ty::iterator)(c.begin());
        }
 
        // ����ָ��
        template<typename _Ty, std::size_t _Size> inline
        OWENT_FOREACH_ANY_TYPE(const _Ty*) begin(OWENT_FOREACH_ARRAY_DECL(const _Ty, arr, _Size))
        {   
            return OWENT_FOREACH_ANY_TYPE(const _Ty*)(arr);
        }
 
        template<typename _Ty, std::size_t _Size> inline
        OWENT_FOREACH_ANY_TYPE(_Ty*) begin(OWENT_FOREACH_ARRAY_DECL(_Ty, arr, _Size))
        {   
            return OWENT_FOREACH_ANY_TYPE(_Ty*)(arr);
        }
 
        /////////////////////////////////////////////////////////////////////////////
        // �ƶ��α�
        /////////////////////////////////////////////////////////////////////////////
     
        // ģ���������
        template<typename _Ty> inline
        void next(OWENT_FOREACH_ALLOC_DECL(const _Ty, _Array), auto_any_t cur)
        {
            ++ auto_any_cast<typename _Ty::const_iterator>(cur);
        }
 
        template<typename _Ty> inline
        void next(OWENT_FOREACH_ALLOC_DECL(_Ty, _Array), auto_any_t cur)
        {
            ++ auto_any_cast<typename _Ty::iterator>(cur);
        }
 
        // ����ָ��
        template<typename _Ty, std::size_t _Size> inline
        void next(OWENT_FOREACH_ARRAY_DECL(const _Ty, arr, _Size), auto_any_t cur)
        {
            ++ auto_any_cast<const _Ty*>(cur);
        }
 
        template<typename _Ty, std::size_t _Size> inline
        void next(OWENT_FOREACH_ARRAY_DECL(_Ty, arr, _Size), auto_any_t cur)
        {
            ++ auto_any_cast<_Ty*>(cur);
        }
     
     
        /////////////////////////////////////////////////////////////////////////////
        // �ж��Ƿ����
        /////////////////////////////////////////////////////////////////////////////
 
        // ģ���������
        template<typename _Ty> inline
        bool end(OWENT_FOREACH_ALLOC_DECL(const _Ty, _Array), auto_any_t cur)
        {
            return auto_any_cast<typename _Ty::const_iterator>(cur) == _Array.end();
        }
 
        template<typename _Ty> inline
        bool end(OWENT_FOREACH_ALLOC_DECL(_Ty, _Array), auto_any_t cur)
        {
            return auto_any_cast<typename _Ty::iterator>(cur) == _Array.end();
        }
 
        // ����ָ��
        template<typename _Ty, std::size_t _Size> inline
        bool end(OWENT_FOREACH_ARRAY_DECL(const _Ty, arr, _Size), auto_any_t cur)
        {
            return static_cast<std::size_t>(auto_any_cast<const _Ty*>(cur) - arr) >= _Size;
        }
 
        template<typename _Ty, std::size_t _Size> inline
        bool end(OWENT_FOREACH_ARRAY_DECL(_Ty, arr, _Size), auto_any_t cur)
        {
            return static_cast<std::size_t>(auto_any_cast<_Ty*>(cur) - arr) >= _Size;
        }
 
        /////////////////////////////////////////////////////////////////////////////
        // ����ת��
        /////////////////////////////////////////////////////////////////////////////
     
        // ģ�������������
        template<typename _Ty> inline
        typename _Ty::const_iterator& deref(OWENT_FOREACH_ALLOC_DECL(const _Ty, arr), auto_any_t cur)
        {
            return auto_any_cast<typename _Ty::const_iterator>(cur);
        }
 
        template<typename _Ty> inline
        typename _Ty::iterator& deref(OWENT_FOREACH_ALLOC_DECL(_Ty, arr), auto_any_t cur)
        {
            return auto_any_cast<typename _Ty::iterator>(cur);
        }
 
        // ����Ԫ��
        template<typename _Ty, std::size_t _Size> inline
        const _Ty*& deref(OWENT_FOREACH_ARRAY_DECL(const _Ty, arr, _Size), auto_any_t cur)
        {
            return auto_any_cast<const _Ty*>(cur);
        }
 
        template<typename _Ty, std::size_t _Size> inline
        _Ty*& deref(OWENT_FOREACH_ARRAY_DECL(_Ty, arr, _Size), auto_any_t cur)
        {
            return auto_any_cast<_Ty*>(cur);
        }
    }
 
    #define OWENT_FOREACH(VAR, COL)                                                                                   \
        if (foreach_detail::auto_any_t OWENT_FOREACH_ID(_owent_foreach_cur) = foreach_detail::begin(COL)){ } else     \
        for (bool OWENT_FOREACH_ID(_owent_foreach_flag) = true;                                                       \
            OWENT_FOREACH_ID(_owent_foreach_flag) && !foreach_detail::end(COL, OWENT_FOREACH_ID(_owent_foreach_cur)); \
            foreach_detail::next(COL, OWENT_FOREACH_ID(_owent_foreach_cur)))                                          \
                                                                                                                      \
            if (bool OWENT_FOREACH_ID(_owent_foreach_flag) = false){ } else                                           \
            for (VAR = *foreach_detail::deref(COL, OWENT_FOREACH_ID(_owent_foreach_cur));                             \
                !OWENT_FOREACH_ID(_owent_foreach_flag);                                                               \
                OWENT_FOREACH_ID(_owent_foreach_flag) = true)
 
    #define owent_foreach(VAR, COL) OWENT_FOREACH(VAR, COL)
#endif
 
#endif
