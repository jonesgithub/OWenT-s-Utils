#pragma once

#include <string>
#include <assert.h>
#include <stdint.h>
#include <cstddef>
#include <functional>
#include <list>
#include <set>
#include <map>

#include <Utils/DesignPattern/Singleton.h>


namespace script {
    namespace lua {
        class LuaBindingMgr : public Singleton<LuaBindingMgr> {
        public:
            typedef std::function<void()> func_type;

        protected:
            LuaBindingMgr();
            ~LuaBindingMgr();


        public:
            int init();

            void addBind(func_type fn);

            void addUserdataType(const char* name_);

            bool isUserdataRegistered(const char* name_);

            uint32_t addRef(void* inst);
            uint32_t removeRef(void* inst);
        private:
            std::list<func_type> auto_bind_list_;
            std::set<std::string> userdata_reg_set_;
            std::map<intptr_t, uint32_t> obj_ref_;
        };


        class LuaBindingWrapper {
        public:
            LuaBindingWrapper(LuaBindingMgr::func_type);
            ~LuaBindingWrapper();
        };
    }
}

#define LUA_BIND_VAR_NAME(name)  script_lua_LuaBindMgr_Var_##name
#define LUA_BIND_FN_NAME(name)  script_lua_LuaBindMgr_Fn_##name

#define LUA_BIND_OBJECT(name)   \
    static void LUA_BIND_FN_NAME(name)(); \
    static script::lua::LuaBindingWrapper LUA_BIND_VAR_NAME(name)(LUA_BIND_FN_NAME(name)); \
    void LUA_BIND_FN_NAME(name)()

