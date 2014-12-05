#pragma once

#include <string>
#include <sstream>
#include <cstdio>
#include <list>
#include <vector>
#include <tuple>
#include <typeinfo>

#include <std/explicit_declare.h>

#ifdef LUA_BINDING_ENABLE_COCOS2D_TYPE
#include "cocos2d.h"
#endif

#include "LuaBindingMgr.h"
#include "LuaBindingUtils.h"

namespace script {
    namespace lua {
        namespace detail {

            template<typename Ty>
            struct wraper_var_lua_type;

            template<>
            struct wraper_var_lua_type<lua_Unsigned> {
                typedef lua_Unsigned value_type;

                template<typename TParam>
                static int wraper(lua_State* L, const TParam& v) {
                    lua_pushinteger(L, static_cast<lua_Unsigned>(v));
                    return 1;
                }
            };

            template<>
            struct wraper_var_lua_type<lua_CFunction> {
                typedef lua_CFunction value_type;

                template<typename TParam>
                static int wraper(lua_State* L, TParam v) {
                    lua_pushcfunction(L, static_cast<lua_CFunction>(v));
                    return 1;
                }
            };

            template<>
            struct wraper_var_lua_type<lua_Integer> {
                typedef lua_Integer value_type;

                template<typename TParam>
                static int wraper(lua_State* L, const TParam& v) {
                    lua_pushinteger(L, static_cast<lua_Integer>(v));
                    return 1;
                }
            };

            template<>
            struct wraper_var_lua_type<lua_Number> {
                typedef lua_Number value_type;

                template<typename TParam>
                static int wraper(lua_State* L, const TParam& v) {
                    lua_pushnumber(L, static_cast<lua_Number>(v));
                    return 1;
                }
            };

            template<typename Ty>
            struct wraper_var_lua_type {
                typedef Ty value_type;

                template<typename TParam>
                static int wraper(lua_State* L, TParam&& v) {
                    typedef typename std::remove_reference<
                        typename std::remove_const<
                            typename std::remove_pointer<Ty>::type
                        >::type
                    >::type obj_t;

                    const char* clazz_name = LuaBindingUserdataInfo<obj_t>::getLuaMetaTableName();
                    if (!LuaBindingMgr::Instance()->isUserdataRegistered(clazz_name)) {
                        lua_pushlightuserdata(L, reinterpret_cast<void*>(v));
                        return 1;
                    }
                    
                    typedef typename LuaBindingUserdataInfo<obj_t>::userdata_type ud_t;
                    ud_t* ptr = reinterpret_cast<ud_t*>(lua_newuserdata(L, sizeof(ud_t)));

                    luaL_getmetatable(L, clazz_name);
                    lua_setmetatable(L, -2);

                    *ptr = v;

                    // 添加引用计数
                    LuaBindingMgr::Instance()->addRef(*ptr);

                    return 1;
                }
            };


            template<typename TRet, typename... Ty>
            struct wraper_var;

            template<typename... Ty>
            struct wraper_var<void, Ty...> {
                static int wraper(lua_State* L, const void* v) {
                    return 0;
                }
            };

            template<typename... Ty> struct wraper_var<uint8_t, Ty...> : public wraper_var_lua_type<lua_Unsigned>{};
            template<typename... Ty> struct wraper_var<uint16_t, Ty...> : public wraper_var_lua_type<lua_Unsigned>{};
            template<typename... Ty> struct wraper_var<uint32_t, Ty...> : public wraper_var_lua_type<lua_Unsigned>{};
            template<typename... Ty> struct wraper_var<uint64_t, Ty...> : public wraper_var_lua_type<lua_Unsigned>{};

            template<typename... Ty> struct wraper_var<int8_t, Ty...> : public wraper_var_lua_type<lua_Integer>{};
            template<typename... Ty> struct wraper_var<int16_t, Ty...> : public wraper_var_lua_type<lua_Integer>{};
            template<typename... Ty> struct wraper_var<int32_t, Ty...> : public wraper_var_lua_type<lua_Integer>{};
            template<typename... Ty> struct wraper_var<int64_t, Ty...> : public wraper_var_lua_type<lua_Integer>{};

            template<typename... Ty> struct wraper_var<float, Ty...> : public wraper_var_lua_type<lua_Number>{};
            template<typename... Ty> struct wraper_var<double, Ty...> : public wraper_var_lua_type<lua_Number>{};

            template<typename... Ty> struct wraper_var<lua_CFunction, Ty...> : public wraper_var_lua_type<lua_CFunction>{};

            template<typename... Ty>
            struct wraper_var<bool, Ty...> {
                static int wraper(lua_State* L, const bool& v) {
                    lua_pushboolean(L, v ? 1 : 0);
                    return 1;
                }
            };

            template<typename... Ty>
            struct wraper_var<char*, Ty...> {
                static int wraper(lua_State* L, const char* v) {
                    lua_pushstring(L, v);
                    return 1;
                }
            };

            template<typename... Ty>
            struct wraper_var<const char*, Ty...> {
                static int wraper(lua_State* L, const char* v) {
                    lua_pushstring(L, v);
                    return 1;
                }
            };

            template<typename... Ty>
            struct wraper_var<std::string, Ty...> {
                static int wraper(lua_State* L, const std::string& v) {
                    lua_pushlstring(L, v.c_str(), v.size());
                    return 1;
                }
            };

#ifdef LUA_BINDING_ENABLE_COCOS2D_TYPE
            template<typename... Ty>
            struct wraper_var<cocos2d::Vec2, Ty...> {
                static int wraper(lua_State* L, const cocos2d::Vec2& v) {
                    lua_newtable(L);

                    lua_pushnumber(L, static_cast<float>(v.x));
                    lua_setfield(L, -2, "x");

                    lua_pushnumber(L, static_cast<float>(v.y));
                    lua_setfield(L, -2, "y");
                    return 1;
                }
            };

            template<typename... Ty>
            struct wraper_var<cocos2d::Vec3, Ty...> {
                static int wraper(lua_State* L, const cocos2d::Vec3& v) {
                    lua_newtable(L);

                    lua_pushnumber(L, static_cast<float>(v.x));
                    lua_setfield(L, -2, "x");

                    lua_pushnumber(L, static_cast<float>(v.y));
                    lua_setfield(L, -2, "y");

                    lua_pushnumber(L, static_cast<float>(v.z));
                    lua_setfield(L, -2, "z");
                    return 1;
                }
            };

            template<typename... Ty>
            struct wraper_var<cocos2d::Vec4, Ty...> {
                static int wraper(lua_State* L, const cocos2d::Vec4& v) {
                    lua_newtable(L);

                    lua_pushnumber(L, static_cast<float>(v.x));
                    lua_setfield(L, -2, "x");

                    lua_pushnumber(L, static_cast<float>(v.y));
                    lua_setfield(L, -2, "y");

                    lua_pushnumber(L, static_cast<float>(v.z));
                    lua_setfield(L, -2, "z");

                    lua_pushnumber(L, static_cast<float>(v.w));
                    lua_setfield(L, -2, "w");
                    return 1;
                }
            };

            template<typename... Ty>
            struct wraper_var<cocos2d::Size, Ty...> {
                static int wraper(lua_State* L, const cocos2d::Size& v) {
                    lua_newtable(L);

                    lua_pushnumber(L, static_cast<float>(v.width));
                    lua_setfield(L, -2, "width");

                    lua_pushnumber(L, static_cast<float>(v.height));
                    lua_setfield(L, -2, "height");
                    return 1;
                }
            };

            template<typename... Ty>
            struct wraper_var<cocos2d::Rect, Ty...> {
                static int wraper(lua_State* L, const cocos2d::Rect& v) {
                    lua_newtable(L);

                    lua_pushnumber(L, static_cast<float>(v.origin.x));
                    lua_setfield(L, -2, "x");

                    lua_pushnumber(L, static_cast<float>(v.origin.y));
                    lua_setfield(L, -2, "y");

                    lua_pushnumber(L, static_cast<float>(v.size.width));
                    lua_setfield(L, -2, "width");

                    lua_pushnumber(L, static_cast<float>(v.size.height));
                    lua_setfield(L, -2, "height");
                    return 1;
                }
            };

#endif
            // ============== stl 扩展 =================
            template<typename TLeft, typename TRight, typename... Ty>
            struct wraper_var<std::pair<TLeft, TRight>, Ty...> {
                static int wraper(lua_State* L, const std::pair<TLeft, TRight>& v) {
                    return wraper_var<TLeft>::wraper(L, v.first) + wraper_var<TRight>::wraper(L, v.second);
                }
            };
            
            template<typename Ty, typename... Tl>
            struct wraper_var<std::vector<Ty>, Tl...> {
                static int wraper(lua_State* L, const std::vector<Ty>& v) {
                    lua_Unsigned res = 1;
                    lua_newtable(L);
                    int tb = lua_gettop(L);
                    for (const Ty& ele : v) {
                        // 目前只支持一个值
                        lua_pushunsigned(L, res);
                        wraper_var<Ty>::wraper(L, ele);
                        lua_settable(L, -3);

                        ++res;
                    }
                    lua_settop(L, tb);
                    return 1;
                }
            };

            template<typename Ty, typename... Tl>
            struct wraper_var<std::list<Ty>, Tl...> {
                static int wraper(lua_State* L, const std::list<Ty>& v) {
                    lua_Unsigned res = 1;
                    lua_newtable(L);
                    int tb = lua_gettop(L);
                    for (const Ty& ele : v) {
                        // 目前只支持一个值
                        lua_pushunsigned(L, res);
                        wraper_var<Ty>::wraper(L, ele);
                        lua_settable(L, -3);

                        ++res;
                    }
                    lua_settop(L, tb);
                    return 1;
                }
            };
            // --------------- stl 扩展 ----------------
            
            template<typename Ty, typename... Tl>
            struct wraper_var : public std::conditional<
                std::is_enum<Ty>::value,
                wraper_var_lua_type<lua_Unsigned>,
                wraper_var_lua_type<Ty>
            >::type {};

            //template<>
            struct static_method_wrapper {

            };
        }
    }
}
