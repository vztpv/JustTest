
#include <iostream>
#include <string>

#include <ctime>

static bool isWhitespace(int ch) {
	switch (ch) {
	case ' ':
	case '\t':
	case '\r':
	case '\n':
		return true;
	}
	return false;
}

static void preScanning(const long long dif, const char* text, const long long length, long long* arr, long long* arr_count) {
	int _arr_count = 0;

	for (long long i = 0; i < length; ++i) {
		switch (text[i]) {
		case ' ':
		case '\t':
		case '\r':
		case '\n':
		case '\"':
		case '=':
		case '{':
		case '}':
			arr[_arr_count] = i + dif; // after called this function, arr[index] += -1?
			_arr_count++;
			break;
		}
	}

	*arr_count = _arr_count;
}

static void Scanning(const char* text, const long long length) {

	long long* arr = new long long[length + 1];
	long long count = -2;

	int a = clock();
	preScanning(0, text, length, arr, &count);
	int b = clock();
	std::cout << b - a << "ms\n";
	{ 
		int state = 0;

		long long token_first = 0;
		long long token_last = -1;

		long long token_arr_count = 0;

		arr[count] = length; // text[arr[count]] == '\0'
		for (long long i = 0; i <= count; ++i) {
			const char ch = text[arr[i]];

			if (0 == state) {
				if ('\"' == ch) {
					state = 1;
				}
				else if (isWhitespace(ch) || '\0' == ch) {
					token_last = arr[i] - 1;
					if (token_last - token_first + 1 > 0) {
						token_arr_count++;
					}
					token_first = arr[i] + 1;
					token_last = arr[i] + 1;
				}
				else if ('{' == ch) {
					token_last = arr[i] - 1;
					if (token_last - token_first + 1 > 0) {
						token_arr_count++;
					}

					token_first = arr[i];
					token_last = arr[i];
					
					token_arr_count++;

					token_first = arr[i] + 1;
					token_last = arr[i] + 1;
				}
				else if ('}' == ch) {
					token_last = arr[i] - 1;
					if (token_last - token_first + 1 > 0) {
						token_arr_count++;
					}
					token_first = arr[i];
					token_last = arr[i];

					token_arr_count++;

					token_first = arr[i] + 1;
					token_last = arr[i] + 1;

				}
				else if ('=' == ch) {
					token_last = arr[i] - 1;
					if (token_last - token_first + 1 > 0) {
						token_arr_count++;
					}
					token_first = arr[i];
					token_last = arr[i];
					
					token_arr_count++;

					token_first = arr[i] + 1;
					token_last = arr[i] + 1;
				}
			}
			else if (1 == state) {
				if ('\\' == ch) {
					state = 2;
				}
				else if ('\"' == ch) {
					state = 0;
				}
			}
			else if (2 == state) {
				if (arr[i] > arr[i - 1] + 1) {
					--i;
				}
				state = 1;
			}
		}

		std::cout << token_arr_count;

		if (0 != state) {
			std::cout << "[" << state << "] state is not zero.\n";
		}
	}

	{
		delete[] arr;
	}
}

static void Scanning2(const char* text, const long long length) {

//	long long* arr = new long long[length + 1];
	long long count = -2;

	//preScanning(0, text, length, arr, &count);

	{
		int state = 0;

		long long token_first = 0;
		long long token_last = -1;

		long long token_arr_count = 0;

		//arr[count] = length; // text[arr[count]] == '\0'
		for (long long i = 0; i <= length; ++i) {
			const char ch = text[i];

			if (0 == state) {
				if ('\"' == ch) {
					state = 1;
				}
				else if (isWhitespace(ch) || '\0' == ch) {
					token_last = i - 1;
					if (token_last - token_first + 1 > 0) {
						token_arr_count++;
					}
					token_first = i + 1;
					token_last = i + 1;
				}
				else if ('{' == ch) {
					token_last = i - 1;
					if (token_last - token_first + 1 > 0) {
						token_arr_count++;
					}

					token_first = i;
					token_last = i;

					token_arr_count++;

					token_first = i+ 1;
					token_last = i + 1;
				}
				else if ('}' == ch) {
					token_last = i - 1;
					if (token_last - token_first + 1 > 0) {
						token_arr_count++;
					}
					token_first = i;
					token_last = i;

					token_arr_count++;

					token_first = i + 1;
					token_last = i + 1;

				}
				else if ('=' == ch) {
					token_last = i - 1;
					if (token_last - token_first + 1 > 0) {
						token_arr_count++;
					}
					token_first = i;
					token_last = i;

					token_arr_count++;

					token_first = i + 1;
					token_last = i + 1;
				}
			}
			else if (1 == state) {
				if ('\\' == ch) {
					state = 2;
				}
				else if ('\"' == ch) {
					state = 0;
				}
			}
			else if (2 == state) {
				state = 1;
			}
		}

		std::cout << token_arr_count;

		if (0 != state) {
			std::cout << "[" << state << "] state is not zero.\n";
		}
	}

	{
		//delete[] arr;
	}
}


int main(void)
{
	
	std::string str;
	std::string temp;

	int N;

	//std::cin >> N; getchar();

	for (int i = 0; std::getline(std::cin, temp); ++i) {
		str += temp;
		str += " ";
		temp.clear();
	}
int a = clock();
	Scanning(str.c_str(), str.size());
	int b = clock();
	std::cout << "\n" << b - a << "\n";
	a = clock();
	Scanning2(str.c_str(), str.size());
	b = clock();
	std::cout << "\n" << b - a << "\n";
}

