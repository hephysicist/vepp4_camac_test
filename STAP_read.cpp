#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
using namespace std;
int StapRead(int N){
int I[4],i=0;
string stap;
ifstream f ("/home/vepp4/kadrs/stap.dat");
if(f.is_open())
{
	while(i<50)
	{
		getline(f,stap);
		if(i==0)cout<<stap<<endl;				
		if(i==46) I[0]=atoi(stap.c_str()); //Ток первого сгустка электронов (мкА)
		if(i==47) I[1]=atoi(stap.c_str()); //Ток второго сгустка электронов (мкА)
		if(i==48) I[2]=atoi(stap.c_str()); //Ток первого сгустка позитронов (мкА)
		if(i==49) I[3]=atoi(stap.c_str()); //Ток второго сгустка позитронов (мкА)
		i++;
	}			
	printf("Ie1=%d\nIe2=%d\nIp1=%d\nIp2=%d\n\n",I[0],I[1],I[2],I[3]);		
	i=0;
}
else
{
	cerr<<"Error opening file!"<< endl;
}
	f.close();
	return I[N];
}
