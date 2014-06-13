#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <term.h>

#define KEY_ESC 27
#define KEY_UP (256 + 72)
#define KEY_DOWN (256 + 80)
#define KEY_LEFT (256 + 75)
#define KEY_RIGHT (256 + 77)
int getch(void)
{
	int ch;
	struct termios buf;
	struct termios save;

	tcgetattr(0, &save);
   buf = save;
   buf.c_lflag &= ~(ICANON|ECHO);
   buf.c_cc[VMIN] = 1;
   buf.c_cc[VTIME] = 0;
   tcsetattr(0, TCSAFLUSH, &buf);
   ch = getchar();
   tcsetattr(0, TCSAFLUSH, &save);
   return ch;
}

int GetKey(void)    // 방향키 입력을 받아들임
{
 int ch = getch();   // 일반적으로는 getch에 의해 문자가 입력되지만

 if (ch == 0 || ch == 224) // 방향키의 경우 0또는 224가 먼저 입력된 후 해당 방향키 값이 입력됨 (2자 입력)
  ch = 256 + getch();  // 원래 일반키와의 입력과의 혼동을 피하기 위해 256을 더해 줌
        // 예) 'H'와 UP키의 값이 72로 동일함
 return ch;
}

int main(void)
{
char buff[255];	
struct termios oldt, curt, newt;
int key;
	/* some initialization */
	fputs("\033[2J", stdout);
	fputs("\033[1;1H", stdout);

	tcgetattr ( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr ( STDIN_FILENO, TCSANOW, &newt );
  
int cur_line = 1;
	int cur_col = 1;
	const int min_line = 1;
	const int min_col = 1;
	const int max_line = 20;
	const int max_col = 80;
	while(1){
		key = GetKey();
   		switch (key) {
		case KEY_LEFT:
		cur_col--;
		if (cur_col < min_col) cur_col = min_col;
		break;
	case KEY_RIGHT:
		cur_line++;
		if (cur_line > max_line) cur_line = max_line;
		break;
	case KEY_UP :
		cur_line--;
		if (cur_line < min_line) cur_line = min_line;
		break;
	case KEY_DOWN :
		cur_col++;
		if (cur_col > max_col) {
			cur_col = 1;
			cur_line++;
			if (cur_line > max_line) cur_line = max_line;
		}
		break;
	}	// end switch
sprintf(buff, "\033[%d;%dH%3d:%3d", 1, 70, cur_line, cur_col);
	fputs(buff, stdout);
sprintf(buff, "\033[%d;%dH", cur_line, cur_col);
}
//tcsetattr ( STDIN_FILENO, TCSANOW, &oldt );


	return 0;
}

