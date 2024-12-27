#define _CRT_SECURE_NO_WARNINGS
#include <easyx.h>
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <graphics.h>
#include <windows.h>

#define width 30
#define height 18
#define grid_size 40
#define HIGH_SCORE_FILE "highscore.txt"


//枚举
enum direction { stop, left, right, up, down };
enum direction dir;// to do

int x, y, fruitx, fruity, score;
//数组版本
int tailx[100], taily[100];//蛇身数组
int taildir[100];
int length = 20;
//结束变量
int gameover;

IMAGE dhead, uhead, lhead, rhead;//蛇头 图片类型的变量
IMAGE body;
IMAGE ur, ul, dr, dl;
IMAGE apple;
IMAGE back;
IMAGE start;

//加载资源函数
void load()
{
	loadimage(&dhead, "images/dhead.png");
	loadimage(&uhead, "images/uhead.png");
	loadimage(&lhead, "images/lhead.png");
	loadimage(&rhead, "images/rhead.png");
	loadimage(&body, "images/body.png");
	loadimage(&ul, "images/ul.png");
	loadimage(&ur, "images/ur.png");
	loadimage(&dr, "images/dr.png");
	loadimage(&dl, "images/dl.png");
	loadimage(&apple, "images/apple.png");
	loadimage(&back, "images/back.jpg");
	loadimage(&start, "images/start.png");
	
}
void setup()
{
	gameover = 0;
	dir = stop;//枚举变量先设为停止

	x = width / 2;
	y = height / 2;
	srand((unsigned)time(0));//调用time函数，不存储东西，只返回当前时间值

	//rand与srand
	fruitx = rand() % width;
	fruity = rand() % height;
	score = 0;
	length = 0;
}
void _putimagePNG(int  picture_x, int picture_y, IMAGE* picture) //x为载入图片的X坐标，y为Y坐标
{
	DWORD* dst = GetImageBuffer();    // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(picture); //获取picture的显存指针
	int picture_width = picture->getwidth(); //获取picture的宽度，EASYX自带
	int picture_height = picture->getheight(); //获取picture的高度，EASYX自带
	int graphWidth = getwidth();       //获取绘图区的宽度，EASYX自带
	int graphHeight = getheight();     //获取绘图区的高度，EASYX自带
	int dstX = 0;    //在显存里像素的角标

	// 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //在显存里像素的角标
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA是透明度
			int sr = ((src[srcX] & 0xff0000) >> 16); //获取RGB里的R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //在显存里像素的角标
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)
					| (sb * sa / 255 + db * (255 - sa) / 255);
			}
		}
	}
}

// 适用于 y <0 以及x<0的任何情况
void startPage();
void gamePage();
int getHighScore();
void saveHighScore(int score);

void startPage() 
{
	putimage(0, 0, &start);
	while (true) 
	{
		// 获取历史最高分
		BeginBatchDraw();
		
		int highScore = getHighScore();

		// 绘制标题
		settextstyle(30, 0, "Consolas");
		outtextxy(300, 100, "贪吃蛇 3.0");

		// 绘制历史最高分
		char scoreText[50];
		sprintf(scoreText, "历史最高分: %d", highScore);
		outtextxy(300, 200, scoreText);
		putimage(0, 0, &start);

		// 绘制按钮
		setfillcolor(LIGHTBLUE);
		solidrectangle(300, 300, 500, 360);  // “开始游戏”按钮
		setfillcolor(LIGHTRED);
		solidrectangle(300, 400, 500, 460);  // “退出游戏”按钮

		settextstyle(20, 0, "Consolas");
		setbkmode(TRANSPARENT);
		outtextxy(350, 320, "开始游戏");
		outtextxy(350, 420, "退出游戏");
		

		// 检测鼠标点击
		if (MouseHit()) {
			MOUSEMSG msg = GetMouseMsg();

			// 判断鼠标点击“开始游戏”按钮
			if (msg.uMsg == WM_LBUTTONDOWN &&
				msg.x >= 300 && msg.x <= 500 &&
				msg.y >= 300 && msg.y <= 360) {
				gamePage();  // 进入游戏页面
			}

			// 判断鼠标点击“退出游戏”按钮
			if (msg.uMsg == WM_LBUTTONDOWN &&
				msg.x >= 300 && msg.x <= 500 &&
				msg.y >= 400 && msg.y <= 460) {
				exit(0);  // 退出程序
			}
		}
		FlushBatchDraw();
	}
}


// 获取历史最高分
int getHighScore() 
{
	FILE* file = fopen(HIGH_SCORE_FILE, "r");
	int highScore = 0;

	if (file) {
		// 检查 fscanf 的返回值
		if (fscanf(file, "%d", &highScore) != 1) {
			// 如果读取失败，设置默认值
			highScore = 0;
		}
		fclose(file);
	}
	else {
		// 文件打开失败时，也设置默认值
		highScore = 0;
	}

	return highScore;
}

// 保存历史最高分
void saveHighScore(int score) 
{
	int highScore = getHighScore();

	// 如果当前分数大于历史最高分，则更新
	if (score > highScore) {
		FILE* file = fopen(HIGH_SCORE_FILE, "w");
		if (file) {
			fprintf(file, "%d", score);
			fclose(file);
		}
	}
}
void putimagePNG(int x, int y, IMAGE* picture) {

	IMAGE imgTmp, imgTmp2, imgTmp3;
	int winWidth = getwidth();
	int winHeight = getheight();
	if (y < 0) {
		SetWorkingImage(picture);
		getimage(&imgTmp, 0, -y,
			picture->getwidth(), picture->getheight() + y);
		SetWorkingImage();
		y = 0;
		picture = &imgTmp;
	}
	else if (y >= getheight() || x >= getwidth()) {
		return;
	}
	else if (y + picture->getheight() > winHeight) {
		SetWorkingImage(picture);
		getimage(&imgTmp, x, y, picture->getwidth(), winHeight - y);
		SetWorkingImage();
		picture = &imgTmp;
	}

	if (x < 0) {
		SetWorkingImage(picture);
		getimage(&imgTmp2, -x, 0, picture->getwidth() + x, picture->getheight());
		SetWorkingImage();
		x = 0;
		picture = &imgTmp2;
	}

	if (x > winWidth - picture->getwidth()) {
		SetWorkingImage(picture);
		getimage(&imgTmp3, 0, 0, winWidth - x, picture->getheight());
		SetWorkingImage();
		picture = &imgTmp3;
	}


	_putimagePNG(x, y, picture);
}


void draw()//接受IMAGE类型的指针
{
	BeginBatchDraw();
	cleardevice();//清空屏幕

	//画背景
	putimage(0, 0, &back);

	//绘制边框
	setlinecolor(WHITE);
	for (int i = 0; i <= width; i++)//width循环？
	{	//绘制竖线
		line(i * grid_size, 0, i * grid_size, height * grid_size);//1.easyx坐标原点位于左上角 2.line函数参数是两个点的坐标 3.height * grid_size,几个像素格
	}
	for (int j = 0; j <= height; j++)
	{
		line(0, j * grid_size, width * grid_size, j * grid_size);
	}
	//画蛇头 1.setfill函数选定填充颜色 2.fillrectangle画矩形，两个参数一个左上一个右下
	setfillcolor(GREEN);
	switch (dir)
	{
	case left:
		putimagePNG(x * grid_size + 3, y * grid_size + 1, &lhead);//前两个参数是左上角的坐标
		break;
	case right:
		putimagePNG(x * grid_size + 3, y * grid_size + 1, &rhead);
		break;
	case up:
		putimagePNG(x * grid_size + 3, y * grid_size + 1, &uhead);
		break;
	case down:
		putimagePNG(x * grid_size + 3, y * grid_size + 1, &dhead);
		break;
	default:
		putimagePNG(x * grid_size + 3, y * grid_size + 1, &dhead);
		break;
	}

	//画蛇身  //蛇身第一节
	if (((taildir[0] == left && dir == left) || (taildir[0] == right && dir == right)) && length > 0)
		putimagePNG(tailx[0] * grid_size+3, taily[0] * grid_size+3 , &body);

	if (((taildir[0] == up && dir == up) || (taildir[0] == down && dir == down)) && length > 0)
		putimagePNG(tailx[0] * grid_size+3, taily[0] * grid_size+3, &body);

	for (int i = 1; i < length; i++)
	{
		putimagePNG(tailx[i] * grid_size +3, taily[i] * grid_size+3, &body);
	}
	//画水果
	setfillcolor(RED);
	putimagePNG(fruitx * grid_size + 2, fruity * grid_size + 2 , &apple);
	//fillrectangle(fruitx * grid_size, fruity * grid_size, (fruitx + 1) * grid_size, (fruity + 1) * grid_size);

	//画分数
	char text[20];
	settextcolor(WHITE);
	settextstyle(20, 0, _T("宋体"));//这个函数的参数变量有哪些？为什么自提这么设置
	sprintf_s(text, "Score:%d", score);//存入分数
	outtextxy(10, height * grid_size + 10, text);//outtextxy画文本，一个起点横坐标，一个纵坐标，还有_T包裹字符串来适应unicode
	SetWorkingImage(NULL);//为什么要设为NULL？
	FlushBatchDraw();
}

//获取输入函数
void input()
{
	if (GetAsyncKeyState('A') && 0x8000)//获取异步状态 0x8000表示高位状态，检测有无输入；0x0001表示低位状态，表示有无输入过
	{
		dir = left;
	}
	if (GetAsyncKeyState('D') && 0x8000)
	{
		dir = right;
	}
	if (GetAsyncKeyState('W') && 0x8000)
	{
		dir = up;
	}
	if (GetAsyncKeyState('S') && 0x8000)
	{
		dir = down;
	}
	if (GetAsyncKeyState('P') && 0x8000)
	{
		gameover = 1;
	}

}

//核心的游戏函数
void key()
{
	//蛇位置更新逻辑
	int  nextx = tailx[0];
	int  nexty = taily[0];
	int  dirtmp = taildir[0];
	int  dirtmp2;

	int next2x, next2y;//这个双指针为什么？
	tailx[0] = x;//蛇身变蛇头
	taily[0] = y;
	taildir[0] = dir;


	for (int i = 1; i < length; i++)//
	{
		next2x = tailx[i];
		next2y = taily[i];
		dirtmp2 = taildir[i];
		tailx[i] = nextx;//两个指针 一个存入 一个存出 我觉得可以优化？
		taily[i] = nexty;
		taildir[i] = dirtmp;
		nextx = next2x;
		nexty = next2y;
		dirtmp = dirtmp2;
	}

	//响应输入，变化蛇头
	switch (dir)
	{
	case left:
		x--;
		break;
	case right:
		x++;
		break;
	case up:
		y--;
		break;
	case down:
		y++;
		break;
	}

	//碰撞检测
	//撞墙检测
	if (x < 0 || y < 0 || x >= width || y >= height)//边界条件判断 width等号判断 0 数组 ？
	{
		gameover = 1;
	}
	//撞蛇身检测
	for (int i = 0; i < length; i++)
	{
		if (x == tailx[i] && y == taily[i])
		{
			gameover = 1;
		}
	}
	//吃水果检测
	if (x == fruitx && y == fruity)
	{
		score += 10;
		fruitx = rand() % width;
		fruity = rand() % height;
		length++;
		tailx[length] = tailx[length - 1];//给新产生的的蛇身赋值
		taily[length] = taily[length - 1];
		taildir[length] = taildir[length - 1];
	}

}
void gamePage() {
	initgraph(width * grid_size, height * grid_size + 40);//initial graph
	load();
	setup();


	while (!gameover)
	{
		input();
		key();
		draw();
		Sleep(64);
	}
	saveHighScore(score);
	EndBatchDraw();
	closegraph();
	exit(0);
	
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)//这个图形窗口什么意思，句柄？
{
	initgraph(width * grid_size, height * grid_size + 40);//initial graph
	startPage();
	load();
	setup();
	

	//while (!gameover)
	//{
		//input();
		//key();
		//draw();
		//Sleep(64);
	//}
	//closegraph();
	//EndBatchDraw();
	return 0;

}
































