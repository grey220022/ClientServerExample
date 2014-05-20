#include "Document.h"
#include <iostream>
#include <errno.h>
#include "time.h"

using namespace std;


typedef struct {
   int threadid;
}type_arg;

int getlang(string lang)
{
	if (lang=="fra")
	  return 0;
	return 1;
}

inline int s2i(string p)
{
	int re;
	stringstream(p) >> re;
	return re;
}


string Document::entry(string query, string langp)
{
	lang=getlang(langp);
	cout<<lang<<endl;

	string re="";
	if (query.substr(0,3)=="del")
	{
		cout<<query<<endl;
		deldoc(s2i(query.substr(query.find(":")+1)));
	}
	else if (query.substr(0,3)=="upd")
	{
		upddoc(query.substr(query.find(":")+1));
	}
	else if (query.substr(0,6)=="search")
	{
		re=search(query.substr(query.find(":")+1));
	}
	return re;
}




int Document::date()
{
	time_t now = time(NULL);
    struct tm *tnow = localtime( &now );
	struct std::tm b = {0,0,0,1,1,104};
    return difftime(mktime(tnow), mktime(&b)) / (60 * 60 * 24)+731612;
}


void Document::upddoc(string strline)
{
	  cout<<strline<<endl;
	  vector<string> strs;
	  boost::split(strs,strline,boost::is_any_of("{"));
	  for (int i=3; i<12; i++)
	  {
		  boost::to_upper(strs[i]);
	  }
	  cout<<s2i(strs[0])<<endl;
	  arr[lang][s2i(strs[0])]=strs;
}

void Document::deldoc(int docid)
{
	arr[0][docid]=vector<string>();
	arr[1][docid]=vector<string>();
}

Document::Document()
{
	std::vector<std::string> strs;
	string blankstring;
	ifstream fin("conf.txt");
	if (!fin.good())
		cout<<"conf read error"<<endl;
	std::getline(fin,blankstring);
	std::getline(fin,blankstring);
	for(int i=0;i<9;i++)
		fin>>weight[i];
	for(int i=0;i<9;i++)
		fin>>reweight[i];
	fin>>basedir;
	cout<<"Reading file"<<endl;
	fin=ifstream("mfra.txt");
	getline(fin,strline);
	while (1)
	{	
	  getline(fin,strline);
	  if (!fin.good())
		  break;	
	  boost::split(strs,strline,boost::is_any_of("{"));
	  arr[0][s2i(strs[0])]=strs;
	}


	fin=ifstream("meng.txt");
	getline(fin,strline);
	while (1)
	{	
	  getline(fin,strline);
	  if (!fin.good())
		  break;	
	  boost::split(strs,strline,boost::is_any_of("{"));
	  arr[1][s2i(strs[0])]=strs;
	}
	cout<<"finish reading file"<<endl;
}

vector<string> split(string s)
{
	vector<string> re;
    int S = s.size();
    int  i = 0;
    while (i < S) {
        while ((i < S) && (!isalnum(s[i])))  ++i;
        if (i == S)  return re;
        int  j = i+1;
        while ((j < S) && (isalnum(s[j])))  ++j;
        re.push_back(s.substr(i,j-i));
        i = j+1;
    }
	return re;
}


set<string> splitset(string s)
{
	set<string> re;
    int S = s.size();
    int  i = 0;
    while (i < S) {
        while ((i < S) && (!isalnum(s[i])))  ++i;
        if (i == S)  return re;
        int  j = i+1;
        while ((j < S) && (isalnum(s[j])))  ++j;
        re.insert(s.substr(i,j-i));
        i = j+1;
    }
	return re;
}


/*
Document * Document::get_instance()
{
	return document_ins;
}*/



void Document::subthreadpertcal(void *param)
{
	type_arg *args = (type_arg*) param;
	int score,ii,ss,jj;
	bool flag;
	string ttstr;
	//Document  *dd = &document;//=document_ins;


	cout<<args->threadid<<" begins"<<endl;

	for (int rowid=args->threadid;rowid<1000000;rowid+=4)
		{			
			if ((arr[lang][rowid].size()!=15)||(!alltbname)&&(tbname.find(arr[lang][rowid][1])==tbname.end())||(!allset)&&(setid.find(arr[lang][rowid][0])==setid.end()))
			{
				//if (rowid<100)
				//cout<<rowid<<" was filtered"<<endl;
				fullscore[rowid]=-10000;
				continue;
			}
			set<int> match_set;
			score=0;
			for (int k=0;k<qw.size();k++)
			{
			  for (int i=0; i<9; i++)
			  {
				flag=true;
				ii=0;
				ttstr=arr[lang][rowid][i+3];
				ss=ttstr.size();
				while (ii < ss) {
				  while ((ii <ss) && (!isalnum(ttstr[ii])))  ++ii;
				  if (ii == ss)  break;
                  jj = ii+1;
                  while ((jj < ss) && (isalnum(ttstr[jj])))  ++jj;
                    if (ttstr.substr(ii,jj-ii)==qw[k]){
					  match_set.insert(k);
					  if (flag) {
					    score+=weight[i];
						if (reweight[i]==0)
						  break;
						flag=false;
						}
					    else score+=reweight[i];
					}
                  ii = jj+1;
				}
			  }
			}			
			score+=match_set.size()*100000;
			score+=(s2i(arr[lang][rowid][12])+min(max((s2i(arr[lang][rowid][13])-curdate)/3,-500),0)+min(max((s2i(arr[lang][rowid][14])-curdate)/3,-500),0));
			fullscore[rowid]=score;
			//scoreresult.push_back(make_pair<int, int>(score,rowid));
		}
}

string Document::search(string query)
{
		int displaynumber;
		string tbnamelist,setidstring;
		//ifstream fin(basedir+"searchparams");
		ifstream fin("searchparams");
		std::getline(fin,tbnamelist);
		std::getline(fin,setidstring);



		//tbname=std::set<std::string>(split(tbnamelist));
		//tbname.clear();

		//std::set<std::string> s(v.begin(), v.end());

		//std::set<string> tbname=std::set<string>(split(tbnamelist));

	    //memset(fullscore, -10000, sizeof(int)*1000000);
		cout<<"tbname chosen:"<<tbnamelist<<endl;
		//std::set<string> myset;
		alltbname=allset=false;
		if (tbnamelist=="ALL")
			alltbname=true;
		else
			tbname=splitset(tbnamelist);

		if (setidstring=="ALL")
			allset=true;
		else
		{
			setid=splitset(setidstring);
		    cout<<setid.size()<<": set size"<<endl;
		}
		cout<<"search : "<<query<<endl;
		curdate=date();
		string re="";
		scoreresult.clear();
		boost::to_upper(query);
		qw=split(query);
		for (int k=0;k<qw.size();k++)
		{
			cout<<qw[k]<<endl;
		}

		type_arg * arg1;
		type_arg * arg2;
		type_arg * arg3;
		type_arg * arg4;
		
		arg1 = (type_arg *)malloc(sizeof(type_arg));
		  
		
		arg2 = (type_arg *)malloc(sizeof(type_arg));
		arg3 = (type_arg *)malloc(sizeof(type_arg));
		arg4 = (type_arg *)malloc(sizeof(type_arg));

		arg1->threadid=0;
		arg2->threadid=1;
		arg3->threadid=2;
		arg4->threadid=3;

		cout<<"begin multithreading";

		clock_t start, finish;
		double  duration;
		start = clock();
		HANDLE handle1=(HANDLE) _beginthread( &Document::subthreadpertcal, 0, (void*) arg1);
		//printf("Unable to create thread 1, errno = %d\n",errno);
		HANDLE handle2=(HANDLE) _beginthread( &Document::subthreadpertcal, 0, (void*) arg2);
		//printf("Unable to create thread 2, errno = %d\n",errno);
		HANDLE handle3=(HANDLE) _beginthread( &Document::subthreadpertcal, 0, (void*) arg3);
		//printf("Unable to create thread 3, errno = %d\n",errno);
		HANDLE handle4=(HANDLE) _beginthread( &Document::subthreadpertcal, 0, (void*) arg4);
		//printf("Unable to create thread 4, errno = %d\n",errno);
		WaitForSingleObject( handle1, INFINITE );
		WaitForSingleObject( handle2, INFINITE );
		WaitForSingleObject( handle3, INFINITE );
		WaitForSingleObject( handle4, INFINITE );
		
		finish = clock();
		duration = (double)(finish - start) / CLOCKS_PER_SEC;
        printf( "%f seconds\n", duration );

		for (int rowid=0;rowid<1000000;rowid++)
		{			
			if (fullscore[rowid]!=-10000)//(arr[lang][rowid].size()==15)
			{
				scoreresult.push_back(make_pair<int, int>(fullscore[rowid],rowid));
			}
		}

		sort(scoreresult.begin(),scoreresult.end());

		displaynumber=min(120,(int)scoreresult.size());
		for (int i=1; i<=displaynumber; i++)
		{
		    	re.append(std::to_string(static_cast<long long>(scoreresult[scoreresult.size()-i].second))+' '); 
		}
		cout<<re<<endl;
		cout<<"___________________________________________________________________"<<endl;
		for (int i=1; i<=displaynumber; i++)
		{
			    cout<< scoreresult[scoreresult.size()-i].second<<"   "<<scoreresult[scoreresult.size()-i].first<<endl;
		}
		return re;
}