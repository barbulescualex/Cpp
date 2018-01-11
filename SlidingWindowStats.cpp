#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

int SWStats(const float dataset[], const int size,
	    const int currentSample, const int windowSize,
	    float& min, float& avg, float& max,
	    float& smplSD, float& mdn);

int SWMinimum(const float dataset[], const int size,
	      const int currentSample, const int windowSize,
	      float& minimum);
int SWAverage(const float dataset[], const int size,
	      const int currentSample, const int windowSize,
	      float& average);
int SWMaximum(const float dataset[], const int size,
	      const int currentSample, const int windowSize,
	      float& maximum);
int SWSmplStdDev(const float dataset[], const int size,
		 const int currentSample, const int windowSize,
		 float& smplStdDev);
int SWMedian(const float dataset[], const int size,
	     const int currentSample, const int windowSize,
	     float& median);


int SWStats(const float dataset[], const int size,
	    const int currentSample, const int windowSize,
	    float& min, float& avg, float& max,
	    float& smplSD, float& mdn) {
  
  float minimum=FLT_MAX;
  float maximum=-FLT_MAX;
  float average;
  float smplStdDev;
  float median;
  
  if(windowSize<=0||size<1){
    return -1;
  }
  if(windowSize>=size||windowSize==1){
    return 1;
  }
  if(SWSmplStdDev(dataset,size,currentSample,windowSize,smplStdDev)<0){
    return -1;
  }
    
  SWMinimum(dataset,size,currentSample,windowSize,minimum);
  SWMaximum(dataset,size,currentSample,windowSize,maximum);
  SWAverage(dataset,size,currentSample,windowSize,average);
  SWSmplStdDev(dataset,size,currentSample,windowSize,smplStdDev);
  SWMedian(dataset,size,currentSample,windowSize,median);
  min = minimum;
  max = maximum;
  avg = average;
  smplSD = smplStdDev;
  mdn = median;
  return 0;
}

int SWMinimum(const float dataset[], const int size,
	      const int currentSample, const int windowSize,
	      float& minimum) {
  int tempS = currentSample;
  int tempWS = windowSize;
  
  while(tempWS!=0){
    if(tempS==0){
  	  if(dataset[tempS]<minimum)minimum=dataset[tempS];
    }
    else{
    	if(dataset[tempS]<minimum)
        minimum=dataset[tempS];
      tempS--;
    }
      tempWS--;
    }
  return 0;
}

int SWAverage(const float dataset[], const int size,
	      const int currentSample, const int windowSize,
	      float& average) {
  int tempS = currentSample;
  int tempWS = windowSize;
  float sum=0;
  while(tempWS!=0){
    if(tempS==0){
      sum+=dataset[tempS];
    }
    else{
      sum+=dataset[tempS];
      tempS--;
    }
    tempWS--;
  }
  int size1 = windowSize;
  average = sum/size1;
  return average;
}


int SWMaximum(const float dataset[], const int size,
	      const int currentSample, const int windowSize,
	      float& maximum) {
  int tempWS = windowSize;
  int tempS = currentSample;

  while(tempWS!=0){
    if(tempS==0)
      (dataset[tempS]>maximum)? maximum=dataset[tempS]:0;
    else{
      (dataset[tempS]>maximum)? maximum=dataset[tempS]:0;
      tempS--;
    }
    tempWS--;
  }
  return 0;
}

int SWSmplStdDev(const float dataset[], const int size,
		 const int currentSample, const int windowSize,
		 float& smplStdDev) {
  float sum = 0;
  float average;
  SWAverage(dataset,size,currentSample,windowSize,average);
  int tempWS = windowSize;
  int tempS=currentSample;
  while(tempWS!=0){
    if(tempS==0)
      sum+=pow(dataset[tempS]-average,2);
    else{
      sum+=pow(dataset[tempS]-average,2);
      tempS--;
    }
    tempWS--;
  }
  smplStdDev=sqrt((1.0/(windowSize-1.0))*sum);
  return 0;
}



int SWMedian(const float dataset[], const int size,
	     const int currentSample, const int windowSize,
	     float& median) {
  float S[windowSize];
  int x=0;
  int num=currentSample;
  int prev = windowSize-(currentSample+1);
  for (size_t i = 0; i < windowSize; i++){
    if(prev>0){
      S[i]=dataset[0];
      prev--;
    }
    else{
      if((currentSample-windowSize)<0) 
        S[i]=dataset[x];
      if((currentSample-windowSize)>=0) 
        S[i]=dataset[(num+1)-windowSize];
      x++;
      num++;
    }
  }
  for(int n = windowSize; n > 1; n--) {
    for(int i = 1; i < n; i++) {
      if(S[i-1] > S[i]) {
        float hold = S[i-1];
        S[i-1] = S[i];
        S[i] = hold;
      }
    }
  }
  (windowSize%2 ==0)? median=((S[(windowSize/2)-1]+S[windowSize/2])/2): median=S[windowSize/2];
  return 0;
}


int main(const int argc, const char* const argv[]) {
  int size = 5;
  float dataset[] = {1, 2, 3, 4, 5};
  int windowSize = 2;
  int currentSample = 0;
  float min;
  float avg;
  float max;
  float smplSD;
  float median;

  cout << "Sample \t Minimum \t Average \t Median \t Maximum \t Sample Standard Deviation" << endl;

  while (currentSample < size) {
    int retCode = SWStats(dataset, size, currentSample, windowSize, min, avg, max, smplSD, median);
    if (retCode >= 0) {
      cout << currentSample << "\t " << min << "\t " << avg << "\t " << max << "\t " << median << "\t " << smplSD << endl;
      if (retCode > 0)
	cerr << "Warning: something weird happened, but we computed stats anyway ... YMMV" << endl;
    }
    else {
      cerr << "Error: unable to compute sliding-window statistics; exiting" << endl;
      return(-1);
    }
    ++currentSample;
  }
  return 0;
}

