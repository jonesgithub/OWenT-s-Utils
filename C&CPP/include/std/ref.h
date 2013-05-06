/**
 * @file functional.h
 * @brief ����߼��⣬hash�����ð�װ��
 * Licensed under the MIT licenses.
 *
 * @version 1.0
 * @author OWenT, owt5008137@live.com
 * @date 2012.08.02
 *
 * @history
 *
 */
 
#ifndef _STD_FUNCTIONAL_H_
#define _STD_FUNCTIONAL_H_
 
#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif
 
// ============================================================
// ������������
// �Զ�����TR1��
// ============================================================
  
/**
* �������ð�װ��ref, ref_wrapper��
* �����G++��֧��c++0x�ݰ�1��tr1�汾����functional[GCC�汾����4.0]
* �������GNU-C++��functional �е�ref
*
* �����VC++��֧��c++0x�ݰ�1��tr1�汾����functional[VC++�汾����9.0 SP1]
* �������VC++��functional
*
* ��������boost�е�functional�⣨��������������Ҫ����boost�⣩
*/
  
// VC9.0 SP1���Ϸ�֧�ж�
#if defined(_MSC_VER) && (_MSC_VER == 1500 && defined (_HAS_TR1) || _MSC_VER > 1500)
    // ����VC std::tr1��
    #include <functional>
#elif defined(__GNUC__) && __GNUC__ >= 4
    // ����G++ std::tr1��
    #ifndef __GXX_EXPERIMENTAL_CXX0X__
        #include <tr1/functional>
        namespace std {
            using tr1::ref;
            using tr1::cref;
            using tr1::reference_wrapper;
        }
    #else
        #include <functional>
    #endif
#else
    // ����boost tr1��
    #include <boost/tr1/functional>
    namespace std {
        using tr1::ref;
        using tr1::cref;
        using tr1::reference_wrapper;
    }
#endif
 
#endif
