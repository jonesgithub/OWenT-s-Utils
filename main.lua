print('============================= Load Lua Start =============================')

local loader = require('utils.loader')
require('utils.event')

-- ע������game�µ�tableΪ�����ռ�
do
    local class = loader.load('utils.class')
    local function reg_game_namespace(tb, prefix, name, base_type)
        local this_path = prefix .. name
        prefix = this_path .. '.'

        for k, v in pairs(tb) do
            -- �����ռ�ע�����
            if 'table' == type(v) and nil == getmetatable(v) then
                reg_game_namespace(v, prefix, k, class.namespace)
            end

            -- native��ע�����
            if 'table' == type(v) and nil ~= getmetatable(v) and nil == v.__index then
                reg_game_namespace(v, prefix, k, class.native)
            end
        end

        class.register(tb, base_type, this_path)
    end

    reg_game_namespace(game, '', 'game', class.namespace)
end

-- ����bootstrap
loader.load_list('bootstrap')


vardump(game)

-- ��������
do
    local cfg = loader.load('data.conf_manager')
    vardump(cfg.__data)
end
print('============================= Load Lua End =============================')
