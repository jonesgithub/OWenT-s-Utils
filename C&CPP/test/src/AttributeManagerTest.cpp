#include <time.h>
#include <iostream>
#include <algorithm>

#include "gtest/gtest.h"
#include "Logic/AttributeManager.h"

enum EN_SAMPLE_ATTR
{
    ESA_UNKNOWN = 0,
    ESA_STRENTH = 1,
    ESA_BLABLAB = 2,
    ESA_BASIC_ATTACK = 3,
    ESA_ATTACK = 4,
    ESA_MAX_HP = 5,
};

struct AttributeManagerValidSample
{
    typedef util::logic::AttributeManager<EN_SAMPLE_ATTR, ESA_MAX_HP + 1, AttributeManagerValidSample, int> mt;

    static void GenAttrFormulaMap(mt::formula_builder_type& stFormulas)
    {
        using namespace util::logic::Operator;

        /* ��ʽ��������� = ���� * 2 + BLABLABLA */
        stFormulas[ESA_MAX_HP] = stFormulas[ESA_STRENTH] * 2 + _<mt>(ESA_BLABLAB);
        /* ��ʽ�������� = 100 * �������� - ���� */
        stFormulas[ESA_UNKNOWN] = stFormulas[ESA_ATTACK] = 100 * _<mt>(ESA_BASIC_ATTACK) - _<mt>(ESA_STRENTH) + _<mt>(ESA_STRENTH) * _<mt>(ESA_BASIC_ATTACK);
        /* ��ʽ���������� = BLABLABLA / 5 */
        stFormulas[ESA_BASIC_ATTACK] = _<mt>(ESA_BLABLAB) / 5;

    }
};


struct AttributeManagerInvalidSample
{
    typedef util::logic::AttributeManager<EN_SAMPLE_ATTR, ESA_MAX_HP + 1, AttributeManagerInvalidSample, int> mt;

    static void GenAttrFormulaMap(mt::formula_builder_type& stFormulas)
    {
        using namespace util::logic::Operator;

        /* ��ʽ��������� = 2 * ���� + BLABLABLA / �������� */
        stFormulas[ESA_MAX_HP] = 2 * stFormulas[ESA_STRENTH] + _<mt>(ESA_BLABLAB) / stFormulas[ESA_BASIC_ATTACK];
        /* ��ʽ��ESA_UNKNOWN = ������ = 100 * �������� - ���� + ���� * �������� */
        stFormulas[ESA_UNKNOWN] = stFormulas[ESA_ATTACK] = 100 * _<mt>(ESA_BASIC_ATTACK) - stFormulas[ESA_STRENTH] + stFormulas[ESA_STRENTH] * stFormulas[ESA_BASIC_ATTACK];
        /* ��ʽ���������� = BLABLABLA / 5 + ������� */
        stFormulas[ESA_BASIC_ATTACK] = _<mt>(ESA_BLABLAB) / 5 + stFormulas[ESA_MAX_HP];

        /* ��ʽѭ����ESA_BLABLAB = ESA_BLABLAB + ������� / 100 */
        stFormulas[ESA_BLABLAB] = stFormulas[ESA_BLABLAB]() + stFormulas[ESA_MAX_HP] / 100;

        /* ��ʽѭ�������� = ������� / 10 - �������� * ESA_BLABLAB */
        stFormulas[ESA_STRENTH] = _<mt>(ESA_MAX_HP) / 10 - stFormulas[ESA_BASIC_ATTACK] * stFormulas[ESA_BLABLAB];

        /* �����ǲ��ӡ���л��ģ����������ϵʽ�������ˡ�Ŀ��ò�����еĻ�����ȷ����� */
    }
};

TEST(AttributeManager, ValidCheck)
{
    AttributeManagerValidSample::mt foo;
    EXPECT_TRUE(foo.CheckValid());

    if (false == foo.CheckValid())
    {
        return;
    }

    // ��ʼ�ÿ�
    foo.Construct();
    foo[ESA_STRENTH] = 1000;
    foo[ESA_BLABLAB] = 512;

    // ================== ������� ==================
    // ��鱻������ϵ
    AttributeManagerValidSample::mt::attr_attach_list_type stList;

    // ���� ESA_STRENTH ������
    foo[ESA_STRENTH].GetAttachedAttributes(stList, false);
    EXPECT_EQ((size_t)3, stList.size());
    EXPECT_NE(stList.end(), std::find(stList.begin(), stList.end(), ESA_UNKNOWN));
    EXPECT_NE(stList.end(), std::find(stList.begin(), stList.end(), ESA_ATTACK));
    EXPECT_NE(stList.end(), std::find(stList.begin(), stList.end(), ESA_MAX_HP));

    // ���� ESA_BLABLAB ������
    stList.clear();
    foo[ESA_BLABLAB].GetAttachedAttributes(stList, false);
    EXPECT_EQ((size_t)2, stList.size());
    EXPECT_NE(stList.end(), std::find(stList.begin(), stList.end(), ESA_BASIC_ATTACK));
    EXPECT_NE(stList.end(), std::find(stList.begin(), stList.end(), ESA_MAX_HP));

    // ���� ESA_BLABLAB ������ - Recursion
    stList.clear();
    foo[ESA_BLABLAB].GetAttachedAttributes(stList, true);
    EXPECT_EQ((size_t)4, stList.size());
    EXPECT_NE(stList.end(), std::find(stList.begin(), stList.end(), ESA_UNKNOWN));
    EXPECT_NE(stList.end(), std::find(stList.begin(), stList.end(), ESA_BASIC_ATTACK));
    EXPECT_NE(stList.end(), std::find(stList.begin(), stList.end(), ESA_ATTACK));
    EXPECT_NE(stList.end(), std::find(stList.begin(), stList.end(), ESA_MAX_HP));

    // ���� ESA_ATTACK ������
    stList.clear();
    foo[ESA_ATTACK].GetAttachedAttributes(stList, true);
    EXPECT_EQ((size_t)0, stList.size());

    // ���� ESA_UNKNOWN ������
    stList.clear();
    foo[ESA_UNKNOWN].GetAttachedAttributes(stList, true);
    EXPECT_EQ((size_t)0, stList.size());

    // ��������
    AttributeManagerValidSample::mt::attr_attach_set_type stSet;

    // ESA_STRENTH �Ĺ�������
    foo[ESA_STRENTH].GetAttachAttributes(stSet, true);
    EXPECT_EQ((size_t)0, stSet.size());

    // ESA_MAX_HP �Ĺ�������
    stSet.clear();
    foo[ESA_MAX_HP].GetAttachAttributes(stSet, true);
    EXPECT_EQ((size_t)2, stSet.size());
    EXPECT_TRUE(stSet.find(ESA_STRENTH) != stSet.end());
    EXPECT_TRUE(stSet.find(ESA_BLABLAB) != stSet.end());

    // ESA_ATTACK �Ĺ�������
    stSet.clear();
    foo[ESA_ATTACK].GetAttachAttributes(stSet, false);
    EXPECT_EQ((size_t)2, stSet.size());
    EXPECT_TRUE(stSet.find(ESA_STRENTH) != stSet.end());
    EXPECT_TRUE(stSet.find(ESA_BASIC_ATTACK) != stSet.end());

    // ESA_ATTACK �Ĺ������� - Recursion
    stSet.clear();
    foo[ESA_ATTACK].GetAttachAttributes(stSet, true);
    EXPECT_EQ((size_t)3, stSet.size());
    EXPECT_TRUE(stSet.find(ESA_STRENTH) != stSet.end());
    EXPECT_TRUE(stSet.find(ESA_BLABLAB) != stSet.end());
    EXPECT_TRUE(stSet.find(ESA_BASIC_ATTACK) != stSet.end());


    // ================== ֵ��֤ ==================
    EXPECT_EQ(foo[ESA_UNKNOWN], foo[ESA_ATTACK]);
    EXPECT_EQ(1000, foo[ESA_STRENTH]);
    EXPECT_EQ(512, foo[ESA_BLABLAB]);
    EXPECT_EQ(foo[ESA_BLABLAB] / 5, foo[ESA_BASIC_ATTACK]);
    EXPECT_EQ(100 * foo[ESA_BASIC_ATTACK] - foo[ESA_STRENTH] + foo[ESA_STRENTH] * foo[ESA_BASIC_ATTACK], foo[ESA_ATTACK]);
    EXPECT_EQ(foo[ESA_STRENTH] * 2 + foo[ESA_BLABLAB], foo[ESA_MAX_HP]);
}

TEST(AttributeManager, InvalidCheck)
{
    EXPECT_FALSE(AttributeManagerInvalidSample::mt::CheckValid());
    AttributeManagerInvalidSample::mt::PrintInvalidLoops(std::cout);
}
