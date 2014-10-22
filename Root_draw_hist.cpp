#include "Root_draw_lib.h"
#include <boost/program_options.hpp>
#include <TROOT.h>
#include "camac.h"
#include <TApplication.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TH1F.h>
#include <TH2F.h>
#include <string>
#include <fstream>
#include <iostream>
#include <time.h>
#include "Extern_prog.h"
#include <vector>
extern void InitGui();
VoidFuncPtr_t initfuncs[] = { InitGui, 0 };
TROOT root("-","-", initfuncs);
using namespace std;
int main(int argc, char** argv)
{
	//int data1[1000000];
	vector<int> data1;
	data1.reserve(1e6);
	int i=0,j=0,mean=0,count_time=0,N_count1;
	double Mean=0,RMS=0,Max=0;
	unsigned int count=0;
	time_t time_before, time_after;
	sscanf(argv[3],"%d",&count_time);
	sscanf(argv[10],"%d",&N_count1);    	
	TApplication theApp("test", &argc, argv);
	Root_hist *histogram = new Root_hist();
	Camac *camac = new Camac();	
	camac->camacInit(argc,argv);
	histogram->makeHist();  
	printf("Prepearing to start count ..."); 
	cout << flush << endl;	
	while(1)
	{
		/*for(int c=0;c<1e6;c++){
		data1[c]=0;
		}
		data1.resize(0);*/
		time_after=0;
		time_before=time(NULL);
          	printf("Start count\n");
		camac->ResetCount();
		//printf("first Count = %d\n",count);	
		while(time_after<count_time)
		{
			//cout << "counting " << i << endl;
			time_after = time(NULL);
			time_after-= time_before;
			//cout << "counting " << i << endl;
			/*data1.push_back(camac->GetCharge(1));
			data1[i]=camac->getCharge(0);	
			printf("Event %d - %d\n",i,data1[i]);*/			
			i++;
			//usleep(100000);
		}
		count = camac->GetCount(0);
        	//camac->ResetCount();
		printf("Count = %d\n",count);
		//StapRead(1);	
		i=0;
		j++;
		//usleep(100000);	
		//histogram->InitHist(data1);
		//RMS=histogram->GetRMS();
		//Mean=histogram->GetMean();
		//printf("Mean value = %f\n",Mean);
	}	
theApp.Run();
}
