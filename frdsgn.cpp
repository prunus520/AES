#include "frdsgn.h" 

bool keyFlag = 0;


/********************  �w�q�禡  ********************/ 


//  �]�w�����D
void setWindows(const char* title, int W, int H, int X, int Y){
	SetConsoleTitle(title);  //  �����W�� 
	setConsoleSize(W, H);  //  �����j�p 
	SetWindowPos(GetConsoleWindow(), 0, X, Y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);  //  ������m 
	setTextColor(0x0F);  //  �r��I���C�� 
	setCursorInfo(100, 0);  //  ����ܥ��� 
	if(GetFileAttributesA("Encrypt") != FILE_ATTRIBUTE_DIRECTORY){  //  �P�_�O�_����Ƨ� 
		_mkdir("Encrypt");  //  �إ߸�Ƨ� 
	}
	if(GetFileAttributesA("Decrypt") != FILE_ATTRIBUTE_DIRECTORY){  //  �P�_�O�_����Ƨ� 
		_mkdir("Decrypt");  //  �إ߸�Ƨ� 
	}
	if(GetFileAttributesA("Done") != FILE_ATTRIBUTE_DIRECTORY){  //  �P�_�O�_����Ƨ� 
		_mkdir("Done");  //  �إ߸�Ƨ� 
	}
}

//  �]�w�����j�p 
void setConsoleSize(int W, int H){
    SMALL_RECT SR = {0, 0, W, H};
    COORD Sz = {W + 1, H + 1};  //  ���úu�ʱ� ( + 1 ) 
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE),true, &SR);  //  �����j�p 
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), Sz);  //  �����w�s�j�p 
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE),true, &SR);  //  ��������j�p�]�� 
}

//  �]�w���Ц�m 
void setCursorXY(int x, int y){
    COORD cursorPosition;
    cursorPosition.X = x;
    cursorPosition.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);  //  �]�w���Ц�m�A���W��0, 0 
}

COORD getCursorXY(){
    CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    COORD cursorPosition = {csbi.dwCursorPosition.X, csbi.dwCursorPosition.Y};
	return cursorPosition;
}

//  �]�w���Ыp�׻P�i���� 
void setCursorInfo(int dwSize, int bVisible){
	CONSOLE_CURSOR_INFO curInfo = {dwSize, bVisible};
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);  //  ���Ыp�׻P�i���� 
}

//  �]�w�r���C�� 
void setTextColor(int color){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);  //  �]�w�r���C�� 
}

//  ���j�u 
void textHR(int sum, bool blank){
	if(blank){
		printf("   +");
	}else{
		printf("+");
	}
	for(int i = 0; i < sum; i++){
		printf("-");
	}
	printf("+\n");
}

//  �r��m���P����r�������C�� 
void textCenter(const char* text, int color, int buffer, bool blank){
	buffer -= strlen(text);  //  �ݸɥR�h�֭Ӫť� 
	if(blank){
		printf("   |");
	}else{
		printf("|");
	}
	for(int i = 0; i < buffer; i++){
		if(i == buffer / 2){  //  �P�_��r�L����m 
			setTextColor(color);
			printf("%s", text);
			setTextColor(0x0F);
		}else{
			printf(" ");
		}
	}
	printf("|\n");
}

//  �榡�ƪ��r��m��
void textCenterf(const char* text, ...){
	va_list arg;
	int len;
	int buffer = 71;
	char btext[70];
	
	// ���p���r�@�@���h�� 
	va_start(arg, text);
	len = vsprintf(btext, text, arg);
	va_end(arg);
	setCursorXY(0, getCursorXY().Y);  //  ���нզ^�쥻�ӦL����m 
	printf("   |");
	if(len < buffer){
		buffer -= len;  //  �ݸɥR�h�֭Ӫť� 
		for(int i = 0; i < buffer; i++){
			if(i == buffer / 2){  //  �P�_��r�L����m
				//  �A���s�L�@�� 
				va_start(arg, text);
				vprintf(text, arg);
				va_end(arg);
			}else{
				printf(" ");
			}
		}
	}else{
		btext[70] = 0x00;
		printf("%s", btext);
	}
	printf("|\n");
}

//  �榡�ƪ��r��m���P����r�������C�� 
void textCenterf(int color, const char* text, ...){
	va_list arg;
	int len;
	int buffer = 71;
	char btext[70];
	
	// ���p���r�@�@���h�� 
	va_start(arg, text);
	len = vsprintf(btext, text, arg);
	va_end(arg);
	setCursorXY(0, getCursorXY().Y);  //  ���нզ^�쥻�ӦL����m 
	printf("   |");
	if(len < buffer){
		buffer -= len;  //  �ݸɥR�h�֭Ӫť� 
		for(int i = 0; i < buffer; i++){
			if(i == buffer / 2){  //  �P�_��r�L����m
				//  �A���s�L�@�� 
				setTextColor(color);
				va_start(arg, text);
				vprintf(text, arg);
				va_end(arg);
				setTextColor(0x0F);
			}else{
				printf(" ");
			}
		}
	}else{
		btext[69] = 0x00;
		setTextColor(color);
		printf("%s ", btext);
		setTextColor(0x0F);
	}
	printf("|\n");
}

//  ���o�r�� 
bool getchars(char* buf, int count, int mode){
	DWORD cnt;
    INPUT_RECORD input;
    char b;
    int c = 0;  //  �ثe��J�h�֭Ӧr�� 
	
	setCursorInfo(100, 1);  //  ��ܥ��� 
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_LINE_INPUT | ENABLE_INSERT_MODE | ENABLE_MOUSE_INPUT);  //  ���o�r�����Ϋ��U Enter�B�T�Υ��k�� 
	
    while(true){  //  Ū����J���ʧ@ 
        if(kbhit() != 0){  //  �P�_�O�_����L�ƥ�A�B�����U�����A 
        	b = getch();  //  ���o�ۤ����L�X 
        	if(b == 0x1B){  //  ���U Esc
				setCursorInfo(100, 0);  //  ���å��� 
				return 1;
			}else if(b == 0x0D){  //  ���U Enter 
				setCursorInfo(100, 0);  //  ���å��� 
        		//  �p�G�����W�w�ƶq�A�h�᭱�� 0 
        		for(int i = c; i <= count; i++)
        			buf[i] = 0x00;
				break;
			}else if(b == 0x08){  //  ���U Backspace 
				//  ��֦r���A�p�G�p�� 0�A�h�ܬ� 0 
				c--;
				if(c < 0)
					c = 0;
				else{
					//  ���Э˰h�A�B�L _ 
					buf[c] = b;
					printf("%c", buf[c]);
					printf("_");
					printf("%c", buf[c]);
				}
			}else if(c < count && b > 0x1F && b < 0x7F && mode == 0){  //  �����W�w�ơA�B���i�L�d�� 
				buf[c] = b;
				printf("%c", buf[c]);
				c++;
			}else if(c < count && (b > 0x2F && b < 0x3A || b > 0x40 && b < 0x47 || b > 0x60 && b < 0x67) && mode == 1){  //  �����W�w�ơA�B�� hex �Ҧ� 0 ~ 9 A ~ F a ~f 
				if(b > 0x2F && b < 0x3A){
					buf[c] = b - 0x30;  //  �ন 0 ~ 9 
				}else if(b > 0x40 && b < 0x47){
					buf[c] = b - 55;  //  �ন 10 ~ 15 
				}else if(b > 0x60 && b < 0x67){
					buf[c] = b - 87;  //  �ন 10 ~ 15
				}
				printf("%c", b);
				c++;
			}
			//  �����ɭԤ���ܥ��� 
			if(c == count)
				setCursorInfo(100, 0);
			else
				setCursorInfo(100, 1);
		}
	}
	return 0;
}

//  �q 19 ��� 43 ���R���ťաA�N�O�M�Ť@�������ù� 
void cls(int from, int to){
	setCursorXY(0, from);
	for(int i = from; i < to; i++){
		printf("                                                                           \n");
	}
}

//  �q 19 ��� 43 ���R���ťաA�N�O�M�Ť@�������ù� 
void border(int from, int to){
	setCursorXY(0, from);
	for(int i = from; i < to; i++){
		printf("   |                                                                      |\n");
	}
}

//  ��J�� ( key �P iv ) 
bool inputBox(const char* name, const char** in, unsigned short size, int mode){
	COORD cursorPosition = getCursorXY();
	int y =  16 + (cursorPosition.Y - 22) / 2;
	//  �p�����\�񪺦�m�M�j�p 
	box(size + 4 ,7, 37 - size / 2, y);
	setCursorXY(39 - strlen(name) / 2, y + 2);
	printf("%s", name);
	//  �K�X���u 
	setCursorXY(39 - size / 2, y + 4);
	for(int i = 0; i < size; i++){
		printf("_");  //  �T�w�i�H��J�h�֭Ӧr�� 
	}
	setCursorXY(39 - size / 2, y + 4);  //  ��J�K�X����m 
	if(*in != NULL)
		free((void*)*in);
	*in = (char*)malloc(sizeof(char) * size + 1);  //  �w���}�ݭn���O����j�p 
	return getchars((char*)*in, size, mode);  //  ���o�r�� 
}

void setpws(const char* name, const char** pw, unsigned short size, int mode){
	if(mode == 0){  //  ���`�Ҧ� ( ASCII �i���d�� ) 
		const char* temp_pw;
		if(!inputBox(name, &temp_pw, size, mode)){
			free((void*)*pw);
			*pw = temp_pw;
		}
	}else if(mode == 1){  //  HEX 
		const char* temp_pw;
		if(!inputBox(name, &temp_pw, size * 2, mode)){
			unsigned char* hex_pw = (unsigned char*)malloc(sizeof(unsigned char) * size);
			unsigned char* get_pw = (unsigned char*)temp_pw;
			for(int i = 0; i < size; i++){
				hex_pw[i] = 16 * get_pw[i * 2] + get_pw[i * 2 + 1];  //  �ন 16 �i�� 
			}
			free((void*)*pw);
			*pw = (char*)hex_pw;
		}
	}
}

//  �j�M�ؼФ��
bool targetFile(const char* name, const char** pw, unsigned short size){
	COORD cursorPosition = getCursorXY();
	int y =  16 + (cursorPosition.Y - 22) / 2;
	  //  �p�����\�񪺦�m�M�j�p 
	box(68, 7, 5, y);
	setCursorXY(34, y + 2);
	printf("%s", name);
	setCursorXY(7, y + 4);
	for(int i = 0; i < size; i++){
		printf("_");  //  �T�w�i�H��J�h�֭Ӧr�� 
	}
	setCursorXY(7, y + 4);  //  ��J�K�X����m 
	if(*pw != NULL)
		free((void*)*pw);
	*pw = (char*)malloc(sizeof(char) * size + 1);  //  �w���}�ݭn���O����j�p 
	return getchars((char*)*pw, size);  //  ���o�r�� 
}

//  ���|�P�ɦW���� 
void splitFile(const char* tpath, const char* path, const char* file){
	char* temp_tpath = (char*)tpath;
	char* temp_path = (char*)path;
	char* temp_file = (char*)file;
	int i = strlen(tpath) - 1;
	while(i > 0){
		//  �ѫ᩹�e�j�M 
		if(temp_tpath[i] == '\\' || temp_tpath[i] == '/'){  //  ���ΧP�_�̾� 
			break;
		}
		i--;
	}
	strcpy(temp_path, temp_tpath);
	temp_path[i] = 0x00;
	strcpy(temp_file, temp_tpath + i + 1);  //  ���]�t / 
}

bool delFile(const char* path){
	int y = 21;
	int i, count = 0;
	bool exist;
	clock_t nStart, nEnd = 0;
	struct _finddata_t file;
	char dir[256], filter[256];
	long File;
	strcpy(dir, path);  //  ��ܬ��[�K�٬O�ѱK�}�Ҫ��a�� 
	strcpy(filter, "*.*");  //  �H . �����ΡA�P�_�O�_���ɮ� 
	_chdir(dir);  //  �i�J�l�ؿ� 
	box(36 ,7, 21, y);
	setCursorXY(27, y + 2);
	printf("���b�M�� %s ��Ƨ�", path);
	if((File = _findfirst(filter, &file)) != -1){  //  �P�_�O�_�����\�}�� 
		exist = _findnext(File, &file);  //  ���L�o�� . 
		while(!(exist = _findnext(File, &file))){  //  �Ĥ@�����L�o�� .. 
			if(rearKeyEvent()) return 1;
			setCursorXY(27, y + 4);
			printf("�ɮײM���� ");
			unlink(file.name);  //  ���o�ثe���쪺�ɮצW�r 
			for(i = 0; i < 6; i++){
				if(i == count){
					printf("�@");
				}else{
					printf("�C");
				}
			}
			//  �C 100 �@�����@�� 
			if(nEnd == 0 || nEnd - nStart > 100){
				nStart = clock();
				count = (count + 1) % 6;
				nEnd = 0;
			}
			nEnd = clock();
		}
		if(exist){
			setCursorXY(27, y + 4);
			printf("��Ƨ����w�g�S���ɮ�");
		}
	}
	_chdir("..");
	return 0;
}

//  �R���ɮת��p���� 
void delBox(const char* path){
	if(delFile(path)){
		setCursorXY(27, 23);
		printf("%s ��Ƨ��w�Q���L", path);
		Sleep(200);  //  �� 200 �@�� 
	}else{
		setCursorXY(27, 23);
		printf("%s ��Ƨ��w�M�ŧ���", path);
	}
}

//  �p�����]�p 
void box(int W, int H, int X, int Y){
	setCursorXY(X, Y);
	textHR(W - 2, false);
	for(int i = 1; i < H - 1; i++){
		setCursorXY(X, Y + i);
		textCenter("", 0x0F, W - 1, false);
	}
	setCursorXY(X, Y + H - 1);
	textHR(W - 2, false);
}


//  �ʤ��� 
void perbar(long completed, long complete, bool crypt){
	static clock_t nStart, nEnd = 0;
	COORD cursorPosition = getCursorXY();
	int i, y = cursorPosition.Y;
	double finished, per = (double)completed / complete;
	if(completed == complete || completed == 0 || nEnd == 0 || nEnd - nStart > 200){
		setCursorXY(5, y + 1);
		printf("�`�@�j�p�G %lu �줸��", complete);
		setCursorXY(5, y + 2);
		printf("�w�g�����G %lu �줸��", completed);
		setCursorXY(5, y + 3);
		printf("%s�i�סG ", crypt ? "�[�K" : "�ѱK");
		setCursorXY(16, y + 3);
		finished = 26 * per; 
		for(i = 1; i < 27; i++){
			if(i <= finished){
				printf("��");
			}else{
				printf("��");
			}
		}
		printf(" %03d%%", (int)(per * 100));
		setCursorXY(16, y);
		nStart = clock();
		nEnd = 0;
	}
	nEnd = clock();
}

//  ���Ū����L�ƥ� 
bool rearKeyEvent(void){
	char c;
	if(kbhit() != 0){
		c = getch();
		if(c == 0x1B){
			keyFlag = 1;
			return 1;
		}
	}
	return 0;
}

void pageMenu(const char* name, const char* option[], int count, int num){
	setCursorXY(0, 19);
	textCenter(name);
	textCenter("");
	for(int i = 0; i < count; i++){
		textCenter(option[i], num == i ? 0xF0 : 0x0F);  //  �P�_�O�_�����ۤv�A�O�h�����C��
		textCenter("");
	}
	textHR();
}

