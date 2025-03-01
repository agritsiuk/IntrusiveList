#include <IntrusiveList.hpp>
#include <vector>
#include <gtest/gtest.h>

TEST(IntrusiveListTest, Smoke) {
    struct List : IntrusiveList<int, List> {};
    List list{};

    ASSERT_TRUE(list.empty());
    ASSERT_EQ(list.size(), 0ul);

    for (int i = 0; i < 3; ++i) {
        ASSERT_EQ(list.emplace_back(i), i);
    }

    ASSERT_FALSE(list.empty());
    ASSERT_EQ(list.size(), 3ul);
    ASSERT_EQ(list.front(), 0);
    ASSERT_EQ(list.back(), 2);
    ASSERT_EQ(*list.begin(), 0);

    ASSERT_EQ((std::vector<int>(list.begin(), list.end())), (std::vector<int>{0, 1, 2}));
    ASSERT_EQ((std::vector<int>(list.rbegin(), list.rend())), (std::vector<int>{2, 1, 0}));

    for (auto iter = list.begin(), end = list.end(); iter != end;) {
        if (*iter == 1) {
            iter = list.erase(iter);
        } else {
            ++iter;
        }
    }
    ASSERT_EQ(list.size(), 2ul);
    ASSERT_EQ((std::vector<int>(list.begin(), list.end())), (std::vector<int>{0, 2}));

    for (auto iter = list.rbegin(), end = list.rend(); iter != end;) {
        if (*iter == 2) {
            iter = list.erase(iter);
        } else {
            ++iter;
        }
    }

    ASSERT_EQ((std::vector<int>(list.begin(), list.end())), (std::vector<int>{0}));
}