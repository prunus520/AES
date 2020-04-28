#include "frdsgn.h" 

bool keyFlag = 0;


/********************  定義函式  ********************/ 


//  設定視窗主
void setWindows(const char* title, int W, int H, int X, int Y){
	SetConsoleTitle(title);  //  視窗名稱 
	setConsoleSize(W, H);  //  視窗大小 
	SetWindowPos(GetConsoleWindow(), 0, X, Y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);  //  視窗位置 
	setTextColor(0x0F);  //  字體背景顏色 
	setCursorInfo(100, 0);  //  不顯示光標 
	if(GetFileAttributesA("Encrypt") != FILE_ATTRIBUTE_DIRECTORY){  //  判斷是否有資料夾 
		_mkdir("Encrypt");  //  建立資料夾 
	}
	if(GetFileAttributesA("Decrypt") != FILE_ATTRIBUTE_DIRECTORY){  //  判斷是否有資料夾 
		_mkdir("Decrypt");  //  建立資料夾 
	}
	if(GetFileAttributesA("Done") != FILE_ATTRIBUTE_DIRECTORY){  //  判斷是否有資料夾 
		_mkdir("Done");  //  建立資料夾 
	}
}

//  設定視窗大小 
void setConsoleSize(int W, int H){
    SMALL_RECT SR = {0, 0, W, H};
    COORD Sz = {W + 1, H + 1};  //  隱藏滾動條 ( + 1 ) 
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE),true, &SR);  //  視窗大小 
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), Sz);  //  視窗緩存大小 
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE),true, &SR);  //  防止視窗大小跑掉 
}

//  設定光標位置 
void setCursorXY(int x, int y){
    COORD cursorPosition;
    cursorPosition.X = x;
    cursorPosition.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);  //  設定光標位置，左上為0, 0 
}

COORD getCursorXY(){
    CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    COORD cursorPosition = {csbi.dwCursorPosition.X, csbi.dwCursorPosition.Y};
	return cursorPosition;
}

//  設定光標厚度與可見度 
void setCursorInfo(int dwSize, int bVisible){
	CONSOLE_CURSOR_INFO curInfo = {dwSize, bVisible};
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);  //  光標厚度與可見度 
}

//  設定字體顏色 
void setTextColor(int color){
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);  //  設定字體顏色 
}

//  分隔線 
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

//  字體置中與有文字部分的顏色 
void textCenter(const char* text, int color, int buffer, bool blank){
	buffer -= strlen(text);  //  需補充多少個空白 
	if(blank){
		printf("   |");
	}else{
		printf("|");
	}
	for(int i = 0; i < buffer; i++){
		if(i == buffer / 2){  //  判斷文字印的位置 
			setTextColor(color);
			printf("%s", text);
			setTextColor(0x0F);
		}else{
			printf(" ");
		}
	}
	printf("|\n");
}

//  格式化的字體置中
void textCenterf(const char* text, ...){
	va_list arg;
	int len;
	int buffer = 71;
	char btext[70];
	
	// 先計算文字一共有多長 
	va_start(arg, text);
	len = vsprintf(btext, text, arg);
	va_end(arg);
	setCursorXY(0, getCursorXY().Y);  //  光標調回原本該印的位置 
	printf("   |");
	if(len < buffer){
		buffer -= len;  //  需補充多少個空白 
		for(int i = 0; i < buffer; i++){
			if(i == buffer / 2){  //  判斷文字印的位置
				//  再重新印一次 
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

//  格式化的字體置中與有文字部分的顏色 
void textCenterf(int color, const char* text, ...){
	va_list arg;
	int len;
	int buffer = 71;
	char btext[70];
	
	// 先計算文字一共有多長 
	va_start(arg, text);
	len = vsprintf(btext, text, arg);
	va_end(arg);
	setCursorXY(0, getCursorXY().Y);  //  光標調回原本該印的位置 
	printf("   |");
	if(len < buffer){
		buffer -= len;  //  需補充多少個空白 
		for(int i = 0; i < buffer; i++){
			if(i == buffer / 2){  //  判斷文字印的位置
				//  再重新印一次 
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

//  取得字元 
bool getchars(char* buf, int count, int mode){
	DWORD cnt;
    INPUT_RECORD input;
    char b;
    int c = 0;  //  目前輸入多少個字元 
	
	setCursorInfo(100, 1);  //  顯示光標 
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_LINE_INPUT | ENABLE_INSERT_MODE | ENABLE_MOUSE_INPUT);  //  取得字元不用按下 Enter、禁用左右鍵 
	
    while(true){  //  讀取輸入的動作 
        if(kbhit() != 0){  //  判斷是否為鍵盤事件，且為按下的狀態 
        	b = getch();  //  取得自元不印出 
        	if(b == 0x1B){  //  按下 Esc
				setCursorInfo(100, 0);  //  隱藏光標 
				return 1;
			}else if(b == 0x0D){  //  按下 Enter 
				setCursorInfo(100, 0);  //  隱藏光標 
        		//  如果不滿規定數量，則後面補 0 
        		for(int i = c; i <= count; i++)
        			buf[i] = 0x00;
				break;
			}else if(b == 0x08){  //  按下 Backspace 
				//  減少字元，如果小於 0，則變為 0 
				c--;
				if(c < 0)
					c = 0;
				else{
					//  光標倒退，且印 _ 
					buf[c] = b;
					printf("%c", buf[c]);
					printf("_");
					printf("%c", buf[c]);
				}
			}else if(c < count && b > 0x1F && b < 0x7F && mode == 0){  //  不滿規定數，且為可印範圍 
				buf[c] = b;
				printf("%c", buf[c]);
				c++;
			}else if(c < count && (b > 0x2F && b < 0x3A || b > 0x40 && b < 0x47 || b > 0x60 && b < 0x67) && mode == 1){  //  不滿規定數，且為 hex 模式 0 ~ 9 A ~ F a ~f 
				if(b > 0x2F && b < 0x3A){
					buf[c] = b - 0x30;  //  轉成 0 ~ 9 
				}else if(b > 0x40 && b < 0x47){
					buf[c] = b - 55;  //  轉成 10 ~ 15 
				}else if(b > 0x60 && b < 0x67){
					buf[c] = b - 87;  //  轉成 10 ~ 15
				}
				printf("%c", b);
				c++;
			}
			//  當滿的時候不顯示光標 
			if(c == count)
				setCursorInfo(100, 0);
			else
				setCursorInfo(100, 1);
		}
	}
	return 0;
}

//  從 19 行到 43 行填充為空白，就是清空一部份的螢幕 
void cls(int from, int to){
	setCursorXY(0, from);
	for(int i = from; i < to; i++){
		printf("                                                                           \n");
	}
}

//  從 19 行到 43 行填充為空白，就是清空一部份的螢幕 
void border(int from, int to){
	setCursorXY(0, from);
	for(int i = from; i < to; i++){
		printf("   |                                                                      |\n");
	}
}

//  輸入框 ( key 與 iv ) 
bool inputBox(const char* name, const char** in, unsigned short size, int mode){
	COORD cursorPosition = getCursorXY();
	int y =  16 + (cursorPosition.Y - 22) / 2;
	//  小視窗擺放的位置和大小 
	box(size + 4 ,7, 37 - size / 2, y);
	setCursorXY(39 - strlen(name) / 2, y + 2);
	printf("%s", name);
	//  密碼底線 
	setCursorXY(39 - size / 2, y + 4);
	for(int i = 0; i < size; i++){
		printf("_");  //  確定可以輸入多少個字元 
	}
	setCursorXY(39 - size / 2, y + 4);  //  輸入密碼的位置 
	if(*in != NULL)
		free((void*)*in);
	*in = (char*)malloc(sizeof(char) * size + 1);  //  預先開需要的記憶體大小 
	return getchars((char*)*in, size, mode);  //  取得字元 
}

void setpws(const char* name, const char** pw, unsigned short size, int mode){
	if(mode == 0){  //  正常模式 ( ASCII 可見範圍 ) 
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
				hex_pw[i] = 16 * get_pw[i * 2] + get_pw[i * 2 + 1];  //  轉成 16 進位 
			}
			free((void*)*pw);
			*pw = (char*)hex_pw;
		}
	}
}

//  搜尋目標文件
bool targetFile(const char* name, const char** pw, unsigned short size){
	COORD cursorPosition = getCursorXY();
	int y =  16 + (cursorPosition.Y - 22) / 2;
	  //  小視窗擺放的位置和大小 
	box(68, 7, 5, y);
	setCursorXY(34, y + 2);
	printf("%s", name);
	setCursorXY(7, y + 4);
	for(int i = 0; i < size; i++){
		printf("_");  //  確定可以輸入多少個字元 
	}
	setCursorXY(7, y + 4);  //  輸入密碼的位置 
	if(*pw != NULL)
		free((void*)*pw);
	*pw = (char*)malloc(sizeof(char) * size + 1);  //  預先開需要的記憶體大小 
	return getchars((char*)*pw, size);  //  取得字元 
}

//  路徑與檔名分割 
void splitFile(const char* tpath, const char* path, const char* file){
	char* temp_tpath = (char*)tpath;
	char* temp_path = (char*)path;
	char* temp_file = (char*)file;
	int i = strlen(tpath) - 1;
	while(i > 0){
		//  由後往前搜尋 
		if(temp_tpath[i] == '\\' || temp_tpath[i] == '/'){  //  分割判斷依據 
			break;
		}
		i--;
	}
	strcpy(temp_path, temp_tpath);
	temp_path[i] = 0x00;
	strcpy(temp_file, temp_tpath + i + 1);  //  不包含 / 
}

bool delFile(const char* path){
	int y = 21;
	int i, count = 0;
	bool exist;
	clock_t nStart, nEnd = 0;
	struct _finddata_t file;
	char dir[256], filter[256];
	long File;
	strcpy(dir, path);  //  選擇為加密還是解密開啟的地方 
	strcpy(filter, "*.*");  //  以 . 為切割，判斷是否為檔案 
	_chdir(dir);  //  進入子目錄 
	box(36 ,7, 21, y);
	setCursorXY(27, y + 2);
	printf("正在清空 %s 資料夾", path);
	if((File = _findfirst(filter, &file)) != -1){  //  判斷是否有成功開啟 
		exist = _findnext(File, &file);  //  先過濾掉 . 
		while(!(exist = _findnext(File, &file))){  //  第一次先過濾掉 .. 
			if(rearKeyEvent()) return 1;
			setCursorXY(27, y + 4);
			printf("檔案清除中 ");
			unlink(file.name);  //  取得目前指到的檔案名字 
			for(i = 0; i < 6; i++){
				if(i == count){
					printf("　");
				}else{
					printf("。");
				}
			}
			//  每 100 毫秒執行一次 
			if(nEnd == 0 || nEnd - nStart > 100){
				nStart = clock();
				count = (count + 1) % 6;
				nEnd = 0;
			}
			nEnd = clock();
		}
		if(exist){
			setCursorXY(27, y + 4);
			printf("資料夾中已經沒有檔案");
		}
	}
	_chdir("..");
	return 0;
}

//  刪除檔案的小視窗 
void delBox(const char* path){
	if(delFile(path)){
		setCursorXY(27, 23);
		printf("%s 資料夾已被跳過", path);
		Sleep(200);  //  休息 200 毫秒 
	}else{
		setCursorXY(27, 23);
		printf("%s 資料夾已清空完畢", path);
	}
}

//  小視窗設計 
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


//  百分條 
void perbar(long completed, long complete, bool crypt){
	static clock_t nStart, nEnd = 0;
	COORD cursorPosition = getCursorXY();
	int i, y = cursorPosition.Y;
	double finished, per = (double)completed / complete;
	if(completed == complete || completed == 0 || nEnd == 0 || nEnd - nStart > 200){
		setCursorXY(5, y + 1);
		printf("總共大小： %lu 位元組", complete);
		setCursorXY(5, y + 2);
		printf("已經完成： %lu 位元組", completed);
		setCursorXY(5, y + 3);
		printf("%s進度： ", crypt ? "加密" : "解密");
		setCursorXY(16, y + 3);
		finished = 26 * per; 
		for(i = 1; i < 27; i++){
			if(i <= finished){
				printf("■");
			}else{
				printf("□");
			}
		}
		printf(" %03d%%", (int)(per * 100));
		setCursorXY(16, y);
		nStart = clock();
		nEnd = 0;
	}
	nEnd = clock();
}

//  後端讀取鍵盤事件 
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
		textCenter(option[i], num == i ? 0xF0 : 0x0F);  //  判斷是否為選到自己，是則改變顏色
		textCenter("");
	}
	textHR();
}

