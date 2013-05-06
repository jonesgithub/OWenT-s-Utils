
#include "gtest/gtest.h"
#include "DataStructure/StaticIdxList.h"

struct static_idx_list_helper_class
{
    int m;
    static_idx_list_helper_class(): m(0){}
    static_idx_list_helper_class(int _m): m(_m){}
    static_idx_list_helper_class(double, int _m): m(_m){}
};

static bool static_idx_list_helper_count_func(int, static_idx_list_helper_class& obj)
{
    return obj.m == 30;
}

struct static_idx_list_helper_class_func_obj
{
    bool operator()(int, const static_idx_list_helper_class& obj)
    {
        return obj.m == 20;
    }
};

TEST(StaticIdxListTest, Count)
{
    util::ds::StaticIdxList<static_idx_list_helper_class, 128> stList;
    stList.construct();

	ASSERT_EQ(0, stList.Count());

    stList.Create();

    stList.Create(10);
    stList.Create(20);
    stList.Create(30);

    stList.Create(30.0, 20);
    stList.Create(30.0, 50);

    ASSERT_EQ(6, stList.Count());
    ASSERT_EQ(1, stList.Count(static_idx_list_helper_count_func));

    ASSERT_EQ(2, stList.Count(static_idx_list_helper_class_func_obj()));
}

struct static_idx_list_helper_class_foreach_func_obj
{
    int& m;
    static_idx_list_helper_class_foreach_func_obj(int& _m): m (_m){}
    void operator()(int, static_idx_list_helper_class& obj)
    {
        m += obj.m;
    }
};

TEST(StaticIdxListTest, Foreach)
{
    int sum = 0;

    util::ds::StaticIdxList<static_idx_list_helper_class, 128> stList;
    stList.construct();

    stList.Create(10);
    stList.Create(20);
    stList.Create(30);
    stList.Create(20);
    stList.Create(10);
    stList.Create(50);
    stList.Foreach(static_idx_list_helper_class_foreach_func_obj(sum));

    ASSERT_EQ(140, sum);
}

TEST(StaticIdxListTest, Create)
{
    util::ds::StaticIdxList<int, 3> stList;
    stList.construct();

    util::ds::StaticIdxList<int, 5>::size_type idx1 = stList.Create();
    util::ds::StaticIdxList<int, 5>::size_type idx2 = stList.Create(1);
    util::ds::StaticIdxList<int, 5>::size_type idx3 = stList.Create(2);
    util::ds::StaticIdxList<int, 5>::size_type idx4 = stList.Create(3);

    ASSERT_EQ(0, idx1);
    ASSERT_EQ(1, idx2);
    ASSERT_EQ(2, idx3);
    ASSERT_EQ(-1, idx4);

    ASSERT_EQ(3, stList.Count());
}

TEST(StaticIdxListTest, Remove)
{
    util::ds::StaticIdxList<int, 5> stList;
    stList.construct();

    stList.Create(4);
    stList.Create(1);
    util::ds::StaticIdxList<int, 5>::size_type idx3 = stList.Create(2);
    stList.Create(3);
    util::ds::StaticIdxList<int, 5>::size_type idx5 = stList.Create(5);

    ASSERT_EQ(4, *stList.Get(0));
    ASSERT_EQ(2, *stList.Get(2));

    ASSERT_EQ(5, stList.Count());

    stList.Remove(idx3);
    stList.Remove(idx3);

    ASSERT_EQ(4, stList.Count());

    idx3 = stList.Create(6);
    ASSERT_EQ(2, idx3);

    ASSERT_EQ(5, *stList.Get(idx5));
    ASSERT_EQ(6, *stList.Get(2));

    ASSERT_EQ(5, stList.Count());
}

TEST(StaticIdxListTest, EdgeCondition)
{
    typedef util::ds::StaticIdxList<int, 4> core_type;
    core_type stList;
    stList.construct();

    core_type::size_type idx1 = stList.Create(4);
    stList.Create(1);
    stList.Create(2);
    core_type::size_type idx4 = stList.Create(3);

    // ���ɾ���ʹ���
    stList.Remove(idx4);
    idx4 = stList.Create(5);
    stList.Remove(idx4);
    idx4 = stList.Create(6);

    ASSERT_EQ(4, stList.Count());
    ASSERT_EQ(6, *stList.Get(idx4));

    // ֻ��һ��Ԫ�أ����Ҷ��Ǳ߽磩
    util::ds::StaticIdxList<int, 1> stEle;
    stEle.construct();

    idx1 = stEle.Create(7);
    stEle.Create(8);
    stEle.Remove(0);

    ASSERT_EQ(0, stEle.Count());

    idx1 = stEle.Create(9);
    ASSERT_EQ(0, idx1);
    ASSERT_EQ(1, stEle.Count());

    stEle.Remove(0);
    ASSERT_EQ(0, stEle.Count());

    idx1 = stEle.Create(9);
    ASSERT_EQ(0, idx1);

    // destruct�󴴽�
    stList.Remove(0);
    idx1 = stList.begin().index();
    ASSERT_NE(0, idx1);

    stList.destruct();
    idx4 = stList.Create(10);

    ASSERT_EQ(idx4, idx1);

    // �����ڴ�ָ�����
    char pData[sizeof(core_type)];
    core_type* pListOri = new (pData)core_type();
    pListOri->construct();

    pListOri->Create(12);
    pListOri->Create(13);
    pListOri->Create(14);
    pListOri->Create(15);

    ASSERT_EQ(static_cast<size_t>(4), pListOri->size());

    core_type* pListRec = new (pData)core_type();
    ASSERT_EQ(static_cast<size_t>(4), pListRec->size());
    ASSERT_EQ(0, pListRec->begin().index());
    ASSERT_EQ(14, *pListRec->get(2));
}

TEST(StaticIdxListTest, Iterator)
{
    typedef util::ds::StaticIdxList<int, 10> core_type;
    core_type stList;
    stList.construct();

    typedef core_type::node_type node_type;
    stList.Create(1);
    stList.Create(2);
    stList.Create(4);
    stList.Create(8);
    stList.Create(16);
    stList.Create(32);

    // const iterator
    int iTestBit = 0;
    for(util::ds::StaticIdxList<int, 10>::const_iterator iter = stList.begin();
        iter != stList.end(); ++ iter)
    {
        iTestBit |= *iter;
    }

    ASSERT_EQ(63, iTestBit);

    // non-const iterator
    for(util::ds::StaticIdxList<int, 10>::iterator iter = stList.begin();
        iter != stList.end(); ++ iter)
    {
        (*iter) <<= 6;
        iTestBit |= *iter;
    }
    ASSERT_EQ(4095, iTestBit);

    ASSERT_GT(0, stList.end().index());
    ASSERT_LE(0, stList.begin().index());
}
