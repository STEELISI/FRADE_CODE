#include<iostream>
#include<fstream>
#include<string>
#include<sys/time.h>
#include<unistd.h>
#include<vector>
#include<sstream>
#include<iterator>
#include<map>
#include <cstddef>
#include <arpa/inet.h> // For converting ip string to uint
#include "FRADE.h"
// Example: ./rsyslog -c conf/imgur/FRADE100.conf -m dyn1 -l /var/log/apache2/access.log
using namespace std;

#define SLEEPTIME 2

string configfile;

void split1(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}


void help()
{
  cout << "You must specify a configuration file like this \"-c path_to_file\"\n";
}

FRADE f;
long start_time = 0;

int main(int argc, char** argv)
{
  string *uri;
  long time;
  unsigned int ip;
  std::streamoff p = 0;
  string log;
  map<long, vector<string> > buffer;
  vector<string> tokens;

  struct timeval t;
  gettimeofday (&t, NULL);
  start_time = (long)(t.tv_sec*1000+t.tv_usec/1000);
  cout << "FRADE started at "<< start_time << endl;
  // Process command line args
  if(argc == 1)
    {
      help();
      exit(1);
    }
  char tmp;
  int modules = 0;
  string mod;
  string log_file;
  while((tmp=getopt(argc,argv,"hc:m:l:"))!=-1)
    {
      switch(tmp)
	{ 
	case 'h':
	  help();
	  break;
	case 'c':
	  configfile = optarg;
	  break;
	case 'm':
	  mod = optarg;
	  if (mod.compare("dyn1") == 0)
	    modules = modules | 1;
	  if (mod.compare("dyn2") == 0)
	    modules = modules | 2;
	  if (mod.compare("dyn3") == 0)
	    modules = modules | 4;
	  if (mod.compare("sem") == 0)
	    modules = modules | 8;
	  if (mod.compare("dec") == 0)
	    modules = modules | 16;
	  break;
        case 'l':
           log_file = optarg;
           break;  
	default:
	  break;
	}
    }
  cout << "Configured FRADE modules " << modules << endl;
  f.configure(configfile, modules);
  
  while(true)
    {
      cout<<endl<<"here ";
      ifstream ifs(log_file);
      cout << "Opened apache log "<< endl;
      ifs.seekg(p);  //*1
      while(getline(ifs,log))
	{
	  p = ifs.tellg();
	  string lmn(configfile);
	  if (lmn.find("reddit") != std::string::npos) {
	    //This block is for reddit. It converts below format
	    //May  7 18:00:35 localhost haproxy[26252]: "155.98.38.135 1494201635 GET / HTTP/1.1 200 207"
	    // To 155.98.38.135 1494201635 GET / HTTP/1.1 200 207
	    
	    size_t firstoccur = log.find_first_of("\"");
	    size_t lastoccur = log.find_last_of("\"");
	    log = log.substr(firstoccur+1, (lastoccur-firstoccur)-1);
	    // Now string became : 155.98.38.135 1494201635 GET / HTTP/1.1 200 207
	    tokens.clear();
	    split1(log, ' ', tokens);
	    log = "";
	    tokens[1] = tokens[1] + "000";
	    int tknlen = tokens.size();
	    for(int kp=0; kp<tknlen; kp++){
	      log += tokens[kp] + " ";
	    }
	    log.pop_back();
	    // Now string became : 155.98.38.135 1494201635000 GET / HTTP/1.1 200 207
	  }
	  
	  //cout << log << endl;
	  //tokens.clear();
	  //split1(log, ' ', tokens);
	  //time = stol(tokens[1]);
	  //if(time < prevtime){p=ifs.tellg();continue;}
	  //buffer[time].push_back(log);
	  //p = ifs.tellg();
	  //counter += 1;
	  //if(counter <= batchsize){
	  // continue;	
	  //}
	  //else{
	  //counter = 0;
	  //for(map<long,vector<string> > :: iterator it = buffer.begin(); it != buffer.end(); it++){
	  //  vector<string> sv = it->second;
	  //  int vl = sv.size();
	  string logline = log;
	  //    for(int kl=0; kl<vl; kl++){
	  //	logline = sv[kl];
	  cout << "LOG :" << logline << endl;
	  tokens.clear();
	  split1(logline, ' ', tokens);
          //cout<<endl<<tokens[4]<<" 5 " <<tokens[5]<<" 6 "<<tokens[6];
	  // Format: <IP> <TIME MILLISECONDS> <METHOD> <URI> <HTTP-VERSION> <STATUS CODE> <PROCESSING TIME>
	  const char * c = tokens[0].c_str();
	  if (inet_pton(AF_INET, c, &ip) == 0) {
		  cout << "Failed to convert " << tokens[0] << " to an unsigned int" << endl;
		  continue;
	  }
          time = long(stod(tokens[1])); 
          if (lmn.find("wikipedia") != std::string::npos) {
	  time = long((stod(tokens[1]) - stod(tokens[6]))*1000);
          }
          // if (lmn.find("reddit") != std::string::npos) {
          //time = long(stod(tokens[1])*1000);
          //} 
	  if (1) //time >= start_time)
	    {
	  //proctime = stod(tokens[tokens.size()-1]);
	      uri = new string(tokens[3]);
	      int statcode = stoi(tokens[tokens.size()-2]);
	      if (statcode != 301)
		{
	      //if(statcode-400 >=3){p = ifs.tellg();continue;}
                  cout << " p is " ;
		  f.beAFRADE(ip, time, uri);
		}
	    }
	}
      cout << " p is " << p << " sleeping 2 "<<endl;
      sleep(2);
    }
  return 0;
}
