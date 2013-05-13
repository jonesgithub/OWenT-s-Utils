/**
 * @file functional.h
 * @brief 导入高级库，hash、引用包装等
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
// 公共包含部分
// 自动导入TR1库
// ============================================================
  
/**
* 导入引用包装（ref, ref_wrapper）
* 如果是G++且支持c++0x草案1（tr1版本）的functional[GCC版本高于4.0]
* 则会启用GNU-C++的functional 中的ref
*
* 如果是VC++且支持c++0x草案1（tr1版本）的functional[VC++版本高于9.0 SP1]
* 则会启用VC++的functional
*
* 否则启用boost中的functional库（如果是这种情况需要加入boost库）
*/
  
// VC9.0 SP1以上分支判断
#if defined(_MSC_VER) && (_MSC_VER == 1500 && defined (_HAS_TR1) || _MSC_VER > 1500)
    // 采用VC std::tr1库
    #include <functional>
#elif defined(__GNUC__) && __GNUC__ >= 4
    // 采用G++ std::tr1库
    #if !defined(__GXX_EXPERIMENTAL_CXX0X__) && !defined(__clang__)
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
    // 采用boost tr1库
    #include <boost/tr1/functional>
    namespace std {
        using tr1::ref;
        using tr1::cref;
        using tr1::reference_wrapper;
    }
#endif
 
#endif
