/* Sierpinski 分形
 * 2022/10/12 10:35PM
 */
#include <iostream>    // for cout, endl
#include "recursion.h"
#include "testing/SimpleTest.h"
using namespace std;

/*
 * This function draws a filled, black triangle on the provided GWindow
 * that is defined by the corner GPoints one, two, and three. This
 * function has been provided for you and does not need to be modified.
 */
void fillBlackTriangle(GWindow& window, GPoint one, GPoint two, GPoint three) {
    window.setColor("black");
    window.fillPolygon( {one, two, three} );
}

/* 根据低阶顶点获得高阶顶点坐标
 * @return int:利用递归过程统计绘制三角的个数
 */
int drawSierpinskiTriangle(GWindow& window, GPoint one, GPoint two, GPoint three, int order) {
    if (order == 0) { //base case
        fillBlackTriangle(window, one, two, three); //order为0直接绘制
        return 1; //绘制了一个三角返回 1
    }
    int cnt = 0;
    //根据order阶的绘图作标传递给order-1阶去绘制
    //上面的三角形
    int upNum = drawSierpinskiTriangle(window, one,
                                       GPoint((one.x+two.x)/2, (one.y+two.y)/2),
                                       GPoint((one.x+three.x)/2, (one.y+three.y)/2), order - 1);
    //左边的三角形
    int leftNum = drawSierpinskiTriangle(window,
                                         GPoint((one.x+two.x)/2, (one.y+two.y)/2),
                                         two,
                                         GPoint((two.x+three.x)/2, (two.y+three.y)/2), order - 1);
    //右边三角形
    int rightNum = drawSierpinskiTriangle(window,
                                          GPoint((one.x+three.x)/2, (one.y+three.y)/2),
                                          GPoint((two.x+three.x)/2, (two.y+three.y)/2),
                                          three, order - 1);


    return upNum + leftNum + rightNum; //返回三个order-1阶函数的返回数量的和
}


/* * * * * * Test Cases * * * * * */

/*
 * Do not modify the code beyond this line! There are no
 * unit tests for this problem. You should do all testing
 * via the interactive demo.
 */

PROVIDED_TEST("Test fractal drawing interactively using graphical demo") {
    runInteractiveDemo();
}

