#include <iostream>
#include <stdlib.h>
#include <limits>
#include <math.h>

using namespace std;

#define NaN std::numeric_limits<float>::quiet_NaN() 
#define isNaN(X) (X != X)

bool VelocityRequired (const float h, const float m, const float theta, 
              const float d, const float step, const float k,
              const float t, const float b, const float w, float& v);

float f(float h,  float m,  float theta, 
							float d, float step, float k,
               float t,  float b,  float w, float v, float& assmp);

bool VelocityRequired (const float h, const float m, const float theta, 
							const float d, const float step, const float k,
              const float t, const float b, const float w, float& v) {
  
	float angle = theta * (M_PI / 180);
	float vMin;
	float vMid;
	float vMax;
	if(h < 0) {
		return false;
	}
	if ((theta >= 90 && theta <= 270) || (theta <= -90 && theta >= -270))
		return false;
	if(d <= 0 || (theta >= 90 && theta <= 270) || (theta <= -90 && theta >= -270) || b <= 0 || k < 0 || w <= 0 || step <= 0 || m <= 0 || h < 0)
		return false;
	v = (((t-h)-tan(angle)*d)*pow(cos(angle),2))/((-4.9)*pow(d,2));
	v = 1/v;
	v = sqrt(v);
	vMax = 100;
	vMin = 0;
	if(k == 0) {
		return true;
	}else{
		v = 50;
		int counter = 0;
		float assmp = 0;
		float ay, ax;
		while((abs(f(h, m, theta, d, step, k, t, b, w, v, assmp)) > step) && (counter <= 1000)) {
			if(assmp > 0) {
				vMax = v;
			}else if(assmp < 0) {
				vMin = v;
			}
			counter++;
			v = (vMax - vMin)/2 + vMin;
		}
		
	}
		
	
	return true;
}

float f(float h, float m, float theta, float d, float step, float k, float t, float b, float w, float v, float& assmp) {
		
	float angle = theta * (M_PI / 180);
	float vX = v*cos(angle);
	float vY = v*sin(angle);
	float placeholder = v;
	float distH = h;
	float distX = 0;
	bool flip = true;
	while(flip == true) {
		if (h <= t && vY <= 0) {
			assmp = distX - d;
			flip = false;
			
		}
		h += vY * step;
		distX += vX * step;
		vY -= ((vY*v*k)/m)*step + 9.8*step;
		vX -= ((vX*v*k)/m)*step;
		
		v = sqrt(pow(vX,2)+pow(vY,2));
		
	}
	return assmp;
	
	}
		
int main() {
	
  //Values
  float h = 50;
  float d = 60;
  float b = 40;
  float t = 30;
  float w = 30;
  float m = 1;
  float step = 0.01;
  float k = 0.005;
  float theta = 45; // degrees
  float v = 25;

  cout << "Given angle: " << theta << endl;
  cout << "Target at (" << d << "," << t << ")" << endl;
  if (VelocityRequired (h, m, theta, d, step, k, t, b, w, v)) {
    cout << "Required initial velocity: " << v << endl << endl;
  }
  else {
    cout << "Cannot calculate the velocity." << endl << endl;
  }
    
  return 0;
}

