/*file maze.cpp
 * modified: 2022/10/3 Wang Xin
 */

#include <iostream>
#include <fstream>
#include "error.h"
#include "filelib.h"
#include "grid.h"
#include "maze.h"
#include "mazegraphics.h"
#include "queue.h"
#include "set.h"
#include "stack.h"
#include "vector.h"
#include "testing/SimpleTest.h"
using namespace std;


/**
 * @brief 寻找迷宫中当前位置的所有相邻位置
 * - 每次只能从当前位置cur移动一步，并且只能沿东西南北(East, West, Sourth, North)这4个方向进行移动
 * - 只能在迷宫所在的区域内移动，不能越界。
 * - 只能沿着通道移动，不能穿过墙壁。
 * @param maze 迷宫
 * @param cur 当前位置
 * @return 当前位置的所有相邻位置
 */
Set<GridLocation> generateValidMoves(Grid<bool>& maze, GridLocation cur) {
    Set<GridLocation> neighbors; //存放迷宫中当前位置的合法的下一步
    //Ask: 为什么结果用Set，不用array
    //moves中存放西东南北4个位置的移动模式，借用GridLocation保证x，y的组合关系，而不用两个数组x、y
    GridLocation moves[] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} }; //
    for (auto move : moves) {
        //下一步的候选
        GridLocation candidate(cur.row + move.row, cur.col + move.col);
        if (maze.inBounds(candidate) && maze.get(candidate)) //不越界且不碰壁
            neighbors.add(candidate);
    }
    return neighbors;
}

/**
 * @brief validatePath 验证path是否为走出maze的合规路径，不合规时抛出error，合规时自然结束函数
 * @param maze 迷宫
 * @param path 验证路径
 */
void validatePath(Grid<bool>& maze, Stack<GridLocation> path) {
    GridLocation cur = {maze.numRows()-1,  maze.numCols()-1};
    GridLocation mazeEntrance = {};
    Set<GridLocation> visited = {cur};
    if (path.pop() != cur) { //path.peek() --> path.pop()
        error("Path does not end at maze exit");
        //如果栈空，会自己抛出error("Stack::pop: Attempting to pop an empty stack")
    }

    while (!path.isEmpty()) {
        //此时路径不为空:
        visited.add(cur);
        GridLocation next = path.pop();
        auto nexts = generateValidMoves(maze, cur); //获得合理的下一步
        if (!nexts.contains(next))
            error("路径不合法（不连续、越界或撞墙）");
        if (visited.contains(next))
            error("存在重复路径");
        cur = next;
    }
    //此时路径为空:
    if (cur == mazeEntrance)
        return; //路径合法退出
    else
        error("路径不是从左上角开始");

}

/**
 * @brief stackContains 判断栈中是否包含元素
 * @param stack
 * @param spot
 * @return bool
 */
bool stackContains(Stack<GridLocation> stack, GridLocation& spot) {
    while (!stack.isEmpty()) {
        if (stack.pop() == spot)
            return true;
    }
    return false;
}
/**
 * @brief solveMaze BFS找到解开迷宫中的最短路径
 * @param maze 迷宫
 * @return 返回路径
 */
Stack<GridLocation> solveMaze(Grid<bool>& maze) {
    MazeGraphics::drawGrid(maze);
    Stack<GridLocation> path;
    path.push(GridLocation(0, 0)); //入口入栈
    Queue<Stack<GridLocation>> q;
    q.enqueue(path); //仅包含起点的路径入栈
    GridLocation mazeExit = {maze.numRows() - 1, maze.numCols() - 1};

    while (!q.isEmpty()) {
        path = q.dequeue();
        if (path.peek() == mazeExit) {
            MazeGraphics::highlightPath(path, "green", 500);
            return path;
        }
        auto nexts = generateValidMoves(maze, path.peek());
        for (auto next : nexts) { //对于每个可能的下一步，创建新路径入队
            if (stackContains(path, next)) continue; //如果为回头路，则跳过;
            //这里不知是否有更好避免回头路方法？感觉这样效率较低
            Stack<GridLocation> newPath(path); //deep copy
            newPath.push(next);
            q.enqueue(newPath); //新路径入队
            MazeGraphics::highlightPath(newPath, "blue", 10);
        }
    }
    return Stack<GridLocation>(); //队列空了都没找到解；如果可以走回头路的话永远不会到这儿
}

/*
 * The given readMazeFile function correctly reads a well-formed
 * maze from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readMazeFile(string filename, Grid<bool>& maze) {
    /* The following code reads data from the file into a Vector
     * of strings representing the lines of the file.
     */
    ifstream in;

    if (!openFile(in, filename))
        error("Cannot open file named " + filename);

    Vector<string> lines;
    readEntireFile(in, lines);

    /* Now that the file data has been read into the Vector, populate
     * the maze grid.
     */
    int numRows = lines.size();        // rows is count of lines
    int numCols = lines[0].length();   // cols is length of line
    maze.resize(numRows, numCols);     // resize grid dimensions

    for (int r = 0; r < numRows; r++) {
        if (lines[r].length() != numCols) {
            error("Maze row has inconsistent number of columns");
        }
        for (int c = 0; c < numCols; c++) {
            char ch = lines[r][c];
            if (ch == '@') {        // wall
                maze[r][c] = false;
            } else if (ch == '-') { // corridor
                maze[r][c] = true;
            } else {
                error("Maze location has invalid character: '" + charToString(ch) + "'");
            }
        }
    }
}

/* The given readSolutionFile correctly reads a path from a file.
 *
 * This provided function is fully correct. You do not need to change
 * any of the code in this function.
 */
void readSolutionFile(string filename, Stack<GridLocation>& soln) {
    ifstream in;

    if (!openFile(in, filename)) {
        error("Cannot open file named " + filename);
    }

    if (!(in >> soln)) {// if not successfully read
        error("Maze solution did not have the correct format.");
    }
}


/* * * * * * Test Cases * * * * * */

PROVIDED_TEST("generateValidMoves on location in the center of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation center = {1, 1};
    Set<GridLocation> expected = {{0, 1}, {1, 0}, {1, 2}, {2, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, center), expected);
}

PROVIDED_TEST("generateValidMoves on location on the side of 3x3 grid with no walls") {
    Grid<bool> maze = {{true, true, true},
                       {true, true, true},
                       {true, true, true}};
    GridLocation side = {0, 1};
    Set<GridLocation> expected = {{0,0}, {0,2}, {1, 1}};

    EXPECT_EQUAL(generateValidMoves(maze, side), expected);
}

PROVIDED_TEST("generateValidMoves on corner of 2x2 grid with walls") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    GridLocation corner = {0, 0};
    Set<GridLocation> expected = {{1, 0}};

    EXPECT_EQUAL(generateValidMoves(maze, corner), expected);
}

PROVIDED_TEST("validatePath on correct solution") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Stack<GridLocation> soln = { {0 ,0}, {1, 0}, {1, 1} };

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on correct solution loaded from file for medium maze") {
    Grid<bool> maze;
    Stack<GridLocation> soln;
    readMazeFile("res/5x7.maze", maze);
    readSolutionFile("res/5x7.soln", soln);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on correct solution loaded from file for large maze") {
    Grid<bool> maze;
    Stack<GridLocation> soln;
    readMazeFile("res/25x33.maze", maze);
    readSolutionFile("res/25x33.soln", soln);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("validatePath on invalid path should raise error") {
    Grid<bool> maze = {{true, false},
                       {true, true}};
    Stack<GridLocation> not_end_at_exit = { {1, 0}, {0, 0} };
    Stack<GridLocation> not_begin_at_entry = { {1, 0}, {1, 1} };
    Stack<GridLocation> go_through_wall = { {0 ,0}, {0, 1}, {1, 1} };
    Stack<GridLocation> teleport = { {0 ,0}, {1, 1} };
    Stack<GridLocation> revisit = { {0 ,0}, {1, 0}, {0, 0}, {1, 0}, {1, 1} };

    EXPECT_ERROR(validatePath(maze, not_end_at_exit));
    EXPECT_ERROR(validatePath(maze, not_begin_at_entry));
    EXPECT_ERROR(validatePath(maze, go_through_wall));
    EXPECT_ERROR(validatePath(maze, teleport));
    EXPECT_ERROR(validatePath(maze, revisit));
}

PROVIDED_TEST("solveMaze on file 5x7") {
    Grid<bool> maze;
    readMazeFile("res/5x7.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

PROVIDED_TEST("solveMaze on file 21x23") {
    Grid<bool> maze;
    readMazeFile("res/21x23.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}

// TODO: add your test cases here
STUDENT_TEST("generateValidMoves on location on the side of 3x3 grid with all walls") {
    Grid<bool> maze = {{false, false, false},
                       {false, false, false},
                       {false, false, false}};
    GridLocation side = {0, 1};
    Set<GridLocation> expected = {};

    EXPECT_EQUAL(generateValidMoves(maze, side), expected);
}

STUDENT_TEST("generateValidMoves on location on the side of 3x3 grid") {
    Grid<bool> maze = {{true, false, false},
                       {false, true, true},
                       {false, false, false}};
    GridLocation side = {0, 1};
    Set<GridLocation> expected = { {0, 0}, {1, 1} };

    EXPECT_EQUAL(generateValidMoves(maze, side), expected);
}

STUDENT_TEST("generateValidMoves on location on the side of 3x3 grid") {
    Grid<bool> maze = {{true, false, false},
                       {false, true, true},
                       {false, false, false}};
    GridLocation side = {2, 2};
    Set<GridLocation> expected = { {1, 2} };

    EXPECT_EQUAL(generateValidMoves(maze, side), expected);
}

STUDENT_TEST("validatePath on invalid path should raise error") {
    Grid<bool> maze = {{true, false, false},
                       {true, true, true},
                       {false, false, true}};
    Stack<GridLocation> right_path = { {0, 0}, {1, 0}, {1, 1}, {1, 2}, {2, 2} };
    Stack<GridLocation> not_begin_at_entry = { {1, 1}, {1, 0}, {1, 1}, {1, 2}, {2, 2} };
    Stack<GridLocation> go_through_wall = { {0, 0}, {0, 1}, {1, 1}, {1, 2}, {2, 2} };
    Stack<GridLocation> teleport = { {0 ,0}, {1, 1} };
    Stack<GridLocation> revisit = { {0 ,0}, {1, 0}, {0, 0}, {1, 0}, {1, 1} };

    EXPECT_NO_ERROR(validatePath(maze, right_path));
    EXPECT_ERROR(validatePath(maze, not_begin_at_entry));
    EXPECT_ERROR(validatePath(maze, go_through_wall));
    EXPECT_ERROR(validatePath(maze, teleport));
    EXPECT_ERROR(validatePath(maze, revisit));
}

STUDENT_TEST("solveMaze on file 33x41") {
    Grid<bool> maze;
    readMazeFile("res/33x41.maze", maze);
    Stack<GridLocation> soln = solveMaze(maze);

    EXPECT_NO_ERROR(validatePath(maze, soln));
}


