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


//ö��
enum direction { stop, left, right, up, down };
enum direction dir;// to do

int x, y, fruitx, fruity, score;
//����汾
int tailx[100], taily[100];//��������
int taildir[100];
int length = 20;
//��������
int gameover;

IMAGE dhead, uhead, lhead, rhead;//��ͷ ͼƬ���͵ı���
IMAGE body;
IMAGE ur, ul, dr, dl;
IMAGE apple;
IMAGE back;
IMAGE start;

//������Դ����
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
	dir = stop;//ö�ٱ�������Ϊֹͣ

	x = width / 2;
	y = height / 2;
	srand((unsigned)time(0));//����time���������洢������ֻ���ص�ǰʱ��ֵ

	//rand��srand
	fruitx = rand() % width;
	fruity = rand() % height;
	score = 0;
	length = 0;
}
void _putimagePNG(int  picture_x, int picture_y, IMAGE* picture) //xΪ����ͼƬ��X���꣬yΪY����
{
	DWORD* dst = GetImageBuffer();    // GetImageBuffer()���������ڻ�ȡ��ͼ�豸���Դ�ָ�룬EASYX�Դ�
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(picture); //��ȡpicture���Դ�ָ��
	int picture_width = picture->getwidth(); //��ȡpicture�Ŀ�ȣ�EASYX�Դ�
	int picture_height = picture->getheight(); //��ȡpicture�ĸ߶ȣ�EASYX�Դ�
	int graphWidth = getwidth();       //��ȡ��ͼ���Ŀ�ȣ�EASYX�Դ�
	int graphHeight = getheight();     //��ȡ��ͼ���ĸ߶ȣ�EASYX�Դ�
	int dstX = 0;    //���Դ������صĽǱ�

	// ʵ��͸����ͼ ��ʽ�� Cp=��p*FP+(1-��p)*BP �� ��Ҷ˹���������е���ɫ�ĸ��ʼ���
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //���Դ������صĽǱ�
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA��͸����
			int sr = ((src[srcX] & 0xff0000) >> 16); //��ȡRGB���R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //���Դ������صĽǱ�
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

// ������ y <0 �Լ�x<0���κ����
void startPage();
void gamePage();
int getHighScore();
void saveHighScore(int score);

void startPage() 
{
	putimage(0, 0, &start);
	while (true) 
	{
		// ��ȡ��ʷ��߷�
		BeginBatchDraw();
		
		int highScore = getHighScore();

		// ���Ʊ���
		settextstyle(30, 0, "Consolas");
		outtextxy(300, 100, "̰���� 3.0");

		// ������ʷ��߷�
		char scoreText[50];
		sprintf(scoreText, "��ʷ��߷�: %d", highScore);
		outtextxy(300, 200, scoreText);
		putimage(0, 0, &start);

		// ���ư�ť
		setfillcolor(LIGHTBLUE);
		solidrectangle(300, 300, 500, 360);  // ����ʼ��Ϸ����ť
		setfillcolor(LIGHTRED);
		solidrectangle(300, 400, 500, 460);  // ���˳���Ϸ����ť

		settextstyle(20, 0, "Consolas");
		setbkmode(TRANSPARENT);
		outtextxy(350, 320, "��ʼ��Ϸ");
		outtextxy(350, 420, "�˳���Ϸ");
		

		// ��������
		if (MouseHit()) {
			MOUSEMSG msg = GetMouseMsg();

			// �ж����������ʼ��Ϸ����ť
			if (msg.uMsg == WM_LBUTTONDOWN &&
				msg.x >= 300 && msg.x <= 500 &&
				msg.y >= 300 && msg.y <= 360) {
				gamePage();  // ������Ϸҳ��
			}

			// �ж���������˳���Ϸ����ť
			if (msg.uMsg == WM_LBUTTONDOWN &&
				msg.x >= 300 && msg.x <= 500 &&
				msg.y >= 400 && msg.y <= 460) {
				exit(0);  // �˳�����
			}
		}
		FlushBatchDraw();
	}
}


// ��ȡ��ʷ��߷�
int getHighScore() 
{
	FILE* file = fopen(HIGH_SCORE_FILE, "r");
	int highScore = 0;

	if (file) {
		// ��� fscanf �ķ���ֵ
		if (fscanf(file, "%d", &highScore) != 1) {
			// �����ȡʧ�ܣ�����Ĭ��ֵ
			highScore = 0;
		}
		fclose(file);
	}
	else {
		// �ļ���ʧ��ʱ��Ҳ����Ĭ��ֵ
		highScore = 0;
	}

	return highScore;
}

// ������ʷ��߷�
void saveHighScore(int score) 
{
	int highScore = getHighScore();

	// �����ǰ����������ʷ��߷֣������
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


void draw()//����IMAGE���͵�ָ��
{
	BeginBatchDraw();
	cleardevice();//�����Ļ

	//������
	putimage(0, 0, &back);

	//���Ʊ߿�
	setlinecolor(WHITE);
	for (int i = 0; i <= width; i++)//widthѭ����
	{	//��������
		line(i * grid_size, 0, i * grid_size, height * grid_size);//1.easyx����ԭ��λ�����Ͻ� 2.line��������������������� 3.height * grid_size,�������ظ�
	}
	for (int j = 0; j <= height; j++)
	{
		line(0, j * grid_size, width * grid_size, j * grid_size);
	}
	//����ͷ 1.setfill����ѡ�������ɫ 2.fillrectangle�����Σ���������һ������һ������
	setfillcolor(GREEN);
	switch (dir)
	{
	case left:
		putimagePNG(x * grid_size + 3, y * grid_size + 1, &lhead);//ǰ�������������Ͻǵ�����
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

	//������  //�����һ��
	if (((taildir[0] == left && dir == left) || (taildir[0] == right && dir == right)) && length > 0)
		putimagePNG(tailx[0] * grid_size+3, taily[0] * grid_size+3 , &body);

	if (((taildir[0] == up && dir == up) || (taildir[0] == down && dir == down)) && length > 0)
		putimagePNG(tailx[0] * grid_size+3, taily[0] * grid_size+3, &body);

	for (int i = 1; i < length; i++)
	{
		putimagePNG(tailx[i] * grid_size +3, taily[i] * grid_size+3, &body);
	}
	//��ˮ��
	setfillcolor(RED);
	putimagePNG(fruitx * grid_size + 2, fruity * grid_size + 2 , &apple);
	//fillrectangle(fruitx * grid_size, fruity * grid_size, (fruitx + 1) * grid_size, (fruity + 1) * grid_size);

	//������
	char text[20];
	settextcolor(WHITE);
	settextstyle(20, 0, _T("����"));//��������Ĳ�����������Щ��Ϊʲô������ô����
	sprintf_s(text, "Score:%d", score);//�������
	outtextxy(10, height * grid_size + 10, text);//outtextxy���ı���һ���������꣬һ�������꣬����_T�����ַ�������Ӧunicode
	SetWorkingImage(NULL);//ΪʲôҪ��ΪNULL��
	FlushBatchDraw();
}

//��ȡ���뺯��
void input()
{
	if (GetAsyncKeyState('A') && 0x8000)//��ȡ�첽״̬ 0x8000��ʾ��λ״̬������������룻0x0001��ʾ��λ״̬����ʾ���������
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

//���ĵ���Ϸ����
void key()
{
	//��λ�ø����߼�
	int  nextx = tailx[0];
	int  nexty = taily[0];
	int  dirtmp = taildir[0];
	int  dirtmp2;

	int next2x, next2y;//���˫ָ��Ϊʲô��
	tailx[0] = x;//�������ͷ
	taily[0] = y;
	taildir[0] = dir;


	for (int i = 1; i < length; i++)//
	{
		next2x = tailx[i];
		next2y = taily[i];
		dirtmp2 = taildir[i];
		tailx[i] = nextx;//����ָ�� һ������ һ����� �Ҿ��ÿ����Ż���
		taily[i] = nexty;
		taildir[i] = dirtmp;
		nextx = next2x;
		nexty = next2y;
		dirtmp = dirtmp2;
	}

	//��Ӧ���룬�仯��ͷ
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

	//��ײ���
	//ײǽ���
	if (x < 0 || y < 0 || x >= width || y >= height)//�߽������ж� width�Ⱥ��ж� 0 ���� ��
	{
		gameover = 1;
	}
	//ײ������
	for (int i = 0; i < length; i++)
	{
		if (x == tailx[i] && y == taily[i])
		{
			gameover = 1;
		}
	}
	//��ˮ�����
	if (x == fruitx && y == fruity)
	{
		score += 10;
		fruitx = rand() % width;
		fruity = rand() % height;
		length++;
		tailx[length] = tailx[length - 1];//���²����ĵ�����ֵ
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

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)//���ͼ�δ���ʲô��˼�������
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
































