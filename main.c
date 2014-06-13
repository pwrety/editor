/*------------------------------------------------------------------------
	Simple line editor
	When it begins, starts with the command mode.
	In command mode, if you press the key 'i', it change to the input mode.
	In input mode, it is impossible to insert and delete the other charact
	ers between character string, because this editor is not implement by 
	using list. 
-------------------------------------------------------------------------*/
/*------------------------------------------------------------------------
	At command mode
		'd' key : cursor moves right
		'a' key : cursor moves left
		'w' key : cursor moves up
		's' key: cursor moves down
		'Enter' key : cursor moves next line
		'f' key: save the file named "EDITOR"
		'i' key: change into the input mode
		'q' key: quit editor
	
	At input mode
		'Tap' key : erase the character as a 'Backspace' key
		'Enter' key : cursor moves next line 
		'Esc' key : change into the command mode
-----------------------------------------------------------------------*/

#include <termios.h>
#include <unistd.h>
#include <stdio.h>

#define ESC 27
#define TAB 9

struct termios oldt, curt, newt;

char buff[255];

char data[23][80];
int cur_line = 1;
int cur_col = 1;
const int min_line = 1;
const int min_col = 1;
const int max_line = 23;
const int max_col = 80;

enum MODE
{
	Command_Mode,
	Input_Mode
};

int getch(void)
{
	int ch;
  	struct termios buf;
 	struct termios save;

  	tcgetattr(STDIN_FILENO, &save);
   	buf = save;
   	buf.c_cc[VMIN] = 1;
   	buf.c_cc[VTIME] = 0;
   	buf.c_iflag |= ECHO;
   	tcsetattr(STDIN_FILENO, TCSAFLUSH, &buf);
   	ch = getchar();
   	tcsetattr(STDIN_FILENO, TCSAFLUSH, &save);
   	return ch;
}

void echo_off(void)
{
	tcgetattr(STDIN_FILENO, &oldt); //save terminal
	newt = oldt; 
    newt.c_lflag &= ~(ICANON | ECHO); //mode off
	tcsetattr(STDIN_FILENO, TCSANOW, &newt); //echo off
}

void echo_on(void) //print out the characters on screen
{		
	tcgetattr (STDIN_FILENO, &curt);
	newt = curt;
	newt.c_lflag |= (ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}	

close_Editor(void)
{
	tcsetattr (STDIN_FILENO, TCSANOW, &oldt);
}

void save_file(void)
{
	FILE *fp;
	fp = fopen("EDITOR.txt", "w");
	
	int i;
	int j;

	if(fp == NULL){
		printf("File Open Error\n");
		fclose(fp);
	}
	else{
 		for(i=0; i<=23; i++){
 			for(j=0; j<=80; j++)
				fprintf(fp, "%c", data[i][j]);
   		}
		if(fclose(fp) == 0 && fp){
   			sprintf(buff, "\033[%d;%dHSuccessfully Closed the File", 24, 1, cur_line, cur_col);
   			fputs(buff, stdout);
    	}	
	}
}
int main(int argc, char *argv[])
{	
	enum MODE mode = Command_Mode; //starts the command mode
	
	int loop = 1;
	int key;
	int ret;

	//initialize
	fputs("\033[2J", stdout);
	fputs("\033[1;1H", stdout); 

	echo_off();

	while(loop)
	{
		key = getch();	
		
		ret =feof(stdin);
		if(ret != 0){
			printf("EOF\n");
			return 0;
		}
		
		ret = ferror(stdin);
		if (ret != 0){
			printf ("ERROR getchar() by %d\n", ret);
			return 0;
   		}

		if(mode == Command_Mode)
		{
			sprintf(buff, "\033[%d;%dHCommand Mode                ", 24, 1, cur_line, cur_col);
			fputs(buff, stdout);

			switch(key)
			{							
				case 'd': //cursor move right
					cur_col++;
                    if(cur_col > max_col){            
		 	        	cur_col = 1;
                        cur_line++;
  	                    if(cur_line > max_line){
                        	cur_line = max_line;
							cur_col = max_col;
						}
                    }
                    break;
				case 'a': //cursor move left
					cur_col--;
					if(cur_col < min_col){
						cur_col = 80;	
						cur_line--;
						if(cur_line < min_line){
							cur_line = min_line;
							cur_col = min_col;
						}
					}
					break;
				case 's': //cursor move down
					cur_line++;
					if(cur_line > max_line)
						cur_line = max_line;
					break;
				case 'w': //cursor move up
					cur_line--;
					if(cur_line < min_line)
						cur_line = min_line;
					break;
				case 'i': //change to the input mode
					mode = Input_Mode;
					echo_on();
					sprintf(buff, "\033[%d;%dHInput Mode                  ", 24, 1, cur_line, cur_col);
					fputs(buff, stdout);
					break;
				case '\n': //enter key
					cur_line++;
					cur_col = min_col;
					if(cur_line > max_line)
						cur_line = max_line;
					break;
				case 'f': //save file
					save_file();
					break;
				/*case 'o':
					fp = fopen("EDITOR.txt", "r");
					while(!feof(fp)){
						file_data = fgetc(fp);
						printf("%c", file_data);
					}
					break;*/
				case 'q': //quit editor
					cur_col = min_col;
					loop = 0;
					break;
			}
		}	
		else if(mode == Input_Mode)
		{
			switch(key)
			{
				case ESC: //change to the command mode
					echo_off();
					mode = Command_Mode;
					sprintf(buff, "\033[%d;%dHCommand Mode                ", 24, 1, cur_line, cur_col);
					fputs(buff, stdout);
					break;
				case '\n': //enter key
					cur_line++;
					cur_col = min_col;
					if(cur_line > max_line)
						cur_line = max_line;
					break;
				case TAB: //erase the character
					cur_col--;
					if(cur_col < min_col){
						cur_col = 80;
						cur_line--;
						if(cur_line < min_line){
							cur_line = min_line;
							cur_col = min_col;
						}
					}
					data[cur_line-1][cur_col-1] = sprintf(buff, "\033[%d;%dH ", cur_line, cur_col);
					fputs(buff, stdout);
					break;
				default: //store the characters in the arry
					cur_col++;
					data[cur_line-1][cur_col-1] = key;				
					if(cur_col > max_col){
						cur_col = 1;
						cur_line++;
						if (cur_line > max_line){ 
							cur_line = max_line;
							cur_col = max_col;
						}
					}
					break;
			}
		}
		
		sprintf(buff, "\033[%d;%dH%3d:%3d", 24, 73, cur_line, cur_col);
		fputs(buff, stdout);
		sprintf(buff, "\033[%d;%dH", cur_line, cur_col); 
		fputs(buff, stdout); 
	} //end while
	
	close_Editor();
	
	return 0;
}
