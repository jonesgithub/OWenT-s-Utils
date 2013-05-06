/**
 *
 * @file smart_ptr.h
 * @brief ��������ָ���
 * Licensed under the MIT licenses.
 *
 * @version 1.0
 * @author OWenT, owt5008137@live.com
 * @date 2012.08.02
 *
 * @history
 *
 */
 
#ifndef _STD_SMARTPTR_H_
#define _STD_SMARTPTR_H_
 
 
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif
 
// ============================================================
// ������������
// �Զ�����TR1��
// ============================================================
  
/**
* ��������ָ�루smart_ptr��
* �����G++��֧��c++0x�ݰ�1��tr1�汾����smart_ptr[GCC�汾����4.0]
* �������GNU-C++������ָ��
*
* �����VC++��֧��c++0x�ݰ�1��tr1�汾����smart_ptr[VC++�汾����9.0 SP1]
* �������VC++������ָ��
*
* ��������boost�е�smart_ptr�⣨��������������Ҫ����boost�⣩
*/
  
// VC9.0 SP1���Ϸ�֧�ж�
#if defined(_MSC_VER) && (_MSC_VER == 1500 && defined (_HAS_TR1) || _MSC_VER > 1500)
    // ����VC std::tr1��
    #include <memory>
#elif defined(__GNUC__) && __GNUC__ >= 4
    // ����G++ std::tr1��
    #ifndef __GXX_EXPERIMENTAL_CXX0X__
        #include <tr1/memory>
        namespace std {
            using tr1::bad_weak_ptr;
            using tr1::const_pointer_cast;
            using tr1::dynamic_pointer_cast;
            using tr1::enable_shared_from_this;
            using tr1::get_deleter;
            using tr1::shared_ptr;
            using tr1::static_pointer_cast;
            using tr1::swap;
            using tr1::weak_ptr;
        }
    #else
        #include <memory>
    #endif
#else
    // ����boost tr1��
    #include <boost/tr1/memory.hpp>
    namespace std {
        using tr1::bad_weak_ptr;
        using tr1::const_pointer_cast;
        using tr1::dynamic_pointer_cast;
        using tr1::enable_shared_from_this;
        using tr1::get_deleter;
        using tr1::shared_ptr;
        using tr1::static_pointer_cast;
        using tr1::swap;
        using tr1::weak_ptr;
    }
#endif
 
#endif
