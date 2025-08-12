//�ô���ʵ����һ���򵥵���������Ϸ�Ļ������ܣ����������ʼ����������ơ��ƶ���������ƥ���⡢�������������ɵȹ��ܡ�

#include <stdio.h>
#include <graphics.h>  //EasyXͼ�οⴴ�����ڣ�����/��ʾͼƬ
#include <time.h>
#include <math.h>
#include <conio.h> 

//��־
//1.������ʼ����        
//3.���Ƴ�ʼ�ķ�������   
//4.ʵ�ַ����ƶ�        
//5.ʵ�ַ���Ļ�ԭ      
//9.����ƥ�����        
//10.ʵ�ַ��������     
//11.ʵ�ַ���ĵ���
//12.ʵ�ַ������������
//13.�Ʒ�ϵͳ 

#define WIN_WIDTH         485  //���ڿ�
#define WIN_HEIGHT        917  // ���ڸ�
#define ROWS              8
#define COLS              8
#define BLOCK_SIZE_COUNT  7


// ���������ռλ�ߴ�
#define MATRIX_WIDTH      (COLS * BLOCK_SIZE + (COLS-1)*GAP)
#define MATRIX_HEIGHT     (ROWS * BLOCK_SIZE + (ROWS-1)*GAP)

// ����ƫ���������Ĳ����߼���
#define OFFSET_X          ((WIN_WIDTH - MATRIX_WIDTH) / 2)
#define OFFSET_Y          ((WIN_HEIGHT - MATRIX_HEIGHT) / 2)

IMAGE imgBg; //����
IMAGE imgBlocks[BLOCK_SIZE_COUNT];

struct block {
	int type; //��������
	int x, y;
	int row, col; //����
	int match; // ƥ�����
	int tmd;
};

struct block map[ROWS + 2][COLS + 2];

const int off_x = 17;
const int off_y = 274;
const int block_size = 52;

int click; //����λ�õĵ�������
int posX1, posY1; //��һ�ε������к���
int posX2, posY2; //�ڶ��ε������к���

bool isMoving;  // ��ǰ�Ƿ����ƶ�
bool isSwap;    //�����������ڷ��������Ϊtrue

int score;     //����
int clickCount = 30; // ��ʼ�������

void init() {
	//������Ϸ����  
	initgraph(WIN_WIDTH, WIN_HEIGHT, 1);
  
	loadimage(&imgBg, "picture/bj2.png", WIN_WIDTH, WIN_HEIGHT);

	char name[64];
	for (int i = 0; i < 7; i++) {
		sprintf_s(name, sizeof(name), "picture/%d.png", i + 1);
		loadimage(&imgBlocks[i], name, block_size, block_size, true);
	}

	//������������������  
	srand(time(NULL));

	//��ʼ����������  
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
	clickCount = 30; // ��ʼ���������
}

//���´���
void updateWindow() {
	BeginBatchDraw(); //��ʼ˫����
	putimage(0, 0, &imgBg);

	for (int i = 1; i <= ROWS; i++) {
		for (int j = 1; j <= COLS; j++) {
			if (map[i][j].type) {
				IMAGE* img = &imgBlocks[map[i][j].type - 1];
				putimage(map[i][j].x, map[i][j].y, img);
			}
		}
	}
	// ��ʾʣ��������
	setbkmode(TRANSPARENT); // ���ñ���͸��
	settextcolor(RGB(255, 255, 0)); // ��ɫ����
	settextstyle(48, 0, "����", 0, 0, FW_BOLD, false, false, false); // 48���֣��Ӵ�
	char buf[16];
	sprintf_s(buf, "%d", clickCount);
	outtextxy(410, 90, buf);  // ����

	EndBatchDraw(); //����˫����
}

//������������
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
	//��ȡ���������
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
				if (clickCount > 0) { // ֻ��ʣ�ಽ������0��������
					exchange(posY1, posX1, posY2, posX2);
					clickCount--; // ������1
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
	//�����ƶ�
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
	//�����ƶ��󣬶��ҵ����ѽ�����
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

//���ƥ������Դﵽ������Ŀ�ģ��˴�ʹ��͸��������
void check() {
	// ������
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
	// ������
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
	//����
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
	//�½�
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
	//�����µķ��飬�����н��䴦��
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
	//��ʼ��

	updateWindow();//���´���

	while (1) {
		userClick(); //����������
		check();    //ƥ��������
		move();
		if (!isMoving) xiaochu();
		huanYuan(); //��ԭ
		updateWindow();
		if (!isMoving)updateGame(); //����
		Sleep(10); //֡�ȴ�
	}



	system("pause");

	return 0;
}