#include <vector>
#include <map>

#include "gtest/gtest.h"
#include "std/foreach.h"

TEST(owent_foreach, Array) 
{
	//����
	const int arr[] = {1, 7, 3, 9, 5, 6, 2, 8, 4};
	int sum1 = 0, sum2 = 0;
	owent_foreach(const int& v, arr) {
		sum1 += v;
	}
	
	for (int i = 0; i < 9; ++ i) {
		sum2 += arr[i];
	}

    EXPECT_EQ(sum1, sum2);
}

TEST(owent_foreach, STL) 
{
	//����
	std::vector<int> vec;
	for (unsigned int i = 0; i < 128; ++ i)
	vec.push_back((i * i + i / 33));
		
	int sum1 = 0, sum2 = 0, index = 0;
	owent_foreach(int& v, vec) {
		v = index * 33 + index;
		sum1 += v;
		++ index;
	}
	
	for (unsigned int i = 0; i < vec.size(); ++ i) {
		sum2 += vec[i];
	}

    EXPECT_EQ(sum1, sum2);
	EXPECT_NE(sum1, 0);
}

TEST(owent_foreach, STL_Pair) 
{
	//����
	std::map<int, int> mp;
	for (unsigned int i = 0; i < 128; ++ i)
		mp[i] = i * i + i / 33;
		
	unsigned int count = 0;
	typedef std::map<int, int>::value_type map_pair; // ����foreach�Ǻ궨�壬��������������ŵĻ�������ô�����������������Ϊ��������ǲ����ָ���
    owent_foreach(map_pair& pr, mp) {
		++ count;
        pr.second = 0;
    }

    EXPECT_EQ(mp.size(), count);
	EXPECT_EQ(mp[15], 0);
}

class SeedForeachRefCopyLimit
{
private:
    SeedForeachRefCopyLimit(const SeedForeachRefCopyLimit&);
    SeedForeachRefCopyLimit& operator=(const SeedForeachRefCopyLimit&);
	
public:
    int count;
    SeedForeachRefCopyLimit(){}
};

TEST(owent_foreach, RefCopyLimit)
{
    SeedForeachRefCopyLimit arr[10];
    int idx = 1, sum = 0;
	owent_foreach(SeedForeachRefCopyLimit& stNode, arr)
    {
        stNode.count = idx*= 2;
        sum += stNode.count;
    }

    EXPECT_EQ(sum, 2046);
} 
