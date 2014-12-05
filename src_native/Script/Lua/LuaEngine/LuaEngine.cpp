#include <cstdlib>
#include <list>
#include <ctime>
#include <cmath>
#include <sstream>

#include "LuaEngine.h"

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "cocos2d.h"

#ifdef LUA_BINDING_ENABLE_ENGINE_MT
#include <thread>
#include "Lock/SpinLock.h"
#endif

#include "../LuaModule/LuaTableExt.h"

namespace script
{
    namespace lua
    {
        extern int LuaProfile_openLib(lua_State *L);


        LuaAutoBlock::LuaAutoBlock(lua_State* L)
            : m_state(L)
            , m_stackTop(lua_gettop(m_state))
        {
        }

        LuaAutoBlock::~LuaAutoBlock()
        {
            lua_settop(m_state, m_stackTop);
        }

        void LuaAutoBlock::NullCall() {}

        LuaAutoBlock::LuaAutoBlock() : m_state(nullptr), m_stackTop(0){ assert(false); }
        LuaAutoBlock::LuaAutoBlock(const LuaAutoBlock& src) : m_state(src.m_state), m_stackTop(src.m_stackTop) { assert(false); }
        const LuaAutoBlock& LuaAutoBlock::operator=(const LuaAutoBlock& src) { 
            m_state = src.m_state;
            m_stackTop = src.m_stackTop;
            assert(false);  
            return (*this); 
        }


        LuaAutoStats::LuaAutoStats() {
            begin_clock_ = std::chrono::steady_clock::now();
        }

        LuaAutoStats::~LuaAutoStats() {
            auto duration = (std::chrono::steady_clock::now() - begin_clock_);
            LuaEngine::Instance()->addLuaStatTime(
                std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() / 1000.0f
            );
            // LuaEngine::Instance()->addLuaStatTime(abs(end_clock_ - begin_clock_) * 1.0f / CLOCKS_PER_SEC);
        }

        LuaEngine::LuaEngine() : state_(NULL)
        {
            lua_update_stats_.lua_time = 0.0f;
            lua_update_stats_.run_time = 0.0f;
        }


        LuaEngine::~LuaEngine()
        {
        }

        int LuaEngine::addOnInited(std::function<void()> fn) {
            on_inited_.push_back(fn);
            return 0;
        }

        int LuaEngine::init(const std::string& script_root_dir, lua_State * state)
        {
            script_root_dir_ = script_root_dir;
            if (state == NULL)
            {
                state_ = luaL_newstate();
                luaL_openlibs(state_);
            }
            else
            {
                state_ = state;
            }
            // 添加脚本根目录为搜索目录
            if (!script_root_dir.empty()) {
                addSearchPath(script_root_dir);
                addCSearchPath(script_root_dir);
            }

            // add 3rdparty librarys
            addExtLib(LuaProfile_openLib);
            addExtLib(LuaTableExt_openLib);
            //addExtLib(luaopen_profiler);
            //addExtLib(luaopen_bit);
            //addExtLib(luaopen_pack);
            //addExtLib(luaopen_mime_core);
            //addExtLib(luaopen_socket_core);
            //addExtLib(luaopen_cjson);
            //addExtLib(luaopen_protobuf_c);

            for(std::function<void()>& fn: on_inited_){
                fn();
            }
            on_inited_.clear();
            return 0;
        }

        void LuaEngine::addExtLib(lua_CFunction regfunc)
        {
            regfunc(state_);
        }

        void LuaEngine::addSearchPath(const std::string& path, bool is_front)
        {
            lua_getglobal(state_, "package");                                  /* L: package */
            lua_getfield(state_, -1, "path");                /* get package.path, L: package path */
            const char* cur_path = lua_tostring(state_, -1);
            if (is_front)
                lua_pushfstring(state_, "%s/?.lua;%s", path.c_str(), cur_path);    /* L: package path newpath */
            else
                lua_pushfstring(state_, "%s;%s/?.lua", cur_path, path.c_str());    /* L: package path newpath */
            lua_setfield(state_, -3, "path");          /* package.path = newpath, L: package path */
            lua_pop(state_, 2);                                                /* L: - */
        }

        void LuaEngine::addCSearchPath(const std::string& path, bool is_front)
        {
            lua_getglobal(state_, "package");                                    /* L: package */
            lua_getfield(state_, -1, "cpath");                 /* get package.path, L: package cpath */
            const char* cur_path = lua_tostring(state_, -1);
#ifdef WIN32
            if (is_front)
                lua_pushfstring(state_, "%s/?.dll;%s", path.c_str(), cur_path);    /* L: package path newpath */
            else
                lua_pushfstring(state_, "%s;%s/?.dll", cur_path, path.c_str());    /* L: package path newpath */
#else
            if (is_front)
                lua_pushfstring(state_, "%s/?.so;%s", path.c_str(), cur_path);    /* L: package path newpath */
            else
                lua_pushfstring(state_, "%s;%s/?.so", cur_path, path.c_str());    /* L: package path newpath */
#endif
            lua_setfield(state_, -3, "cpath");          /* package.cpath = newpath, L: package cpath */
            lua_pop(state_, 2);
        }


        void LuaEngine::addLuaLoader(lua_CFunction func)
        {
            if (!func) return;

            // stack content after the invoking of the function
            // get loader table
            lua_getglobal(state_, "package");                                  /* L: package */
            lua_getfield(state_, -1, "loaders");                               /* L: package, loaders */

            // insert loader into index 2
            lua_pushcfunction(state_, func);                                   /* L: package, loaders, func */
            for (int i = lua_objlen(state_, -2) + 1; i > 2; --i)
            {
                lua_rawgeti(state_, -2, i - 1);                                /* L: package, loaders, func, function */
                // we call lua_rawgeti, so the loader table now is at -3
                lua_rawseti(state_, -3, i);                                    /* L: package, loaders, func */
            }
            lua_rawseti(state_, -2, 2);                                        /* L: package, loaders */

            // set loaders into package
            lua_setfield(state_, -2, "loaders");                               /* L: package */

            lua_pop(state_, 1);
        }

        bool LuaEngine::runCode(const char* codes)
        {
            run_pending_codes();

            LuaAutoBlock autoBlock(state_);
            lua::LuaAutoStats autoLuaStat;

            int hmsg = getPCallHMsg(state_);
            if (luaL_loadstring(state_, codes) || lua_pcall(state_, 0, LUA_MULTRET, hmsg))
            {
                cocos2d::log("%s", luaL_checkstring(state_, -1));
                return false;
            }

            return true;
        }

        bool LuaEngine::runFile(const char* file_path)
        {
            run_pending_codes();

            LuaAutoBlock autoBlock(state_);
            lua::LuaAutoStats autoLuaStat;

            int hmsg = getPCallHMsg(state_);
            if (luaL_loadfile(state_, file_path) || lua_pcall(state_, 0, LUA_MULTRET, hmsg))
            {
                cocos2d::log("%s", luaL_checkstring(state_, -1));
                return false;
            }

            return true;
        }

        lua_State* LuaEngine::getLuaState()
        {
            return state_;
        }

        bool LuaEngine::loadItem(const std::string& path, bool auto_create_table) {
            return loadItem(getLuaState(), path, auto_create_table);
        }

        bool LuaEngine::loadItem(const std::string& path, int table_index, bool auto_create_table) {
            return loadItem(getLuaState(), path, table_index, auto_create_table);
        }


        bool LuaEngine::loadItem(lua_State* L, const std::string& path, bool auto_create_table) {
#ifdef LUA_RIDX_GLOBALS
            lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
            bool res = loadItem(L, path, -1, auto_create_table);
            lua_remove(L, -2);
            return res;
#else
            return loadItem(L, path, LUA_GLOBALSINDEX, auto_create_table);
#endif
        }

        bool LuaEngine::loadItem(lua_State* L, const std::string& path, int table_index, bool auto_create_table) {
            lua::LuaAutoStats autoLuaStat;

            int res = true;
            std::list<std::string> idents;
            // strtok(str, ".") is not thread safe
            const char* s = path.c_str(), *e = path.c_str();
            while (*e) {
                if ('.' == *e) {
                    idents.push_back(std::string(s, e));
                    s = e + 1;
                }

                ++e;
            }
            if (s < e)
                idents.push_back(std::string(s, e));

            lua_pushvalue(L, table_index);
            for (std::list<std::string>::iterator iter = idents.begin(); res && iter != idents.end(); ++iter) {
                lua_getfield(L, -1, iter->c_str());
                if (lua_isnil(L, -1)) {
                    if (auto_create_table) {
                        lua_pop(L, 1);
                        lua_newtable(L);
                        lua_pushvalue(L, -1);
                        lua_setfield(L, -3, iter->c_str());
                    } else {
                        res = false;
                    }
                }

                lua_remove(L, -2);
            }

            return res && !lua_isnil(L, -1);
        }

        bool LuaEngine::removeItem(const std::string& path) {
            return removeItem(getLuaState(), path);
        }

        bool LuaEngine::removeItem(const std::string& path, int table_index) {
            return removeItem(getLuaState(), path, table_index);
        }


        bool LuaEngine::removeItem(lua_State* L, const std::string& path) {
#ifdef LUA_RIDX_GLOBALS
            lua_rawgeti(L, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
            bool res = removeItem(L, path, -1);
            lua_pop(L, 1);
            return res;
#else
            return loadItem(L,path, LUA_GLOBALSINDEX, false);
#endif
        }

        bool LuaEngine::removeItem(lua_State* L, const std::string& path, int table_index) {
            LuaAutoBlock block(L);
            lua::LuaAutoStats autoLuaStat;

            std::list<std::string> idents;
            // strtok(str, ".") is not thread safe
            const char* s = path.c_str(), *e = path.c_str();
            while (*e) {
                if ('.' == *e) {
                    idents.push_back(std::string(s, e));
                    s = e + 1;
                }

                ++e;
            }
            if (s < e)
                idents.push_back(std::string(s, e));

            if (idents.empty())
                return true;

            std::string last_ident = idents.back();
            idents.pop_back();

            lua_pushvalue(L, table_index);
            for (std::string& ident: idents) {
                lua_getfield(L, -1, ident.c_str());
                if (lua_isnil(L, -1)) {
                    return true;
                }
            }

            lua_pushnil(L);
            lua_setfield(L, -2, last_ident.c_str());

            return true;
        }

        bool LuaEngine::loadGlobalEventTrigger(lua_State* L, const std::string& bind_name) {
            lua::LuaAutoStats autoLuaStat;

            lua_getglobal(L, "utils");
            do {
                if (lua_istable(L, -1)) {
                    lua_getfield(L, -1, "event");
                    lua_remove(L, -2);
                    if (lua_istable(L, -1)) {
                        lua_getfield(L, -1, "global");
                        lua_remove(L, -2);
                        if (lua_istable(L, -1)) {
                            lua_getfield(L, -1, "trigger");
                            lua_pushvalue(L, -2);
                            lua_remove(L, -3);
                            lua_pushlstring(L, bind_name.c_str(), bind_name.size());
                            return lua_isfunction(L, -3);
                        }
                    }
                }
            } while (false);

            // 保证增加两个栈对象
            lua_pushnil(L);
            lua_pushnil(L);
            return false;
        }

        bool LuaEngine::loadGlobalEventTrigger(const std::string& bind_name) {
            return loadGlobalEventTrigger(getLuaState(), bind_name);
        }

        int LuaEngine::getPCallHMsg(lua_State* L) {
            if (NULL == L)
                return 0;

            int hmsg = 0;
            lua_getglobal(L, "stackdump");
            if (lua_isfunction(L, -1))
                hmsg = lua_gettop(L);

            return hmsg;
        }

        int LuaEngine::getPCallHMsg() {
            return getPCallHMsg(getLuaState());
        }

        void LuaEngine::updateGlobalTimer(float delta) {
            run_pending_codes();

            lua_State* state = getLuaState();
            lua::LuaAutoBlock block(state);
            LuaAutoStats autoLuaStat;

            int hmsg = getPCallHMsg(state);

            lua_getglobal(state, "utils");
            if (lua_istable(state, -1)) {
                lua_getfield(state, -1, "event");
                if (lua_istable(state, -1)) {
                    lua_getfield(state, -1, "update");
                    if (lua_isfunction(state, -1)) {
                        lua_pushnumber(state, delta);

                        if (0 != lua_pcall(state, 1, LUA_MULTRET, hmsg)) {
                            cocos2d::log("[Lua]: %s", luaL_checkstring(state, -1));
                        }
                    }
                }
            }

            lua_update_stats_.run_time += delta;
        }

        void LuaEngine::addLuaStatTime(float delta) {
            lua_update_stats_.lua_time += delta;
        }

        std::pair<float, float> LuaEngine::getAndResetLuaStats() {
            std::pair<float, float> ret = std::make_pair(lua_update_stats_.lua_time, lua_update_stats_.run_time);
            lua_update_stats_.lua_time = lua_update_stats_.run_time = 0.0f;
            return ret;
        }

        void LuaEngine::printStack(lua_State* luaState)
        {
            int iArgs = lua_gettop(luaState);
            //LUALOGCALL("STACK TOP:%d", iArgs);

            std::stringstream ss;
            for (int i = 1; i <= iArgs; i++)
            {
                ss << i << ":";

                if (lua_istable(luaState, i))
                {
                    ss << "table";
                }
                else if (lua_isnone(luaState, i))
                {
                    ss << "none";
                }
                else if (lua_isnil(luaState, i))
                {
                    ss << "nil";
                }
                else if (lua_isboolean(luaState, i))
                {
                    if (lua_toboolean(luaState, i) != 0)
                        ss << "true";
                    else
                        ss << "false";
                }
                else if (lua_isfunction(luaState, i))
                {
                    ss << "function";
                }
                else if (lua_islightuserdata(luaState, i))
                {
                    ss << "lightuserdata";
                }
                else if (lua_isthread(luaState, i))
                {
                    ss << "thread";
                }
                else
                {
                    const char* tinfo = lua_tostring(luaState, i);
                    if (tinfo)
                        ss << tinfo;
                    else
                        ss << lua_typename(luaState, lua_type(luaState, i));
                }

                if (i != iArgs)
                {
                    ss << " |";
                }
            }

            cocos2d::log("STACK :%s", ss.str().c_str());
        }

        void LuaEngine::printTrackBack(lua_State* luaState)
        {
            LuaAutoBlock autoBlock(luaState);

            lua_getglobal(luaState, "debug");
            lua_getfield(luaState, -1, "traceback");

            printStack(luaState);

            if (lua_pcall(luaState, 0, 1, 0) != 0)
            {
                cocos2d::log("%s", luaL_checkstring(luaState, -1));
                return;
            }

            cocos2d::log("TRACE:\n%s", luaL_checkstring(luaState, -1));
        }


#ifdef LUA_BINDING_ENABLE_ENGINE_MT
        static util::lock::SpinLock running_thread_lock_;
        static std::thread::id running_thread_id_;
#endif

        void LuaEngine::addPendingCode(const std::string& script) {
#ifdef LUA_BINDING_ENABLE_ENGINE_MT
            // 防止重入
            std::thread::id td = std::this_thread::get_id();
            bool need_reset = false;
            if (td != running_thread_id_) {
                running_thread_lock_.Lock();
                running_thread_id_ = td;
                need_reset = true;
            }
#endif
            pending_codes_.push_back(script);

#ifdef LUA_BINDING_ENABLE_ENGINE_MT
            if (need_reset) {
                running_thread_id_ = std::thread::id();
                running_thread_lock_.Unlock();
            }
#endif
        }

        void LuaEngine::run_pending_codes() {
#ifdef LUA_BINDING_ENABLE_ENGINE_MT
            // 防止重入
            std::thread::id td = std::this_thread::get_id();
            bool need_reset = false;
            if (td != running_thread_id_) {
                running_thread_lock_.Lock();
                running_thread_id_ = td;
                need_reset = true;
            }
#endif

            std::list<std::string> clear_codes;
            clear_codes.swap(pending_codes_);
            for (const std::string& code : clear_codes) {
                runCode(code.c_str());
            }

#ifdef LUA_BINDING_ENABLE_ENGINE_MT
            if (need_reset) {
                running_thread_id_ = std::thread::id();
                running_thread_lock_.Unlock();
            }
#endif
        }
    }
}
