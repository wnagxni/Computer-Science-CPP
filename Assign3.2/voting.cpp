// file: voting.cpp
// 10/22/2022 11:08PM WangXin

#include <iostream>    // for cout, endl
#include <string>      // for string class
#include "voting.h"
#include "testing/SimpleTest.h"
#include "vector.h"
using namespace std;

/**
 * @brief computePowerIndexes wrapper function 遍历每一个block获得关键选票数并归一化
 * @param blocks
 * @return
 */
Vector<int> computePowerIndexes(Vector<int>& blocks)
{
    Vector<int> result;
    int totalVotes = 0;
    for (auto block : blocks)
        totalVotes += block; //统计总选票，方便递归时判断当前路径是否绝对优势，同时当作参数传入避免每次递归都统计一遍
    for (int i = 0; i < blocks.size(); i++) {
        // 遍历选中每一个特定block
        int hasSame = -1; //避免重复计算，由于选区不可能太多，对于每个选区都从头比较一遍消耗很小，如果选区非常多可以sort之后用编码保存block中相对位置index后再按照大小计算，避免重复
        for (int before = 0; before < i; before++) {
            if (blocks.get(i) == blocks.get(before)) {
                hasSame = before;
                break;
            }
        }
        //如果在计算过的选区中找到重复了的，直接用它的结果，不然就自己算
        result.add(hasSame == -1 ? computeSpecificCriticalVotes(blocks, totalVotes, i, 0, 0) : result.get(hasSame));
    }
    normalize(result);
    return result;
}


/**
 * @brief computeSpecificCriticalVotes 递归地计算特定选区的Critical votes, 思路如下：
 * 1. 采用include/exclude recursive partern可以获得各个可能票形
 * 2. 判断每一路径中该选区是否具有关键投票，统计
 * @param blocks
 * @param totalVotes 提前计算好的总票数，避免递归函数内重复计算
 * @param specificBlock 本次递归中需获得重要票数的选区
 * @param curIndex 目前path搜索的index
 * @param subSum 除特定block外的当前子集的和
 * @return
 */
int computeSpecificCriticalVotes(Vector<int>& blocks, const int& totalVotes, int specificBlock,
                                 int curIndex, int subSum) {
    //base case: 1. 遍历结束，获得子集，开始判断特定选区是否critical
    //2. 如果当前递归到最后一个block且为本次特定的block那么提前结束
    if (curIndex == blocks.size() || (curIndex == blocks.size() - 1 && curIndex == specificBlock)) {
        /* 1. 在现有票赢不了（包括平局）的情况下有特定选区的联盟就赢 1.0 * subSum / totalVotes <= 0.5 && subSum + blocks.get(specificBlock) > totalVotes/2
         * 2. 或者在现有情况下会输但有了特定选区就不会输（包括平局），两种情况 1.0 * subSum / totalVotes < 0.5 && subSum + blocks.get(specificBlock) >= 1.0 * totalVotes/2
         * 对于本题而言，由于将不考虑平局，事实上只有两种结果：赢和没赢（注意，这里不是输，因为包含了平局）
         */
        if ( (1.0 * subSum / totalVotes <= 0.5 && subSum + blocks.get(specificBlock) > totalVotes/2)) //如果从没赢-->赢
            return 1;
        return 0;
    }

    if (curIndex  == specificBlock) //对于前n-1个区，判断是否需要跳过
        curIndex++; //排除标记特定选区


    //include case:
    int includeCnt = computeSpecificCriticalVotes(blocks, totalVotes, specificBlock, curIndex + 1,
                                 subSum + blocks.get(curIndex));
    //exclude case
    int excludeCnt = computeSpecificCriticalVotes(blocks, totalVotes, specificBlock, curIndex + 1,
                                 subSum);
    return includeCnt + excludeCnt;
}

//将V中元素归一化，单位 %
void normalize(Vector<int>& v) {
    int sum = 0;
    for (int num : v) //求和
        sum += num;

    for (int i = 0; i < v.size(); i++) {
        v[i] = v[i] * 100 / sum; //归一化，percentage
    }
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("Test power index, blocks 50-49-1") {
    Vector<int> blocks = {50, 49, 1};
    Vector<int> expected = {60, 20, 20};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Test power index, blocks Hempshead 1-1-3-7-9-9") {
    Vector<int> blocks = {1, 1, 3, 7, 9, 9};
    Vector<int> expected = {0, 0, 0, 33, 33, 33};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Test power index, blocks CA-TX-NY 55-38-39") {
    Vector<int> blocks = {55, 38, 29};
    Vector<int> expected = {33, 33, 33};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Test power index, blocks CA-TX-GA 55-38-16") {
    Vector<int> blocks = {55, 38, 16};
    Vector<int> expected = {100, 0, 0};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Test power index, blocks EU post-Nice") {
    // Estonia is one of those 4s!!
    Vector<int> blocks = {29,29,29,29,27,27,14,13,12,12,12,12,12,10,10,10,7,7,7,7,7,4,4,4,4,4,3};
    Vector<int> expected = {8, 8, 8, 8, 7, 7, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0};
    EXPECT_EQUAL(computePowerIndexes(blocks), expected);
}

PROVIDED_TEST("Time power index operation大量重复") {
    Vector<int> blocks;
    for (int i = 0; i < 30; i++) {
        blocks.add(randomInteger(1, 2));
    }
    TIME_OPERATION(blocks.size(), computePowerIndexes(blocks));
}

STUDENT_TEST("时间复杂度估计") {
    for (int size = 10; size <= 30; size++) {
        Vector<int> blocks;
        for (int i = 0; i < size; i++) {
            blocks.add(randomInteger(1, 100)); //range大一些避免碰撞,减少优化，可以更多看到复杂度
        }
        TIME_OPERATION(blocks.size(), computePowerIndexes(blocks));
        cout << size << " completed" << endl;
    }
}

