#include <stdio.h>
#include <stdlib.h>

inline double f (double x) { return x*x; }

int main (int argc, char *argv[])
{
  int n;
  double a, b, x, h;
  double I;
  a = 0;
  b = 3;
  n = 1000000;
  h = (b-a) / n;
  I = 0;
  for (int k = 0; k < n; k++)
  {
    x = a + k*h;
    I += f(x)*h;
  }
  printf("Integral value is = %e\n",I);
  return 0;
}
