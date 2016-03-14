/* coherent noise function over 1, 2 or 3 dimensions */
/* (copyright Ken Perlin) */

#include "noise.h"

#include <assert.h>
#include <iso646.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

inline static float ease(const float in_T) {
    return in_T * in_T * in_T * (in_T * (in_T * 6.0f - 15.0f) + 10.0f);
    //return in_T * in_T * ( -in_T * 2.0f + 3.0f );
}

inline static float mix(const float in_A, const float in_B, const float in_T) {
    return in_A + in_T * (in_B - in_A);
}
/*
static const int B = 0x100;
static const int BM = 0xff;

#define N 0x1000
#define NP 12   // 2^N
#define NM 0xfff

static int p[B + B + 2];
static float g3[B + B + 2][3];
static float g2[B + B + 2][2];
static float g1[B + B + 2];
static int start = 1;

static void init(void);



#define setup(i,b0,b1,r0,r1)\
	t = vec[i] + N;\
	b0 = ((int)t) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - (int)t;\
	r1 = r0 - 1.;

float noise1(float arg)
{
	int bx0, bx1;
	float rx0, rx1, sx, t, u, v, vec[1];

	vec[0] = arg;
	if (start) {
		start = 0;
		init();
	}

	setup(0, bx0,bx1, rx0,rx1);

	sx = ease(rx0);

	u = rx0 * g1[ p[ bx0 ] ];
	v = rx1 * g1[ p[ bx1 ] ];

	return mix( u, v, sx );
}

float noise2(float vec[2])
{
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
	register int i, j;

	if (start) {
		start = 0;
		init();
	}

	setup(0, bx0,bx1, rx0,rx1);
	setup(1, by0,by1, ry0,ry1);

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

	sx = ease(rx0);
	sy = ease(ry0);

#define at2(rx,ry) ( rx * q[0] + ry * q[1] )

	q = g2[ b00 ] ; u = at2(rx0,ry0);
	q = g2[ b10 ] ; v = at2(rx1,ry0);
	a = mix( u, v, sx );

	q = g2[ b01 ] ; u = at2(rx0,ry1);
	q = g2[ b11 ] ; v = at2(rx1,ry1);
	b = mix( u, v, sx );

	return mix( a, b, sy );
}

float noise3(float vec[3])
{
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	float rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
	register int i, j;

	if (start) {
		start = 0;
		init();
	}

	setup(0, bx0,bx1, rx0,rx1);
	setup(1, by0,by1, ry0,ry1);
	setup(2, bz0,bz1, rz0,rz1);

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

	t  = ease(rx0);
	sy = ease(ry0);
	sz = ease(rz0);

#define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

	q = g3[ b00 + bz0 ] ; u = at3(rx0,ry0,rz0);
	q = g3[ b10 + bz0 ] ; v = at3(rx1,ry0,rz0);
	a = mix( u, v, t );

	q = g3[ b01 + bz0 ] ; u = at3(rx0,ry1,rz0);
	q = g3[ b11 + bz0 ] ; v = at3(rx1,ry1,rz0);
	b = mix( u, v, t );

	c = mix( a, b, sy);

	q = g3[ b00 + bz1 ] ; u = at3(rx0,ry0,rz1);
	q = g3[ b10 + bz1 ] ; v = at3(rx1,ry0,rz1);
	a = mix( u, v, t);

	q = g3[ b01 + bz1 ] ; u = at3(rx0,ry1,rz1);
	q = g3[ b11 + bz1 ] ; v = at3(rx1,ry1,rz1);
	b = mix( u, v, t );

	d = mix( a, b, sy );

	return mix( c, d, sz );
}

static void normalize2(float v[2])
{
	float s;

	s = sqrt(v[0] * v[0] + v[1] * v[1]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
}

static void normalize3(float v[3])
{
	float s;

	s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
	v[2] = v[2] / s;
}

static void init(void)
{
	int i, j, k;

	for (i = 0 ; i < B ; i++) {
		p[i] = i;

		g1[i] = (float)((random() % (B + B)) - B) / B;

		for (j = 0 ; j < 2 ; j++)
			g2[i][j] = (float)((random() % (B + B)) - B) / B;
		normalize2(g2[i]);

		for (j = 0 ; j < 3 ; j++)
			g3[i][j] = (float)((random() % (B + B)) - B) / B;
		normalize3(g3[i]);
	}

	while (--i) {
		k = p[i];
		p[i] = p[j = random() % B];
		p[j] = k;
	}

	for (i = 0 ; i < B + 2 ; i++) {
		p[B + i] = p[i];
		g1[B + i] = g1[i];
		for (j = 0 ; j < 2 ; j++)
			g2[B + i][j] = g2[i][j];
		for (j = 0 ; j < 3 ; j++)
			g3[B + i][j] = g3[i][j];
	}
}
*/


// DERIVED FROM JAVA REFERENCE IMPLEMENTATION OF IMPROVED NOISE - COPYRIGHT 2002 KEN PERLIN.

static const int P[256] = {
    151, 160, 137,  91,  90,  15, 131,  13, 201,  95,  96,  53, 194, 233,   7, 225, 140,  36, 103,  30,  69, 142,   8,
    99,  37, 240,  21,  10,  23, 190,   6, 148, 247, 120, 234,  75,   0,  26, 197,  62,  94, 252, 219, 203, 117,  35,
    11,  32,  57, 177,  33,  88, 237, 149,  56,  87, 174,  20, 125, 136, 171, 168,  68, 175,  74, 165,  71, 134, 139,
    48,  27, 166,  77, 146, 158, 231,  83, 111, 229, 122,  60, 211, 133, 230, 220, 105,  92,  41,  55,  46, 245,  40,
    244, 102, 143,  54,  65,  25,  63, 161,   1, 216,  80,  73, 209,  76, 132, 187, 208,  89,  18, 169, 200, 196, 135,
    130, 116, 188, 159,  86, 164, 100, 109, 198, 173, 186,   3,  64,  52, 217, 226, 250, 124, 123,   5, 202,  38, 147,
    118, 126, 255,  82,  85, 212, 207, 206,  59, 227,  47,  16,  58,  17, 182, 189,  28,  42, 223, 183, 170, 213, 119,
    248, 152,   2,  44, 154, 163,  70, 221, 153, 101, 155, 167,  43, 172,   9, 129,  22,  39, 253,  19,  98, 108, 110,
    79, 113, 224, 232, 178, 185, 112, 104, 218, 246,  97, 228, 251,  34, 242, 193, 238, 210, 144,  12, 191, 179, 162,
    241,  81,  51, 145, 235, 249,  14, 239, 107,  49, 192, 214,  31, 181, 199, 106, 157, 184,  84, 204, 176, 115, 121,
    50,  45, 127,   4, 150, 254, 138, 236, 205,  93, 222, 114,  67,  29,  24,  72, 243, 141, 128, 195,  78,  66, 215,
    61, 156, 180
};

static float grad(int hash, float x, float y, float z) {
    // CONVERT LO 4 BITS OF HASH CODE INTO 12 GRADIENT DIRECTIONS.
    int h = hash bitand 0xf;
    float u = h < 8 ? x : y,
          v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float noise(const float in_X, const float in_Y, const float in_Z) {
    // FIND UNIT CUBE THAT CONTAINS POINT.
    int x = (int)floorf(in_X) bitand 0xff;
    int y = (int)floorf(in_Y) bitand 0xff;
    int z = (int)floorf(in_Z) bitand 0xff;

    // FIND RELATIVE X,Y,Z OF POINT IN CUBE.
    float dx = in_X - floorf(in_X);
    float dy = in_Y - floorf(in_Y);
    float dz = in_Z - floorf(in_Z);

    // COMPUTE FADE CURVES FOR EACH OF X,Y,Z.
    float u = ease(dx);
    float v = ease(dy);
    float w = ease(dz);

    // HASH COORDINATES OF THE 8 CUBE CORNERS
    int a  = (P[x]                 + y) bitand 0xff;
    int aa = (P[a]                 + z) bitand 0xff;
    int ab = (P[(a + 1) bitand 0xff] + z) bitand 0xff;
    int b  = (P[(x + 1) bitand 0xff] + y) bitand 0xff;
    int ba = (P[b]                 + z) bitand 0xff;
    int bb = (P[(b + 1) bitand 0xff] + z) bitand 0xff;

    // AND ADD BLENDED RESULTS FROM 8 CORNERS OF CUBE
    float x0, x1, y0, y1, z0, z1;

    x0 = grad(P[aa], dx,     dy,     dz);
    x1 = grad(P[ba], dx - 1, dy,     dz);
    y0 = mix(x0, x1, u);
    x0 = grad(P[ab], dx,     dy - 1, dz);
    x1 = grad(P[bb], dx - 1, dy - 1, dz);
    y1 = mix(x0, x1, u);
    z0 = mix(y0, y1, v);

    x0 = grad(P[aa + 1], dx,     dy,     dz - 1);
    x1 = grad(P[ba + 1], dx - 1, dy,     dz - 1);
    y0 = mix(x0, x1, u);
    x0 = grad(P[ab + 1], dx  ,   dy - 1, dz - 1);
    x1 = grad(P[bb + 1], dx - 1, dy - 1, dz - 1);
    y1 = mix(x0, x1, u);
    z1 = mix(y0, y1, v);

    return mix(z0, z1, w);
}

