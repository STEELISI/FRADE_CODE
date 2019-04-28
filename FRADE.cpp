#include <unordered_map>
#include <unordered_set>
#include <sys/time.h>
#include <ctime>
#include <cstdio>
#include <exception>
#include <memory>
#include <cstdlib>
#include <iostream>
#include <arpa/inet.h>
#include <stdexcept> // For out_of_range
#include <utility> // For pair
#include <string>
#include <fstream>
#include <sstream> // For the split helper function
#include <vector> // For the split helper function
#include <boost/regex.hpp> // Boost is best
#include "FRADE.h" // Fo' shizzle
#include <boost/algorithm/string/predicate.hpp>
#include <ctime>
using namespace std;

#define DEBUG 1

long req_count=0;
long start_req =0;
long end_req =0;
//Wikipedia
//string deceptionfile = "/var/www/html/dummy.html";
//string apache_doc_root = ""; 
///var/www/html/mediawiki";

//Imgur
//string deceptionfile = "/var/www/html/imgur/dummy.html";
//string apache_doc_root = "/var/www/html/imgur";

//Reddit
//string deceptionfile = "/home/rajattan/src/reddit/r2/build/public/static/dummy.html";
//string apache_doc_root = "/home/rajattan/src/reddit/r2/build/public/static";

void read_dyn_threshes(const string &s, char delim, unsigned int *array) {
    stringstream ss(s);
    string item;
    int i = 0;
    while (getline(ss, item, delim)) {
      array[i] = stoul(item); 
      i++;
    }
}

void split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

/**
 A helper function for figuring out why a regex object threw an error.
 Taken from: http://stackoverflow.com/questions/14776605/c-11-regex-error
string parseCode(regex_constants::error_type etype) {
    switch (etype) {
    case regex_constants::error_collate:
        return "error_collate: invalid collating element request";
    case regex_constants::error_ctype:
        return "error_ctype: invalid character class";
    case regex_constants::error_escape:
        return "error_escape: invalid escape character or trailing escape";
    case regex_constants::error_backref:
        return "error_backref: invalid back reference";
    case regex_constants::error_brack:
        return "error_brack: mismatched bracket([ or ])";
    case regex_constants::error_paren:
        return "error_paren: mismatched parentheses(( or ))";
    case regex_constants::error_brace:
        return "error_brace: mismatched brace({ or })";
    case regex_constants::error_badbrace:
        return "error_badbrace: invalid range inside a { }";
    case regex_constants::error_range:
        return "erro_range: invalid character range(e.g., [z-a])";
    case regex_constants::error_space:
        return "error_space: insufficient memory to handle this regular expression";
    case regex_constants::error_badrepeat:
        return "error_badrepeat: a repetition character (*, ?, +, or {) was not preceded by a valid regular expression";
    case regex_constants::error_complexity:
        return "error_complexity: the requested match is too complex";
    case regex_constants::error_stack:
        return "error_stack: insufficient memory to evaluate a match";
    default:
        return "";
    }
}
*/

FRADE::FRADE(string config_file, int modules) {
  configure(config_file,modules);
    }

FRADE::FRADE()
{
}

void FRADE::configure(string config_file, int modules) {

  ifstream cfile(config_file);
  activemodules = modules;
  if (!cfile) {
    cerr << "Unable to open configuration file: " << config_file << ".\n";
    cerr << "If this file path is incorrect, you should change it in main.cpp.\n";
  }
  unordered_map<string, string> config;
  int eqIndex, confIndex;
  string name, value, line, path; 

  // Remember path to configuration file since other files are listed
  // relative to this path
  confIndex = config_file.rfind("/");
  if (confIndex == -1)
    path = "./";
  else
    path = string(config_file.substr(0,confIndex+1));

  cout << "Path "<< path << endl;


  while (getline(cfile, line)) {
    if (line[0] == '\n' || line[0] == '#') {
      // comment
      continue;
    }

    eqIndex = line.find("=");
    name = line.substr(0, eqIndex);
    value = line.substr(eqIndex + 1);
    config.insert(make_pair(name, value));
  }
  cfile.close();

  cout << "Read config file "<< endl;
  // Ensure we have all the necessary parameters
  string dyn_conf_file, sem_conf_file, windows_string, folders_string, main_reqs_string, dummy_links_file, group_file = "";
  try {
    cout<<endl<<"Here";
    dyn_conf_file = path + config.at("DYN_CONF_FILE");
  } catch (const out_of_range e) {
    cerr << "DYN_CONF_FILE not set. Set it in " << config_file << ".\n";
    exit(1);
  }
  try {
    sem_conf_file = path + config.at("SEM_CONF_FILE");
  } catch (const out_of_range e) {
    cerr << "SEM_CONF_FILE not set. Set it in " << config_file << ".\n";
    exit(1);
  }
  try {
    dummy_links_file = path + config.at("DUMMY_LINKS_FILE");
  } catch (const out_of_range e) {
    cerr << "DUMMY_LINKS_FILE not set. Set it in " << config_file << ".\n";
    exit(1);
  }
  try {
    windows_string = config.at("WINDOWS");
  } catch (const out_of_range e) {
    cerr << "WINDOWS not set. Set it in " << config_file << ".\n";
    exit(1);
  }
  try {
    folders_string = config.at("FOLDERS");
  } catch (const out_of_range e) {
    cerr << "FOLDERS not set. FRADE will only model based off files.\n";
    cerr << "You can set it in " << config_file << ".\n";
    folders_string = "";
  }
  try {
    main_reqs_string = config.at("MAIN_REQS");
  } catch (const out_of_range e) {
    cerr << "MAIN_REQS not set. Set it in " << config_file << ".\n";
    exit(1);
  }
  try {
     NO_FILE_PROB= stod(config.at("NO_FILE_PROB"));
  } catch (const out_of_range e) {
    cerr << "NO_FILE_PROB not set. Set it in " << config_file << ".\n";
    exit(1);
  }
  cout << "Read files "<< endl;
  try {
    blacklistpipe = config.at("PIPE");
    pipe.open(blacklistpipe); //Read the name of pipe from conf file
  } catch (const out_of_range e) {
    cerr << "PIPE not set. Set it in " << config_file << ".\n";
    exit(1);
  }
  try {
     group_file= path + config.at("GROUP_FILE");
     cout<<endl<<group_file<<"Here";
  } catch (const out_of_range e) {
    // Ignore it
    //cerr << "GROUP_FILE not set. Set it in " << config_file << ".\n";
    //exit(1);

  }
  cout << "Opened pipe "<< endl;
  string dyn2conf = path + config.at("DYN2_DETAILS");
  string dyn3conf = path + config.at("DYN3_DETAILS");

  ifstream d2cfile(dyn2conf);
  ifstream d3cfile(dyn3conf);

  string d2line;
  vector<string> splitline;
  while (getline(d2cfile, d2line)) {
    splitline.clear();
    d2line.pop_back(); //to remove the \n at the end of line
    split(d2line, ' ', splitline);
    int sz = splitline.size();
    for(int f=1;f<sz; f++){
      main_embed[splitline[0]].push_back(splitline[f]);
    }
  }
  d2cfile.close();

  string d3line;
  while(getline(d3cfile,d3line)) {
    splitline.clear();
    d3line.pop_back();
    split(d3line, ' ',splitline);
    //cout<<endl<<splitline[0]<<" "<<splitline[1] <<" Line ";
    proc_time[splitline[0]] = stod(splitline[1]);
  }
  d3cfile.close();


  // Dummy links
  ifstream dummyfile(dummy_links_file);
  string linkline;
  while(getline(dummyfile, linkline)) {
    //cout << " Dummy link "<<linkline << endl;
    dummy_links[linkline] = 1;
  }

  // Dynamics data 
  vector<string> tokens;
  split(windows_string, ' ', tokens);
  // + 1 for the 0-terminator
  windows = (unsigned int*) malloc(sizeof(unsigned int)*tokens.size());
  dyn1Threshes = (unsigned int*) malloc(sizeof(unsigned int)*tokens.size());
  dyn2Threshes = (unsigned int*) malloc(sizeof(unsigned int)*tokens.size());
  dyn3Threshes = (unsigned int*) malloc(sizeof(unsigned int)*tokens.size());
  
  numWindows = tokens.size();
  for (unsigned int i = 0; i < tokens.size(); i++) {
    windows[i] = stoul(tokens[i]);
  } 
  ifstream dfile(dyn_conf_file);
  getline(dfile, line);
  read_dyn_threshes(line, ' ', dyn1Threshes);
  getline(dfile, line);
  read_dyn_threshes(line, ' ', dyn2Threshes);
  getline(dfile, line);
  read_dyn_threshes(line, ' ', dyn3Threshes);
  dfile.close();  

  if (group_file != "")
    {
      //cout<<"Rchd Group File"<<endl;
      ifstream gfile(group_file);
      // First line is the sequence length thresholds

  string grp;
  vector<string> splitline;
  while(getline(gfile,grp)) {
    splitline.clear();
    //grp.pop_back();
    split(grp,' ',splitline);
    //cout<<endl<<splitline[0]<<" "<<splitline[1] <<" Line ";
    groups.insert(pair<string, string>(splitline[1],splitline[0]));
    //cout<<endl<<groups[splitline[1]];
    //proc_time[splitline[0]] = stod(splitline[1]);
  }
  gfile.close();





      /*getline(gfile, line);
      tokens.clear();
      split(line, ' ', tokens);
      cout<<endl<<"Rchd Group File End"<<endl; 
      groups.insert(pair<string, string>(tokens[1],tokens[0]));*/
    }

  
  ifstream sfile(sem_conf_file);
  // First line is the sequence length thresholds
  getline(sfile, line);
  tokens.clear();
  split(line, ' ', tokens); 
  seqThreshes = (double*) malloc(sizeof(double)*tokens.size());
  maxSeqLen = tokens.size();
  for (int i = 0; i < maxSeqLen; i++) {
    seqThreshes[i] = stod(tokens[i]);
  }

  getline(sfile, line); // chomp new line

  // Now the transition probabilities
  SemNode *start, *end;
  while (getline(sfile, line)) {
    tokens.clear();
    split(line, ' ', tokens);
    // tokens[0]: the first file
    // tokens[1]: the second file
    // tokens[2]: the transition probability
    // tokens[3]: either "True" or "False". False indicates that the first file
    //            is actually a folder
    // tokens[4]: either "True" or "False". False inficates that the second file
    //            is actually a folder
    //if (tokens[3].find("True") == string::npos) {
      // We denote a folder node by replacing the leading '/' with an 'f'
      // hopefully this doesn't come back to bite us
      //tokens[0][0] = 'f';
    //}
    //if (tokens[4].find("True") == string::npos) {
      // We denote a folder node by replacing the leading '/' with an 'f'
      //tokens[1][0] = 'f';
    //}
    try {
      start = semNodes.at(tokens[0]);
    } catch (const out_of_range e) {
      start = new SemNode(tokens[0]);
      semNodes.insert(make_pair(tokens[0], start));
    }
    try {
      end = semNodes.at(tokens[1]);
    } catch (const out_of_range e) {
      end = new SemNode(tokens[1]);
      semNodes.insert(make_pair(tokens[1], end));
    }
    start->addEdge(stof(tokens[2]), end);
    //end->addEdge(stof(tokens[2]), start);
  }
  sfile.close();

  // Read in the regexes for folders and main requests
  tokens.clear();
  split(main_reqs_string, ' ', tokens);
  // + 1 for null terminator
  mainReqs = (boost::regex**) malloc(sizeof(boost::regex*)*(tokens.size() + 1));
  cerr << "Making regexes" << endl;
  for (unsigned int i = 0; i < tokens.size(); i++) {
    cerr << "Making main req regex: " << tokens[i] << endl;
    mainReqs[i] = new boost::regex(tokens[i]);
  }
  mainReqs[tokens.size()] = NULL;

  if (!folders_string.empty()) {
    tokens.clear();
    split(folders_string, ' ', tokens);
    folders = (boost::regex**) malloc(sizeof(boost::regex*)*(tokens.size() + 1));
    for (unsigned int i = 0; i < tokens.size(); i++) {
      cerr << "Making folder regex: " << tokens[i] << endl;
      folders[i] = new boost::regex(tokens[i]);
    }
    folders[tokens.size()] = NULL;
  } else {
    folders = (boost::regex**) malloc(sizeof(boost::regex*));
    folders[0] = NULL;
  }
#ifdef DEBUG
  cerr << "Done loading config" << endl;
#endif
}

string arrayToStr(unsigned int *array, int len) {
  string ret;
  for (int i = 0; i < len; i++) {
    ret.append(to_string(array[i]).append(" "));
  }
  return ret;
}

void FRADE::dumpData() {
  cout << "[Dynamics]" << endl;
  cout << "No Transition Prob: " << NO_FILE_PROB << endl;
  cout << "Windows: " << arrayToStr(windows, numWindows) << endl;
  cout << "Dyn1: " << arrayToStr(dyn1Threshes, numWindows) << endl;
  cout << "Dyn2: " << arrayToStr(dyn2Threshes, numWindows) << endl;

  cout << endl << "[Transition Probabilities]" << endl;

  cout << endl << "Sequence thresholds: " << endl;
  for (int i = 0; i < maxSeqLen; i++) {
    cout << i << ": " << seqThreshes[i] << endl;
  }

  unordered_map<unsigned int, UserData*>::iterator it2;
  cout << endl << "[User Data]" << endl;
  char ipstr[INET_ADDRSTRLEN];
  for (it2 = users.begin(); it2 != users.end(); it2++) {
    inet_ntop(AF_INET, &it2->first, ipstr, INET_ADDRSTRLEN);
    cout << ipstr << endl;
    cout << "Seq Prob: " << it2->second->seqProb << endl;
    cout << "Prev File: " << *it2->second->prevFile << endl;
    // TODO: output the rest of user data
  }

  unordered_set<unsigned int>::iterator it3;
  cout << endl << "[Blacklist]" << endl;
  for (it3 = blacklist.begin(); it3 != blacklist.end(); it3++) { 
    unsigned int ip = *it3;
    inet_ntop(AF_INET, &ip, ipstr, INET_ADDRSTRLEN);
    cout << ipstr << endl;
  }
}

/**
  Helper function to tell us if the given file (represented as a uri string) is located
  in a folder that we are modeling.
  @return: the length of the substring of the folder uri. For example:
  if the file uri is "/foo/bar/baz.html", and we are modeling the "/foo" folder, the
  function would return 4. If the file uri does not a contain a folder we care about, it
  returns -1.
  Note: we only need the length becuase the start index will always be 0.
*/
int FRADE::hasFolder(string *uri) {
  boost::smatch m;
  for (int i = 0; folders[i] != NULL; i++) {
    cout << " Looking for " << *uri << " folder "<< *folders[i] << endl;
    string urislash = *uri + "/";
    if (boost::regex_match(*uri, m, *folders[i]) || boost::regex_match(urislash, m, *folders[i])) {
#ifdef DEBUG
      cerr << "found " << folders[i]->str() << " in " << *uri << endl;
#endif
      // m[0].first is the starting index of the match
      // m[1].second is the end index of the match
      return m.length();
    }
  }
  return -1;
}

double FRADE::getTransitionProb(UserData *ud, string *end) {
  // Use the endNode that we saved from the last transition lookup if we have it
  SemNode *start;
  bool fellback = false;
  int startLength;
  if (ud->endNode != NULL) {
    start = ud->endNode;
#ifdef DEBUG
    cerr << "using endNode of user data. File:" << endl;
    cerr << start->uri << endl;
#endif
  } else {
    try {
      start = semNodes.at(*ud->prevFile);
#ifdef DEBUG
      cerr << " found start node for " << ud->ip << endl;
#endif
    } catch (const out_of_range e) {
      // First node wasn't found, fallback
      startLength = hasFolder(ud->prevFile);
      // fallback is the length of the substring that represents the file which always
      // begins at 0, or -1 if a fallback was not found
#ifdef DEBUG
      cerr << *ud->prevFile << " not found. falling back" << endl;
#endif
      if (startLength == -1) {
#ifdef DEBUG
        cerr << "fallback not found for start. no transitions found" << endl;
#endif
        ud->prevFile = end;
        ud->endNode = NULL;
        return NO_FILE_PROB;
      }
      fellback = true;
#ifdef DEBUG
      cerr << "fell back to: " << ud->prevFile->substr(0, startLength) << endl;
#endif
      try {
        start = semNodes.at(ud->prevFile->substr(0, startLength));
      } catch (const out_of_range e) {
#ifdef DEBUG
        cerr << "fall back uri not found in semantic tree. no transitions found" << endl;
#endif
        ud->prevFile = end;
        ud->endNode = NULL;
        return NO_FILE_PROB;
      }
    }
  }
  // Now start points to the start node in this transition
  // if we had to, start will point to the fallback folder
  ud->prevFile = end;
  // First we check if trans to end file exists. 
  SemNode *endNode;
  double prob = start->transTo(end, &endNode);
  if (prob != 0) {
#ifdef DEBUG
    cerr << "found ";
    if (fellback) cerr << "folder ";
    else cerr << "file ";
    cerr << "-> file" << endl;
    cerr << start->uri << " -> " << endNode->uri << endl;
#endif
    ud->endNode = endNode;
    return prob;
  }
  // Oh dang, not found. Check if trans to folder exists
#ifdef DEBUG
  cerr << "could not find ";
  if (fellback) cerr << "folder ";
  else cerr << "file ";
  cerr << "-> file" << "for " << ud->ip << endl;
#endif
  int endLength = hasFolder(end);
  if (endLength == -1 && fellback) {
#ifdef DEBUG
    cerr << "could not fall back for end, and aready fell back for start. no transitions found" << " for " << ud->ip << endl;
#endif
    ud->endNode = NULL;
    return NO_FILE_PROB;
  }
  // first see if end will fallback 
  string endFolder;
  if (endLength != -1) {
  // There was a folder, and we may or may not have fallen back already. Check if the transition exists
    endFolder = end->substr(0, endLength);
    prob = start->transTo(&endFolder, &endNode);
#ifdef DEBUG
    cerr << "found fall back for end: " << endFolder << endl;
    cerr << "endLength = " << endLength << endl;
#endif
    if (prob != 0) {
#ifdef DEBUG
      cerr << "found ";
      if (fellback) cerr << "folder ";
      else cerr << "file ";
      cerr << "-> file" << endl;
      cerr << start->uri << " -> " << endFolder << " for " << ud->ip <<endl;
#endif
      ud->endNode = NULL;
      return prob;
    }
    else
      {
	cerr << " no prob found  for " << start->uri << " to " << *end << endl;
      }
  }
  // At this point two cases have failed: start -> {file,folder}
  // now we check if we can have start fallback
  if (!fellback) {
    // start hasn't fallen back yet. check if folder -> {file,folder} exists
#ifdef DEBUG
    cerr << "could not find file -> {file,folder}. falling back start" << " for " << ud->ip << endl;
#endif
    startLength = hasFolder(&start->uri);
    if (startLength == -1) {
#ifdef DEBUG
      cerr << "could not fallback start. transition not found" << " for " << ud->ip << endl;
#endif
      // file -> file and file -> folder not found, and start can't fallback
      ud->endNode = NULL;
      return NO_FILE_PROB;
    }
#ifdef DEBUG
    cerr << "found fallback for start: " << start->uri.substr(0, startLength) << " for " << ud->ip << endl;
#endif
    try {
      start = semNodes.at(start->uri.substr(0, startLength));
    } catch (const out_of_range e) {
#ifdef DEBUG
      cerr << "could not find node entry for: " << start->uri.substr(0, startLength) << endl;
      cerr << "transition not found" << " for " << ud->ip << endl;
#endif
      ud->endNode = NULL;
      return NO_FILE_PROB;
    }
    // folder -> file test
    prob = start->transTo(end, &endNode);
    if (prob != 0) {
#ifdef DEBUG
      cerr << "found folder -> file transition: " << endl;
      cerr << start->uri << " -> " << end << " for " << ud->ip << endl;
#endif
      ud->endNode = endNode;
      return prob;
    }
    // folder -> folder test
    endFolder = end->substr(0, endLength);
    prob = start->transTo(&endFolder, &endNode);
    if (prob != 0) {
#ifdef DEBUG
      cerr << "found folder -> folder transition: " << " for " << ud->ip << endl;
      cerr << start->uri << " -> " << endFolder << endl;
#endif
      ud->endNode = NULL;
      return prob;
    } else {
#ifdef DEBUG
      cerr << "could not find folder -> {file,folder}. no transition found" << " for " << ud->ip << endl;
#endif
      ud->endNode = NULL;
      return NO_FILE_PROB;
    }
  } else {
    // start points to a folder node, and
    // we didn't have a folder -> file or folder -> folder
    ud->endNode = NULL;
    return NO_FILE_PROB;
  }
}



double FRADE::getTransitionProb2(UserData *ud, string *end) {
  // Use the endNode that we saved from the last transition lookup if we have it
  SemNode *start, *endNode;
  if (ud->endNode != NULL) {
    start = ud->endNode;
    cout << "Looking for transition " << start->uri <<  " to " << *end << " for " << ud->ip << endl;
  } else {
    // First file, no transition yet
    ud->prevFile = end;
    cout << "First file " << end << " for " << ud->ip << " looking for node " << endl;
    try{
      ud->endNode = semNodes.at(*ud->prevFile);
      cout << "found start  node for  "<< ud->ip <<"endNode"<<ud->endNode<< endl;
      return 1;
    }
    catch (const out_of_range e) 
      {
	// Have to try subfolders of start
	// or if there are groups try that instead
        //cout<<endl<<"Group Size"<<groups.size();
	if (groups.size() > 0)
	  { 
            //cout<<endl<<"Group here";
	    if (groups.find(*end) == groups.end())
	      return NO_FILE_PROB;
	    else
	      {
		if(semNodes.find(groups[*end]) != semNodes.end())
		  {
		    cout << "Found start node for " << ud->ip;
		    ud->endNode = semNodes.at(groups[*end]);
		    ud->prevFile = &ud->endNode->uri;
		    return 1;
		  }
	      }
	  }
	else
	  {
	    string *oldstart = ud->prevFile;
	    int prev = oldstart->size()+1;
	    string temp;
	    while (true)
	      {
		int pos = oldstart->rfind("/", prev-1);
		if (pos <= 0)
		  {
		    break;
		  }
		prev = pos;
		temp = oldstart->substr(0,pos+1);
		if(semNodes.find(temp) != semNodes.end())
		  {
		    cout << "Found start node for " << ud->ip << temp;
		    ud->endNode = semNodes.at(temp);
		    ud->prevFile = &ud->endNode->uri;
		    return 1;
		  }
	      }
	    cout << "Start not found for " << ud->ip << endl;
	    return NO_FILE_PROB;
	  }
      }
  }
  // Find transition to end node
  vector<string> startList, endList;
  startList.push_back(start->uri);
  if (semNodes.find(*end) != semNodes.end())
    {
      endList.push_back(*end);
    }
  // Find all subfolders or groups of start
  string *oldstart = &start->uri;
  int prev = oldstart->size();
  string temp;
  if (groups.size() > 0)
    {
      //cout<<endl<<"Group here"<<start->uri;
      //cout<<endl<<"Find "<<groups[start->uri]<<" Start Uri"<<groups.size();
      if (groups.find(start->uri) != groups.end())
	{
          //cout<<endl<<"Temp "<<temp;
	  temp = groups[start->uri];
	  if(semNodes.find(temp) != semNodes.end())
	    {
              //cout<<endl<<"Stl Temp "<<temp;
	      startList.push_back(temp);
	    }
	}
    }
  else
    {
      while (true)
	{
	  int pos = oldstart->rfind("/", prev-1);
	  if (pos <= 0)
	    {
	      break;
	    }
	  prev = pos;
	  temp = oldstart->substr(0,pos+1);
	  if(semNodes.find(temp) != semNodes.end())
	    {
	      startList.push_back(temp);
	    }
	}
    }
  // Find all subfolders or groups of end
  string *oldend = end;
  prev = oldend->size();
  if (groups.size() > 0)
    {
      if (groups.find(*end) != groups.end())
	{ //cout<<endl<<"St2 Temp "<<temp;
	  temp = groups[*end];
	  if(semNodes.find(temp) != semNodes.end())
	    {
              //cout<<endl<<"St3 Temp "<<temp;
	      endList.push_back(temp);
	    }
	}
    }
  else
    {
      while (true)
	{
	  int pos = oldend->rfind("/", prev-1);
	  if (pos <= 0)
	    {
	      break;
	    }
	  prev = pos;
	  temp = oldend->substr(0,pos+1);
	  if(semNodes.find(temp) != semNodes.end())
	    {
	      endList.push_back(temp);
	    }
	}
    }
  double prob = start->transTo(end, &endNode);
  if (prob > 0)
    {
      cout <<" Found exact transition to " << end << " returning " << prob << " for " << ud->ip << endl;
      ud->endNode = endNode;
      ud->prevFile = &endNode->uri;
      return prob;
    }
  // Find the first probability (starting with longest first)
  // of transition that is non-zero
  //for(string s : startList) 
  // cout << " startList " << s << " for " << ud->ip << endl;
  //for(string e : endList) 
  //cout << " endList " << e << " for " << ud->ip << endl;

  double max = 0;
  for(string s : startList) 
    for(string e : endList) 
      {
	start = semNodes.at(s);
	//cout << "Looking for transition from " << s << " to " << e << " for " << ud->ip << endl;
	double prob = start->transTo(&e, &endNode);
	if (prob > 0 && prob > max)
	  {
	    cout <<" Found transition to " << e << " returning " << prob << " for " << ud->ip << endl;
	    max = prob;
	    ud->endNode = endNode;
	    ud->prevFile = &endNode->uri;
	    //return prob;
	  }
      }
  if (max > 0)
    {
      return max;
    }
  if(semNodes.find(*end) != semNodes.end())
    {
      cout << "Found end node for " << ud->ip << *end;
      ud->endNode = semNodes.at(*end);
    }
  else if (endList.size() > 0)
    {
      cout << "Found end folder for " << ud->ip << endList[0];
      endNode = semNodes.at(endList[0]);
      ud->endNode = endNode;
    }
  cout << "Transition not found for " << ud->ip << endl;
  return NO_FILE_PROB;
}

bool FRADE::semantics(UserData *ud, string *end) {
#ifdef DEBUG
  cout << "Semantics: processing transition: " << *ud->prevFile << " -> " << *end << " for " << ud->ip << endl;
#endif

  ud->seqProb *= getTransitionProb2(ud, end);

#ifdef DEBUG
  cout << "New sequence probability: " << ud->seqProb << endl;
#endif

  int sl = ud->seqLength - 1;
  int al = maxSeqLen;
  int lookupindex = 0; //threshold of this seq length will be used to compare 
  int slotlength = 5;  // seq length 1->5 will be compared against 5, 6->10 will be compared against 10 and so on.
  if (sl%slotlength == 0){
  lookupindex = sl;
  }
  else{
  lookupindex = ((sl/slotlength) + 1)*slotlength;
  }
  if(lookupindex > al){lookupindex = al;}
  lookupindex -= 1; // coz array is zero indexed

#ifdef DEBUG
  cout << "Lookupindex "<<lookupindex<<" thresh "<<seqThreshes[lookupindex]<<" Seq length "<<sl<< endl;
#endif

  if(ud->seqProb <= seqThreshes[lookupindex]){
  cout<<endl<<"===:"<<sl<<":"<<lookupindex<<":"<< seqThreshes[lookupindex]<<":"<<ud->seqProb<<endl;
      return true;
  }
  return false;
  
}


std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    cout << "exec finished!" <<endl;
    return result;
}


bool FRADE::deception(UserData *ud, string *uri)
{
	string deceptionfile = "";

	string basefile = *uri;
	cerr << "base file " << basefile << " " << basefile.size()<< endl;
	if (dummy_links.find(*uri) != dummy_links.end())
	  {
	    cerr << "This is a dummy link " << endl;
	    blacklistIP(ud->ip);
	    struct timeval t;
	    gettimeofday (&t, NULL);
	    cout << "Blacklist deception " << ud->ip << " time: " << t.tv_sec<<endl;
	    return true;
	  }
	return false;
}

/* ------ OLD dyn1and3 function---------
bool FRADE::dynamics1and3(UserData *ud, unsigned long reqTime, long proctime) {
  // If we roll over into the next time window
  // we are garunteed to not break any thresholds
  // because all thresholds are > 1
//int j=0;
  for (int i = 0; i < numWindows; i++) {
    if(ud->times[i] == 0){
      ud->counts[i] = 1;
      ud->procs[i] = proctime;
      ud->times[i] = reqTime;
      continue;
    }
    if (reqTime - ud->times[i] >= windows[i]) {
      ud->counts[i] = 1;
      ud->procs[i] = proctime;
      ud->times[i] = reqTime;
    }
    else {
	ud->counts[i] += 1;
	ud->procs[i] += proctime;
	if(ud->counts[i] > dyn1Threshes[i] || (ud->procs[i]/1000) > dyn3Threshes[i]){
		//cout << "waiting for blacklisting.." << endl;
		cout<<endl<<"Black: " << dyn1Threshes[i]<<" "<<ud->ip << endl;
		blacklistIP(ud->ip);
		return true;
	}
    }
  } 
  return false;
}*/

bool FRADE::dynamics1and3(UserData *ud, unsigned long reqTime, string *reqfile) {
  // If we roll over into the next time window
  // we are garunteed to not break any thresholds
  // because all thresholds are > 1
  //int j=0;
    double time_to_process;

    try{
	time_to_process = proc_time[*reqfile];
    }
    catch (const out_of_range e) {
        time_to_process = 0.600;
    }

  for (int i = 0; i < numWindows; i++) {
    if(ud->times[i] == 0){
      ud->counts[i] = 1;
      ud->procs[i] = time_to_process;  //proc_time[*reqfile];
      ud->times[i] = reqTime;
      //cout<<"\n\n\n Count:"<<ud->counts[i]<<" At 1 "<<*reqfile<<" i:"<<i<<" ip:"<<ud->ip << endl;
      continue;
    }
    //cout<<endl<<"Req Time :  "<<reqTime<<" UD-time: "<<ud->times[i]<<" "<<windows[i]<<" "<<ud->counts[i]<<endl;
    if (reqTime - ud->times[i] >= windows[i]) {
      ud->counts[i] = 1;
      ud->procs[i] = time_to_process; //proc_time[*reqfile];
      ud->times[i] = reqTime;
      //cout<<"\n\n\n Count:"<<ud->counts[i]<<" At 2 "<<*reqfile<<" i:"<<i<<" ip:"<<ud->ip << endl;
    }
    else {
        ud->counts[i] += 1;
        ud->procs[i] += time_to_process;  //proc_time[*reqfile];
	// Is Dyn1 on?
        //cout<<endl<<"Dyn3";
        //cout<<" "<<ud->counts[i]<<" At 3 "<<*reqfile<<" i:"<<i<<" ip:"<<ud->ip <<" threshold "<<dyn1Threshes[i]<<" reqtime "<<reqTime<<endl;
	// cout<<" "<<ud->procs[i]<<" At 3 "<<dyn3Threshes[i]<<" "<<*reqfile<<" i:"<<i<<" ip:"<<ud->ip << endl;
	if ((activemodules & 1) > 0)
	  {
	    if(ud->counts[i] > dyn1Threshes[i]) 
	      {
		#ifdef DEBUG
                //cout<<endl<<"Black: " << dyn1Threshes[i]<<" "<<ud->ip << endl;
		#endif
                blacklistIP(ud->ip);
		struct timeval t;
		gettimeofday (&t, NULL);
		cout << "Blacklist dyn1 " << ud->ip << " time: " << t.tv_sec<<" Seq length: "<<ud->seqLength <<endl;
                return true;
	      }
	  }
	if ((activemodules & 4) > 0)
	  {
	    if(ud->procs[i] > (dyn3Threshes[i])) 
	      { 
                blacklistIP(ud->ip);
		struct timeval t;
		gettimeofday (&t, NULL);
		cout << "Blacklist dyn3 " << ud->ip << " time: " << t.tv_sec<<endl;
                return true;
	      }
	  }
    }
  }
  return false;
}


/* -------------------- OLD DYN2----------------------------------
bool FRADE::dynamics2(UserData *ud, unsigned long reqTime) {
  // Detection function for embedded requests
if (ud==NULL){
cout << "UD is NULL !!\n";
}
  for (int i = 0; i < numWindows; i++) {
    
    if(ud->embedtimes[i] == 0){
	
      ud->embedcounts[i] = 1;
      ud->embedtimes[i] = reqTime;
	continue;
    }
     if (reqTime - ud->embedtimes[i] >= windows[i]) {
      ud->embedcounts[i] = 1;
      ud->embedtimes[i] = reqTime;
	
     } else if (ud->embedcounts[i]++ > dyn2Threshes[i]) {
      cout << "DYN2:Blacklist: "<<dyn2Threshes[i]<<" "<<ud->ip;
      blacklistIP(ud->ip);
	cout << "dyn2 leaving with blacklisting\n";
      return true;
       }

  }
  return false;
}*/

/*------------------- DYN2 alternate version (not in use)----------
bool FRADE::dynamics2(UserData *ud, string *reqfile) {
  // Detection function for embedded requests
  
  //First check if list of allowed embedded obj reqs (ud->current_list) for the most recent main req (ud->current_main) is empty or not
  if(ud->current_list.empty()){
	if(ud->current_main.empty()){
  		cerr << ud->ip << "'s interaction started with embedded obj requests" <<endl;
  		//blacklistIP(ud->ip);
  		return false;//return true;
	}
	else{
		cerr << ud->current_main << " is not in our table of DYN2" <<endl;
		blacklistIP(ud->ip);
		return true;
	}
  }
  if(find(ud->current_list.begin(),ud->current_list.end(),*reqfile) != ud->current_list.end())
  {	ud->current_list.erase(std::remove(ud->current_list.begin(), ud->current_list.end(), *reqfile), ud->current_list.end()); 
	return false;
  }
  else{
	cerr << "Embedded obj requested is not in the allowed list for " << ud->current_main << endl;
	blacklistIP(ud->ip);
	return true;
  }
  
}*/

bool FRADE::dynamics2(UserData *ud, unsigned long reqTime, string *reqfile){
        if(find(ud->current_list.begin(),ud->current_list.end(),*reqfile) != ud->current_list.end())
        {	//cout << "***was in the list" <<endl;
                auto foundIt = std::find(ud->current_list.rbegin(), ud->current_list.rend(), *reqfile);
                if (foundIt != ud->current_list.rend()) { // if it was found
    			// get back to the right iterator
    			auto toRemove = --(foundIt.base());
    			// and erase it
    			ud->current_list.erase(toRemove);
		}                

                //ud->current_list.erase(std::remove(ud->current_list.begin(), ud->current_list.end(), *reqfile), ud->current_list.end());
                for (int i=0; i < numWindows; i++) {
                        if(ud->embedtimes[i] == 0){
                                ud->embedcounts[i] = 0;
                                ud->embedtimes[i] = reqTime;
                                continue;
                        }
                        if (reqTime - ud->embedtimes[i] >= windows[i]) {
                                ud->embedcounts[i] = 0;
                                ud->embedtimes[i] = reqTime;
                        }
                }
                return false;
        }
        else {
		//cout << "***Not in the list" << endl;
                /*for (int i = 0; i < numWindows; i++) {
                        if(ud->embedtimes[i] == 0){
                                ud->embedcounts[i] = 1;
                                ud->embedtimes[i] = reqTime;
                                continue;
                        }
                        if (reqTime - ud->embedtimes[i] >= windows[i]) {
                                ud->embedcounts[i] = 1;
                                ud->embedtimes[i] = reqTime;
                        } else if (ud->embedcounts[i]++ > dyn2Threshes[i]) {
			  struct timeval t;
			  gettimeofday (&t, NULL);
			  cout << "Blacklist dyn2 " << ud->ip << " time: " << t.tv_sec<<endl;
			  blacklistIP(ud->ip);
			  return true;
                        }
			cout <<endl<<ud->embedcounts[i] <<" "<<dyn2Threshes[i]<<endl;

                }*/
                          struct timeval t;
                          gettimeofday (&t, NULL);
                          cout << "Blacklist dyn2 " << ud->ip << " time: " << t.tv_sec<<endl;
                          blacklistIP(ud->ip);
                          return true;

                //return false;
        }
 }

void FRADE::blacklistIP(unsigned int userIP) {
  bool ok = blacklist.insert(userIP).second;
  cout<<endl<<" Blacklist "<<ok;
  struct timeval t;
  gettimeofday (&t, NULL);
  if (ok)
    {
      cout << "Put into pipe " << userIP << " time " << t.tv_sec << endl;
      std::string l= std::to_string(userIP);
      cout<<endl<<l<<endl;
      pipe << l + "\n" << flush;
    }
  /*
  char* ipstr;
  unsigned int ip = userIP;
  inet_ntop(AF_INET, &ip, ipstr, INET_ADDRSTRLEN);*/
  // Write to PIPE
 

  //ofstream pipe;
  //pipe.open(blacklistpipe); //Read the name of pipe from conf file
  //pipe.close();
}

bool FRADE::isMainRequest(string *uri) {
  for (int i = 0; mainReqs[i] != NULL; i++) {
    // regex_match function only tells you if the whole sting matches
    // regex_search tells you if part of the string matches
    if (boost::regex_search(*uri, *mainReqs[i])) {
      return true;
    }
  }
  return false;
}

void FRADE::beAFRADE(unsigned int userIP, long reqTime, string *requestedFile) {
  UserData *ud;
  

  req_count++;
//  if(req_count%10 == 1)
  //{
      start_req = clock();
  //}
  

  try
  {	
    ud = users.at(userIP);
  } 
  catch (const out_of_range e) 
  {
    // First request from this user
    // Create record for them and quit

    double proctime;
    try{
        proctime = proc_time[*requestedFile];
    }
    catch (const out_of_range e) {
        proctime = 0.600;
    }
    vector<string> vembd;
    try{
	
         vembd = main_embed[*requestedFile];
    }
    catch (const out_of_range e){
	vembd = vector<string>();
    }

    if (isMainRequest(requestedFile)){
      ud = new UserData(userIP, reqTime, requestedFile, numWindows, proctime,vembd);
    }
    else{
      ud = new UserData(userIP, reqTime, NULL, numWindows,proctime,vembd);
    }
    users.insert(make_pair(userIP, ud));
    //return;
  }

  #ifdef DEBUG
  cerr << "got request: " << *requestedFile << " time " << reqTime << " userip " << userIP << endl;
  #endif

/*----------Deception Part Start-------*/
    if((activemodules & 16) > 0)
      {
        #ifdef DEBUG
        cerr << "activating deception file " << *requestedFile << endl;
        #endif
	if(deception(ud, requestedFile)){
        #ifdef DEBUG
  	cerr << "blacklisted through deception module!!" << endl;
  	#endif
  	return; 
	}
      }
/*---------Deception Part End-------------*/


  if (isMainRequest(requestedFile)) {
      ud->seqLength += 1;
      #ifdef DEBUG
      cerr << "Got a main request: " << *requestedFile << endl;
      #endif
	
      ud->current_main = *requestedFile; //update current main request which is used in DYN2
      try{
         ud->current_list = main_embed[ud->current_main];
      }
      catch (const out_of_range e){
        ud->current_list = vector<string> ();
      }

      /*----------- DYN1 and DYN3 start---------*/
      
      #ifdef DEBUG
      cerr << "dyn1and3 executing.." << endl;
      #endif

      if (dynamics1and3(ud, reqTime,requestedFile)){
        //cout << "blacklist 1and3: " << userIP << endl;
	//blacklistIP(userIP);
        return;
      }
      /*------------DYN1 and DYN3 End-----------*/

      if (ud->prevFile == NULL) {
        	// we haven't seen the first main request for this user until now
        	// need to wait til the next main request to calculate a transition prob
       		 ud->prevFile = requestedFile;
      } else {
	/*----------------- Semantics Start-----------*/
	if ((activemodules & 8) > 0)
	  {
	    #ifdef DEBUG
	    cerr << "semantics executing.." << endl;
            #endif
	    if (semantics(ud, requestedFile)) {
	      struct timeval t;
	      gettimeofday (&t, NULL);
	      cout << "Blacklist sem " << userIP << " time: " << t.tv_sec<<endl;
	      blacklistIP(userIP);
	      return;
	  }
	    /*----------------Semantics End---------------*/
	  }
      }
  }
  else{
	/*----------- DYN2 Start------------*/
    if ((activemodules & 2) > 0)
      {
      	#ifdef DEBUG
      	cerr << "dyn2 executing.." << endl;
      	#endif 
        if(! (requestedFile->find("?") != std::string::npos)) {   	
	if (dynamics2(ud, reqTime, requestedFile)){
	  //cerr << "blacklist dyn2 " << userIP << endl;
	  //blacklistIP(userIP);
	  return;
	}
       }
      }
    /*------------ DYN2 End------------*/
  } 

 // if(req_count%10 == 0)
  //{
      end_req = clock();
      //cout<<endl<<" Time taken"<<(end_req - start_req)/double(CLOCKS_PER_SEC)*1000<<" "; 
      ud->proc_time = ud->proc_time + (end_req - start_req)/double(CLOCKS_PER_SEC)*1000;
      //if(ud->seqLength%10 == 0)
      //{
        cout<<endl<<"User "<<ud->ip<<" Time "<<ud->proc_time<<" Avg "<<(ud->proc_time/ud->seqLength)<<" Seq Len "<<ud->seqLength;
      //}  

  //}



}

