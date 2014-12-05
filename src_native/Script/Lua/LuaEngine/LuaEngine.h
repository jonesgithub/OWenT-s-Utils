#pragma once

#include <string>
#include <assert.h>
#include <list>
#include <ctime>
#include <functional>
#include <chrono>
#include "Utils/DesignPattern/Singleton.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

// 加了多线程参数也不一定会用多线程，所以手动控制线程安全
//#ifdef LUA_BINDING_ENABLE_ENGINE_MT
//#undef LUA_BINDING_ENABLE_ENGINE_MT
//#endif

namespace script {
    namespace lua {
        class LuaAutoBlock
        {
        public:
            LuaAutoBlock(lua_State*);

            ~LuaAutoBlock();

            void NullCall();

        private:
            LuaAutoBlock();
            LuaAutoBlock(const LuaAutoBlock& src);
            const LuaAutoBlock& operator=(const LuaAutoBlock& src);

            lua_State* m_state;
            int m_stackTop;
        };

        /** A lua automatic statistics. */
        struct LuaAutoStats
        {
            LuaAutoStats();
            ~LuaAutoStats();

            std::chrono::steady_clock::time_point begin_clock_;
            //std::chrono::steady_clock::time_point end_clock_;
        };

        class LuaEngine : public Singleton < LuaEngine > {
        protected:
            LuaEngine();
            ~LuaEngine();


        public:
            int addOnInited(std::function<void()> fn);

            int init(const std::string& script_root_dir, lua_State *state = NULL);

            void addExtLib(lua_CFunction regfunc);

            void addSearchPath(const std::string& path, bool is_front = false);

            void addCSearchPath(const std::string& path, bool is_front = false);


            void addLuaLoader(lua_CFunction func);

            bool runCode(const char* codes);

            bool runFile(const char* file_path);

            lua_State* getLuaState();

            static bool loadItem(lua_State* L, const std::string& path, bool auto_create_table = false);

            static bool loadItem(lua_State* L, const std::string& path, int table_index, bool auto_create_table = false);

            bool loadItem(const std::string& path, bool auto_create_table = false);

            bool loadItem(const std::string& path, int table_index, bool auto_create_table = false);

            static bool removeItem(lua_State* L, const std::string& path);

            static bool removeItem(lua_State* L, const std::string& path, int table_index);

            bool removeItem(const std::string& path);

            bool removeItem(const std::string& path, int table_index);

            /**
             * Loads event trigger.
             * lua栈将会添加三个元素，分别是utils.event.trigger, utils.event.global 和 bind_name
             * @note 注意执行pcall时参数数量要加2
             *       
             * @param [in,out]  L   If non-null, the lua_State* to process.
             * @param   bind_name   Name of the bind.
             *
             * @return  true if it succeeds, false if it fails.
             */

            static bool loadGlobalEventTrigger(lua_State* L, const std::string& bind_name);

            /**
             * Loads event trigger.
             * lua栈将会添加三个元素，分别是utils.event.global.trigger, utils.event.global 和 bind_name
             * @note 注意执行pcall时参数数量要加2
             * 
             * @param [in,out]  L   If non-null, the lua_State* to process.
             * @param   bind_name   Name of the bind.
             *
             * @return  true if it succeeds, false if it fails.
             */
            bool loadGlobalEventTrigger(const std::string& bind_name);

            /**
             * 用于lua_pcall函数内的hmsg参数值
             * 
             * @note 会添加 lua function到lua虚拟机顶层，失败则添加nil，返回0
             * @param [in,out]  L   If non-null, the lua_State* to process.
             *
             * @return  The p call h message.
             */

            static int getPCallHMsg(lua_State* L);

            int getPCallHMsg();

            void updateGlobalTimer(float delta);
            void addLuaStatTime(float delta);

            std::pair<float, float> getAndResetLuaStats();

            void printStack(lua_State* luaState);
            void printTrackBack(lua_State* luaState);

            void addPendingCode(const std::string& script);

        private:
            void run_pending_codes();

        private:
            struct LuaStats{
                float lua_time;
                float run_time;
            };


            std::string script_root_dir_;
            lua_State* state_;
            std::list<std::function<void()> > on_inited_;
            
            LuaStats lua_update_stats_;


            std::list<std::string> pending_codes_;
        };

    }
}
