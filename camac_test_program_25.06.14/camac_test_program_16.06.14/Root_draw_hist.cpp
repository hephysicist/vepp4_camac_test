#include "Root_draw_lib.h"
#include <boost/program_options.hpp>
#include <TROOT.h>
#include "camac1.3.h"
#include <TApplication.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TH1F.h>
#include <TH2F.h>
#include <string>
#include <time.h>
extern void InitGui();
VoidFuncPtr_t initfuncs[] = { InitGui, 0 };
TROOT root("-","-", initfuncs);
using namespace std;
int main(int argc, char** argv)
{
	int data1[1000000];
	int i=0,j=0,mean=0,count_time=0,N_count1;
	time_t time_before, time_after;
	TApplication theApp("test", &argc, argv);
	Root_hist *histogram = new Root_hist();
	Camac *camac = new Camac();	
	camac->camacInit(argc,argv);
	histogram->makeHist();
	sscanf(argv[3],"%d",&count_time);
	sscanf(argv[10],"%d",&N_count1);       	
	while(1)
	{
		for(int c=0;c<1e6;c++){
		data1[c]=0;
		}
		time_after=0;
		time_before=time(NULL);
		while(time_after!=count_time)
		{
			time_after= time(NULL);
			time_after=time_after - time_before;
			data1[i]=camac->getCharge(0);	
			printf("Event %d - %d\n",i,data1[i]);		
			i++;
			//usleep(10000);
		}
		i=0;
		j++;	
		mean=histogram->addToHist(data1);
		printf("Mean value = %d\n",mean);
	}	
theApp.Run();
}
