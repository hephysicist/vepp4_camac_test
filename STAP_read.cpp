#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <fstream>

using namespace std;
int main(){
int I1=0,I2=0,i=0;
string stap;
	while(1)
	{
		ifstream f ("/home/vepp4/kadrs/stap.dat");
		if(f.is_open())
		{
			while(i<49)
			{
				getline(f,stap);
				i++;
				if(i==0)cout<<stap<<endl;				
				if(i==46) IE1=atoi(stap.c_str()); //Ток первого сгустка электронов (мкА)
				if(i==47) IE2=atoi(stap.c_str()); //Ток второго сгустка электронов (мкА)
				if(i==48) IP1=atoi(stap.c_str()); //Ток первого сгустка позитронов (мкА)
				if(i==49) IP2=atoi(stap.c_str()); //Ток второго сгустка позитронов (мкА)
			}			
			printf("I1=%d\nI2=%d\n\n",I1,I2);		
			i=0;
		}
		else
		{
			cerr<<"Error opening file!"<< endl;
			break;
		}
	f.close();
	usleep(1000000);
	}
}
