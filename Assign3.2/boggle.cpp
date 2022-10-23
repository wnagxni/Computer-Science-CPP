/*
 * 实现拼字游戏
 */
#include <iostream>    // for cout, endl
#include <string>      // for string class
#include "backtracking.h"
#include "gridlocation.h"
#include "grid.h"
#include "set.h"
#include "lexicon.h"
#include "testing/SimpleTest.h"
#include "hashSet.h"
#include "boggle.h"
using namespace std;

/*
 * 拼字游戏主函数：主逻辑是包装了遍历board每个位置去回溯以该字母为prefix的单词得分
 */
int scoreBoard(Grid<char>& board, Lexicon& lex) {
    Grid<bool> isMarked(board.numRows(), board.numCols(), false); //用作标记沿着某个字符串路径过程中board中走过的路
    GridLocation start(0, 0); //起点
    string curWord = "";
    HashSet<string> countedWordList;
    int total = 0; //Boggle板上所有单词的总分数
    for (int i = 0; i < board.numRows(); i++) { //遍历统计以各个位置为开头的得分
        for (int j = 0; j < board.numCols(); j++) {
            GridLocation start(i, j);
            total += backtracking(board, isMarked, lex, start, curWord, countedWordList);
        }
    }
    return total;
}


/*
 * 仅按照字符串长度返回分数（和是否在字典无关）
 * 规则：长度小于4返回0，其余返回length - 3
 */
int getPoints(const string &str) {
    int len = str.length();
    return len < 4 ? 0 : len - 3;
}
/**
 * @brief backtracking 以 *start* 为起点寻找单词
 * @param board
 * @param isMarked 在一次寻找单词过程中记录board中哪些位置已经被访问过
 * @param lex 词典
 * @param loc 当前位置
 * @param curWord 递归当前这个字母时之前的字符串
 * @param total 统计总得分，传引用，在递归过程共同维护这个值
 * @param countedWordList 单词不能重复记分，每个单词只记分一次，使用Set去重,hash表查询O(1)
 */
int backtracking(const Grid<char>& board, Grid<bool>& isMarked, const Lexicon& lex,
                  GridLocation& loc, string& curWord, HashSet<string>& countedWordList) {
    /* 判断是否需要当前路径是否合法 （不越界不走老路）
     * 判断顺序重要: 由于短路运算，避免isMarked.get(loc)越界*/
    if (!board.inBounds(loc) || isMarked.get(loc)) return 0;

    //路径合法
    int total = 0; //统计以curWord开头的单词得分
    curWord.push_back(board.get(loc)); //路径合法就加上当前的char，push_back, O(1)
    isMarked.set(loc, true); //标记当前走过的路
    if (lex.containsPrefix(curWord)) { //词典中包含以当前字符串开头的单词
        if (lex.contains(curWord)) {
            int points = getPoints(curWord); //当前字符串本身就在词典中
            if (points > 0 && !countedWordList.contains(curWord)) {
                total += points; //统计不重复的单词分数
                countedWordList.add(curWord); //添加到记分单词统计表中
            }
        }

        //遍历邻近8个位置，递归
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (i == 0 && j == 0) continue; //同时为0就在原地不动了，直接跳过
                GridLocation next(loc.row + i, loc.col + j);
                total += backtracking(board, isMarked, lex, next, curWord, countedWordList);
            }
        }
    }

    isMarked.set(loc, false); //回退状态
    curWord.pop_back(); //回退，这样可以传引用，是否比深拷贝速度块？？？？curWord传值深拷贝不需要回退状态，但是有多余的空间和复制时间消耗
    return total;
}



/* * * * * * Test Cases * * * * * */

/* Test helper function to return shared copy of Lexicon. Use to
 * avoid (expensive) re-load of word list on each test case. */
static Lexicon& sharedLexicon() {
    static Lexicon lex("res/EnglishWords.txt");
    return lex;
}

PROVIDED_TEST("Load shared Lexicon, confirm number of words") {
    Lexicon lex = sharedLexicon();
    EXPECT_EQUAL(lex.size(), 127145);
}

PROVIDED_TEST("Test point scoring") {
    EXPECT_EQUAL(getPoints("and"), 0);
    EXPECT_EQUAL(getPoints("quad"), 1);
    EXPECT_EQUAL(getPoints("quint"), 2);
    EXPECT_EQUAL(getPoints("sextet"), 3);
    EXPECT_EQUAL(getPoints("seventh"), 4);
    EXPECT_EQUAL(getPoints("supercomputer"), 10);
}

PROVIDED_TEST("Test scoreBoard, board contains no words, score of zero") {
    Grid<char> board = {{'B','C','D','F'}, //no vowels, no words
                        {'G','H','J','K'},
                        {'L','M','N','P'},
                        {'Q','R','S','T'}};
    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 0);
}

PROVIDED_TEST("Test scoreBoard, board contains one word, score of 1") {
    Grid<char> board = {{'C','_','_','_'},
                        {'Z','_','_','_'},
                        {'_','A','_','_'},
                        {'_','_','R','_'}};
    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 1);
}

PROVIDED_TEST("Test scoreBoard, alternate paths for same word, still score of 1") {
    Grid<char> board = {{'C','C','_','_'},
                        {'C','Z','C','_'},
                        {'_','A','_','_'},
                        {'R','_','R','_'}};//CCAR两次仅算作一次，相同单词不重复记分
    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 1);
}

PROVIDED_TEST("Test scoreBoard, small number of words in corner of board") {
    Grid<char> board = {{'L','I','_','_'},
                        {'M','E','_','_'},
                        {'_','S','_','_'},
                        {'_','_','_','_'}};
    Set<string> words = {"SMILE", "LIMES", "MILES", "MILE", "MIES", "LIME", "LIES", "ELMS", "SEMI"};

    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()),  2 + 2 + 2 + 1 + 1 + 1 + 1 + 1 + 1);
}

PROVIDED_TEST("Test scoreBoard, full board, small number of words") {
    Grid<char> board = {{'E','Z','R','R'},
                        {'O','H','I','O'},
                        {'N','J','I','H'},
                        {'Y','A','H','O'}};
    Set<string> words = { "HORIZON", "OHIA", "ORZO", "JOHN", "HAJI"};

    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 4 + 1 + 1 + 1 + 1);
}

PROVIDED_TEST("Test scoreBoard, full board, medium number of words") {
    Grid<char> board = {{'O','T','H','X'},
                        {'T','H','T','P'},
                        {'S','S','F','E'},
                        {'N','A','L','T'}};

    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 76);
}

PROVIDED_TEST("Test scoreBoard, full board, large number of words") {
    Grid<char> board = {{'E','A','A','R'},
                        {'L','V','T','S'},
                        {'R','A','A','N'},
                        {'O','I','S','E'}};

    EXPECT_EQUAL(scoreBoard(board, sharedLexicon()), 234);
}
