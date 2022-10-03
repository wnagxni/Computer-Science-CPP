/*file: search.cpp
 * modified: 2022/10/3 wangxin
 */
#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "map.h"
#include "search.h"
#include "set.h"
#include "strlib.h"
#include "vector.h"
#include "testing/SimpleTest.h"
#include "simpio.h"
using namespace std;


/**
 * @brief cleanToken 将字符串转换为可用作索引的纯净版形式
 * @param s 预处理的字符串
 * @return 返回处理后的字符串
 */
string cleanToken(string s)
{
    //去除首尾标点符号
    int left = 0;
    int right = s.length() - 1;
    while(left <= right && ispunct(s.at(left))) ++left;
    while (right >= left && ispunct(s.at(right))) --right;
    if (left > right)  //字符串中全是标点
        return "";
    s = s.substr(left, right - left + 1);

    //确认当前string包含字母 + to lower case
    bool hasAlpha = false; //标记字符串中是否有字母
    for (auto& ch : s) { //增强for加引用来修改string
        if (isalpha(ch)) {
            hasAlpha = true; //标志位修改
            if (isupper(ch))
                ch = toLowerCase(ch); //是大写字母的话转变为小写
        }
    }
    if (!hasAlpha)
        return ""; //字符串中无字母
    return s;
}

/**
 * @brief gatherTokens 按空格拆分字符串后用cleanToken处理
 * @param text
 * @return 返回不重复的子字符串
 */
Set<string> gatherTokens(string text)
{
    Set<string> tokens;
    auto subStrs = stringSplit(text, " "); //按空格进行分隔
    for (string elem : subStrs) {
        string cleanedStr = cleanToken(elem);
        if (cleanedStr == "") continue; //空串对应cleanToken中不合法的返回值，直接丢弃
        tokens.add(cleanedStr);
    }
    return tokens;
}

/**
 * @brief buildIndex 将数据库文件中的单词提取并进行反向映射
 * @param dbfile 存储所有网页信息的数据库文件
 * @param index 反向映射map
 * @return 返回处理的网页数量
 */
int buildIndex(string dbfile, Map<string, Set<string>>& index)
{
    ifstream in;

    if (!openFile(in, dbfile))
        error("Cannot open file named " + dbfile);

    Vector<string> lines;
    readEntireFile(in, lines); //文件中行存入lines vector

    if (lines.size() & 1) //size为奇数，则去除最后一行
        lines.remove(lines.size() - 1);
    for (int i = 0; i < lines.size(); i += 2) {
        string url = lines[i];
        Set<string> strs = gatherTokens(lines[i + 1]);
        for (string str : strs)
            index[str].add(url); //有记录则返回记录，没有则返回set默认构造;无需判断contains
    }
    return lines.size() / 2;
}

/**
 * @brief findQueryMatches 根据关键词和反向索引查找结果
 * @param index 已创建好的反向索引
 * @param query 查询关键词
 * @return 返回查询结果
 */
Set<string> findQueryMatches(Map<string, Set<string>>& index, string query)
{
    Vector<string> terms = stringSplit(query, " "); //以空格分割检索词
    for (string& term : terms) {
        //对每个检索词进行清洗
        if (term[0] == '+' || term[0] == '-') { //有modifier的保留
            term = term[0] + cleanToken(term.substr(1));
        } else
            term = cleanToken(term);
    }

    Set<string> result = index.get(terms[0]); //匹配项
    for (int i = 1; i < terms.size(); i++) {
        char firstLetter = terms[i][0];
        if (firstLetter == '+'){ //根据关键词开头进行判断集合规则
            auto curQuery = index.get(terms[i].substr(1)); //去掉关键词的modifier
            result.intersect(curQuery);
        } else if (firstLetter == '-'){
            auto curQuery = index.get(terms[i].substr(1));
            result.difference(curQuery);
        } else {
            auto curQuery = index.get(terms[i]);
            result.unionWith(curQuery);
        }
    }
    return result;
}

// TODO: Add a function header comment here to explain the
// behavior of the function and how you implemented this behavior
void searchEngine(string dbfile)
{
    Map<string, Set<string>> index;
    int pageNum = buildIndex(dbfile, index); //建立索引并获得网页数量
    cout << "Indexed " << pageNum <<" pages containing "
         << index.size() << " unique terms" << endl;
    while (true) {
        string str = getLine("Enter a surname (RETURN to quit): "); //从键盘读取输入，直到enter停止
        if (str == "")
            break; //程序中止
        Set<string> matches = findQueryMatches(index, str);
        cout << "Found " << matches.size() <<" matching pages" << endl;
        cout << matches << endl;
    }
}

/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("cleanToken on strings with no punctuation at beginning or end") {
    EXPECT_EQUAL(cleanToken("hello"), "hello");
    EXPECT_EQUAL(cleanToken("WORLD"), "world");
    EXPECT_EQUAL(cleanToken("CS*106B"), "cs*106b");
}

PROVIDED_TEST("cleanToken on strings with some punctuation at beginning and end") {
    EXPECT_EQUAL(cleanToken("/hello/"), "hello");
    EXPECT_EQUAL(cleanToken("~woRLD!"), "world");
}

PROVIDED_TEST("cleanToken on non-word strings (no letters)"){
    EXPECT_EQUAL(cleanToken("106"), "");
    EXPECT_EQUAL(cleanToken("~!106!!!"), "");
}

PROVIDED_TEST("gatherTokens from simple string") {
    Set<string> expected = {"go", "gophers"};
    EXPECT_EQUAL(gatherTokens("go go go gophers"), expected);
}

PROVIDED_TEST("gatherTokens correctly cleans tokens") {
    Set<string> expected = {"i", "love", "cs*106b"};
    EXPECT_EQUAL(gatherTokens("I _love_ CS*106B!"), expected);
}

PROVIDED_TEST("gatherTokens from seuss, 5 unique words, mixed case, punctuation") {
    Set<string> tokens = gatherTokens("One Fish Two Fish *Red* fish Blue fish ** 10 RED Fish?");
    EXPECT_EQUAL(tokens.size(), 5);
    EXPECT(tokens.contains("fish"));
    EXPECT(!tokens.contains("Fish"));
}

PROVIDED_TEST("buildIndex from tiny.txt, 4 pages, 11 unique tokens") {
    Map<string, Set<string>> index;
    int nPages = buildIndex("res/tiny.txt", index);
    EXPECT_EQUAL(nPages, 4);
    EXPECT_EQUAL(index.size(), 11);
    EXPECT(index.containsKey("fish"));
}

PROVIDED_TEST("findQueryMatches from tiny.txt, single word query") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRed = findQueryMatches(index, "red");
    EXPECT_EQUAL(matchesRed.size(), 2);
    EXPECT(matchesRed.contains("www.dr.seuss.net"));
    Set<string> matchesHippo = findQueryMatches(index, "hippo");
    EXPECT(matchesHippo.isEmpty());
}

PROVIDED_TEST("findQueryMatches from tiny.txt, compound queries") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRedOrFish = findQueryMatches(index, "red fish");
    EXPECT_EQUAL(matchesRedOrFish.size(), 4);
    Set<string> matchesRedAndFish = findQueryMatches(index, "red +fish");
    EXPECT_EQUAL(matchesRedAndFish.size(), 1);
    Set<string> matchesRedWithoutFish = findQueryMatches(index, "red -fish");
    EXPECT_EQUAL(matchesRedWithoutFish.size(), 1);
}


// TODO: add your test cases here
STUDENT_TEST("cleanToken on strings") {
    EXPECT_EQUAL(cleanToken(":\'1a2?b?"), "1a2?b");
    EXPECT_EQUAL(cleanToken("a!106!"), "a!106");
    EXPECT_EQUAL(cleanToken("!!!A"), "a");
    EXPECT_EQUAL(cleanToken("ASD"), "asd");
}

STUDENT_TEST("gatherToken()验证") {
    string text = " a A @B b bBb# Bbb! !!! !!@#  ";
    Set<string> expexted = {"a", "b", "bbb"};
    EXPECT_EQUAL(gatherTokens(text), expexted);
}

STUDENT_TEST("buildIndex from website.txt") {
    Map<string, Set<string>> index;
    int nPages = buildIndex("res/website.txt", index);
    EXPECT_EQUAL(nPages, 32);
    EXPECT(index.containsKey("is"));
    TIME_OPERATION(32, buildIndex("res/website.txt", index));
}

STUDENT_TEST("findQueryMatches from tiny.txt, compound queries") {
    Map<string, Set<string>> index;
    buildIndex("res/tiny.txt", index);
    Set<string> matchesRedOrFish = findQueryMatches(index, "10");
    Set<string> matchesRed = findQueryMatches(index, "51b_m&ms");
    Set<string> matches = findQueryMatches(index, "green");
}
