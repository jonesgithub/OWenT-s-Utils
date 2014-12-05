#include <cstdlib>
#include <sstream>

#include "LuaBindingNamespace.h"

#include "LuaEngine.h"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "cocos2d.h"

namespace script
{
    namespace lua
    {
        LuaBindingNamespace::LuaBindingNamespace() : base_stack_top_(0), this_ns_(0){
        }

        LuaBindingNamespace::LuaBindingNamespace(const char* namespace_) : base_stack_top_(0), this_ns_(0) {
            open(namespace_);
        }

        LuaBindingNamespace::LuaBindingNamespace(const char* namespace_, LuaBindingNamespace& ns) {
            this_ns_ = ns.this_ns_;
            base_stack_top_ = 0;
            build_ns_set(namespace_);
            find_ns();
            ns_.insert(ns_.begin(), ns.ns_.begin(), ns.ns_.end());
        }

        LuaBindingNamespace::~LuaBindingNamespace() {
            close();
        }

        LuaBindingNamespace::LuaBindingNamespace(LuaBindingNamespace& ns) : base_stack_top_(0), this_ns_(0) {
            (*this) = ns;
        }
        
        LuaBindingNamespace& LuaBindingNamespace::operator=(LuaBindingNamespace& ns) {
            this_ns_ = ns.this_ns_;
            ns_ = ns.ns_;
            base_stack_top_ = ns.base_stack_top_;

            ns.base_stack_top_ = 0;
            return (*this);
        }


        bool LuaBindingNamespace::open(const char* namespace_) {
            close();

            if (NULL == namespace_) {
                return true;
            }

            ns_.clear();
            build_ns_set(namespace_);

            base_stack_top_ = base_stack_top_ ? base_stack_top_: lua_gettop(LuaEngine::Instance()->getLuaState());
            return find_ns();
        }

        void LuaBindingNamespace::close() {
            if (0 != base_stack_top_)
                lua_settop(LuaEngine::Instance()->getLuaState(), base_stack_top_);

            base_stack_top_ = 0;
            ns_.clear();
        }

        int LuaBindingNamespace::getNamespaceTable() {
            if (this_ns_)
                return this_ns_;
#ifdef LUA_RIDX_GLOBALS
            lua_State* state = LuaEngine::Instance()->getLuaState();
            base_stack_top_ = base_stack_top_ ? base_stack_top_ : lua_gettop(state);
            lua_rawgeti(state, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
            return this_ns_ = lua_gettop(state);
#else
            return this_ns_ = LUA_GLOBALSINDEX;
#endif
        }

        LuaBindingNamespace::self_type& LuaBindingNamespace::addConst(const char* const_name, const char* n, size_t s) {
            lua_State* state = getLuaState();
            lua_pushstring(state, const_name);
            lua_pushlstring(state, n, s);
            lua_settable(state, getNamespaceTable());

            return *this;
        }

        LuaBindingNamespace::self_type& LuaBindingNamespace::addStaticMethod(const char* method_name, static_method fn) {
            lua_State* state = getLuaState();
            lua_pushstring(state, method_name);
            lua_pushlightuserdata(state, reinterpret_cast<void*>(fn));
            lua_pushcclosure(state, __static_method_wrapper, 1);
            lua_settable(state, getNamespaceTable());

            return *this;
        }

        lua_State* LuaBindingNamespace::getLuaState() {
            return lua::LuaEngine::Instance()->getLuaState();
        }

        int LuaBindingNamespace::__static_method_wrapper(lua_State *L) {
            static_method fn = reinterpret_cast<static_method>(lua_touserdata(L, lua_upvalueindex(1)));
            if (NULL == fn) {
            	cocos2d::log("lua try to call static method but fn not set.\n");
                return 0;
            }

            return fn(L);
        }

        void LuaBindingNamespace::build_ns_set(const char* namespace_) {
            // strtok(str, ".") is not thread safe
            const char* s = namespace_, *e = namespace_;
            while (*e) {
                if ('.' == *e) {
                    ns_.push_back(std::string(s, e));
                    s = e + 1;
                }

                ++e;
                }
            if (s < e)
                ns_.push_back(std::string(s, e));
        }

        bool LuaBindingNamespace::find_ns() {
            int cur_ns = getNamespaceTable();
            lua_State* state = LuaEngine::Instance()->getLuaState();

            for (std::string& ns : ns_) {
                lua_pushlstring(state, ns.c_str(), ns.size());
                lua_gettable(state, cur_ns);
                if (lua_isnil(state, -1)) {
                    lua_pop(state, 1);
                    lua_newtable(state);
                    int top = lua_gettop(state);
                    lua_pushlstring(state, ns.c_str(), ns.size());
                    lua_pushvalue(state, top);
                    lua_settable(state, cur_ns);
                } 

                if (0 == lua_istable(state, -1)) {
                    return false;
                }
                cur_ns = lua_gettop(state);
            }

            this_ns_ = cur_ns;
            return true;
        }
    }
}
