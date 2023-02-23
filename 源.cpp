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

	static void trim(string& str) {//ȥ���ַ���strǰ��Ŀո�
		str.erase(0, str.find_first_not_of(' '));
		return;
	}

	static string rand_str(const int len) {  //������ɳ���Ϊlen���ַ���������Ϊ�ַ����ĳ���
		/*��ʼ��*/
		string str;                 //����������������ַ�����str
		char c;                     //�����ַ�c����������������ɵ��ַ�
		int idx;                    //����ѭ���ı���
		/*ѭ�����ַ��������������ɵ��ַ�*/
		for (idx = 0; idx < len; idx++) {
			//rand()%26��ȡ�࣬����Ϊ0~25����'a',������ĸa~z,���asc���
			c = 'a' + rand() % 26;
			str.push_back(c);       //push_back()��string��β�庯���������������ַ�c
		}
		return str;                 //�������ɵ�����ַ���
	}

};

class Test {
public:
	vector<string> testStep;//״̬ת�Ʊ�
	static void testParser();//���Է�����
	static void testTalk();//���Խ�����
	static void testData();//���������ļ��Ĵ���
	static void testTokens();//���ԼǺŴ���
	static void testGetKey();//���Թؼ�����ȡ
};

class Interpreter {
private:
	int wordNum = 0;
	int UserID = 5;//�û�ID
	int amount = 100;//�û��˵�
	string name;//�û�����
	string curStep;//��ǰ����Step
	vector<string> testWords;//�����õĶԻ����
	vector<string> Log;//��־�ļ�
	string getKeywords(string teststr);//��ȡ�Ի��еĹؼ���
	void getUserInfo(int uid);//��ȡ��ǰ�û�����Ϣ
	void robotTalk(int test);
public:
	friend Test;
	void Talk(int test);//���û��Ի��ĺ���
	void outputLog();//�����־�ļ�
};

class Parser {
private:
	string	curFunc;//��ǰ���ڴ���ĺ���
	void ParseLine(string line);//����һ��
	string ProcessTokens(vector<string> token, int test);//����ÿһ�еļǺŽ��в�ͬ�Ĵ���,test�������λ
	void ProcessStep(string StepName);	//�µĺ���
	void ProcessSpeak(vector<string> words);
protected:
	struct Func {
		vector<pair<int, string>> Speak;	//�����ַ����洢����Ϊ0
		int Listen = 0;		//�Ƿ�Ѱ���û������
		map<string, string> Branch;		//�����ؼ�����Ҫ��ת����һ��
		string Silence;		//�û���Ĭ��Ҫ��ת����һ��
		string Default;		//���������Ҫ��ת����һ��
		int Exit = 0;		//�Ƿ�����Ի�
	};//���庯���Ľṹ��
	map<string, struct Func*> funcs;	//�洢���еĺ���
	map<int, pair<string, int>> data;	//�洢�����ļ�
	set<string> Keywords;		//�洢�ؼ���
	string mainStep;	//������
	void result();		//��ʾ�ű����
public:
	friend Test;
	friend Interpreter;
	void readData(string dataname);//���������ļ�
	void ParseFile(string filename);//����ű��ļ�
	void DeleteData();//�����������
};

Parser parser;//������
Interpreter interpreter;//������
Test TEST;//���Գ���

void Test::testData() {
	parser.readData("data.test");
	ifstream testfile("data.test");
	cout << "============================Test����Data���ԣ�============================" << endl;
	int id;
	int amount;
	string name;
	while (!testfile.eof()) {
		testfile >> id;
		if (parser.data.find(id) == parser.data.end()) {
			cout << "============================Test����Data���ԣ�============================" << endl;
			cout << "=========================ERROR���������ڸ��û�ID��========================" << endl;
			cout << "=========================ERROR����Data����ʧ�ܣ�==========================" << endl;
			break;
		}
		testfile >> name;
		testfile >> amount;
		if (parser.data[id].first != name || parser.data[id].second != amount) {
			cout << "=========================ERROR�����û���Ϣ��ƥ�䣡========================" << endl;
			cout << "=========================ERROR����Data����ʧ�ܣ�==========================" << endl;
		}
		break;
	}
	cout << "==================Test SUCCESS����Data���Գɹ���==========================" << endl;
	for (auto it : parser.data) {//��ӡ��������
		cout << "UserId��" << it.first;
		cout << "		Name��" << it.second.first;
		cout << "		Amount��" << it.second.second << endl;;
	}
	return;
}

void Test::testTokens() {
	vector<vector<string> > testTokens;
	vector<string> temp;
	string tst[] = { "Entry", "File", "Step", "Speak", "Branch", "Listen", "Silence", "Default", "Exit","ERROR" };
	srand(time(NULL));
	int i = 300; 
	cout << "===========================Test����Tokens���ԣ�===========================" << endl;
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
			cout << "=========================ERROR����Tokens����ʧ�ܣ�========================" << endl;
			return;
		}
	}
	cout << "=================Test SUCCESS����Tokens���Գɹ���=========================" << endl;
	return;
}

void Test::testGetKey() {
	string teststr = "ghjoewubvbjkshg";
	string buff = teststr;
	cout << "===========================Test����GetKey���ԣ�===========================" << endl;
	for (auto it : parser.Keywords) {
		string buff2 = buff;
		buff = teststr + it + buff2;
		if (interpreter.getKeywords(buff) != it)
			cout << "=========================ERROR����GetKey����ʧ�ܣ�========================" << endl;
		//	cout << buff + "  " + it + "GetKey����ʧ�ܣ���" << endl;
	}
	cout << "=================Test SUCCESS����GetKey���Գɹ���=========================" << endl;
	return;
}

string Interpreter::getKeywords(string teststr) {
	string wordbuff;
	string key;
	if (teststr == "") {
		cout << "Speak��" ;
		getchar();
		getline(cin, wordbuff);//��ȡ�û�����
	}
	else
		wordbuff = teststr;//�����ַ���
	if (wordbuff == "" ) {
		return "";//�û�û��˵��
	}
	for (int i = 0; i < wordbuff.length() - 1; i++) {
		key.clear();
		key = wordbuff.substr(i, 4);
		if (parser.Keywords.find(key) != parser.Keywords.end()) {//���յ���һ�����õĹؼ���
			return key;
		}
	}
	return "#";//û�н��յ����õĹؼ���
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
	//cout << curStep << endl;				//���ڵ��ԣ���ӡ��ǰ������
	string word = "#";
	if (stepFunc) {
		if (!stepFunc->Speak.empty() && !test) {
			cout << "	";
			for (auto it : stepFunc->Speak) {
				if (it.first == 0)//�þ���Ϊ�ַ�������
					cout << it.second;
				if (it.first == 1)//�þ���Ϊamount����
					cout << amount;
				if (it.first == 2)//�þ���Ϊname����
					cout << name;
				if (it.first == 3)//�þ���ΪUserID����
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
		cout << "=========================ERROR���������߼�����==========================" << endl;
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
			cout << "	����������ID�Ե�¼��" << endl;
			cout << "Speak��";
			cin >> Uid;
			if (parser.data.find(Uid) == parser.data.end())
				cout << "	��������ȷ��ID!" << endl;
			else
				break;
		}
	}
	interpreter.getUserInfo(Uid);//��ȡ��ǰ�û�����Ϣ
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
		cout << "=========================ERROR���������ļ��򿪴���======================" << endl;
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
		if (it[0] == '$') {//�þ����Ǳ���
			string varname;
			varname = it.substr(1, it.size() - 1);
			if (varname == "amount") {
				talk = make_pair(1, varname);//�ñ������˵�
				funcs[curFunc]->Speak.push_back(talk);
			}
			else if (varname == "name") {
				talk = make_pair(2, varname);//�ñ���������
				funcs[curFunc]->Speak.push_back(talk);
			}
			else if (varname == "number") {
				talk = make_pair(3, varname);//�ñ�����ID
				funcs[curFunc]->Speak.push_back(talk);
			}
			else {
				cout << "=========================ERROR����������δ������==========================" << endl;
				return;
			}
		}
		else if (it[0] == '"') {//�þ������ַ�������
			int len = it.size();
			if (it[len - 1] != '"') {
				cout << "=========================ERROR��������ƥ�䷢������======================" << endl;
				return;
			}
			else {
				string buff = it.substr(1, len - 2);
				talk = make_pair(0, buff);	//����0Ϊ�����ַ���
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
			cout << "=========================ERROR��������Դ����============================" << endl;
		else
			return "ERROR";
	}
	return "";
}

void Parser::ParseLine(string line) {
	vector<string> token;
	token.clear();
	string buff;//��¼һ��token�Ļ�����       
	for (auto it : line) {
		if (it == '#') {	//#��ʾ��β�������
			token.push_back(buff);
			break;
		}
		else if (it == ' ' && !buff.empty()) {	//��ʾһ��token��������ñ�ʶ��
			token.push_back(buff);
			buff.clear();
		}
		else {
			buff += it;
		}
	}
	ProcessTokens(token, 0);//0����ǲ���
	return;
}

void Parser::ParseFile(string filename) {
	ifstream inputFile(filename);
	string buff;//����ű��ļ��Ļ�����
	while (!inputFile.eof()) {
		buff.clear();
		getline(inputFile, buff);
		Tools::trim(buff);	//ɾ�����׿հ�
		if (buff[0] != '#' && !buff.empty()) {	//��ʾ���в���ע�����Ҳ��ǿ���
			buff += '#';	//��Ϊÿһ�еĽ�����־
			ParseLine(buff);
		}
	}
	if (parser.mainStep == "") {
		cout << "=========================ERROR����δ����������==========================" << endl;
	}
	if (parser.data.empty()) {
		cout << "=========================ERROR����δ���������ļ���========================" << endl;
	}
	inputFile.close();
	return;
}

void Parser::result() {
	cout << "=========================Steps�������к������£�==========================" << endl;
	for (auto it : funcs) {
		cout << "====������" << it.first << endl;
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
	cout << "====��������" << mainStep << endl;
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
	cout << "===========================Test����Parser���ԣ�===========================" << endl;
	int len = rand() % 20 + 1;//����������1~20
	string entry = Tools::rand_str(len);
	string exit = Tools::rand_str(len);
	tokens.push_back("Entry " + entry);
	tokens.push_back("File data.txt");
	int Nstep = rand() % 10 + 10;//���������ĺ�������Ϊ10~20��
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
			len = rand() % 20 + 1;//����������1~20
			step = Tools::rand_str(len);
		}
		tokens.push_back("Step " + step);
		int flag = rand() % 4;
		switch (flag) {
		case 0:
			tokens.push_back("    Speak $name + \"���ã�������ʲô���԰�����\"");
			break;
		case 1:
			tokens.push_back("    Speak \"�����µ��˵���\" + $amount  +  \"Ԫ��\"");
			break;
		case 2:
			tokens.push_back("    Speak \"��л�������磬�ټ���\"");
			break;
		case 3:
			tokens.push_back("    Speak \"������˼û���壬������˵һ�顣\"");
			break;
		default:
			tokens.push_back("    Speak \"����IDΪ��\" + $number");
		}
		flag = rand() % 3;
		switch (flag) {
		case 0:
			tokens.push_back("    Branch �˵� " + laststep);
			break;
		case 1:
			tokens.push_back("    Branch ���� " + laststep);
			break;
		default:
			tokens.push_back("    Branch ���� " + laststep);
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
	parser.result();	//��ʾ�ű��ļ�������
	cout << "=================Test SUCCESS����Parser���Գɹ���=========================" << endl;
	parser.DeleteData();
	return;
}

void Test::testTalk() {
	vector<string> original;
	srand(time(NULL));
	cout << "============================Test����Talk���ԣ�============================" << endl;
	interpreter.testWords.clear();
	interpreter.wordNum = 0;
	TEST.testStep.clear();
	int flag;
	int i = 0;
	while (i < 20) {
		flag = rand()%4;
		switch (flag) {
		case 0:
			interpreter.testWords.push_back("���ã���Ҫ���˵���");
			original.push_back("bill");
			original.push_back("welcome");
			break;
		case 1:
			interpreter.testWords.push_back("���ã���Ҫѧϰ��");
			original.push_back("default");
			original.push_back("welcome");
			break;
		case 2:
			interpreter.testWords.push_back("���ã��ҵ��û������Ƕ��٣�");
			original.push_back("number");
			original.push_back("welcome");
			break;
		case 3:
			interpreter.testWords.push_back("���ã���ҪͶ�ߣ�");
			original.push_back("complaint");
			original.push_back("welcome");
			break;
		}
		cout << "	Talk����	" + interpreter.testWords[interpreter.testWords.size()-1] << endl;
		i++;
	}
	interpreter.testWords.push_back("#");
	original.push_back("silence");
	interpreter.Talk(1);//���в���
	for (i = 0; i < original.size() && i < TEST.testStep.size(); i++) {
		if (original[i] != TEST.testStep[i]) {
			cout << "======================Test  FAILED����Talk����ʧ�ܣ�======================" << endl;
			cout << "	ԭ��״̬ӦΪ��" + original[i] + "����״̬Ϊ��" + TEST.testStep[i] << endl;
			break;
		}
		else
			cout << "	Talk����	" + TEST.testStep[i] << endl;
	}
	if(!(i < original.size() && i < TEST.testStep.size()))
		cout << "======================Test SUCCESS����Talk���Գɹ���======================" << endl;
	return;
}

int main() {
	//Test::testData();		//�����Ƿ������ȷ����
	//system("pause");
	//Test::testTokens();	//�����Ƿ���Խ��ǺŴ�ת��Ϊ��Ӧ�ĺ���
	//system("pause");
	//Test::testParser();	//������������������ӡ�﷨��
	//system("pause");
	parser.ParseFile("script.txt");//����ű��ļ�
	//Test::testGetKey();	//�����Ƿ������ȷ����ȡ��һ�仰�Ĺؼ���
	//system("pause");
	//Test::testTalk();	//ģ���˻��Ի�������������������
	//system("pause");
	while (1) {
		interpreter.Talk(0);//0��ʾ�ǲ���
		//system("pause");
	}
	parser.DeleteData();//����ڶ��з�������ݣ������������
	system("pause");
	return 0;
}