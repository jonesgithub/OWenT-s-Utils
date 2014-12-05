#pragma once

#include <string>
#include <cstring>
#include <stdint.h>
#include <cstddef>
#include <cstdio>
#include <list>
#include <vector>
#include <tuple>
#include <functional>
#include <type_traits>

#include <std/explicit_declare.h>

#include "LuaBindingWrapper.h"

namespace script {
    namespace lua {
        namespace detail{

            template<typename Tt, typename Ty>
            struct unwraper_var_lua_type;

            template<typename Tt>
            struct unwraper_var_lua_type<Tt, lua_Unsigned> {
                typedef lua_Unsigned value_type;

                static Tt unwraper(lua_State* L, int index) {
                    if (lua_gettop(L) < index)
                        return static_cast<Tt>(0);

                    return static_cast<Tt>(luaL_checkinteger(L, index));
                }
            };

            template<typename Tt>
            struct unwraper_var_lua_type<Tt, lua_CFunction> {
                typedef lua_CFunction value_type;

                static Tt unwraper(lua_State* L, int index) {
                    if (lua_gettop(L) < index)
                        return static_cast<Tt>(nullptr);

                    return static_cast<Tt>(lua_tocfunction(L, index));
                }
            };

            template<typename Tt>
            struct unwraper_var_lua_type<Tt, lua_Integer> {
                typedef lua_Integer value_type;

                static Tt unwraper(lua_State* L, int index) {
                    if (lua_gettop(L) < index)
                        return static_cast<Tt>(0);

                    return static_cast<Tt>(luaL_checkinteger(L, index));
                }
            };

            template<typename Tt>
            struct unwraper_var_lua_type<Tt, lua_Number> {
                typedef lua_Number value_type;

                static Tt unwraper(lua_State* L, int index) {
                    if (lua_gettop(L) < index)
                        return static_cast<Tt>(0);

                    return static_cast<Tt>(luaL_checknumber(L, index));
                }
            };

            template<typename Tt, typename Ty>
            struct unwraper_var_lua_type {
                typedef Ty value_type;

                static Tt unwraper(lua_State* L, int index) {
                    typedef typename std::remove_reference<
                        typename std::remove_const<
                            typename std::remove_pointer<Tt>::type
                        >::type
                    >::type obj_t;

                    if (lua_gettop(L) < index)
                        return static_cast<Tt>(nullptr);

                    typedef typename LuaBindingUserdataInfo<obj_t>::userdata_type ud_t;
                    const char* clazz_name = LuaBindingUserdataInfo<obj_t>::getLuaMetaTableName();

                    if (!LuaBindingMgr::Instance()->isUserdataRegistered(clazz_name)) {
                        return static_cast<Tt>(
                            const_cast<void*>(lua_topointer(L, index))
                        );
                    }

                    if (lua_isnil(L, index)) {
                        return static_cast<Tt>(nullptr);
                    }

                    ud_t* ptr = reinterpret_cast<ud_t*>(luaL_checkudata(L, index, clazz_name));
                    if (nullptr == ptr) {
                        return static_cast<Tt>(nullptr);
                    }

                    return static_cast<Tt>(*ptr);
                }
            };


            template<typename TRet, typename... Ty>
            struct unwraper_var;

            template<typename... Ty>
            struct unwraper_var<void, Ty...> {
                template<typename TParam>
                static void unwraper(lua_State* L, int index) {
                }
            };

            template<typename... Ty> struct unwraper_var<uint8_t, Ty...> : public unwraper_var_lua_type<uint8_t, lua_Unsigned>{};
            template<typename... Ty> struct unwraper_var<uint16_t, Ty...> : public unwraper_var_lua_type<uint16_t, lua_Unsigned>{};
            template<typename... Ty> struct unwraper_var<uint32_t, Ty...> : public unwraper_var_lua_type<uint32_t, lua_Unsigned>{};
            template<typename... Ty> struct unwraper_var<uint64_t, Ty...> : public unwraper_var_lua_type<uint64_t, lua_Unsigned>{};

            template<typename... Ty> struct unwraper_var<int8_t, Ty...> : public unwraper_var_lua_type<int8_t, lua_Integer>{};
            template<typename... Ty> struct unwraper_var<int16_t, Ty...> : public unwraper_var_lua_type<int16_t, lua_Integer>{};
            template<typename... Ty> struct unwraper_var<int32_t, Ty...> : public unwraper_var_lua_type<int32_t, lua_Integer>{};
            template<typename... Ty> struct unwraper_var<int64_t, Ty...> : public unwraper_var_lua_type<int64_t, lua_Integer>{};

            template<typename... Ty> struct unwraper_var<float, Ty...> : public unwraper_var_lua_type<float, lua_Number>{};
            template<typename... Ty> struct unwraper_var<double, Ty...> : public unwraper_var_lua_type<double, lua_Number>{};

            template<typename... Ty> struct unwraper_var<lua_CFunction, Ty...> : public unwraper_var_lua_type<lua_CFunction, lua_CFunction>{};

            template<typename... Ty>
            struct unwraper_var<bool, Ty...> {
                static bool unwraper(lua_State* L, int index) {
                    if (lua_gettop(L) < index)
                        return static_cast<bool>(false);

                    return !!lua_toboolean(L, index);
                }
            };

            template<typename... Ty>
            struct unwraper_var<const char*, Ty...> {
                static const char* unwraper(lua_State* L, int index) {
                    if (lua_gettop(L) < index)
                        return static_cast<const char*>(nullptr);

                    return luaL_checkstring(L, index);
                }
            };

            template<typename... Ty>
            struct unwraper_var<char*, Ty...> {
                static char* unwraper(lua_State* L, int index) {
                    if (lua_gettop(L) < index)
                        return static_cast<char*>(nullptr);

                    return const_cast<char*>(luaL_checkstring(L, index));
                }
            };

#ifdef LUA_BINDING_ENABLE_COCOS2D_TYPE
            template<typename... Ty>
            struct unwraper_var<cocos2d::Vec2, Ty...> {
                static cocos2d::Vec2 unwraper(lua_State* L, int index) {
                    cocos2d::Vec2 ret;
                    if (lua_gettop(L) < index)
                        return static_cast<cocos2d::Vec2>(ret);

                    lua_getfield(L, index, "x");
                    ret.x = static_cast<float>(luaL_checknumber(L, -1));
                    lua_pop(L, 1);

                    lua_getfield(L, index, "y");
                    ret.y = static_cast<float>(luaL_checknumber(L, -1));
                    lua_pop(L, 1);

                    return ret;
                }
            };

            template<typename... Ty>
            struct unwraper_var<cocos2d::Vec3, Ty...> {
                static cocos2d::Vec3 unwraper(lua_State* L, int index) {
                    cocos2d::Vec3 ret;
                    lua_getfield(L, index, "x");
                    ret.x = static_cast<float>(luaL_checknumber(L, -1));
                    lua_pop(L, 1);

                    lua_getfield(L, index, "y");
                    ret.y = static_cast<float>(luaL_checknumber(L, -1));
                    lua_pop(L, 1);

                    lua_getfield(L, index, "z");
                    ret.z = static_cast<float>(luaL_checknumber(L, -1));
                    lua_pop(L, 1);

                    return ret;
                }
            };

            template<typename... Ty>
            struct unwraper_var<cocos2d::Vec4, Ty...> {
                static cocos2d::Vec4 unwraper(lua_State* L, int index) {
                    cocos2d::Vec4 ret;
                    lua_getfield(L, index, "x");
                    ret.x = static_cast<float>(luaL_checknumber(L, -1));
                    lua_pop(L, 1);

                    lua_getfield(L, index, "y");
                    ret.y = static_cast<float>(luaL_checknumber(L, -1));
                    lua_pop(L, 1);

                    lua_getfield(L, index, "z");
                    ret.z = static_cast<float>(luaL_checknumber(L, -1));
                    lua_pop(L, 1);

                    lua_getfield(L, index, "w");
                    ret.w = static_cast<float>(luaL_checknumber(L, -1));
                    lua_pop(L, 1);

                    return ret;
                }
            };

            template<typename... Ty>
            struct unwraper_var<cocos2d::Size, Ty...> {
                static cocos2d::Size unwraper(lua_State* L, int index) {
                    cocos2d::Size ret;
                    lua_getfield(L, index, "width");
                    ret.width = static_cast<float>(luaL_checknumber(L, -1));
                    lua_pop(L, 1);

                    lua_getfield(L, index, "height");
                    ret.height = static_cast<float>(luaL_checknumber(L, -1));
                    lua_pop(L, 1);

                    return ret;
                }
            };

            template<typename... Ty>
            struct unwraper_var<cocos2d::Rect, Ty...> {
                static cocos2d::Rect unwraper(lua_State* L, int index) {
                    cocos2d::Rect ret;
                    lua_getfield(L, index, "x");
                    ret.origin.x = static_cast<float>(luaL_checknumber(L, -1));
                    lua_pop(L, 1);

                    lua_getfield(L, index, "y");
                    ret.origin.y = static_cast<float>(luaL_checknumber(L, -1));
                    lua_pop(L, 1);

                    lua_getfield(L, index, "width");
                    ret.size.width = static_cast<float>(luaL_checknumber(L, -1));
                    lua_pop(L, 1);

                    lua_getfield(L, index, "height");
                    ret.size.height = static_cast<float>(luaL_checknumber(L, -1));
                    lua_pop(L, 1);

                    return ret;
                }
            };

#endif
            // ============== stl 扩展 =================
            template<typename TLeft, typename TRight, typename... Ty>
            struct unwraper_var<std::pair<TLeft, TRight>, Ty...> {
                static std::pair<TLeft, TRight> unwraper(lua_State* L, int index) {
                    std::pair<TLeft, TRight> ret;
                    ret.first = unwraper_var<TLeft>::unwraper(L, index);
                    ret.second = unwraper_var<TRight>::unwraper(L, index + 1);
                    return ret;
                }
            };

            template<typename Ty, typename... Tl>
            struct unwraper_var<std::vector<Ty>, Tl...> {
                static std::vector<Ty> unwraper(lua_State* L, int index) {
                    std::vector<Ty> ret;

                    LUA_GET_TABLE_LEN(size_t len, L, index);
                    ret.reserve(len);
                    for (size_t i = 1; i <= len; ++i) {
                        lua_pushinteger(L, static_cast<lua_Unsigned>(i));
                        lua_gettable(L, index);
                        ret.push_back(unwraper_var<Ty>::unwraper(L, -1));
                        lua_pop(L, 1);
                    }
                    return ret;
                }
            };

            template<typename Ty, typename... Tl>
            struct unwraper_var<std::list<Ty>, Tl...> {
                static std::list<Ty> unwraper(lua_State* L, int index) {
                    std::list<Ty> ret;
                    LUA_GET_TABLE_LEN(size_t len, L, index);
                    for (size_t i = 1; i <= len; ++i) {
                        lua_pushinteger(L, static_cast<lua_Unsigned>(i));
                        lua_gettable(L, index);
                        ret.push_back(unwraper_var<Ty>::unwraper(L, -1));
                        lua_pop(L, 1);
                    }
                    return ret;
                }
            };

            template<typename... Ty>
            struct unwraper_var<std::string, Ty...> {
                static std::string unwraper(lua_State* L, int index) {
                    std::string ret;
                    size_t len = 0;
                    const char* start = luaL_checklstring(L, index, &len);
                    ret.assign(start, len);
                    return ret;
                }
            };

            template<typename... Ty>
            struct unwraper_var<const std::string, Ty...> {
                static const std::string unwraper(lua_State* L, int index) {
                    std::string ret;
                    size_t len = 0;
                    const char* start = luaL_checklstring(L, index, &len);
                    ret.assign(start, len);
                    return ret;
                }
            };

            // --------------- stl 扩展 ----------------

            template<typename Ty, typename... Tl>
            struct unwraper_var : public std::conditional<
                std::is_enum<Ty>::value,
                unwraper_var_lua_type<Ty, lua_Unsigned>,
                unwraper_var_lua_type<Ty, Ty>
            >::type {};

            /*************************************\
            |* 以下type_traits用于枚举动态模板参数下标 *|
            \*************************************/

            template<int... _index>
            struct index_seq_list{ typedef index_seq_list<_index..., sizeof...(_index)> next_type; };

            template <typename... TP>
            struct build_args_index;

            template <>
            struct build_args_index<>
            {
                typedef index_seq_list<> index_seq_type;
            };

            template <typename TP1, typename... TP>
            struct build_args_index<TP1, TP...>
            {
                typedef typename build_args_index<TP...>::index_seq_type::next_type index_seq_type;
            };

            /*************************************\
            |* 以上type_traits用于枚举动态模板参数下标 *|
            \*************************************/
            template<typename Tr>
            struct unwraper_static_fn_base;

            template<>
            struct unwraper_static_fn_base<void> {
                template<typename Tfn, class TupleT, int... N >
                static int run_fn(lua_State* L, Tfn fn, index_seq_list<N...>) {
                    fn(
                        unwraper_var<typename std::tuple_element<N, TupleT>::type>::unwraper(
                            L, 
                            N + 1
                        )...
                    );
                    return 0;
                }
            };

            template<typename Tr>
            struct unwraper_static_fn_base {
                template<typename Tfn, class TupleT, int... N >
                static int run_fn(lua_State* L, Tfn fn, index_seq_list<N...>) {
                    return wraper_var<
                        typename std::remove_const<
                            typename std::remove_reference<Tr>::type
                        >::type
                    >::wraper(
                        L,
                        fn(
                            unwraper_var<typename std::tuple_element<N, TupleT>::type>::unwraper(
                                L,
                                N + 1
                            )...
                        )
                    );
                }
            };

            /*************************************\
            |* 静态函数Lua绑定，动态参数个数          *|
            \*************************************/
            template<typename Tr, typename... TParam>
            struct unwraper_static_fn : public unwraper_static_fn_base<Tr> {
                typedef unwraper_static_fn_base<Tr> base_type;
                typedef Tr (*value_type)(TParam...);

                 
                // 动态参数个数
                static int LuaCFunction(lua_State* L) {
                    value_type fn = reinterpret_cast<value_type>(lua_touserdata(L, lua_upvalueindex(1)));
                    if (NULL == fn) {
                        // 找不到函数
                        return 0;
                    }

                    return base_type::template run_fn<value_type, std::tuple<TParam...> >(L,
                        fn,
                        typename build_args_index<TParam...>::index_seq_type()
                    );
                }
            };


            /*************************************\
            |* 成员函数Lua绑定，动态参数个数          *|
            \*************************************/
            template<typename Tr, typename TClass, typename... TParam>
            struct unwraper_member_fn : public unwraper_static_fn_base<Tr> {
                typedef unwraper_static_fn_base<Tr> base_type;
                
                // 动态参数个数 - 成员函数
                static int LuaCFunction(lua_State* L, TClass* obj, Tr(TClass::*fn)(TParam...)) {
                    auto fn_wraper = [obj, fn](TParam&&... args){
                        return (obj->*fn)(std::forward<TParam>(args)...);
                    };

                    return base_type::template run_fn<decltype(fn_wraper), std::tuple<TParam...> >(
                        L,
                        fn_wraper,
                        typename build_args_index<TParam...>::index_seq_type()
                    );
                }

                // 动态参数个数 - 常量成员函数
                static int LuaCFunction(lua_State* L, TClass* obj, Tr(TClass::*fn)(TParam...) const) {
                    auto fn_wraper = [obj, fn](TParam&&... args){
                        return (obj->*fn)(std::forward<TParam>(args)...);
                    };

                    return base_type::template run_fn<decltype(fn_wraper), std::tuple<TParam...> >(
                        L,
                        fn_wraper,
                        typename build_args_index<TParam...>::index_seq_type()
                    );
                }
            };
        }
    }
}
