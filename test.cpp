#include <iostream>
#include <cstring>
#include <sstream>
#include <omp.h>
#include "mat.hpp"
using namespace std;

int main()
{
    float *a = new float[30];
    for (size_t i = 0; i < 30; i++)
    {
        a[i] = i + 1;
    }

    Matrix<float> m1(3, 5, 2, a);
    Matrix<float> m5(5, 3, 2, a);
    // Matrix m2 = m1 + 2;
    // cout << "m1: " << endl;
    // cout << m1 << endl;

    // cout << "m2=m1+2: " << endl;
    // cout << m2 << endl;

    // Matrix m3 = m1 + m2;
    // cout << "m3=m1+m2: " << endl;
    // cout << m3 << endl;

    // m3 = m2 - m1;
    // cout << "m3=m1-m2: " << endl;
    // cout << m3 << endl;

    // Matrix m4 = m1 * m5;
    // cout << "m1: " << endl;
    // cout << m1 << endl;
    // cout << "m5: " << endl;
    // cout << m5 << endl;
    // cout << "m4 = m1*m5" << endl;
    // cout << m4 << endl;

    // cout << "m1: " << endl;
    // cout << m1 << endl;

    // Matrix m2 = !m1;

    // cout << "m2 = m1的转置 " << endl;
    // cout << m2 << endl;

    Matrix<float> m6(6,5,1,a,2,1,2,3);
     cout << "m6: " << endl;
    cout << m6 << endl;
    Matrix<float> sub = m6.ROI();
    cout << "sub: " << endl;
    cout << sub << endl;

    

    

    return 0;
}