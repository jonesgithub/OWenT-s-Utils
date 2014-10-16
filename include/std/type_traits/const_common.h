/**
 * Copyright (c) 2014, Tencent
 * All rights reserved.
 *
 * @file const_common.h
 * @brief 
 *
 *
 * @version 1.0
 * @author owentou, owentou@tencent.com
 * @date 2014年3月27日
 *
 * @history
 *
 */

#ifndef _UTIL_TYPE_TRAITS_CONST_COMMON_H_
#define _UTIL_TYPE_TRAITS_CONST_COMMON_H_

namespace util
{
    namespace type_traits
    {
        /** ==================== type traits utils ==================== **/
        template <typename TC>
        struct type_checker
        {
            typedef char yes_type;
            struct no_type
            {
               char padding[8];
            };

            struct any_conversion
            {
                template <typename T> any_conversion(const volatile T&);
                template <typename T> any_conversion(const T&);
                template <typename T> any_conversion(volatile T&);
                template <typename T> any_conversion(T&);
            };
            static no_type _m_check(any_conversion);
            static yes_type _m_check(TC);
        };


        // TEMPLATE CLASS integral_constant
        template<class _Ty, _Ty _Val>
        struct integral_constant
        {
            // convenient template for integral constant types
            static const _Ty value = _Val;

            typedef _Ty value_type;
            typedef integral_constant<_Ty, _Val> type;

            operator value_type() const
            {   // return stored value
                return (value);
            }
        };

        typedef integral_constant<bool, true> true_type;
        typedef integral_constant<bool, false> false_type;


        /**
         * type traits - enable if
         */
        template<bool, typename Ty = void>
        struct enable_if {};

        template<typename Ty>
        struct enable_if<true, Ty>
        {
            typedef Ty type;
        };
    }
}


#endif /* CONST_COMMON_H_ */
