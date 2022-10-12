/*
 * 编写代码确定输入字符串的括号是否平衡，顺序是否有问题
 */
#include <iostream>    // for cout, endl
#include <string>      // for string class
#include "recursion.h"
#include "testing/SimpleTest.h"

using namespace std;

/**
 * @brief operatorsFrom 接收一个字符串参数,通过*递归*处理后并返回一个只包含括号操作符的字符串
 * @param str
 * @return
 */
//string operatorsFrom(string str) {

//    /*思考:
//     *1. 每次递归都需要调用substr求剩余子串，可以用指针标记来避免
//     *2. 这里采用brackets来判断是否包含括号，但是N次递归会产生N个`brackets`，比较冗余，占用不少空间
//     *   我希望它把设定为局部变量，每次判断完自己销毁，这样就不会存在栈帧里了，暂时没想到怎么搞，类似匿名函数?
//     *   如果用比如`if(true) {...}`包起来倒是可以实现，但是感觉有点不太优雅，怎么处理比较好呢？我觉得应该有处理这种问题的方法，
//     *   如果用 `for (char ch : "()[]{}") {...}`倒是可以避免产生string对象，但是对与这个字符串字面量时静态对象，在程序运行过程中会一直存在，
//     *   那递归过程中是否还是会产生多个字面量呢?还是共用一个，如何获得字面量的地址来进行比较呢？希望和老师讨论一下
//     *
//     *   仔细想想好像也没必要，假设str长度为n，也就占用了 6n Byte，还好
//     *   可能还是递归这个模式中，函数生命周期内的变量是不会自动销毁的，这是递归固有的毛病，复杂的程序中无法避免不定义函数内部变量
//     */

//    if (str.empty())
//        return ""; //当字符串为空时，直接返回空串
//    //当字符串不为空时，处理第一个char并递归调用处理剩下的子串
//    string res = ""; //存储结果，若在str上原地操作时间复杂度较高，如erase删除不是括号的，均摊复杂度为O(n),不划算
//    string brackets = "()[]{}";

//    if (brackets.find(str.at(0)) != string::npos) { //str首字符是括号
//        res += str.at(0);
//    }

//    return res + operatorsFrom(str.substr(1)); //递归调用处理剩下的子串
//}



/**
 * @brief operatorsFrom
 * @param str 引用传参
 * @param ptr 采用指针标识当前字符串处理的位置
 * @return
 */
string operatorsFrom_recursion(string& str, int ptr) {
    if (ptr == str.length()) //base case
        return ""; //当字符串长度为0，或字符串已经全处理
    char curCh = str.at(ptr); //获取当前处理的字符,避免重复索引，虽然也是O(1),但应该还是空间换点时间
    for (char ch : "()[]{}") { //for循环操作和上面brackets.find()哪个更好一些呢？差不多把？
        if (curCh == ch)
            return curCh + operatorsFrom_recursion(str, ptr + 1); //是括号就加上
    }
    return operatorsFrom_recursion(str, ptr + 1); //不是括号，递归调用处理下一个字符
}

//wrapper function for `operatorsFrom_recursion`
string operatorsFrom(string str) {
    return operatorsFrom_recursion(str, 0);
}


/**
 * @brief operatorsAreMatched 判断括号序列是否合规
 * operatorsAreMatched("[(){}]") -> "()" exists, remove it and check whether rest is matched
 * operatorsAreMatched("[{}]") -> "{}" exists, remove it and check whether rest is matched
 * operatorsAreMatched("[]") -> "[]" exists, remove it and check whether rest is matched
 * operatorsAreMatched("") -> true
 * @param ops
 * @return
 */
bool operatorsAreMatched(string ops) {
    if (ops == "") //字符串为空时认为合规,同时作为递归出口base case
        return true;
    //字符串不为空时,核心思路是从左往右寻直接相邻的配对括号并移除,从而转化为为规模较小的子问题
    //这覆盖了两种可能,一是并列的括号对,如`(){[]}`的小括号;而是最内层括号,如`{[()]}`的小括号
    for (int i = 0; i < (int) ops.length() - 1; i++) { //可去除的配对左括号仅可能出现在前length - 1个位置
        char curCh = ops.at(i);
        if ( (curCh == '(' && ops.at(i + 1) == ')') || //应该会比substr(i, 2)快点,因为有短路运算
             (curCh == '[' && ops.at(i + 1) == ']') ||
             (curCh == '{' && ops.at(i + 1) == '}') ) {
            return operatorsAreMatched(ops.erase(i, 2)); //发现配对括号即删除,函数结果为剩余序列结果
            //??erase复杂度为O(n),有无更好的思路呢?
            /* 值得注意的是,由于erase返回的是*this即修改的string本身,这里没有新建一个深拷贝或复原的原因基于以下考虑,
             * 只要if条件满足,那么递归就确认会走这一分支,且走到头,直到字符串本身为空返回true,和其他情况,
             * 这种单向的递归意味着所有递归可以共用同一个string,如果是还需要走其他分支则必须考虑复原了
             */
        } //如果没找到就下一个字符
    }
    return false; //没有相邻配对的,即"("或")("或"{(})"三种情况
}

/*
 * The isBalanced function assumes correct implementation of
 * the above two functions operatorsFrom and operatorsMatch.
 * It uses operatorsFrom to extract the operator characters
 * from the input string and then confirms that those
 * operators are balanced by using operatorsMatch.
 * You should not modify the provided code in the isBalanced
 * function.  If the previous two functions have been implemented
 * correctly, the provided isBalanced will correctly report whether
 * the input string has balanced bracketing operators.
 */
bool isBalanced(string str) {
    string ops = operatorsFrom(str);
    return operatorsAreMatched(ops);
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("operatorsFrom on simple example") {
    EXPECT_EQUAL(operatorsFrom("vec[3]"), "[]");
}

PROVIDED_TEST("operatorsAreMatched on simple example") {
    EXPECT(operatorsAreMatched("{}"));
}

PROVIDED_TEST("isBalanced on example from writeup") {
    string example ="int main() { int x = 2 * (vec[2] + 3); x = (1 + random()); }";
    EXPECT(isBalanced(example));
}

PROVIDED_TEST("isBalanced on non-balanced examples from writeup") {
    EXPECT(!isBalanced("( ( [ a ] )"));
    EXPECT(!isBalanced("3 ) ("));
    EXPECT(!isBalanced("{ ( x } y )"));
}


/* * * * * * My Test Cases * * * * * */
STUDENT_TEST("operatorsFrom on simple examples") {
    EXPECT_EQUAL(operatorsFrom(""), "");
    EXPECT_EQUAL(operatorsFrom("int main() { int x = 2 * (vec[2] + 3); x = (1 + random()); }"), "(){([])(())}");
    EXPECT_EQUAL(operatorsFrom("vesdf  @#c[ ( )3]"), "[()]");
}

STUDENT_TEST("operatorsAreMatched on simple examples") {
    EXPECT(operatorsAreMatched("")); //空
    EXPECT(operatorsAreMatched("{}")); //配对
    EXPECT(operatorsAreMatched("{([])}"));//配对
    EXPECT(!operatorsAreMatched("({)}")); //交错
    EXPECT(!operatorsAreMatched(")(")); //左右反
    EXPECT(!operatorsAreMatched("({}")); //左右缺失

}


