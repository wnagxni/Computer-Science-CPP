#ifndef BOGGLE_H
#define BOGGLE_H

#include "grid.h"
#include "lexicon.h"
#include <string.h>
#include "hashSet.h"
using namespace std;

/*
 * 拼字游戏主函数：主逻辑是包装了遍历board每个位置去回溯以该字母为prefix的单词得分
 */
int scoreBoard(Grid<char>& board, Lexicon& lex);

int backtracking(const Grid<char>& board, Grid<bool>& isMarked, const Lexicon& lex,
                  GridLocation& loc, string& curWord, HashSet<string>& countedWordList);

/*
 * 仅按照字符串长度返回分数（和是否在字典无关）
 * 规则：长度小于4返回0，其余返回length - 3
 */
int getPoints(const string &str);

#endif // BOGGLE_H
