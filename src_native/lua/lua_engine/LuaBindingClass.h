﻿#pragma once

#include <assert.h>
#include <string>
#include <sstream>
#include <cstdio>
#include <list>
#include <functional>
#include <type_traits>
#include <typeinfo>

#include <std/explicit_declare.h>

#include "LuaBindingNamespace.h"
#include "LuaBindingUnwrapper.h"
#include "LuaBindingWrapper.h"
#include "LuaBindingUtils.h"
#include "LuaBindingMgr.h"

namespace script {
    namespace lua {

        /**
         * lua 类，注意只能用于局部变量
         *
         * @author  owent
         * @date    2014/10/25
         */
        template<typename TC, typename TProxy = TC>
        class LuaBindingClass {
        public:
            typedef TC value_type;
            typedef TProxy proxy_type;
            typedef LuaBindingClass<value_type, proxy_type> self_type;
            typedef LuaBindingNamespace::static_method static_method;
            typedef typename LuaBindingUserdataInfo<value_type>::userdata_type userdata_type;
            typedef typename LuaBindingUserdataInfo<proxy_type>::pointer_type pointer_type;
            typedef typename LuaBindingUserdataInfo<proxy_type>::userdata_ptr_type userdata_ptr_type;
            typedef std::function<int(lua_State*, pointer_type)> member_proxy_method_t;


            enum FUNC_TYPE {
                FT_NEW = 0,
                FT_GC,
                FT_TOSTRING,
                FT_MAX,
            };
        private:
            lua_CFunction default_funcs_[FT_MAX];

        public:
            LuaBindingClass(const char* lua_name, const char* namespace_, lua_State* L) : lua_state_(L), lua_class_name_(lua_name), owner_ns_(namespace_, L){
                register_class();
                memset(default_funcs_, NULL, sizeof(default_funcs_));
            }

            LuaBindingClass(const char* lua_name, LuaBindingNamespace& ns) : lua_state_(ns.getLuaState()), lua_class_name_(lua_name), owner_ns_(ns){
                register_class();
                memset(default_funcs_, NULL, sizeof(default_funcs_));
            }

            ~LuaBindingClass() {
                finish_class();
            }

            /**
             * Gets the owner namespace.
             *
             * @return  The owner namespace.
             */

            LuaBindingNamespace& getOwnerNamespace() {
                return owner_ns_;
            }

            const LuaBindingNamespace& getOwnerNamespace() const {
                return owner_ns_;
            }

            /**
             * 获取静态class的table
             *
             * @return  The static class table.
             */
            int getStaticClassTable() const { return class_table_; }

            /**
             * 获取类成员的table
             *
             * @return  The static class table.
             */
            int getMemberTable() const { return class_memtable_; }

            /**
             * 获取UserData的映射关系MetaTable
             *
             * @return  The static class table.
             */
            int getUserMetaTable() const { return class_metatable_; }

            lua_State* getLuaState() {
                return lua_state_;
            }

            /**
             * 添加常量(自动类型推断)
             *
             * @return  self.
             */
            template<typename Ty>
            self_type& addConst(const char* const_name, Ty n) {
                lua_State* state = getLuaState();
                int ret_num = detail::wraper_var<Ty>::wraper(state, n);
                if (ret_num > 0)
                    lua_setfield(state, getStaticClassTable(), const_name);

                return *this;
            }

            /**
             * 添加常量(字符串)
             *
             * @return  self.
             */
            self_type& addConst(const char* const_name, const char* n, size_t s) {
                lua_State* state = getLuaState();
                lua_pushstring(state, const_name);
                lua_pushlstring(state, n, s);
                lua_settable(state, getStaticClassTable());

                return *this;
            }


            /**
             * 给类添加方法，自动推断类型
             *
             * @tparam  TF  Type of the tf.
             * @param   func_name   Name of the function.
             * @param   fn          The function.
             */
            template<typename R, typename... TParam>
            self_type& addMethod(const char* func_name, R(*fn)(TParam... param)) {
                lua_State* state = getLuaState();
                lua_pushstring(state, func_name);
                lua_pushlightuserdata(state, reinterpret_cast<void*>(fn));
                lua_pushcclosure(state, detail::unwraper_static_fn<R, TParam...>::LuaCFunction, 1);
                lua_settable(state, getStaticClassTable());

                return (*this);
            }

            /**
             * 给类添加仿函数，自动推断类型
             *
             * @tparam  R           Type of the r.
             * @tparam  TParam      Type of the parameter.
             * @param   func_name   Name of the function.
             * @param   fn          functor
             *
             * @return  A self_type&amp;
             */
            template<typename R, typename... TParam>
            self_type& addMethod(const char* func_name, std::function<R(TParam...)> fn) {
                typedef std::function<R(TParam...)> fn_t;

                lua_State* state = getLuaState();
                lua_pushstring(state, func_name);

                LuaBindingPlacementNewAndDelete<fn_t>::create(state, fn);
                lua_pushcclosure(state, detail::unwraper_functor_fn<R, TParam...>::LuaCFunction, 1);
                lua_settable(state, getStaticClassTable());

                return (*this);
            }

            /**
             * 添加成员方法
             *
             * @tparam  R       Type of the r.
             * @tparam  TParam  Type of the parameter.
             * @param   func_name                   Name of the function.
             * @param [in,out]  fn)(TParam...param) If non-null, the fn)( t param...param)
             *
             * @return  A self_type&amp;
             */
            template<typename R, typename TClass,typename... TParam>
            self_type& addMethod(const char* func_name, R(TClass::*fn)(TParam... param)) {
                typedef R(TClass::*fn_t)(TParam...);
                static_assert(std::is_convertible<value_type*, TClass*>::value, "class of member method invalid");

                lua_State* state = getLuaState();
                lua_pushstring(state, func_name);

                member_proxy_method_t* fn_ptr = LuaBindingPlacementNewAndDelete<member_proxy_method_t>::create(state);
                *fn_ptr = [fn](lua_State* L, pointer_type pobj){
                    return detail::unwraper_member_fn<R, TClass, TParam...>::LuaCFunction(
                        L, 
                        dynamic_cast<TClass*>(pobj.get()),
                        fn
                    );
                };
                lua_pushcclosure(state, __member_method_unwrapper, 1);
                lua_settable(state, getMemberTable());

                return (*this);
            }

            /**
             * 添加常量成员方法
             *
             * @tparam  R       Type of the r.
             * @tparam  TParam  Type of the parameter.
             * @param   func_name       Name of the function.
             * @param [in,out]  const   If non-null, the constant.
             *
             * @return  A self_type&amp;
             */
            template<typename R, typename TClass, typename... TParam>
            self_type& addMethod(const char* func_name, R(TClass::*fn)(TParam... param) const) {
                typedef R(TClass::*fn_t)(TParam...);
                static_assert(std::is_convertible<value_type*, TClass*>::value, "class of member method invalid");

                lua_State* state = getLuaState();
                lua_pushstring(state, func_name);

                member_proxy_method_t* fn_ptr = LuaBindingPlacementNewAndDelete<member_proxy_method_t>::create(state);
                *fn_ptr = [fn](lua_State* L, pointer_type pobj){
                    return detail::unwraper_member_fn<R, TClass, TParam...>::LuaCFunction(
                        L, 
                        dynamic_cast<TClass*>(pobj.get()),
                        fn
                    );
                };
                lua_pushcclosure(state, __member_method_unwrapper, 1);
                lua_settable(state, getMemberTable());

                return (*this);
            }

            /**
             * 转换为namespace，注意有效作用域是返回的LuaBindingNamespace和这个Class的子集
             *
             * @return  The static class table.
             */
            LuaBindingNamespace& asNamespace() {
                // 第一次获取时初始化
                if (as_ns_.ns_.empty()) {
                    as_ns_.ns_ = owner_ns_.ns_;
                    as_ns_.ns_.push_back(getLuaName());
                    as_ns_.base_stack_top_ = 0;
                    as_ns_.this_ns_ = class_table_;
                }
                
                return as_ns_;
            }

            const char* getLuaName() const { return lua_class_name_.c_str(); }

            static const char* getLuaMetaTableName() { return LuaBindingUserdataInfo<value_type>::getLuaMetaTableName(); }

            self_type& setNew(lua_CFunction f, const std::string& method_name = "new") {
                lua_State* state = getLuaState();
                // new 方法
                lua_pushstring(state, method_name.c_str());
                lua_pushcfunction(state, f);
                lua_settable(state, class_table_);

                default_funcs_[FT_NEW] = f;

                return (*this);
            }

            template<typename... TParams>
            self_type& setDefaultNew(const std::string& method_name = "new") {
                typedef std::function<pointer_type(TParams&&...)> new_fn_t;
                lua_State* L = getLuaState();

                new_fn_t fn = [L](TParams&&... params) {
                    return create<TParams&&...>(L, params...);
                };

                return addMethod<pointer_type, TParams&&...>(method_name.c_str(), fn);
            }

            self_type& setToString(lua_CFunction f) {
                lua_State* state = getLuaState();
                // __tostring方法
                lua_pushliteral(state, "__tostring");
                lua_pushcfunction(state, f);
                lua_settable(state, class_metatable_);

                default_funcs_[FT_TOSTRING] = f;

                return (*this);
            }

            self_type& setGC(lua_CFunction f) {
                lua_State* state = getLuaState();
                // 垃圾回收方法（注意函数内要判断排除table类型）
                lua_pushliteral(state, "__gc");
                lua_pushcfunction(state, f);
                lua_settable(state, class_metatable_);

                default_funcs_[FT_GC] = f;

                return (*this);
            }

        private:

            /**
             * Registers the class.
             *
             * @author  
             * @date    2014/10/25
             */
            void register_class() {
                // 初始化后就不再允许新的类注册了
                assert(false == LuaBindingMgr::Instance()->isInited());

                LuaBindingClassMgrInst<proxy_type>::Instance();

                lua_State* state = getLuaState();
                // 注册C++类
                {
                    lua_newtable(state);
                    class_table_ = lua_gettop(state);

                    lua_newtable(state);
                    class_memtable_ = lua_gettop(state);

                    luaL_newmetatable(state, getLuaMetaTableName());
                    class_metatable_ = lua_gettop(state);


                    // 注册类到namespace
                    // 注意__index留空
                    lua_pushstring(state, getLuaName());
                    lua_pushvalue(state, class_table_);
                    lua_settable(state, owner_ns_.getNamespaceTable());

                    /**
                    * table 初始化(静态成员)
                    */
                    lua_pushvalue(state, class_table_);
                    lua_setmetatable(state, class_table_);

                    // table 的__type默认设为native class(这里仅仅是为了和class.lua交互，如果不设置的话默认就是native code)
                    lua_pushliteral(state, "__type");
                    lua_pushliteral(state, "native class");
                    lua_settable(state, class_table_);

                    /**
                    * memtable 初始化(成员函数及变量)
                    */
                    lua_pushvalue(state, class_memtable_);
                    lua_setmetatable(state, class_memtable_);

                    lua_pushliteral(state, "__index");
                    lua_pushvalue(state, class_table_);
                    lua_settable(state, class_memtable_);


                    // memtable 的__type默认设为native object(这里仅仅是为了和class.lua交互，如果不设置的话默认就是native code)
                    lua_pushliteral(state, "__type");
                    lua_pushliteral(state, "native object");
                    lua_settable(state, class_memtable_);

                    /**
                    * metatable 初始化(userdata映射表)
                    */
                    lua_pushvalue(state, class_metatable_);
                    lua_setmetatable(state, class_metatable_);
                    // 继承关系链 userdata -> metatable(实例接口表): memtable(成员接口表): table(静态接口表) : ...
                    lua_pushliteral(state, "__index");
                    lua_pushvalue(state, class_memtable_);
                    lua_settable(state, class_metatable_);
                }
            }

            void finish_class() {
                if (nullptr == default_funcs_[FT_TOSTRING])
                    setToString(__tostring);

                if (nullptr == default_funcs_[FT_GC])
                    setGC(__lua_gc);
            }

        //================ 以下方法皆为lua接口，并提供给C++层使用 ================
        public:

            /**
             * 创建新实例，并把相关的lua对象入栈
             *
             * @param [in,out]  L   If non-null, the lua_State * to process.
             *
             * @return  null if it fails, else a pointer_type.
             */
            template<typename... TParams>
            static pointer_type create(lua_State *L, TParams&&... params) {
                pointer_type obj = pointer_type(new proxy_type(std::forward<TParams>(params)...));

                // 添加到缓存表，防止被立即析构
                LuaBindingClassMgrInst<proxy_type>::Instance()->addRef(L, obj);
                return obj;
            }

        private:
            /**
             * __tostring 方法
             *
             * @author  
             * @date    2014/10/25
             *
             * @param [in,out]  L   If non-null, the lua_State * to process.
             *
             * @return  An int.
             */

            static int __tostring(lua_State *L) {
                if (lua_gettop(L) <= 0) {
                    lua_pushliteral(L, "[native code]");
                    return 1;
                }

                std::stringstream ss;
                userdata_ptr_type pobj = static_cast<userdata_ptr_type>(lua_touserdata(L, 1));
                pointer_type real_ptr = pobj->lock();

                lua_pushliteral(L, "__type");
                lua_gettable(L, -2);

                lua_pushliteral(L, "__classname");
                lua_gettable(L, -3);

                ss << "[";;
                if (lua_isstring(L, -2))
                    ss<< lua_tostring(L, -2);
                else
                    ss<< "native code";
                ss<< " : ";

                if (lua_isstring(L, -1))
                    ss<< lua_tostring(L, -1);
                else
                    ss<< " unknown type";
                ss << "] @" << real_ptr.get();

                std::string str = ss.str();
                lua_pushlstring(L, str.c_str(), str.size());
                return 1;
            }

            /**
             * 垃圾回收方法
             *
             * @author  
             * @date    2014/10/25
             *
             * @param [in,out]  L   If non-null, the lua_State * to process.
             *
             * @return  An int.
             */

            static int __lua_gc(lua_State *L) {
                if (0 == lua_gettop(L)) {
                    fn::print_traceback(L, "userdata __gc is called without self");
                    return 0;
                }

                // metatable表触发
                if (0 == lua_isuserdata(L, 1)) {
                    lua_remove(L, 1);
                    return 0;
                }

                // 析构
                userdata_ptr_type pobj = static_cast<userdata_ptr_type>(lua_touserdata(L, 1));
                pobj->~userdata_type();

                return 0;
            }


            /**
             * __call 方法，不存在的方法要输出错误
             */
            //static int __call(lua_State *L) {
            //	WLOGERROR("lua try to call invalid member method [%s].%s(%d parameters)\n",
            //        getLuaMetaTableName(),
            //        luaL_checklstring(L, 1, NULL),
            //        lua_gettop(L) - 1
            //    );
            //    return 0;
            //}

            static int __member_method_unwrapper(lua_State *L) {
                member_proxy_method_t* fn = reinterpret_cast<member_proxy_method_t*>(lua_touserdata(L, lua_upvalueindex(1)));
                if (nullptr == fn) {
                    WLOGERROR("lua try to call member method in class %s but fn not set.\n", getLuaMetaTableName());
                    fn::print_traceback(L, "");
                    return 0;
                }

                const char* class_name = getLuaMetaTableName();
                userdata_ptr_type pobj = static_cast<userdata_ptr_type>(luaL_checkudata(L, 1, class_name));  // get 'self'

                if (nullptr == pobj) {
                    WLOGERROR("lua try to call %s's member method but self not set or type error.\n", class_name);
                    fn::print_traceback(L, "");
                    return 0;
                }

                pointer_type obj_ptr = pobj->lock();
                lua_remove(L, 1);

                if (!obj_ptr) {
                    WLOGERROR("lua try to call %s's member method but this=nullptr.\n", class_name);
                    fn::print_traceback(L, "");
                    return 0;
                }

                return (*fn)(L, obj_ptr);
            }

        private:
            lua_State* lua_state_;

            std::string lua_class_name_;
            LuaBindingNamespace owner_ns_;
            LuaBindingNamespace as_ns_;

            int class_table_ = 0;       /**< 公共类型的Lua Table*/
            int class_memtable_ = 0;    /**< The class table*/
            int class_metatable_ = 0;    /**< The class table*/
        };

    }
}
