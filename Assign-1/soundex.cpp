/*
 * TODO: remove and replace this file header comment
 * This is a .cpp file you will edit and turn in.
 * Remove starter comments and add your own
 * comments on each function and on complex code sections.
 */
#include "hashmap.h"
#include "testing/SimpleTest.h"
#include "strlib.h"
#include "filelib.h"
#include "simpio.h"
#include <fstream>
#include <cctype>
#include <string>
#include "vector.h"

using namespace std;

/* This function is intended to return a string which
 * contains only the letter characters from the original
 * (all non-letter characters are removed)
 *
 * WARNING: The provided code is buggy!
 *
 * Use unit tests to identify which inputs to this function
 * are incorrectly handled. Then, remove this comment and
 * replace it with a description of the bug you fixed.
 */
string removeNonLetters(string s) {
    string result = "";
    for (int i = 0; i < s.length(); i++) {
        if (isalpha(s[i])) {
            result += s[i];
        }
    }
    return result;
}


/**
 * @brief 创建算法中对应的映射关系
 * @return 哈希表引用
 */
HashMap<char, char> createMap() {
    HashMap<char, char> map = {{ 'D', '3' }, { 'T', '3' },
                              { 'L', '4' },
                              { 'M', '5' }, { 'N', '5' },
                              { 'R', '6' }};
    for (char ch : "AEIOUHWY")
        map.put(ch, '0');
    for (char ch : "BFPV")
        map.put(ch, '1');
    for (char ch : "CGJKQSXZ")
        map.put(ch, '2');
    return map; //这里不知道可不可以优化？无法返回函数内的局部对象，只能返回值了，得多复制一次 ><
}



/* 该函数实现了 对于仅含**大写**字母的字符串的每个字母按照soundex算法中的映射表进行编码，
 * 当字母对应的编码找不到时，默认编码为'7'，不抛出异常
 */
string encodeString(string s, HashMap<char, char> map) {
    string res = ""; //保存结果
    for (int i = 0; i < s.length(); i++) {
        char ch = s.at(i);
        if (map.containsKey(ch))
            res += map.get(ch); //不能用 s.append(char c) 即append不能添加单个字符到string
        else
            res += '7'; //映射表有问题，没找到大写字母对应的V,添加错误的'7',magic number ):
    }
    return res;
}


/* 计算任一字符串的soundex编码
 */
string soundex(string s) {
    s = removeNonLetters(s); //移除非字母，同时保证第一个肯定是字母，方便后面取
    if (s.length() == 0)
        return ""; //如果仅有符号直接返回空串
    s = toUpperCase(s); //转换为大写，减少创建哈希keys
    auto map = createMap(); //创建哈希表
    string code = encodeString(s, map);
    //面没有再去把代码逻辑分开，由于前面辅助函数都已经测试过，所以后面的逻辑卸载一块可以直接测试整个程序
    //创建一个合并重复数字并去0的字符串
    string codeAfterMerge(1, s[0]); // 首字母大写后的
    for (int i = 0; i < code.length() - 1; i++) {
        char before = code.at(i), cur = code.at(i + 1);
        if (cur != before && cur != '0')
            codeAfterMerge += cur;
    }
    int len = codeAfterMerge.length();
    if (len < 4)
        return codeAfterMerge.append(4-len, '0'); //长度小于补足
    return codeAfterMerge.substr(0, 4); //长度大于等于4
}


/* 该函数读取filepath内的姓氏数据库，从中查询和键盘输入姓氏编码一致的姓氏
 */
void soundexSearch(string filepath) {
    // The proivded code opens the file with the given name
    // and then reads the lines of that file into a vector.
    ifstream in;
    Vector<string> databaseNames;

    if (openFile(in, filepath)) {
        readEntireFile(in, databaseNames);
    }
    cout << "Read file " << filepath << ", "
         << databaseNames.size() << " names found." << endl;

    // The names in the database are now stored in the provided
    // vector named databaseNames
    while (true) {
        string str = getLine("Enter a surname (RETURN to quit): "); //从键盘读取输入，直到enter停止
        if (str == "")
            break; //程序中止
        Vector<string> vec; //存放编码一样的姓氏
        string code = soundex(str);
        cout << "Soundex code is " << code << endl;
        for (auto item : databaseNames) { //数据查询编码一样的
            if (soundex(item) == code)
                vec.add(item);
        }
        vec.sort();//排序
        cout << "Matches from database: " << vec << endl;
    }
    cout <<"All done!" << endl;

}


/* * * * * * Test Cases * * * * * */


PROVIDED_TEST("Test removing puntuation, digits, and spaces") {
    string s = "O'Hara";
    string result = removeNonLetters(s);
    EXPECT_EQUAL(result, "OHara");
    s = "Planet9";
    result = removeNonLetters(s);
    EXPECT_EQUAL(result, "Planet");
    s = "tl dr";
    result = removeNonLetters(s);
    EXPECT_EQUAL(result, "tldr");
}


PROVIDED_TEST("Sample inputs from handout") {
    EXPECT_EQUAL(soundex("Curie"), "C600");
    EXPECT_EQUAL(soundex("O'Conner"), "O256");
}

PROVIDED_TEST("hanrahan is in lowercase") {
    EXPECT_EQUAL(soundex("hanrahan"), "H565");
}

PROVIDED_TEST("DRELL is in uppercase") {
    EXPECT_EQUAL(soundex("DRELL"), "D640");
}

PROVIDED_TEST("Liu has to be padded with zeros") {
    EXPECT_EQUAL(soundex("Liu"), "L000");
}

PROVIDED_TEST("Tessier-Lavigne has a hyphen") {
    EXPECT_EQUAL(soundex("Tessier-Lavigne"), "T264");
}

PROVIDED_TEST("Au consists of only vowels") {
    EXPECT_EQUAL(soundex("Au"), "A000");
}

PROVIDED_TEST("Egilsdottir is long and starts with a vowel") {
    EXPECT_EQUAL(soundex("Egilsdottir"), "E242");
}

PROVIDED_TEST("Jackson has three adjcaent duplicate codes") {
    EXPECT_EQUAL(soundex("Jackson"), "J250");
}

PROVIDED_TEST("Schwarz begins with a pair of duplicate codes") {
    EXPECT_EQUAL(soundex("Schwarz"), "S620");
}

PROVIDED_TEST("Van Niekerk has a space between repeated n's") {
    EXPECT_EQUAL(soundex("Van Niekerk"), "V526");
}

PROVIDED_TEST("Wharton begins with Wh") {
    EXPECT_EQUAL(soundex("Wharton"), "W635");
}

PROVIDED_TEST("Ashcraft is not a special case") {
    // Some versions of Soundex make special case for consecutive codes split by hw
    // We do not make this special case, just treat same as codes split by vowel
    EXPECT_EQUAL(soundex("Ashcraft"), "A226");
}


//************************************************************
// TODO: add your test cases here

STUDENT_TEST("Find the bug in removeNonLetters") {
    string s = "'OHara"; //非字母符号放在string首位
    string result = removeNonLetters(s);
    EXPECT_EQUAL(result, "OHara");
    s = "9Planet";
    result = removeNonLetters(s);
    EXPECT_EQUAL(result, "Planet");
    s = " tl dr";
    result = removeNonLetters(s);
    EXPECT_EQUAL(result, "tldr");
}

STUDENT_TEST("createMap哈希表功能是否实现？") {
    HashMap<char, char> map = createMap();
    cout << map << endl;
    EXPECT_EQUAL(map.get('A'), '0');
    EXPECT_EQUAL(map.get('R'), '6');
}

STUDENT_TEST("createMap哈希表功能是否实现？") {
    HashMap<char, char> map = createMap();
    cout << map << endl;
    EXPECT_EQUAL(map.get('A'), '0');
    EXPECT_EQUAL(map.get('R'), '6');
}

//STUDENT_TEST("encodeString 功能实现") {
//    HashMap<char, char> map = createMap();
//    EXPECT_EQUAL(encodeString("ANGELOU", map), "0520400");
//}

//STUDENT_TEST("验证去除重复功能") {
//    EXPECT_EQUAL(soundex("AAAABAA"), "1");
//    EXPECT_EQUAL(soundex("AabAB;a'aAa"), "A1");
//}
