/**
 * @file tuple.h
 * @brief �����Ԫ���
 * Licensed under the MIT licenses.
 *
 * @version 1.0
 * @author OWenT, owt5008137@live.com
 * @date 2012.08.02
 *
 * @history
 *
 */
 
#ifndef _STD_TUPLE_H_
#define _STD_TUPLE_H_
 
 
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif
 
// ============================================================
// ������������
// �Զ�����TR1��
// ============================================================
  
/**
* �����άԪ�飨tuple��
* �൱��std::pair����ǿ�棬���֧��10ά
* �����G++��֧��c++0x�ݰ�1��tr1�汾����tuple[GCC�汾����4.0]
* �������GNU-C++�Ķ�άԪ��
*
* �����VC++��֧��c++0x�ݰ�1��tr1�汾����tuple[VC++�汾����9.0 SP1]
* �������VC++�Ķ�άԪ��
*
* ��������boost�е�tuple�⣨��������������Ҫ����boost�⣩
*/
  
 
#include "utility.h"
 
// VC9.0 SP1���Ϸ�֧�ж�
#if defined(_MSC_VER) && (_MSC_VER == 1500 && defined (_HAS_TR1) || _MSC_VER > 1500)
    // ����VC std::tr1��
    #include <tuple>
#elif defined(__GNUC__) && __GNUC__ >= 4
    // ����G++ std::tr1��
    #ifndef __GXX_EXPERIMENTAL_CXX0X__
        #include <tr1/tuple>
        namespace std {
            using tr1::get;
            using tr1::ignore;
            using tr1::make_tuple;
            using tr1::tie;
            using tr1::tuple;
            using tr1::tuple_element;
            using tr1::tuple_size;
        }
    #else
        #include <tuple>
    #endif
#else
// ����boost��
#include <boost/tr1/tuple.hpp>
namespace std {
    using tr1::get;
    using tr1::ignore;
    using tr1::make_tuple;
    using tr1::tie;
    using tr1::tuple;
    using tr1::tuple_element;
    using tr1::tuple_size;
}
#endif
 
#endif
