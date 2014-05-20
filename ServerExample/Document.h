#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <boost/regex.hpp> 
#include <boost/algorithm/string.hpp>
#include <boost/pending/stringtok.hpp>
#include <boost/tokenizer.hpp>
#include <boost/pending/stringtok.hpp>
#include <boost/asio.hpp>
#include <map>
#include <sstream>
#include <set>

static int weight[]={500,500,500,100,500,100,100,500,500,1000};
static std::string colname[]={"docid","tbname","commentaires","personnes","descmc","titre","resume","modelepneu","legende","sommaire","metier","performance","cotedamour","datedebprod","webdate"};
static int reweight[]={0,0,0,100,0,100,100,0,0,0};
static int curdate;
static int fullscore[1000000];
static std::string basedir;

static std::vector <std::string> arr[2][1000000];
//static std::string query;
//static bool flag;
static 	std::vector<std::pair<int, int> > scoreresult;
static   std::vector<std::string> qw;
static   std::set<std::string> tbname,setid;
static int lang;
static bool alltbname;
static bool allset;


class Document
{
private:
	//int stweight[1000000];
	
	//std::vector<std::string>::iterator ll;
	
	
	//std::multimap<std::string, int> total_map;
	//std::multimap<std::string, int>::iterator it;
	//int num;
	
	
	std::ifstream fin;
	std::string strline;
	//static Document * document_ins;

public:
	Document();
	
	//inline int s2i(std::string p);
	int date();
	void upddoc(std::string strline);
	void deldoc(int docid);
	std::string search(std::string query);
	std::string entry(std::string query, std::string langp="fra");
	static void subthreadpertcal(void *param);
	//static Document * get_instance();
	//static int lang;
};

//int Document::lang=0;
