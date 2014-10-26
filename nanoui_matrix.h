/*
Copyright (c) 2014 Shinya Miyamoto( smiyaxdev@gmail.com )

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef NANOUI_MATRIX4X4
#define NANOUI_MATRIX4X4

#include <memory.h>
#include <math.h>

#ifndef M_PI
#define M_PI (3.141519)
#endif


inline float DEG2RAD( float a ){ return a * M_PI/180.0f; }
inline float RAD2DEG( float a ){ return a * 180.0f / M_PI; }


class Matrix4x4
{
public:
	float m[16];

	Matrix4x4()
	{
		loadIdentity();
	}

	void loadIdentity()
	{
		m[0] = 1.0f;
		m[1] = 0.0f;
		m[2] = 0.0f;
		m[3] = 0.0f;

		m[4] = 0.0f;
		m[5] = 1.0f;
		m[6] = 0.0f;
		m[7] = 0.0f;

		m[8] = 0.0f;
		m[9] = 0.0f;
		m[10] = 1.0f;
		m[11] = 0.0f;

		m[12] = 0.0f;
		m[13] = 0.0f;
		m[14] = 0.0f;
		m[15] = 1.0f;

	}


	inline float get( int row, int col ){ return m[row*4+col]; }
	void operator = (  const Matrix4x4 & rhs ) { memcpy( this->m, rhs.m, sizeof(float) * 16); };
	float operator [] ( int index ) { return m[index]; }
	Matrix4x4 operator * (  const Matrix4x4 & rhs ) { return multiply(rhs); }

	void translate( float x, float y, float z )
	{
		Matrix4x4 t;
		t.m[0+3*4] = x;
		t.m[1+3*4] = y;
		t.m[2+3*4] = z;
		*this = *this * t;
	}

	void rotateZ( float dir )
	{

		float a = DEG2RAD(dir);
		float cs = cosf(a);
		float sn = sinf(a);
		Matrix4x4 t;

		t.m[0] = cs;
		t.m[1] = sn;
		t.m[4] = -sn;
		t.m[5] = cs;
		*this = *this * t;

	}


	void scale( float x, float y, float z )
	{
		Matrix4x4 t;
		t.m[0] = x;
		t.m[5] = y;
		t.m[10] = z;
		*this = *this * t;
	}

	Matrix4x4 multiply( const Matrix4x4 & mtx )
	{
		Matrix4x4 ret;
		ret.m[0] = 0.0f;
		ret.m[5] = 0.0f;
		ret.m[10] = 0.0f;
		ret.m[15] = 0.0f;

		const int n = 4;
		for (int i = 0; i < n; i++){
			for (int j = 0; j < n; j++){
				for ( int k = 0; k < n; k++){
					ret.m[i + j*n] += m[i + k*n]*mtx.m[k + j*n];
				}
			}
		}
		return ret;
	}
	Matrix4x4 inverse( Matrix4x4 & mtx );

	void transform( float & x, float & y )
	{
		float dx = x*m[0] + y*m[4] + m[8] + m[12];
		float dy = x*m[1] + y*m[5] + m[9] + m[13];
		x = dx;
		y = dy;
	}

	// The parameters are interpreted as matrix as follows:
	//   [a c e]
	//   [b d f]
	//   [0 0 1]
	void getMatrix2x3( float * p )
	{
		p[0] = m[0];
		p[1] = m[1];
		p[2] = m[4];
		p[3] = m[5];
		p[4] = m[12];
		p[5] = m[13];

	}
};


#endif
