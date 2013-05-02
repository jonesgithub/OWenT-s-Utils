/**
 * @file array.h
 * @brief ����֧��STL����������
 * Licensed under the MIT licenses.
 *
 * @version 1.0
 * @author OWenT, owt5008137@live.com
 * @date 2012.08.02
 *
 * @history
 *
 */
 
#ifndef _STD_ARRAY_H_
#define _STD_ARRAT_H_
 
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif
 
// ============================================================
// ������������
// �Զ�����TR1��
// ============================================================
  
/**
* �������飨array��
* �����G++��֧��c++0x�ݰ�1��tr1�汾����array[GCC�汾����4.0]
* �������GNU-C++������
*
* �����VC++��֧��c++0x�ݰ�1��tr1�汾����array[VC++�汾����9.0 SP1]
* �������VC++������
*
* ��������boost�е�array�⣨��������������Ҫ����boost�⣩
*/
  
// VC9.0 SP1���Ϸ�֧�ж�
#if defined(_MSC_VER) && (_MSC_VER == 1500 && defined (_HAS_TR1) || _MSC_VER > 1500)
    // ����VC std::tr1��
    #include <array>
#elif defined(__GNUC__) && __GNUC__ >= 4
    // ����G++ std::tr1��
    #ifndef __GXX_EXPERIMENTAL_CXX0X__
        #include <tr1/array>
        namespace std {
            using tr1::array;
            using tr1::get;
            using tr1::tuple_element;
            using tr1::tuple_size;
        }
    #else
        #include <array>
    #endif
 
#else
    // ����boost��
    #include <boost/tr1/array.hpp>
    namespace std {
        using tr1::array;
        using tr1::get;
        using tr1::tuple_element;
        using tr1::tuple_size;
    }
#endif
 
#endif
