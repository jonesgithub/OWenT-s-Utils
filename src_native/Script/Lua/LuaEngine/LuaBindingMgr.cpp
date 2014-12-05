#include <cstdlib>

#include "LuaBindingMgr.h"

#include "LuaEngine.h"

namespace script
{
    namespace lua
    {
        LuaBindingMgr::LuaBindingMgr()
        {
        }


        LuaBindingMgr::~LuaBindingMgr()
        {
        }

        int LuaBindingMgr::init() {
            auto state = LuaEngine::Instance()->getLuaState();
            for (func_type& fn : auto_bind_list_) {
                LuaAutoBlock block(state);
                fn();
            }
            return 0;
        }

        void LuaBindingMgr::addBind(func_type fn){
            auto_bind_list_.push_back(fn);
        }

        void LuaBindingMgr::addUserdataType(const char* name_) {
            userdata_reg_set_.insert(name_);
        }

        bool LuaBindingMgr::isUserdataRegistered(const char* name_) {
            return userdata_reg_set_.end() != userdata_reg_set_.find(name_);
        }

        uint32_t LuaBindingMgr::addRef(void* inst) {
            intptr_t key = reinterpret_cast<intptr_t>(inst);
            auto iter = obj_ref_.find(key);
            if (obj_ref_.end() == iter)
                return obj_ref_[key] = 1;

            return (++iter->second);
        }

        uint32_t LuaBindingMgr::removeRef(void* inst) {
            intptr_t key = reinterpret_cast<intptr_t>(inst);
            auto iter = obj_ref_.find(key);
            if (obj_ref_.end() == iter)
                return 0;

            uint32_t ret = (--iter->second);
            if (0 == ret)
                obj_ref_.erase(iter);

            return ret;
        }

        LuaBindingWrapper::LuaBindingWrapper(LuaBindingMgr::func_type fn) {
            LuaBindingMgr::Instance()->addBind(fn);
        }

        LuaBindingWrapper::~LuaBindingWrapper() {
        }
    }
}
