// JSGFKit.cpp : 定义控制台应用程序的入口点。
//

#include "grammar.h"
#include <iostream>
#include <string>
#include <time.h>
#include <locale>

using namespace std;

/**
* \file
* An example that shows how to match test string against Rules and extract the tags that the input matched.
* Functions used: Grammar::matchesRule(), Grammar::getMatchingTags()
*
*/

/*
1.把代码全部变成utf-8格式的
2.把VS中的属性变成unicode编码
3.全部用UTF-8格式的
*/

#if 0
int main() {
	wcout.imbue(std::locale("chs"));
	wcin.imbue(std::locale("chs"));

#if 0
	// A string representing the grammar that we will be using.
	wstring s = L"grammar matching-test;\
    public <命令> = <行为> <方向>;\
    <行为> = 脸 {脸_tag} | 移动 {移动_tag} | 火 激光器 {火_tag} | 停止 {停止标签};\
    <方向> = 左边 | 右 | 上面 | 下面;\
    ";
#endif

#if 1
	// A string representing the grammar that we will be using.
	wstring s = L"grammar matching-test;\
    public <命令> = <行为><方向>;\
    <行为> = 脸{脸_tag}|移动{移动_tag}|火激光器{火_tag}|停止{停止标签};\
    <方向> = 左边{左边_tag}|右|上面|下面;\
    ";
#endif

#if 0
	// A string representing the grammar that we will be using.
	wstring s = L"grammar matching-test;\
    public <action> = 面向|移动;\
    ";
#endif


	wcout << L"Parsing example Grammar..." << endl;
	Grammar g;
	clock_t t;
	t = clock();
	Grammar::parseGrammarFromString(s, g);
	t = clock() - t;
	printf("It took me %d clicks (%f seconds).\n", t, ((float)t) / CLOCKS_PER_SEC);

	wcout << L"Parsed:" << endl;
	wstring stmp0 = g.getText();
	wcout << stmp0 << endl; // Grammar::getText returns a JSGF text to define the grammar
	bool done = false;
	while (!done) {
		wcout << L"Type in a query and see if it matches one of the rules:";
		wstring response=L"移动";
		//getline(wcin, response); // Get an input test string from the user
		//getline(wcin, response, wchar_t('\n'));
		getline(wcin, response, L'\n');
		t = clock();
		MatchResult result = g.match(response); // Match the test string against the command
		t = clock() - t;
		printf("It took me %d clicks (%f millseconds).\n", t, ((float)t));

		if (!result.matches) { // If no match
			wcout << L"Your response doesn't match the <command> Rule!" << endl;
		}
		else {
			wcout << L"Your response matched the <" << result.getMatchingRule()->getRuleName() << "> Rule!" << endl;
			wcout << L"Matching tags: ";
			vector<wstring> tags = result.getMatchingTags(); // Get a vector/vector of the tags that this test string matched
			for (wstring s : tags) { // Print them out
				wcout << s << L", ";

#if 0
				if (s == "quit_tag") {
					done = true;
					break;
				}
#endif
			}
		}
	}

	return 0;
}
#endif

int main()
{
	const wchar_t* strzh = L"中文abc";
	try
	{
		locale lc("zh_CN.UTF-8");
		locale::global(lc);
		// wcout.imbue(lc); // libstdc++ 的实现 imbue() 不起作用
		wcout << L"Zhong text is: " << strzh << endl;
	}
	catch (runtime_error& e)
	{
		cerr << "Error: " << e.what() << endl;
		cerr << "Type:" << typeid(e).name() << endl;
	}



	locale lc;
	cout << "Current global locale is: " << lc.name() << endl;//windows:lc.name()=C unix:lc.name()=C

	auto s0 = locale::global(locale(""));
	std::wcout << L"再次输出同一数字 User-preferred locale setting is 再次输出同一数字" << std::locale("").name().c_str() << L'\n';
	// 在启动时，全局本地环境是 "C" 本地环境
	std::wcout << 1000.01 << L'\n';
	// 以用户偏好的本地环境替换 C++ 本地环境和 C 本地环境
	std::locale::global(std::locale(""));
	// 将来的宽字符输出使用新的全局本地环境
	std::wcout.imbue(std::locale());
	std::wcout << L"再次输出同一数字" << L'\n';
	// 再次输出同一数字
	std::wcout << 1000.01 << L'\n';
	getchar();

}
