/*
 * 实现一个高效的分治算法来合并有序序列
 * 2022/10/12 11:00PM
 */
#include <iostream>    // for cout, endl
#include "queue.h"
#include "testing/SimpleTest.h"
//#include "recursion.h"
using namespace std;

/**
 * @brief binaryMerge 使用迭代将两个有序队列合并为一个有序队列,对无序元素抛出error
 * @param a
 * @param b
 * @return 返回合并和队列
 */
Queue<int> binaryMerge(Queue<int> a, Queue<int> b) {
    //感觉直接在函数里添加检验队列有序代码比较冗余(2处判断),如果是数组直接遍历判断显然更合适,考虑到这里是队列,如果
    //采用方法一先判断输入队列是否有序的话,得出队后再入队,就不如放在后面比较的时候判断了,
    //但问题是我这里得判断逻辑是否能简化?
    Queue<int> result;

    int aLastNum = 0x80000000; //定义为int最小
    int bLastNum = 0x80000000; //用来存储a和b中的前个数,以和队首相比较,判断是否队列有序
    while (!a.isEmpty() && !b.isEmpty()) {
        if (a.peek() <= b.peek()) {
            if (a.peek() < aLastNum) { //在队首出队前判断比较合适
                error("输入队列无序"); //如果a队首原色小于之前的队首,那么就无序了,报错
            }
            aLastNum = a.dequeue(); //更新a上个元素
            result.enqueue(aLastNum); //小的出原队入result队
        } else {
            if (b.peek() < bLastNum) { //在队首出队前判断比较合适
                error("输入队列无序"); //如果b队首原色小于之前的队首,那么就无序了,报错
            }
            bLastNum = b.dequeue();//更新b上个元素
            result.enqueue(bLastNum);
        }
    }
    //此时a和b至少有一个为队列空了,其余那个直接出队入队即可
    while (!a.isEmpty()) {
        if (a.peek() < aLastNum) { //在队首出队前判断比较合适
            error("输入队列无序"); //如果a队首原色小于之前的队首,那么就无序了,报错
        }
        result.enqueue(a.dequeue());
    }

    while (!b.isEmpty()) {
        if (b.peek() < bLastNum) { //在队首出队前判断比较合适
            error("输入队列无序"); //如果b队首原色小于之前的队首,那么就无序了,报错
        }
        result.enqueue(b.dequeue());
    }

    return result;
}

/*
 * This function assumes correct functionality of the previously
 * defined binaryMerge function and makes use of this function to
 * iteratively merge a collection of sequences, starting off with
 * an empty sequence and progressively merging in one sequence at
 * a time. This function is provided for you fully implemented –
 * it does not need to modified at all.
 */
Queue<int> naiveMultiMerge(Vector<Queue<int>>& all) {
    Queue<int> result;

    for (Queue<int>& q : all) {
        result = binaryMerge(q, result);
    }
    return result;
}

/**
 * @brief recMultiMerge_recursion 递归多路归并(调用二路归并),实际上可以迭代用最小堆处理?
 * @param all
 * @param left left 和right避免标记当前处理得vector部分
 * @param right
 * @return
 */
Queue<int> recMultiMerge_recursion(Vector<Queue<int>>& all, int left, int right) {
    //base case
    if (left > right) return {}; //如果为空vector直接返回空q
    if (left == right) return  all.get(left); //如果vector只剩一个q,那么直接返回即可

    int mid = left + ((right - left) >> 1); //左中位数
    Queue<int> leftRes = recMultiMerge_recursion(all, left, mid);
    Queue<int> rightRes = recMultiMerge_recursion(all, mid + 1, right);
    Queue<int> result = binaryMerge(leftRes, rightRes); //获得左右答案后进行二路归并
    return result;
}

//wrapper function
Queue<int> recMultiMerge(Vector<Queue<int>>& all) {
    return recMultiMerge_recursion(all, 0, all.size() - 1);
}

/* * * * * * Test Cases * * * * * */

Queue<int> createSequence(int size);
void distribute(Queue<int> input, Vector<Queue<int>>& all);

PROVIDED_TEST("binaryMerge, two short sequences") {
    Queue<int> a = {2, 4, 5};
    Queue<int> b = {1, 3, 3};
    Queue<int> expected = {1, 2, 3, 3, 4, 5};
    EXPECT_EQUAL(binaryMerge(a, b), expected);
    EXPECT_EQUAL(binaryMerge(b, a), expected);
}

STUDENT_TEST("binaryMerge, 一个队列为空") {
    Queue<int> a = {};
    Queue<int> b = {1, 3, 3};
    Queue<int> expected = {1, 3, 3};
    EXPECT_EQUAL(binaryMerge(a, b), expected);
    EXPECT_EQUAL(binaryMerge(b, a), expected);
}

STUDENT_TEST("binaryMerge, two short sequences") {
    Queue<int> a = {1,2,1}; //一个队列无序
    Queue<int> b = {1, 3, 3};
    EXPECT_ERROR(binaryMerge(a, b));
    EXPECT_ERROR(binaryMerge(b, a));
}


PROVIDED_TEST("naiveMultiMerge, small collection of short sequences") {
    Vector<Queue<int>> all = {{3, 6, 9, 9, 100},
                             {1, 5, 9, 9, 12},
                             {5},
                             {},
                             {-5, -5},
                             {3402}
                            };
    Queue<int> expected = {-5, -5, 1, 3, 5, 5, 6, 9, 9, 9, 9, 12, 100, 3402};
    EXPECT_EQUAL(naiveMultiMerge(all), expected);
}

STUDENT_TEST("naiveMultiMerge, 空Vector") {
    Vector<Queue<int>> all = {};
    Queue<int> expected = {};
    EXPECT_EQUAL(naiveMultiMerge(all), expected);
}

STUDENT_TEST("naiveMultiMerge, 多个空队列") {
    Vector<Queue<int>> all = {{}, {}, {}};
    Queue<int> expected = {};
    EXPECT_EQUAL(naiveMultiMerge(all), expected);
}

PROVIDED_TEST("recMultiMerge, compare to naiveMultiMerge") {
    int n = 20;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(n);
    distribute(input, all);
    EXPECT_EQUAL(recMultiMerge(all), naiveMultiMerge(all));
}

PROVIDED_TEST("Time binaryMerge operation") {
    int n = 1000000;
    Queue<int> a = createSequence(n);
    Queue<int> b = createSequence(n);
    TIME_OPERATION(a.size() + b.size(), binaryMerge(a, b));
}

PROVIDED_TEST("Time naiveMultiMerge operation") {
    int n = 11000;
    int k = n/10;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(k);
    distribute(input, all);
    TIME_OPERATION(input.size(), naiveMultiMerge(all));
}
PROVIDED_TEST("Time recMultiMerge operation") {
    int n = 90000;
    int k = n/10;
    Queue<int> input = createSequence(n);
    Vector<Queue<int>> all(k);
    distribute(input, all);
    TIME_OPERATION(input.size(), recMultiMerge(all));
}


/* Test helper to fill queue with sorted sequence */
Queue<int> createSequence(int size) {
    Queue<int> q;
    for (int i = 0; i < size; i++) {
        q.enqueue(i);
    }
    return q;
}

/* Test helper to distribute elements of sorted sequence across k sequences,
   k is size of Vector */
void distribute(Queue<int> input, Vector<Queue<int>>& all) {
    while (!input.isEmpty()) {
        all[randomInteger(0, all.size()-1)].enqueue(input.dequeue());
    }
}

