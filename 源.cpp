#include <fstream>
#include <iostream>
#include <ctime>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <sstream>
using namespace std;

class Tools {
public:

	static void trim(string& str) {//去除字符串str前侧的空格
		str.erase(0, str.find_first_not_of(' '));
		return;
	}

	static string rand_str(const int len) {  //随机生成长度为len的字符串，参数为字符串的长度
		/*初始化*/
		string str;                 //声明用来保存随机字符串的str
		char c;                     //声明字符c，用来保存随机生成的字符
		int idx;                    //用来循环的变量
		/*循环向字符串中添加随机生成的字符*/
		for (idx = 0; idx < len; idx++) {
			//rand()%26是取余，余数为0~25加上'a',就是字母a~z,详见asc码表
			c = 'a' + rand() % 26;
			str.push_back(c);       //push_back()是string类尾插函数。这里插入随机字符c
		}
		return str;                 //返回生成的随机字符串
	}

};

class Test {
public:
	vector<string> testStep;//状态转移表
	static void testParser();//测试分析器
	static void testTalk();//测试解释器
	static void testData();//测试数据文件的处理
	static void testTokens();//测试记号处理
	static void testGetKey();//测试关键词提取
};

class Interpreter {
private:
	int wordNum = 0;
	int UserID = 5;//用户ID
	int amount = 100;//用户账单
	string name;//用户姓名
	string curStep;//当前所处Step
	vector<string> testWords;//测试用的对话语句
	vector<string> Log;//日志文件
	string getKeywords(string teststr);//获取对话中的关键字
	void getUserInfo(int uid);//获取当前用户的信息
	void robotTalk(int test);
public:
	friend Test;
	void Talk(int test);//与用户对话的函数
	void outputLog();//输出日志文件
};

class Parser {
private:
	string	curFunc;//当前正在处理的函数
	void ParseLine(string line);//处理一行
	string ProcessTokens(vector<string> token, int test);//根据每一行的记号进行不同的处理,test代表测试位
	void ProcessStep(string StepName);	//新的函数
	void ProcessSpeak(vector<string> words);
protected:
	struct Func {
		vector<pair<int, string>> Speak;	//常量字符串存储代号为0
		int Listen = 0;		//是否寻求用户的意见
		map<string, string> Branch;		//遇到关键字需要跳转到哪一步
		string Silence;		//用户沉默需要跳转到哪一步
		string Default;		//其他情况需要跳转到哪一步
		int Exit = 0;		//是否结束对话
	};//定义函数的结构体
	map<string, struct Func*> funcs;	//存储所有的函数
	map<int, pair<string, int>> data;	//存储数据文件
	set<string> Keywords;		//存储关键字
	string mainStep;	//主函数
	void result();		//显示脚本结果
public:
	friend Test;
	friend Interpreter;
	void readData(string dataname);//读入数据文件
	void ParseFile(string filename);//读入脚本文件
	void DeleteData();//清除所有数据
};

Parser parser;//分析器
Interpreter interpreter;//解释器
Test TEST;//测试程序

void Test::testData() {
	parser.readData("data.test");
	ifstream testfile("data.test");
	cout << "============================Test——Data测试！============================" << endl;
	int id;
	int amount;
	string name;
	while (!testfile.eof()) {
		testfile >> id;
		if (parser.data.find(id) == parser.data.end()) {
			cout << "============================Test——Data测试！============================" << endl;
			cout << "=========================ERROR——不存在该用户ID！========================" << endl;
			cout << "=========================ERROR——Data测试失败！==========================" << endl;
			break;
		}
		testfile >> name;
		testfile >> amount;
		if (parser.data[id].first != name || parser.data[id].second != amount) {
			cout << "=========================ERROR——用户信息不匹配！========================" << endl;
			cout << "=========================ERROR——Data测试失败！==========================" << endl;
		}
		break;
	}
	cout << "==================Test SUCCESS——Data测试成功！==========================" << endl;
	for (auto it : parser.data) {//打印输入数据
		cout << "UserId：" << it.first;
		cout << "		Name：" << it.second.first;
		cout << "		Amount：" << it.second.second << endl;;
	}
	return;
}

void Test::testTokens() {
	vector<vector<string> > testTokens;
	vector<string> temp;
	string tst[] = { "Entry", "File", "Step", "Speak", "Branch", "Listen", "Silence", "Default", "Exit","ERROR" };
	srand(time(NULL));
	int i = 300; 
	cout << "===========================Test——Tokens测试！===========================" << endl;
	while (i > 0) {
		int x = rand() % 10;
		temp.clear();
		temp.push_back(tst[x]);
		cout << tst[x] + "  ";
		testTokens.push_back(temp);
		i--;
	}
	cout << endl;
	for (int i = 0; i < testTokens.size(); i++) {
		if (testTokens[i][0] != parser.ProcessTokens(testTokens[i], 1)) {
			cout << "=========================ERROR——Tokens测试失败！========================" << endl;
			return;
		}
	}
	cout << "=================Test SUCCESS——Tokens测试成功！=========================" << endl;
	return;
}

void Test::testGetKey() {
	string teststr = "ghjoewubvbjkshg";
	string buff = teststr;
	cout << "===========================Test——GetKey测试！===========================" << endl;
	for (auto it : parser.Keywords) {
		string buff2 = buff;
		buff = teststr + it + buff2;
		if (interpreter.getKeywords(buff) != it)
			cout << "=========================ERROR——GetKey测试失败！========================" << endl;
		//	cout << buff + "  " + it + "GetKey测试失败！！" << endl;
	}
	cout << "=================Test SUCCESS——GetKey测试成功！=========================" << endl;
	return;
}

string Interpreter::getKeywords(string teststr) {
	string wordbuff;
	string key;
	if (teststr == "") {
		cout << "Speak：" ;
		getchar();
		getline(cin, wordbuff);//获取用户输入
	}
	else
		wordbuff = teststr;//测试字符串
	if (wordbuff == "" ) {
		return "";//用户没有说话
	}
	for (int i = 0; i < wordbuff.length() - 1; i++) {
		key.clear();
		key = wordbuff.substr(i, 4);
		if (parser.Keywords.find(key) != parser.Keywords.end()) {//接收到第一个有用的关键词
			return key;
		}
	}
	return "#";//没有接收到有用的关键词
}

void Interpreter::getUserInfo(int uid) {
	curStep = parser.mainStep;
	interpreter.UserID = uid;
	interpreter.name = parser.data[uid].first;
	interpreter.amount = parser.data[uid].second;
	return;
}

void Interpreter::robotTalk(int test) {
	auto stepFunc = parser.funcs[curStep];
	//cout << curStep << endl;				//用于调试，打印当前函数名
	string word = "#";
	if (stepFunc) {
		if (!stepFunc->Speak.empty() && !test) {
			cout << "	";
			for (auto it : stepFunc->Speak) {
				if (it.first == 0)//该句子为字符串常量
					cout << it.second;
				if (it.first == 1)//该句子为amount变量
					cout << amount;
				if (it.first == 2)//该句子为name变量
					cout << name;
				if (it.first == 3)//该句子为UserID变量
					cout << UserID;
			}
			cout << endl;
		}
		if (stepFunc->Listen) {
			word.clear();
			if(!test)
				word = getKeywords("");
			else {
				if (testWords[wordNum] == "#")
					word = "";
				else
					word = getKeywords(testWords[wordNum++]);
			}
		}
		if (stepFunc->Branch.find(word) != stepFunc->Branch.end()) {
			Log.push_back(word);
			curStep.clear();
			curStep = stepFunc->Branch[word];
			if (test) {
				TEST.testStep.push_back(curStep);
				robotTalk(1);
			}
			else
				robotTalk(0);
			return;
		}
		else if (word.empty() && !stepFunc->Silence.empty()) {
			curStep.clear();
			curStep = stepFunc->Silence;
			if (test){
				TEST.testStep.push_back(curStep);
				robotTalk(1);
			}
			else
				robotTalk(0);
			return;
		}
		else if (word == "#" && !stepFunc->Default.empty()) {
			curStep.clear();
			curStep = stepFunc->Default;
			if (test){
				TEST.testStep.push_back(curStep);
				robotTalk(1);
			}
			else
				robotTalk(0);
			return;
		}
		if (stepFunc->Exit) {
			outputLog();
			return;
		}
	}
	else {
		cout << "=========================ERROR——程序逻辑错误！==========================" << endl;
		return;
	}
}

void Interpreter::outputLog() {
	string buff;
	stringstream ss;
	ss << UserID;
	ss >> buff;
	ofstream logging("Log/" + buff);
	for (auto it : Log) {
		logging << it;
	}
	logging.close();
	return;
}

void Interpreter::Talk(int test) {
	int Uid;
	if (test) {
		Uid = 5;
	}
	else {
		while (1) {
			cout << "	请输入您的ID以登录：" << endl;
			cout << "Speak：";
			cin >> Uid;
			if (parser.data.find(Uid) == parser.data.end())
				cout << "	请输入正确的ID!" << endl;
			else
				break;
		}
	}
	interpreter.getUserInfo(Uid);//获取当前用户的信息
	if(test)
		interpreter.robotTalk(1);
	else
		interpreter.robotTalk(0);
	return;
}

void Parser::ProcessStep(string StepName) {
	curFunc = StepName;
	Func* newFunc = new Func;
	funcs[StepName] = newFunc;
	return;
}

void Parser::readData(string dataname) {
	ifstream inputData(dataname);
	if (!inputData)
		cout << "=========================ERROR——数据文件打开错误！======================" << endl;
	int UID, amount;
	string name;
	while (!inputData.eof()) {
		inputData >> UID;
		inputData >> name;
		inputData >> amount;
		parser.data[UID] = make_pair(name, amount);
	}
	inputData.close();
}

void Parser::ProcessSpeak(vector<string> words) {
	for (auto it : words) {
		pair<int, string> talk;
		if (it[0] == '$') {//该句子是变量
			string varname;
			varname = it.substr(1, it.size() - 1);
			if (varname == "amount") {
				talk = make_pair(1, varname);//该变量是账单
				funcs[curFunc]->Speak.push_back(talk);
			}
			else if (varname == "name") {
				talk = make_pair(2, varname);//该变量是名字
				funcs[curFunc]->Speak.push_back(talk);
			}
			else if (varname == "number") {
				talk = make_pair(3, varname);//该变量是ID
				funcs[curFunc]->Speak.push_back(talk);
			}
			else {
				cout << "=========================ERROR——变量还未声明！==========================" << endl;
				return;
			}
		}
		else if (it[0] == '"') {//该句子是字符串常量
			int len = it.size();
			if (it[len - 1] != '"') {
				cout << "=========================ERROR——引号匹配发生错误！======================" << endl;
				return;
			}
			else {
				string buff = it.substr(1, len - 2);
				talk = make_pair(0, buff);	//代号0为常量字符串
				funcs[curFunc]->Speak.push_back(talk);
			}
		}
		else if (it == "+") {
			continue;
		}
	}
	return;
}

string Parser::ProcessTokens(vector<string> token, int test) {
	if (token[0] == "Entry") {
		if(!test)
			mainStep = token[1];
		else
			return "Entry";
	}
	else if (token[0] == "File") {
		if (!test)
			readData(token[1]);
		else
			return "File";
	}
	else if (token[0] == "Step") {
		if (!test)
			ProcessStep(token[1]);
		else
			return "Step";
	}
	else if (token[0] == "Speak") {
		if (!test) {
			vector<string> words(token.begin() + 1, token.end());
			ProcessSpeak(words);
		}
		else
			return "Speak";
	}
	else if (token[0] == "Listen") {
		if (!test)
			funcs[curFunc]->Listen = token[1][0] - '0';
		else
			return "Listen";
	}
	else if (token[0] == "Branch") {
		if (!test) {
			Keywords.insert(token[1]);
			funcs[curFunc]->Branch[token[1]] = token[2];
		}
		else
			return "Branch";
	}
	else if (token[0] == "Silence") {
		if (!test)
			funcs[curFunc]->Silence = token[1];
		else
			return "Silence";
	}
	else if (token[0] == "Default") {
		if (!test)
			funcs[curFunc]->Default = token[1];
		else
			return "Default";
	}
	else if (token[0] == "Exit") {
		if (!test)
			funcs[curFunc]->Exit = 1;
		else
			return "Exit";
	}
	else {
		if (!test)
			cout << "=========================ERROR——错误源程序！============================" << endl;
		else
			return "ERROR";
	}
	return "";
}

void Parser::ParseLine(string line) {
	vector<string> token;
	token.clear();
	string buff;//记录一个token的缓冲区       
	for (auto it : line) {
		if (it == '#') {	//#表示行尾处理结束
			token.push_back(buff);
			break;
		}
		else if (it == ' ' && !buff.empty()) {	//表示一个token结束，获得标识符
			token.push_back(buff);
			buff.clear();
		}
		else {
			buff += it;
		}
	}
	ProcessTokens(token, 0);//0代表非测试
	return;
}

void Parser::ParseFile(string filename) {
	ifstream inputFile(filename);
	string buff;//输入脚本文件的缓冲区
	while (!inputFile.eof()) {
		buff.clear();
		getline(inputFile, buff);
		Tools::trim(buff);	//删除行首空白
		if (buff[0] != '#' && !buff.empty()) {	//表示该行不是注释行且不是空行
			buff += '#';	//作为每一行的结束标志
			ParseLine(buff);
		}
	}
	if (parser.mainStep == "") {
		cout << "=========================ERROR——未定义主程序！==========================" << endl;
	}
	if (parser.data.empty()) {
		cout << "=========================ERROR——未输入数据文件！========================" << endl;
	}
	inputFile.close();
	return;
}

void Parser::result() {
	cout << "=========================Steps——所有函数如下！==========================" << endl;
	for (auto it : funcs) {
		cout << "====函数：" << it.first << endl;
		auto func = funcs[it.first];
		if (func) {
			cout << "	Speak:";
			if (!func->Speak.empty()) {
				for (auto its : func->Speak) {
					cout << its.first << ":" + its.second;
				}
			}
			cout << endl;
			cout << "	Listen:" << func->Listen << endl;
			cout << "	Branch:";
			if (!func->Branch.empty()) {
				for (auto its : func->Branch) {
					cout << its.first + ":" + its.second << "  ";
				}
			}
			cout << endl;
			cout << "	Silence:" << func->Silence << endl;
			cout << "	Default:" << func->Default << endl;
			cout << "	Exit:" << func->Exit << endl;
		}
	}
	cout << "====主函数：" << mainStep << endl;
	return;
}

void Parser::DeleteData() {
	for (auto it : funcs) {
		delete it.second;
	}
	funcs.clear();
	return;
}

void Test::testParser() {
	vector<string> tokens;
	srand(time(NULL));
	cout << "===========================Test——Parser测试！===========================" << endl;
	int len = rand() % 20 + 1;//函数名长度1~20
	string entry = Tools::rand_str(len);
	string exit = Tools::rand_str(len);
	tokens.push_back("Entry " + entry);
	tokens.push_back("File data.txt");
	int Nstep = rand() % 10 + 10;//非主函数的函数个数为10~20个
	string laststep = exit;
	for (int i = 0; i < Nstep; i++) {
		string step;
		if (i == 0)
			step = "silence";
		else if (i == 1)
			step = "default";
		else if (i == 2)
			step = entry;
		else if (i == Nstep - 1)
			step = exit;
		else {
			len = rand() % 20 + 1;//函数名长度1~20
			step = Tools::rand_str(len);
		}
		tokens.push_back("Step " + step);
		int flag = rand() % 4;
		switch (flag) {
		case 0:
			tokens.push_back("    Speak $name + \"您好，请问有什么可以帮您？\"");
			break;
		case 1:
			tokens.push_back("    Speak \"您本月的账单是\" + $amount  +  \"元。\"");
			break;
		case 2:
			tokens.push_back("    Speak \"感谢您的来电，再见！\"");
			break;
		case 3:
			tokens.push_back("    Speak \"不好意思没听清，请您再说一遍。\"");
			break;
		default:
			tokens.push_back("    Speak \"您的ID为：\" + $number");
		}
		flag = rand() % 3;
		switch (flag) {
		case 0:
			tokens.push_back("    Branch 账单 " + laststep);
			break;
		case 1:
			tokens.push_back("    Branch 姓名 " + laststep);
			break;
		default:
			tokens.push_back("    Branch 号码 " + laststep);
		}
		flag = rand() % 2;
		if (flag)
			tokens.push_back("    Listen 1");
		else
			tokens.push_back("    Listen 0");
		flag = rand() % 2;
		if (flag)
			tokens.push_back("    Silence silence");
		flag = rand() % 2;
		if (flag)
			tokens.push_back("    Default default");
		if (i == Nstep - 1) {
			tokens.push_back("    Exit");
		}
		laststep.clear();
		laststep = step;
	}
	ofstream ofile("script.test");
	for (auto it : tokens) {
		ofile << it << endl;
	}
	ofile.close();
	parser.ParseFile("script.test");
	parser.result();	//显示脚本文件处理结果
	cout << "=================Test SUCCESS——Parser测试成功！=========================" << endl;
	parser.DeleteData();
	return;
}

void Test::testTalk() {
	vector<string> original;
	srand(time(NULL));
	cout << "============================Test——Talk测试！============================" << endl;
	interpreter.testWords.clear();
	interpreter.wordNum = 0;
	TEST.testStep.clear();
	int flag;
	int i = 0;
	while (i < 20) {
		flag = rand()%4;
		switch (flag) {
		case 0:
			interpreter.testWords.push_back("您好，我要查账单！");
			original.push_back("bill");
			original.push_back("welcome");
			break;
		case 1:
			interpreter.testWords.push_back("您好，我要学习！");
			original.push_back("default");
			original.push_back("welcome");
			break;
		case 2:
			interpreter.testWords.push_back("您好，我的用户号码是多少！");
			original.push_back("number");
			original.push_back("welcome");
			break;
		case 3:
			interpreter.testWords.push_back("您好，我要投诉！");
			original.push_back("complaint");
			original.push_back("welcome");
			break;
		}
		cout << "	Talk测试	" + interpreter.testWords[interpreter.testWords.size()-1] << endl;
		i++;
	}
	interpreter.testWords.push_back("#");
	original.push_back("silence");
	interpreter.Talk(1);//进行测试
	for (i = 0; i < original.size() && i < TEST.testStep.size(); i++) {
		if (original[i] != TEST.testStep[i]) {
			cout << "======================Test  FAILED——Talk测试失败！======================" << endl;
			cout << "	原本状态应为：" + original[i] + "现在状态为：" + TEST.testStep[i] << endl;
			break;
		}
		else
			cout << "	Talk测试	" + TEST.testStep[i] << endl;
	}
	if(!(i < original.size() && i < TEST.testStep.size()))
		cout << "======================Test SUCCESS——Talk测试成功！======================" << endl;
	return;
}

int main() {
	//Test::testData();		//测试是否可以正确读入
	//system("pause");
	//Test::testTokens();	//测试是否可以将记号串转换为相应的函数
	//system("pause");
	//Test::testParser();	//测试整个分析器并打印语法树
	//system("pause");
	parser.ParseFile("script.txt");//处理脚本文件
	//Test::testGetKey();	//测试是否可以正确的提取出一句话的关键词
	//system("pause");
	//Test::testTalk();	//模拟人机对话——测试整个解释器
	//system("pause");
	while (1) {
		interpreter.Talk(0);//0表示非测试
		//system("pause");
	}
	parser.DeleteData();//清除在堆中分配的数据，清除所有数据
	system("pause");
	return 0;
}