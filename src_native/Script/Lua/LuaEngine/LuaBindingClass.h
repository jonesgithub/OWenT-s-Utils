#pragma once

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
#include <Script/Lua/LuaEngine/LuaEngine.h>

#include "cocos2d.h"

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
            typedef int(*member_method)(lua_State*, value_type*);
            typedef typename LuaBindingUserdataInfo<value_type>::userdata_type userdata_type;
            typedef typename LuaBindingUserdataInfo<value_type>::userdata_ptr_type userdata_ptr_type;
            typedef std::function<int(lua_State* L, value_type* obj)> member_proxy_method_t;


            enum FUNC_TYPE {
                FT_NEW = 0,
                FT_GC,
                FT_TOSTRING,
                FT_MAX,
            };
        private:
            lua_CFunction default_funcs_[FT_MAX];

        public:
            LuaBindingClass(const char* lua_name, const char* namespace_) : lua_class_name_(lua_name), owner_ns_(namespace_){
                register_class();
                memset(default_funcs_, NULL, sizeof(default_funcs_));
            }

            LuaBindingClass(const char* lua_name, LuaBindingNamespace& ns) : lua_class_name_(lua_name), owner_ns_(ns){
                register_class();
                memset(default_funcs_, NULL, sizeof(default_funcs_));
            }

            ~LuaBindingClass() {
                finish_class();

                // 使用内建new函数时必须使用内建回收函数
                assert(__new != default_funcs_[FT_NEW] || (
                    __lua_gc == default_funcs_[FT_GC] || __lua_null_gc == default_funcs_[FT_GC]
                ));
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
                return lua::LuaEngine::Instance()->getLuaState();
            }

            /**
             * 添加静态方法
             *
             * @return  self.
             */
            self_type& addStaticMethod(const char* method_name, static_method fn) {
                lua_State* state = getLuaState();
                lua_pushstring(state, method_name);
                lua_pushlightuserdata(state, reinterpret_cast<void*>(fn));
                lua_pushcclosure(state, __static_method_wrapper, 1);
                lua_settable(state, getStaticClassTable());

                return *this;
            }

            /**
             * 添加成员方法
             *
             * @return  self.
             */
            self_type& addMemberMethod(const char* method_name, member_method fn) {
                lua_State* state = getLuaState();
                lua_pushstring(state, method_name);
                lua_pushlightuserdata(state, reinterpret_cast<void*>(fn));
                lua_pushcclosure(state, __member_method_wrapper, 1);
                lua_settable(state, getMemberTable());

                return *this;
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
             * 给类添加方法，自动推断类型（暂时没空实现，先用简单暴力的方法）
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
                *fn_ptr = [fn](lua_State* L, TClass* obj){
                    return detail::unwraper_member_fn<R, TClass, TParam...>::LuaCFunction(L, obj, fn);
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
                *fn_ptr = [fn](lua_State* L, TClass* obj){
                    return detail::unwraper_member_fn<R, TClass, TParam...>::LuaCFunction(L, obj, fn);
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

            self_type& setNew(lua_CFunction f) {
                lua_State* state = getLuaState();
                // new 方法
                lua_pushliteral(state, "new");
                lua_pushcfunction(state, f);
                lua_settable(state, class_table_);

                default_funcs_[FT_NEW] = f;

                return (*this);
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

                LuaBindingMgr::Instance()->addUserdataType(getLuaMetaTableName());
            }

            void finish_class() {
                if (NULL == default_funcs_[FT_NEW])
                    setNew(__new);

                if (NULL == default_funcs_[FT_TOSTRING])
                    setToString(__tostring);

                if (NULL == default_funcs_[FT_GC])
                    setGC(__lua_gc);
            }

        //================ 以下方法皆为lua接口，并提供给C++层使用 ================
        public:

            /**
             * 创建新实例，并把相关的lua对象入栈
             *
             * @param [in,out]  L   If non-null, the lua_State * to process.
             *
             * @return  null if it fails, else a value_type*.
             */

            static value_type* create(lua_State *L) {
                proxy_type* obj = new proxy_type();
                userdata_ptr_type pobj = static_cast<userdata_ptr_type>(lua_newuserdata(L, sizeof(userdata_type)));
                *pobj = static_cast<value_type*>(obj);

                const char* class_name = getLuaMetaTableName();
                luaL_getmetatable(L, class_name);
                lua_setmetatable(L, -2);

                LuaBindingMgr::Instance()->addRef(pobj);
                return *pobj;
            }


            static int __lua_null_gc(lua_State *L) {
                if (0 == lua_gettop(L)) {
                    cocos2d::log("[ERROR]: userdata __gc is called without self");
                    lua::LuaEngine::Instance()->printTrackBack(L);
                    return 0;
                }

                // metatable表触发
                if (0 == lua_isuserdata(L, 1)) {
                    lua_remove(L, 1);
                    return 0;
                }

                // 查找并移除引用计数
                userdata_ptr_type pobj = static_cast<userdata_ptr_type>(lua_touserdata(L, 1));
                proxy_type* real_ptr = static_cast<proxy_type*>(*pobj);
                LuaBindingMgr::Instance()->removeRef(real_ptr);

                return 0;
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
                value_type** pobj = static_cast<value_type**>(lua_touserdata(L, 1));

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
                ss<< "] @"<< *pobj;

                std::string str = ss.str();
                lua_pushlstring(L, str.c_str(), str.size());
                return 1;
            }

            /**
             * __new 方法.
             *
             * @author  
             * @date    2014/10/25
             *
             * @param [in,out]  L   If non-null, the lua_State * to process.
             *
             * @return  An int.
             */

            static int __new(lua_State *L) {
                // remove self
                if (lua_gettop(L) > 0)
                    lua_remove(L, 1);

                create(L);
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
                    cocos2d::log("[ERROR]: userdata __gc is called without self");
                    lua::LuaEngine::Instance()->printTrackBack(L);
                    return 0;
                }

                // metatable表触发
                if (0 == lua_isuserdata(L, 1)) {
                    lua_remove(L, 1);
                    return 0;
                }

                userdata_ptr_type pobj = static_cast<userdata_ptr_type>(lua_touserdata(L, 1));
                proxy_type* real_ptr = static_cast<proxy_type*>(*pobj);
                uint32_t ref_count = LuaBindingMgr::Instance()->removeRef(real_ptr);

                if (0 == ref_count) {
                    delete real_ptr;
                }

                return 0;
            }


            /**
             * __call 方法，不存在的方法要输出错误
             */
            //static int __call(lua_State *L) {
            //	cocos2d::log("lua try to call invalid member method [%s].%s(%d parameters)\n",
            //        getLuaMetaTableName(),
            //        luaL_checklstring(L, 1, NULL),
            //        lua_gettop(L) - 1
            //    );
            //    return 0;
            //}


            static int __static_method_wrapper(lua_State *L) {
                static_method fn = reinterpret_cast<static_method>(lua_touserdata(L, lua_upvalueindex(1)));
                if (NULL == fn) {
                    cocos2d::log("lua try to call static method in class %s but fn not set.\n", getLuaMetaTableName());
                    return 0;
                }

                return fn(L);
            }

            static int __member_method_wrapper(lua_State *L) {
                member_method fn = reinterpret_cast<member_method>(lua_touserdata(L, lua_upvalueindex(1)));
                if (NULL == fn) {
                    cocos2d::log("lua try to call member method in class %s but fn not set.\n", getLuaMetaTableName());
                    return 0;
                }

                const char* class_name = getLuaMetaTableName();
                userdata_ptr_type obj = static_cast<userdata_ptr_type>(luaL_checkudata(L, 1, class_name));  // get 'self'
                lua_remove(L, 1);

                if (NULL == obj) {
                	cocos2d::log("lua try to call %s's member method but self not set.\n", class_name);
                    return 0;
                }

                return fn(L, *obj);
            }

            static int __member_method_unwrapper(lua_State *L) {
                member_proxy_method_t* fn = reinterpret_cast<member_proxy_method_t*>(lua_touserdata(L, lua_upvalueindex(1)));
                if (NULL == fn) {
                    cocos2d::log("lua try to call member method in class %s but fn not set.\n", getLuaMetaTableName());
                    return 0;
                }

                const char* class_name = getLuaMetaTableName();
                userdata_ptr_type obj = static_cast<userdata_ptr_type>(luaL_checkudata(L, 1, class_name));  // get 'self'
                lua_remove(L, 1);

                if (NULL == obj) {
                    cocos2d::log("lua try to call %s's member method but self not set.\n", class_name);
                    return 0;
                }

                return (*fn)(L, *obj);
            }

        private:
            std::string lua_class_name_;
            LuaBindingNamespace owner_ns_;
            LuaBindingNamespace as_ns_;

            int class_table_ = 0;       /**< 公共类型的Lua Table*/
            int class_memtable_ = 0;    /**< The class table*/
            int class_metatable_ = 0;    /**< The class table*/
        };

    }
}
