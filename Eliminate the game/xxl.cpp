//该代码实现了一个简单的消除类游戏的基本功能，包括界面初始化、方块绘制、移动、交换、匹配检测、消除和重新生成等功能。

#include <stdio.h>
#include <graphics.h>  //EasyX图形库创建窗口，加载/显示图片
#include <time.h>
#include <math.h>
#include <conio.h> 

//日志
//1.构建初始界面        
//3.绘制初始的方块数组   
//4.实现方块移动        
//5.实现方块的还原      
//9.计算匹配次数        
//10.实现方块的消除     
//11.实现方块的掉落
//12.实现方块的重新生成
//13.计分系统 

#define WIN_WIDTH         485  //窗口宽
#define WIN_HEIGHT        917  // 窗口高
#define ROWS              8
#define COLS              8
#define BLOCK_SIZE_COUNT  7


// 方块矩阵总占位尺寸
#define MATRIX_WIDTH      (COLS * BLOCK_SIZE + (COLS-1)*GAP)
#define MATRIX_HEIGHT     (ROWS * BLOCK_SIZE + (ROWS-1)*GAP)

// 居中偏移量（核心布局逻辑）
#define OFFSET_X          ((WIN_WIDTH - MATRIX_WIDTH) / 2)
#define OFFSET_Y          ((WIN_HEIGHT - MATRIX_HEIGHT) / 2)

IMAGE imgBg; //背景
IMAGE imgBlocks[BLOCK_SIZE_COUNT];

struct block {
	int type; //方块类型
	int x, y;
	int row, col; //行列
	int match; // 匹配次数
	int tmd;
};

struct block map[ROWS + 2][COLS + 2];

const int off_x = 17;
const int off_y = 274;
const int block_size = 52;

int click; //相邻位置的单击次数
int posX1, posY1; //第一次单击的行和列
int posX2, posY2; //第二次单击的行和列

bool isMoving;  // 当前是否在移动
bool isSwap;    //单击两个相邻方块后设置为true

int score;     //分数
int clickCount = 30; // 初始点击次数

void init() {
	//创建游戏界面  
	initgraph(WIN_WIDTH, WIN_HEIGHT, 1);
  
	loadimage(&imgBg, "picture/bj2.png", WIN_WIDTH, WIN_HEIGHT);

	char name[64];
	for (int i = 0; i < 7; i++) {
		sprintf_s(name, sizeof(name), "picture/%d.png", i + 1);
		loadimage(&imgBlocks[i], name, block_size, block_size, true);
	}

	//随机数的随机种子配置  
	srand(time(NULL));

	//初始化方块数组  
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			map[i][j].type = 1 + rand() % 4;
			map[i][j].row = i;
			map[i][j].col = j;
			map[i][j].x = off_x + (j - 1) * (block_size + 5);
			map[i][j].y = off_y + (i - 1) * (block_size + 5);
			map[i][j].match = 0;
			map[i][j].tmd = 255;
		}
	}
	
	click = 0;
	isMoving = false;
	isSwap = false;
	score = 0;
	clickCount = 30; // 初始化点击次数
}

//更新窗口
void updateWindow() {
	BeginBatchDraw(); //开始双缓冲
	putimage(0, 0, &imgBg);

	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			if (map[i][j].type) {
				IMAGE* img = &imgBlocks[map[i][j].type - 1];
				putimage(map[i][j].x, map[i][j].y, img);
			}
		}
	}
	// 显示剩余点击次数
	setbkmode(TRANSPARENT); // 设置背景透明
	settextcolor(RGB(255, 255, 0)); // 黄色字体
	settextstyle(48, 0, "黑体", 0, 0, FW_BOLD, false, false, false); // 48号字，加粗
	char buf[16];
	sprintf_s(buf, "%d", clickCount);
	outtextxy(410, 90, buf);  // 坐标

	EndBatchDraw(); //结束双缓冲
}

//交换两个方块
void exchange(int row1, int col1, int row2, int col2) {
	struct block tmp = map[row1][col1];
	map[row1][col1] = map[row2][col2];
	map[row2][col2] = tmp;

	map[row1][col1].row = row1;
	map[row1][col1].col = col1;
	map[row2][col2].row = row2;
	map[row2][col2].col = col2;
}

void userClick() {
	//获取鼠标点击坐标
	ExMessage msg;
	if (peekmessage(&msg) && msg.message == WM_LBUTTONDOWN) {
		/*
			map[i][j].x = off_x + (j - 1) * (block_size + 5);
			map[i][j].y = off_y + (i - 1) * (block_size + 5);
		*/
		if (msg.x < off_x || msg.y < off_y) return;

		int col = (msg.x - off_x) / (block_size + 5) + 1;
		int row = (msg.y - off_y) / (block_size + 5) + 1;

		if (col > COLS || row > ROWS) return;

		click++;
		if (click == 1) {
			posX1 = col;
			posY1 = row;
		}
		else if (click == 2) {
			posX2 = col;
			posY2 = row;

			if (abs(posX2 - posX1) + abs(posY2 - posY1) == 1) {
				if (clickCount > 0) { // 只有剩余步数大于0才允许交换
					exchange(posY1, posX1, posY2, posX2);
					clickCount--; // 步数减1
					click = 0;
					isSwap = true;
				}
			}
			else {
				click = 1;
				posX1 = col;
				posY1 = row;
			}
		}

	}
}

void move() {
	//方块移动
	isMoving = false;
	for (int i = ROWS; i >= 0; i--) {
		for (int j = 1; j <= COLS; j++) {
			struct block* p = &map[i][j];
			int dx, dy;

			for (int k = 0; k < 4; k++) {
				int x = off_x + (p->col - 1) * (block_size + 5);
				int y = off_y + (p->row - 1) * (block_size + 5);
				dx = p->x - x;
				dy = p->y - y;

				if (dx) p->x -= dx / abs(dx);
				if (dy) p->y -= dy / abs(dy);
			}

			if (dx || dy) isMoving = true;
		}
	}
}

void huanYuan() {
	//发生移动后，而且单向已结束。
	if (isSwap && !isMoving) {

		int count = 0;
		for (int i = 1; i <= ROWS; i++) {
			for (int j = 1; j <= COLS; j++) {
				count += map[i][j].match;
			}
		}

		if (count == 0) {
			exchange(posY1, posX1, posY2, posX2);
		}

		isSwap = false;
	}
}

//检测匹配次数以达到消除的目的，此处使用透明化操作
void check() {
	// 横向检测
	for (int i = 1; i <= ROWS; i++) {
		int count = 1;
		for (int j = 2; j <= COLS + 1; j++) {
			if (j <= COLS && map[i][j].type == map[i][j - 1].type && map[i][j].type != 0) {
				count++;
			}
			else {
				if (count >= 3) {
					for (int k = 0; k < count; k++) {
						map[i][j - 1 - k].match++;
					}
				}
				count = 1;
			}
		}
	}
	// 纵向检测
	for (int j = 1; j <= COLS; j++) {
		int count = 1;
		for (int i = 2; i <= ROWS + 1; i++) {
			if (i <= ROWS && map[i][j].type == map[i - 1][j].type && map[i][j].type != 0) {
				count++;
			}
			else {
				if (count >= 3) {
					for (int k = 0; k < count; k++) {
						map[i - 1 - k][j].match++;
					}
				}
				count = 1;
			}
		}
	}
}

void xiaochu() {
	//消除
	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			if (map[i][j].match && map[i][j].tmd > 10) {
				map[i][j].tmd -= 9;
				isMoving = true;
			}
		}
	}
}

void updateGame() {
	//下降
	for (int i = ROWS; i >= 1; i--) {
		for (int j = 1; j <= COLS; j++) {
			if (map[i][j].match) {
				for (int k = i - 1; k >= 1; k--) {
					if (map[k][j].match == 0) {
						exchange(k, j, i, j);
						break;
					}
				}
			}
		}
	}
	//生成新的方块，并进行降落处理
	for (int j = 1; j <= COLS; j++) {
		int n = 0;
		for (int i = ROWS; i >= 1; i--) {
			if (map[i][j].match) {
				map[i][j].type = 1 + rand() % 7;
				map[i][j].y = off_y - (n + 1) * (block_size + 5);
				n++;
				map[i][j].match = 0;
				map[i][j].tmd = 255;
			}
		}
		score += n;
	}
}

int main(void) {
	init();
	//初始化

	updateWindow();//更新窗口

	while (1) {
		userClick(); //处理点击操作
		check();    //匹配次数检查
		move();
		if (!isMoving) xiaochu();
		huanYuan(); //还原
		updateWindow();
		if (!isMoving)updateGame(); //降落
		Sleep(10); //帧等待
	}



	system("pause");

	return 0;
}