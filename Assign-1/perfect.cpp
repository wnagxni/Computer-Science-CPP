/*
 * Assignment 1
 * 2022-09-22
 */
#include "console.h"
#include <iostream>
#include "testing/SimpleTest.h"
using namespace std;


/* This function takes one argument `n` and calculates the sum
 * of all proper divisors of `n` excluding itself. To find divisors
 * a loop iterates over all numbers from 1 to n-1, testing for a
 * zero remainder from the division.
 *
 * Note: long is a C++ type is a variant of int that allows for a
 * larger range of values. For all intents and purposes, you can
 * treat it like you would an int.
 */
long divisorSum(long n) {
    long total = 0;
    for (long divisor = 1; divisor < n; divisor++) {
        if (n % divisor == 0) {
            total += divisor;
        }
    }
    return total;
}

/* This function takes one argument `n` and returns a boolean
 * (true/false) value indicating whether or not `n` is perfect.
 * A perfect number is a non-zero positive number whose sum
 * of its proper divisors is equal to itself.
 */
bool isPerfect(long n) {
    return (n != 0) && (n == divisorSum(n));
}

/* This function does an exhaustive search for perfect numbers.
 * It takes one argument `stop` and searches the range 1 to `stop`,
 * checking each number to see whether it is perfect and if so,
 * printing it to the console.
 */
void findPerfects(long stop) {
    for (long num = 1; num < stop; num++) {
        if (isPerfect(num)) {
            cout << "Found perfect number: " << num << endl;
        }
        if (num % 10000 == 0) cout << "." << flush; // progress bar
    }
    cout << endl << "Done searching up to " << stop << endl;
}

/* 统计n的所有因子（不包括n）的和：
 * 基于每个小于平方根的除数都与一个大于平方根的除数相互唯一对应的事实
 * 因此寻找一个数的所有因子时，仅需遍历[1, sqrt(n)]，即可找到所有因子
*/
long smarterSum(long n) {

    //为了`total += divisor + n / divisor;`循环内统一，避免在divisor=1时total加上1和本身，直接把1拿出来
    long total = n==1 ? 0 : 1;
    long divisor;
    for (divisor = 2; divisor < sqrt(n); divisor++) {
        if (n % divisor == 0) {
            total += divisor + n / divisor;
        }
    }
    if (divisor * divisor ==  n)
        total += divisor;//避免平方根加两遍
    return total;
}

/* 使用smarterSum替代后的isPerfect
 */
bool isPerfectSmarter(long n) {
    return (n != 0) && (n == smarterSum(n));
}

/* 使用smarterSum替代后的findPerfects
 */
void findPerfectsSmarter(long stop) {
    for (long num = 1; num < stop; num++) {
        if (isPerfectSmarter(num)) {
            cout << "Found perfect number: " << num << endl;
        }
        if (num % 10000 == 0) cout << "." << flush; // progress bar
    }
    cout << endl << "Done searching up to " << stop << endl;
}

/* 找到第n个梅森完全数：由于smarterSum()得到的所有真因子和为1时表明为素数
 * 因此可以借用判断是否为素数，再利用Euclid算法判断是否可以构造出下一个完全数
 */
long findNthPerfectEuclid(long n) {
    int cnt = 0; //统计目前发现的梅森素数个数
    int k = 1;
    while (true) {
        int m = pow(2, k) - 1; //梅森数
        if (smarterSum(m) == 1 && ++cnt == n) // 若m为素数则可构造出下个梅森完全数，若找到了，且为第n个则返回
            return pow(2, k-1) * m;
        k++; //递增循环index
    }
    return 0;
}


/* * * * * * Test Cases * * * * * */

/* Note: Do not add or remove any of the PROVIDED_TEST tests.
 * You should add your own STUDENT_TEST tests below the
 * provided tests.
 */

PROVIDED_TEST("Confirm divisorSum of small inputs") {
    EXPECT_EQUAL(divisorSum(1), 0);
    EXPECT_EQUAL(divisorSum(6), 6);
    EXPECT_EQUAL(divisorSum(12), 16);
}

PROVIDED_TEST("Confirm 6 and 28 are perfect") {
    EXPECT(isPerfect(6));
    EXPECT(isPerfect(28));
}

PROVIDED_TEST("Confirm 12 and 98765 are not perfect") {
    EXPECT(!isPerfect(12));
    EXPECT(!isPerfect(98765));
}

PROVIDED_TEST("Test oddballs: 0 and 1 are not perfect") {
    EXPECT(!isPerfect(0));
    EXPECT(!isPerfect(1));
}

PROVIDED_TEST("Confirm 33550336 is perfect") {
    EXPECT(isPerfect(33550336));
}

PROVIDED_TEST("Time trials of findPerfects on doubling input sizes") {
    TIME_OPERATION(10000, findPerfects(10000));
    TIME_OPERATION(20000, findPerfects(20000));
    TIME_OPERATION(40000, findPerfects(40000));
}

// TODO: add your student test cases here
STUDENT_TEST("Time trials of findPerfects on doubling input sizes") {
    TIME_OPERATION(20000, findPerfects(20000));
    TIME_OPERATION(40000, findPerfects(40000));
    TIME_OPERATION(80000, findPerfects(80000));
    TIME_OPERATION(160000, findPerfects(160000));
}



STUDENT_TEST("Time trials of findPerfects on doubling input sizes") {
    EXPECT(!isPerfect(0));
    EXPECT(!isPerfect(-5));
    EXPECT(!isPerfect(-1000));
}


STUDENT_TEST("Confirm smarterSum works") {
    for (int i = 1; i <=2000; i++) {
        EXPECT_EQUAL(smarterSum(i), divisorSum(i));
    }
    EXPECT_EQUAL(smarterSum(1), divisorSum(1));
    EXPECT_EQUAL(smarterSum(6), divisorSum(6));
    EXPECT_EQUAL(smarterSum(25), divisorSum(25));
}

STUDENT_TEST("Confirm 12 and 98765 are not perfect") {
    EXPECT(!isPerfectSmarter(12));
    EXPECT(!isPerfectSmarter(98765));
}

STUDENT_TEST("Test oddballs: 0 and 1 are not perfect") {
    EXPECT(!isPerfectSmarter(0));
    EXPECT(!isPerfectSmarter(1));
}

STUDENT_TEST("Confirm isPerfectSmarter is perfect") { //验证isPerfectSmarter
        for (int i = 1; i <=10000; i++) {
            EXPECT_EQUAL(isPerfect(i), isPerfectSmarter(i));
        }
}

STUDENT_TEST("Time trials of findPerfects on doubling input sizes") {
    TIME_OPERATION(500000, findPerfectsSmarter(500000));
    TIME_OPERATION(1000000, findPerfectsSmarter(1000000));
    TIME_OPERATION(2000000, findPerfectsSmarter(2000000));
    TIME_OPERATION(4000000, findPerfectsSmarter(4000000));
}

STUDENT_TEST("Time trials of findPerfects on doubling input sizes") {
    TIME_OPERATION(33550336, findPerfectsSmarter(33550336));
}

STUDENT_TEST("Confirm findNthPerfectEuclid works") {
    EXPECT_EQUAL(findNthPerfectEuclid(1), 6);
    EXPECT_EQUAL(findNthPerfectEuclid(2), 28);
    EXPECT_EQUAL(findNthPerfectEuclid(3), 496);
    EXPECT_EQUAL(findNthPerfectEuclid(5), 33550336);
}


