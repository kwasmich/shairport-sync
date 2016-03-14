//
//  Math3D.h
//  Math3D
//
//  Created by Michael Kwasnicki on 02.08.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef Math3D_Math3D_h
#define Math3D_Math3D_h

#include <math.h>   // sqrt
#include <stdio.h>  // printf


#define GENERIC_SV(FN, X) _Generic((X), float: FN##f, vec2: FN##f2, vec3: FN##f3, vec4: FN##f4 )(X)
#define GENERIC_SVM(FN, X) _Generic((X), float: FN##f, vec2: FN##f2, vec3: FN##f3, vec4: FN##f4, mat2: FN##m2, mat3: FN##m3, mat4: FN##m4 )(X)
#define GENERIC_SVM_REF(FN, X) _Generic((X), float: FN##f, vec2: FN##f2, vec3: FN##f3, vec4: FN##f4, mat2: FN##m2, mat3: FN##m3, mat4: FN##m4 )(&X)
#define GENERIC_SV2(FN, X, Y) _Generic((X), float: FN##f, vec2: FN##f2, vec3: FN##f3, vec4: FN##f4 )(X, Y)
#define GENERIC_SVM2(FN, X, Y) _Generic((X), float: FN##f, vec2: FN##f2, vec3: FN##f3, vec4: FN##f4, mat2: FN##m2, mat3: FN##m3, mat4: FN##m4 )(X, Y)
#define GENERIC_SV3(FN, X, Y, Z) _Generic((X), float: FN##f, vec2: FN##f2, vec3: FN##f3, vec4: FN##f4 )(X, Y, Z)
#define GENERIC_V2(FN, X, Y) _Generic((X), vec2: FN##f2, vec3: FN##f3, vec4: FN##f4 )(X, Y)
#define GENERIC_M(FN, X) _Generic((X), mat2: FN##m2, mat3: FN##m3, mat4: FN##m4 )(X)
#define GENERIC_M2(FN, X, Y) _Generic((X), mat2: FN##m2, mat3: FN##m3, mat4: FN##m4 )(X, Y)

#define GENERIC_V_SUB(FN, Y) _Generic((Y), vec2: FN##f2, vec3: FN##f3, vec4: FN##f4, default: 0)
#define GENERIC_SV_SUB(FN, DIM, Y) _Generic((Y), float: FN##f, vec##DIM: FN##f##DIM, default: 0)
#define GENERIC_SVM_SUB(FN, DIM, Y) _Generic((Y), float: FN##f, vec##DIM: FN##f##DIM, mat##DIM: FN##m##DIM, default: 0)

#define GENERIC_VM_SVM(FN, X, Y) _Generic((X), vec2: GENERIC_SVM_SUB(FN##f2, 2, Y), vec3: GENERIC_SVM_SUB(FN##f3, 3, Y), vec4: GENERIC_SVM_SUB(FN##f4, 4, Y), mat2: GENERIC_SVM_SUB(FN##m2, 2, Y), mat3: GENERIC_SVM_SUB(FN##m3, 3, Y), mat4: GENERIC_SVM_SUB(FN##m4, 4, Y))(X, Y)
#define GENERIC_V_SV(FN, X, Y) _Generic((X), vec2: GENERIC_SV_SUB(FN##f2, 2, Y), vec3: GENERIC_SV_SUB(FN##f3, 3, Y), vec4: GENERIC_SV_SUB(FN##f4, 4, Y))(X, Y)
#define GENERIC_V_SV2(FN, X, Y, Z) _Generic((X), vec2: GENERIC_SV_SUB(FN##f2, 2, Y), vec3: GENERIC_SV_SUB(FN##f3, 3, Y), vec4: GENERIC_SV_SUB(FN##f4, 4, Y))(X, Y, Z)
#define GENERIC_SV_V(FN, X, Y) _Generic((X), float: GENERIC_V_SUB(FN##f, Y), vec2: GENERIC_SV_SUB(FN##f2, 2, Y), vec3: GENERIC_SV_SUB(FN##f3, 3, Y), vec4: GENERIC_SV_SUB(FN##f4, 4, Y))(X, Y)
#define GENERIC_SV2_V(FN, X, Y, Z) _Generic((X), float: GENERIC_V_SUB(FN##f, Z), vec2: GENERIC_SV_SUB(FN##f2, 2, Z), vec3: GENERIC_SV_SUB(FN##f3, 3, Z), vec4: GENERIC_SV_SUB(FN##f4, 4, Z))(X, Y, Z)



#pragma mark - Arithmetic Operations

#define print(X)    GENERIC_SVM_REF(print_, X)
#define add(X, Y)   GENERIC_SVM2(add, X, Y)
#define sub(X, Y)   GENERIC_SVM2(sub, X, Y)
#define mul(X, Y)   GENERIC_VM_SVM(mul, X, Y)



#pragma mark - Angle & Trigonometry Functions

#define radians(X)  GENERIC_SV(radians, X)
#define degrees(X)  GENERIC_SV(degrees, X)
//#define sin(X)      GENERIC_SV(sin, X)
//#define cos(X)      GENERIC_SV(cos, X)
#define tan(X)      GENERIC_SV(tan, X)
#define asin(X)     GENERIC_SV(asin, X)
#define acos(X)     GENERIC_SV(acos, X)
#define atan(X)     GENERIC_SV(atan, X)
#define atan2(X, Y) GENERIC_SV2(atan2, X, Y)
#define sinh(X)     GENERIC_SV(sinh, X)
#define cosh(X)     GENERIC_SV(cosh, X)
#define tanh(X)     GENERIC_SV(tanh, X)
#define asinh(X)    GENERIC_SV(asinh, X)
#define acosh(X)    GENERIC_SV(acosh, X)
#define atanh(X)    GENERIC_SV(atanh, X)



#pragma mark - Exponential Functions

#define pow(X, Y)       GENERIC_SV2(pow, X, Y)
#define exp(X)          GENERIC_SV(exp, X)
#define log(X)          GENERIC_SV(log, X)
#define exp2(X)         GENERIC_SV(exp2, X)
#define log2(X)         GENERIC_SV(log2, X)
#define sqrt(X)         GENERIC_SV(sqrt, X)
#define inversesqrt(X)  GENERIC_SV(inversesqrt, X)



#pragma mark - Common Functions

//#define abs(X)              GENERIC_SV(abs, X)
#define sign(X)             GENERIC_SV(sign, X)
#define floor(X)            GENERIC_SV(floor, X)
#define trunc(X)            GENERIC_SV(trunc, X)
#define round(X)            GENERIC_SV(round, X)
#define roundEven(X)        GENERIC_SV(roundEven, X)
#define ceil(X)             GENERIC_SV(ceil, X)
#define fract(X)            GENERIC_SV(fract, X)
#define mod(X, Y)           GENERIC_V_SV(mod, X, Y)
#define min(X, Y)           GENERIC_V_SV(min, X, Y)
#define max(X, Y)           GENERIC_V_SV(max, X, Y)
#define clamp(X, Y, Z)      GENERIC_V_SV2(clamp, X, Y, Z)
#define mix(X, Y)           GENERIC_V_SV(mix, X, Y)
#define step(X, Y)          GENERIC_SV_V(step, X, Y)
#define smoothstep(X, Y, Z) GENERIC_SV2_V(smoothstep, X, Y, Z)



#pragma mark - Geometric Functions

#define length(X)               GENERIC_SV(length, X)
#define distance(X, Y)          GENERIC_SV2(distance, X, Y) // fdim
#define dot(X, Y)               GENERIC_SV2(dot, X, Y)
#define cross(X, Y)             crossf3(X, Y)
#define normalize(X)            GENERIC_SV(normalize, X)
#define faceforward(X, Y, Z)    GENERIC_SV3(faceforward, X, Y, Z)
#define reflect(X, Y)           GENERIC_SV2(reflect, X, Y)
#define refract(X, Y, Z)        GENERIC_SV3(refract, X, Y, Z)



#pragma mark - Matrix Functions

#define matrixCompMult(X, Y)    GENERIC_M2(matrixCompMult, X, Y)
#define outerProduct(X, Y)      GENERIC_V2(outerProduct, X, Y)
#define transpose(X)            GENERIC_M(transpose, X)
#define determinant(X)          GENERIC_M(determinant, X)
#define inverse(X)              GENERIC_M(inverse, X)





#define matSet(X, S) _Generic((X), \
                              mat2: mat2Set, \
                              mat3: mat3Set, \
                              mat4: mat4Set)(X, S)

#define inverseTranspose(X) _Generic((X), \
                                     mat2: mat2InverseTranspose, \
                                     mat3: mat3InverseTranspose, \
                                     mat4: mat4InverseTranspose)(X)




//abs, clamp, smoothstep, min, max, floor

static inline float signf(const float in_X);
static inline float fractf(const float in_X);
static inline float clampf(const float in_X, const float in_MIN, const float in_MAX);
static inline float mixf(const float in_X, const float in_Y, const float in_A);
static inline float stepf(const float in_EDGE, const float in_X);
static inline float smoothstepf(const float in_EDGE_0, const float in_EDGE_1, const float in_X);
static inline float inverseSQRT(const float in_S);
static inline float fastInverseSQRT(const float in_S);

typedef union {
    struct {
        float x, y;
    };
    struct {
        float s, t;
    };
    float v[2];
} vec2;

typedef union {
    struct {
        float x, y, z;
    };
    struct {
        float r, g, b;
    };
    struct {
        float s, t, p;
    };
    float v[3];
} vec3;

typedef union {
    struct {
        float x, y, z, w;
    };
    struct {
        float r, g, b, a;
    };
    struct {
        float s, t, p, q;
    };
    float v[4];
} vec4;



// matrix in column major order
// 0 2
// 1 3
//
// 0 3 6
// 1 4 7
// 2 5 8
//
// 0 4 8  12
// 1 5 9  13
// 2 6 10 14
// 3 7 11 15
typedef union {
    struct {
        float m00, m10, m01, m11;
    };
    struct {
        vec2 v0, v1;
    };
    float m[4];
} mat2;

typedef union {
    struct {
        float m00, m10, m20, m01, m11, m21, m02, m12, m22;
    };
    struct {
        vec3 v0, v1, v2;
    };
    float m[9];
} mat3;

typedef union {
    struct {
        float m00, m10, m20, m30, m01, m11, m21, m31, m02, m12, m22, m32, m03, m13, m23, m33;
    };
    struct {
        vec4 v0, v1, v2, v3;
    };
    float m[16];
} mat4;



static inline mat2 mat2Make(const float in_M00, const float in_M10, const float in_M01, const float in_M11);
static inline mat3 mat3Make(const float in_M00, const float in_M10, const float in_M20, const float in_M01, const float in_M11, const float in_M21, const float in_M02, const float in_M12,
                            const float in_M22);
static inline mat4 mat4Make(const float in_M00, const float in_M10, const float in_M20, const float in_M30, const float in_M01, const float in_M11, const float in_M21, const float in_M31,
                            const float in_M02, const float in_M12, const float in_M22, const float in_M32, const float in_M03, const float in_M13, const float in_M23, const float in_M33);

static inline mat2 mat2MakeWithDiagonal(const float in_S);
static inline mat3 mat3MakeWithDiagonal(const float in_S);
static inline mat4 mat4MakeWithDiagonal(const float in_S);

static inline mat2 mat2MakeWithColumns(const vec2 in_V0, const vec2 in_V1);
static inline mat3 mat3MakeWithColumns(const vec2 in_V0, const vec2 in_V1, const vec2 in_V2);
static inline mat4 mat4MakeWithColumns(const vec2 in_V0, const vec2 in_V1, const vec2 in_V2, const vec2 in_V3);

static inline mat4 mat4MakeOrtho(const float in_L, const float in_R, const float in_B, const float in_T, const float in_N, const float in_F);
static inline mat4 mat4MakeFrustum(const float in_L, const float in_R, const float in_B, const float in_T, const float in_N, const float in_F);
static inline mat4 mat4MakePerspective(const float in_FOV_RADIANS, const float in_AR, const float in_N, const float in_F);
static inline mat4 mat4MakeRotate(const float in_RADIANS, const vec3 in_V);
static inline mat4 mat4MakeRotateX(const float in_RADIANS);
static inline mat4 mat4MakeRotateY(const float in_RADIANS);
static inline mat4 mat4MakeRotateZ(const float in_RADIANS);
static inline mat4 mat4MakeScale(const float in_S);
static inline mat4 mat4MakeTranslate(const vec3 in_V);





static inline float signf(const float in_X) {
    float result = 0;
    result = (in_X < 0) ? -1 : result;
    result = (in_X > 0) ?  1 : result;
    return result;
}

// I'm not sure if this is correct for negative numbers
static inline float fractf(const float in_X) {
    return in_X - floorf(in_X);
}

static inline float clampf(const float in_X, const float in_MIN, const float in_MAX) {
    return fminf(fmaxf(in_X, in_MIN), in_MAX);
}

static inline float mixf(const float in_X, const float in_Y, const float in_A) {
    return fmaf(in_A, in_Y - in_X, in_X);
}

static inline float stepf(const float in_EDGE, const float in_X) {
    return (in_X < in_EDGE) ? 0 : 1;
}

static inline float smoothstepf(const float in_EDGE_0, const float in_EDGE_1, const float in_X) {
    float t = clampf((in_X - in_EDGE_0) / (in_EDGE_1 - in_EDGE_0), 0, 1);
    return t * t * (3.0f - 2.0f * t);
}

static inline float inverseSQRT(const float in_S) {
    return 1.0f / sqrtf(in_S);
}

static inline float fastInverseSQRT(const float in_S) {
    // use SSE's rsqrtss instead
    float xhalf = 0.5f * in_S;

    union {
        float f;
        unsigned int i;
    } fi;

    fi.f = in_S;
    fi.i = 0x5f375a86 - (fi.i >> 1);
    fi.f = fi.f * (1.5f - xhalf * fi.f * fi.f);
    return fi.f;

    //unsigned int i = *(unsigned int*)&in_S;
    //i = 0x5f375a86 - (i >> 1);
    //float tmp = *(float*)&i;
    //tmp = tmp * (1.5f - xhalf * tmp * tmp);
    //tmp = tmp * (1.5f - xhalf * tmp * tmp);
    //return tmp;
}


static inline void print_f(const float *in_S) {
    puts("float:");
    printf("%f\n", *in_S);
}

static inline void print_f2(const vec2 *in_V) {
    puts("vec2:");
    printf("( %f, %f )\n", in_V->v[0], in_V->v[1]);
}

static inline void print_f3(const vec3 *in_V) {
    puts("vec3:");
    printf("( %f, %f, %f )\n", in_V->v[0], in_V->v[1], in_V->v[2]);
}

static inline void print_f4(const vec4 *in_V) {
    puts("vec4:");
    printf("( %f, %f, %f, %f )\n", in_V->v[0], in_V->v[1], in_V->v[2], in_V->v[3]);
}

static inline void print_m2(const mat2 *in_M) {
    puts("mat2:");
    printf("%f %f\n", in_M->m[0], in_M->m[2]);
    printf("%f %f\n", in_M->m[1], in_M->m[3]);
}

static inline void print_m3(const mat3 *in_M) {
    puts("mat3:");
    printf("%f %f %f\n", in_M->m[0], in_M->m[3], in_M->m[6]);
    printf("%f %f %f\n", in_M->m[1], in_M->m[4], in_M->m[7]);
    printf("%f %f %f\n", in_M->m[2], in_M->m[5], in_M->m[8]);
}

static inline void print_m4(const mat4 *in_M) {
    puts("mat4:");
    printf("%f %f %f %f\n", in_M->m[0], in_M->m[4], in_M->m[8],  in_M->m[12]);
    printf("%f %f %f %f\n", in_M->m[1], in_M->m[5], in_M->m[9],  in_M->m[13]);
    printf("%f %f %f %f\n", in_M->m[2], in_M->m[6], in_M->m[10], in_M->m[14]);
    printf("%f %f %f %f\n", in_M->m[3], in_M->m[7], in_M->m[11], in_M->m[15]);
}

static inline mat2 mat2MakeWithDiagonal(const float in_S) {
    mat2 m;
    m.m[0] = in_S;
    m.m[2] = 0;
    m.m[1] = 0;
    m.m[3] = in_S;
    return m;
}

static inline mat3 mat3MakeWithDiagonal(const float in_S) {
    mat3 m;
    m.m[0] = in_S;
    m.m[3] = 0;
    m.m[6] = 0;
    m.m[1] = 0;
    m.m[4] = in_S;
    m.m[7] = 0;
    m.m[2] = 0;
    m.m[5] = 0;
    m.m[8] = in_S;
    return m;
}

static inline mat4 mat4MakeWithDiagonal(const float in_S) {
    mat4 m;
    m.m[0] = in_S;
    m.m[4] = 0;
    m.m[8]  = 0;
    m.m[12] = 0;
    m.m[1] = 0;
    m.m[5] = in_S;
    m.m[9]  = 0;
    m.m[13] = 0;
    m.m[2] = 0;
    m.m[6] = 0;
    m.m[10] = in_S;
    m.m[14] = 0;
    m.m[3] = 0;
    m.m[7] = 0;
    m.m[11] = 0;
    m.m[15] = in_S;
    return m;
}

static inline float addf(const float in_A, const float in_B) {
    return in_A + in_B;
}

static inline vec2 addf2(const vec2 in_A, const vec2 in_B) {
    vec2 v;
    v.v[0] = in_A.v[0] + in_B.v[0];
    v.v[1] = in_A.v[1] + in_B.v[1];
    return v;
}

static inline vec3 addf3(const vec3 in_A, const vec3 in_B) {
    vec3 v;
    v.v[0] = in_A.v[0] + in_B.v[0];
    v.v[1] = in_A.v[1] + in_B.v[1];
    v.v[2] = in_A.v[2] + in_B.v[2];
    return v;
}

static inline vec4 addf4(const vec4 in_A, const vec4 in_B) {
    vec4 v;
    v.v[0] = in_A.v[0] + in_B.v[0];
    v.v[1] = in_A.v[1] + in_B.v[1];
    v.v[2] = in_A.v[2] + in_B.v[2];
    v.v[3] = in_A.v[3] + in_B.v[3];
    return v;
}

static inline mat2 addm2(const mat2 in_A, const mat2 in_B) {
    mat2 m;
    m.m[0] = in_A.m[0] + in_B.m[0];
    m.m[1] = in_A.m[1] + in_B.m[1];
    m.m[2] = in_A.m[2] + in_B.m[2];
    m.m[3] = in_A.m[3] + in_B.m[3];
    return m;
}

static inline mat3 addm3(const mat3 in_A, const mat3 in_B) {
    mat3 m;
    m.m[0] = in_A.m[0] + in_B.m[0];
    m.m[1] = in_A.m[1] + in_B.m[1];
    m.m[2] = in_A.m[2] + in_B.m[2];
    m.m[3] = in_A.m[3] + in_B.m[3];
    m.m[4] = in_A.m[4] + in_B.m[4];
    m.m[5] = in_A.m[5] + in_B.m[5];
    m.m[6] = in_A.m[6] + in_B.m[6];
    m.m[7] = in_A.m[7] + in_B.m[7];
    m.m[8] = in_A.m[8] + in_B.m[8];
    return m;
}

static inline mat4 addm4(const mat4 in_A, const mat4 in_B) {
    mat4 m;
    m.m[0]  = in_A.m[0]  + in_B.m[0];
    m.m[1]  = in_A.m[1]  + in_B.m[1];
    m.m[2]  = in_A.m[2]  + in_B.m[2];
    m.m[3]  = in_A.m[3]  + in_B.m[3];
    m.m[4]  = in_A.m[4]  + in_B.m[4];
    m.m[5]  = in_A.m[5]  + in_B.m[5];
    m.m[6]  = in_A.m[6]  + in_B.m[6];
    m.m[7]  = in_A.m[7]  + in_B.m[7];
    m.m[8]  = in_A.m[8]  + in_B.m[8];
    m.m[9]  = in_A.m[9]  + in_B.m[9];
    m.m[10] = in_A.m[10] + in_B.m[10];
    m.m[11] = in_A.m[11] + in_B.m[11];
    m.m[12] = in_A.m[12] + in_B.m[12];
    m.m[13] = in_A.m[13] + in_B.m[13];
    m.m[14] = in_A.m[14] + in_B.m[14];
    m.m[15] = in_A.m[15] + in_B.m[15];
    return m;
}

static inline float subf(const float in_A, const float in_B) {
    return in_A - in_B;
}

static inline vec2 subf2(const vec2 in_A, const vec2 in_B) {
    vec2 v;
    v.v[0] = in_A.v[0] - in_B.v[0];
    v.v[1] = in_A.v[1] - in_B.v[1];
    return v;
}

static inline vec3 subf3(const vec3 in_A, const vec3 in_B) {
    vec3 v;
    v.v[0] = in_A.v[0] - in_B.v[0];
    v.v[1] = in_A.v[1] - in_B.v[1];
    v.v[2] = in_A.v[2] - in_B.v[2];
    return v;
}

static inline vec4 subf4(const vec4 in_A, const vec4 in_B) {
    vec4 v;
    v.v[0] = in_A.v[0] - in_B.v[0];
    v.v[1] = in_A.v[1] - in_B.v[1];
    v.v[2] = in_A.v[2] - in_B.v[2];
    v.v[3] = in_A.v[3] - in_B.v[3];
    return v;
}

static inline mat2 subm2(const mat2 in_A, const mat2 in_B) {
    mat2 m;
    m.m[0] = in_A.m[0] - in_B.m[0];
    m.m[1] = in_A.m[1] - in_B.m[1];
    m.m[2] = in_A.m[2] - in_B.m[2];
    m.m[3] = in_A.m[3] - in_B.m[3];
    return m;
}

static inline mat3 subm3(const mat3 in_A, const mat3 in_B) {
    mat3 m;
    m.m[0] = in_A.m[0] - in_B.m[0];
    m.m[1] = in_A.m[1] - in_B.m[1];
    m.m[2] = in_A.m[2] - in_B.m[2];
    m.m[3] = in_A.m[3] - in_B.m[3];
    m.m[4] = in_A.m[4] - in_B.m[4];
    m.m[5] = in_A.m[5] - in_B.m[5];
    m.m[6] = in_A.m[6] - in_B.m[6];
    m.m[7] = in_A.m[7] - in_B.m[7];
    m.m[8] = in_A.m[8] - in_B.m[8];
    return m;
}

static inline mat4 subm4(const mat4 in_A, const mat4 in_B) {
    mat4 m;
    m.m[0]  = in_A.m[0]  - in_B.m[0];
    m.m[1]  = in_A.m[1]  - in_B.m[1];
    m.m[2]  = in_A.m[2]  - in_B.m[2];
    m.m[3]  = in_A.m[3]  - in_B.m[3];
    m.m[4]  = in_A.m[4]  - in_B.m[4];
    m.m[5]  = in_A.m[5]  - in_B.m[5];
    m.m[6]  = in_A.m[6]  - in_B.m[6];
    m.m[7]  = in_A.m[7]  - in_B.m[7];
    m.m[8]  = in_A.m[8]  - in_B.m[8];
    m.m[9]  = in_A.m[9]  - in_B.m[9];
    m.m[10] = in_A.m[10] - in_B.m[10];
    m.m[11] = in_A.m[11] - in_B.m[11];
    m.m[12] = in_A.m[12] - in_B.m[12];
    m.m[13] = in_A.m[13] - in_B.m[13];
    m.m[14] = in_A.m[14] - in_B.m[14];
    m.m[15] = in_A.m[15] - in_B.m[15];
    return m;
}

static inline vec2 mulf2f2(const vec2 in_A, const vec2 in_B) {
    vec2 v;
    v.v[0] = in_A.v[0] * in_B.v[0];
    v.v[1] = in_A.v[1] * in_B.v[1];
    return v;
}

static inline vec3 mulf3f3(const vec3 in_A, const vec3 in_B) {
    vec3 v;
    v.v[0] = in_A.v[0] * in_B.v[0];
    v.v[1] = in_A.v[1] * in_B.v[1];
    v.v[2] = in_A.v[2] * in_B.v[2];
    return v;
}

static inline vec4 mulf4f4(const vec4 in_A, const vec4 in_B) {
    vec4 v;
    v.v[0] = in_A.v[0] * in_B.v[0];
    v.v[1] = in_A.v[1] * in_B.v[1];
    v.v[2] = in_A.v[2] * in_B.v[2];
    v.v[3] = in_A.v[3] * in_B.v[3];
    return v;
}

static inline mat2 mulm2m2(const mat2 in_A, const mat2 in_B) {
    mat2 m;
    m.m[0] = in_A.m[0] * in_B.m[0] + in_A.m[2] * in_B.m[1];
    m.m[1] = in_A.m[1] * in_B.m[0] + in_A.m[3] * in_B.m[1];
    m.m[2] = in_A.m[0] * in_B.m[2] + in_A.m[2] * in_B.m[3];
    m.m[3] = in_A.m[1] * in_B.m[2] + in_A.m[3] * in_B.m[3];
    return m;
}

static inline mat3 mulm3m3(const mat3 in_A, const mat3 in_B) {
    mat3 m;
    m.m[0] = in_A.m[0] * in_B.m[0] + in_A.m[3] * in_B.m[1] + in_A.m[6] * in_B.m[2];
    m.m[1] = in_A.m[1] * in_B.m[0] + in_A.m[4] * in_B.m[1] + in_A.m[7] * in_B.m[2];
    m.m[2] = in_A.m[2] * in_B.m[0] + in_A.m[5] * in_B.m[1] + in_A.m[8] * in_B.m[2];
    m.m[3] = in_A.m[0] * in_B.m[3] + in_A.m[3] * in_B.m[4] + in_A.m[6] * in_B.m[5];
    m.m[4] = in_A.m[1] * in_B.m[3] + in_A.m[4] * in_B.m[4] + in_A.m[7] * in_B.m[5];
    m.m[5] = in_A.m[2] * in_B.m[3] + in_A.m[5] * in_B.m[4] + in_A.m[8] * in_B.m[5];
    m.m[6] = in_A.m[0] * in_B.m[6] + in_A.m[3] * in_B.m[7] + in_A.m[6] * in_B.m[8];
    m.m[7] = in_A.m[1] * in_B.m[6] + in_A.m[4] * in_B.m[7] + in_A.m[7] * in_B.m[8];
    m.m[8] = in_A.m[2] * in_B.m[6] + in_A.m[5] * in_B.m[7] + in_A.m[8] * in_B.m[8];
    return m;
}

static inline mat4 mulm4m4(const mat4 in_A, const mat4 in_B) {
    mat4 m;
    m.m[0]  = in_A.m[0] * in_B.m[0]  + in_A.m[4] * in_B.m[1]  + in_A.m[8]  * in_B.m[2]  + in_A.m[12] * in_B.m[3];
    m.m[1]  = in_A.m[1] * in_B.m[0]  + in_A.m[5] * in_B.m[1]  + in_A.m[9]  * in_B.m[2]  + in_A.m[13] * in_B.m[3];
    m.m[2]  = in_A.m[2] * in_B.m[0]  + in_A.m[6] * in_B.m[1]  + in_A.m[10] * in_B.m[2]  + in_A.m[14] * in_B.m[3];
    m.m[3]  = in_A.m[3] * in_B.m[0]  + in_A.m[7] * in_B.m[1]  + in_A.m[11] * in_B.m[2]  + in_A.m[15] * in_B.m[3];
    m.m[4]  = in_A.m[0] * in_B.m[4]  + in_A.m[4] * in_B.m[5]  + in_A.m[8]  * in_B.m[6]  + in_A.m[12] * in_B.m[7];
    m.m[5]  = in_A.m[1] * in_B.m[4]  + in_A.m[5] * in_B.m[5]  + in_A.m[9]  * in_B.m[6]  + in_A.m[13] * in_B.m[7];
    m.m[6]  = in_A.m[2] * in_B.m[4]  + in_A.m[6] * in_B.m[5]  + in_A.m[10] * in_B.m[6]  + in_A.m[14] * in_B.m[7];
    m.m[7]  = in_A.m[3] * in_B.m[4]  + in_A.m[7] * in_B.m[5]  + in_A.m[11] * in_B.m[6]  + in_A.m[15] * in_B.m[7];
    m.m[8]  = in_A.m[0] * in_B.m[8]  + in_A.m[4] * in_B.m[9]  + in_A.m[8]  * in_B.m[10] + in_A.m[12] * in_B.m[11];
    m.m[9]  = in_A.m[1] * in_B.m[8]  + in_A.m[5] * in_B.m[9]  + in_A.m[9]  * in_B.m[10] + in_A.m[13] * in_B.m[11];
    m.m[10] = in_A.m[2] * in_B.m[8]  + in_A.m[6] * in_B.m[9]  + in_A.m[10] * in_B.m[10] + in_A.m[14] * in_B.m[11];
    m.m[11] = in_A.m[3] * in_B.m[8]  + in_A.m[7] * in_B.m[9]  + in_A.m[11] * in_B.m[10] + in_A.m[15] * in_B.m[11];
    m.m[12] = in_A.m[0] * in_B.m[12] + in_A.m[4] * in_B.m[13] + in_A.m[8]  * in_B.m[14] + in_A.m[12] * in_B.m[15];
    m.m[13] = in_A.m[1] * in_B.m[12] + in_A.m[5] * in_B.m[13] + in_A.m[9]  * in_B.m[14] + in_A.m[13] * in_B.m[15];
    m.m[14] = in_A.m[2] * in_B.m[12] + in_A.m[6] * in_B.m[13] + in_A.m[10] * in_B.m[14] + in_A.m[14] * in_B.m[15];
    m.m[15] = in_A.m[3] * in_B.m[12] + in_A.m[7] * in_B.m[13] + in_A.m[11] * in_B.m[14] + in_A.m[15] * in_B.m[15];
    return m;
}

static inline vec2 mulf2m2(const vec2 in_V, const mat2 in_M) {
    vec2 v;
    v.v[0] = in_V.v[0] * in_M.m[0] + in_V.v[1] * in_M.m[1]; // v dot m.v0
    v.v[1] = in_V.v[0] * in_M.m[2] + in_V.v[1] * in_M.m[3]; // v dot m.v1
    return v;
}

static inline vec3 mulf3m3(const vec3 in_V, const mat3 in_M) {
    vec3 v;
    v.v[0] = in_V.v[0] * in_M.m[0] + in_V.v[1] * in_M.m[1] + in_V.v[2] * in_M.m[2]; // v dot m.v0
    v.v[1] = in_V.v[0] * in_M.m[3] + in_V.v[1] * in_M.m[4] + in_V.v[2] * in_M.m[5]; // v dot m.v1
    v.v[2] = in_V.v[0] * in_M.m[6] + in_V.v[1] * in_M.m[7] + in_V.v[2] * in_M.m[8]; // v dot m.v2
    return v;
}

static inline vec4 mulf4m4(const vec4 in_V, const mat4 in_M) {
    vec4 v;
    v.v[0] = in_V.v[0] * in_M.m[0]  + in_V.v[1] * in_M.m[1]  + in_V.v[2] * in_M.m[2]  + in_V.v[3] * in_M.m[3];  // v dot m.v0
    v.v[1] = in_V.v[0] * in_M.m[4]  + in_V.v[1] * in_M.m[5]  + in_V.v[2] * in_M.m[6]  + in_V.v[3] * in_M.m[7];  // v dot m.v1
    v.v[2] = in_V.v[0] * in_M.m[8]  + in_V.v[1] * in_M.m[9]  + in_V.v[2] * in_M.m[10] + in_V.v[3] * in_M.m[11]; // v dot m.v2
    v.v[3] = in_V.v[0] * in_M.m[12] + in_V.v[1] * in_M.m[13] + in_V.v[2] * in_M.m[14] + in_V.v[3] * in_M.m[15]; // v dot m.v3
    return v;
}

static inline vec2 mulm2f2(const mat2 in_M, const vec2 in_V) {
    vec2 v;
    v.v[0] = in_M.m[0] * in_V.v[0] + in_M.m[2] * in_V.v[1] ;
    v.v[1] = in_M.m[1] * in_V.v[0] + in_M.m[3] * in_V.v[1] ;
    return v;
}

static inline vec3 mulm3f3(const mat3 in_M, const vec3 in_V) {
    vec3 v;
    v.v[0] = in_M.m[0] * in_V.v[0] + in_M.m[3] * in_V.v[1] + in_M.m[6] * in_V.v[2];
    v.v[1] = in_M.m[1] * in_V.v[0] + in_M.m[4] * in_V.v[1] + in_M.m[7] * in_V.v[2];
    v.v[2] = in_M.m[2] * in_V.v[0] + in_M.m[5] * in_V.v[1] + in_M.m[8] * in_V.v[2];
    return v;
}

static inline vec4 mulm4f4(const mat4 in_M, const vec4 in_V) {
    vec4 v;
    v.v[0] = in_M.m[0] * in_V.v[0] + in_M.m[4] * in_V.v[1] + in_M.m[8]  * in_V.v[2] + in_M.m[12] * in_V.v[3];
    v.v[1] = in_M.m[1] * in_V.v[0] + in_M.m[5] * in_V.v[1] + in_M.m[9]  * in_V.v[2] + in_M.m[13] * in_V.v[3];
    v.v[2] = in_M.m[2] * in_V.v[0] + in_M.m[6] * in_V.v[1] + in_M.m[10] * in_V.v[2] + in_M.m[14] * in_V.v[3];
    v.v[3] = in_M.m[3] * in_V.v[0] + in_M.m[7] * in_V.v[1] + in_M.m[11] * in_V.v[2] + in_M.m[15] * in_V.v[3];
    return v;
}

static inline vec2 mulf2f(const vec2 in_V, const float in_S) {
    vec2 v;
    v.v[0] = in_V.v[0] * in_S;
    v.v[1] = in_V.v[1] * in_S;
    return v;
}

static inline vec3 mulf3f(const vec3 in_V, const float in_S) {
    vec3 v;
    v.v[0] = in_V.v[0] * in_S;
    v.v[1] = in_V.v[1] * in_S;
    v.v[2] = in_V.v[2] * in_S;
    return v;
}

static inline vec4 mulf4f(const vec4 in_V, const float in_S) {
    vec4 v;
    v.v[0] = in_V.v[0] * in_S;
    v.v[1] = in_V.v[1] * in_S;
    v.v[2] = in_V.v[2] * in_S;
    v.v[3] = in_V.v[3] * in_S;
    return v;
}

static inline mat2 mulm2f(const mat2 in_M, const float in_S) {
    mat2 m;
    m.m[0] = in_M.m[0] * in_S;
    m.m[1] = in_M.m[1] * in_S;
    m.m[2] = in_M.m[2] * in_S;
    m.m[3] = in_M.m[3] * in_S;
    return m;
}

static inline mat3 mulm3f(const mat3 in_M, const float in_S) {
    mat3 m;
    m.m[0] = in_M.m[0] * in_S;
    m.m[1] = in_M.m[1] * in_S;
    m.m[2] = in_M.m[2] * in_S;
    m.m[3] = in_M.m[3] * in_S;
    m.m[4] = in_M.m[4] * in_S;
    m.m[5] = in_M.m[5] * in_S;
    m.m[6] = in_M.m[6] * in_S;
    m.m[7] = in_M.m[7] * in_S;
    m.m[8] = in_M.m[8] * in_S;
    return m;
}

static inline mat4 mulm4f(const mat4 in_M, const float in_S) {
    mat4 m;
    m.m[0]  = in_M.m[0]  * in_S;
    m.m[1]  = in_M.m[1]  * in_S;
    m.m[2]  = in_M.m[2]  * in_S;
    m.m[3]  = in_M.m[3]  * in_S;
    m.m[4]  = in_M.m[4]  * in_S;
    m.m[5]  = in_M.m[5]  * in_S;
    m.m[6]  = in_M.m[6]  * in_S;
    m.m[7]  = in_M.m[7]  * in_S;
    m.m[8]  = in_M.m[8]  * in_S;
    m.m[9]  = in_M.m[9]  * in_S;
    m.m[10] = in_M.m[10] * in_S;
    m.m[11] = in_M.m[11] * in_S;
    m.m[12] = in_M.m[12] * in_S;
    m.m[13] = in_M.m[13] * in_S;
    m.m[14] = in_M.m[14] * in_S;
    m.m[15] = in_M.m[15] * in_S;
    return m;
}

static inline float vec2Length(const vec2 in_V) {
    float sqr = in_V.v[0] * in_V.v[0] + in_V.v[1] * in_V.v[1];
    return sqrtf(sqr);
}

static inline float vec3Length(const vec3 in_V) {
    float sqr = in_V.v[0] * in_V.v[0] + in_V.v[1] * in_V.v[1] + in_V.v[2] * in_V.v[2];
    return sqrtf(sqr);
}

static inline float vec4Length(const vec4 in_V) {
    float sqr = in_V.v[0] * in_V.v[0] + in_V.v[1] * in_V.v[1] + in_V.v[2] * in_V.v[2] + in_V.v[3] * in_V.v[3];
    return sqrtf(sqr);
}

static inline float vec2Distance(const vec2 in_A, const vec2 in_B) {
    vec2 v;
    v.v[0] = in_A.v[0] - in_B.v[0];
    v.v[1] = in_A.v[1] - in_B.v[1];
    return vec2Length(v);
}

static inline float vec3Distance(const vec3 in_A, const vec3 in_B) {
    vec3 v;
    v.v[0] = in_A.v[0] - in_B.v[0];
    v.v[1] = in_A.v[1] - in_B.v[1];
    v.v[2] = in_A.v[2] - in_B.v[2];
    return vec3Length(v);
}

static inline float vec4Distance(const vec4 in_A, const vec4 in_B) {
    vec4 v;
    v.v[0] = in_A.v[0] - in_B.v[0];
    v.v[1] = in_A.v[1] - in_B.v[1];
    v.v[2] = in_A.v[2] - in_B.v[2];
    v.v[3] = in_A.v[3] - in_B.v[3];
    return vec4Length(v);
}

static inline float vec2Dot(const vec2 in_A, const vec2 in_B) {
    return in_A.v[0] * in_B.v[0] + in_A.v[1] * in_B.v[1];
}

static inline float vec3Dot(const vec3 in_A, const vec3 in_B) {
    return in_A.v[0] * in_B.v[0] + in_A.v[1] * in_B.v[1] + in_A.v[2] * in_B.v[2];
}

static inline float vec4Dot(const vec4 in_A, const vec4 in_B) {
    return in_A.v[0] * in_B.v[0] + in_A.v[1] * in_B.v[1] + in_A.v[2] * in_B.v[2] + in_A.v[3] * in_B.v[3];
}

static inline vec3 vec3Cross(const vec3 in_A, const vec3 in_B) {
    vec3 v;
    v.v[0] = in_A.v[1] * in_B.v[2] - in_A.v[2] * in_B.v[1];
    v.v[1] = in_A.v[2] * in_B.v[0] - in_A.v[0] * in_B.v[2];
    v.v[2] = in_A.v[0] * in_B.v[1] - in_A.v[1] * in_B.v[0];
    return v;
}

static inline float normalizef(const float in_S) {
    float result = 0;
    result = (in_S < 0) ? -1 : result;
    result = (in_S > 0) ?  1 : result;
    return result;
}

static inline vec2 normalizef2(const vec2 in_V) {
    vec2 v;
    float sqr = in_V.v[0] * in_V.v[0] + in_V.v[1] * in_V.v[1];
    float invSQRT = inverseSQRT(sqr);
    v.v[0] = in_V.v[0] * invSQRT;
    v.v[1] = in_V.v[1] * invSQRT;
    return v;
}

static inline vec3 normalizef3(const vec3 in_V) {
    vec3 v;
    const float sqr = in_V.v[0] * in_V.v[0] + in_V.v[1] * in_V.v[1] + in_V.v[2] * in_V.v[2];
    const float invSQRT = inverseSQRT(sqr);
    v.v[0] = in_V.v[0] * invSQRT;
    v.v[1] = in_V.v[1] * invSQRT;
    v.v[2] = in_V.v[2] * invSQRT;
    return v;
}

static inline vec4 normalizef4(const vec4 in_V) {
    vec4 v;
    const float sqr = in_V.v[0] * in_V.v[0] + in_V.v[1] * in_V.v[1] + in_V.v[2] * in_V.v[2] + in_V.v[3] * in_V.v[3];
    const float invSQRT = inverseSQRT(sqr);
    v.v[0] = in_V.v[0] * invSQRT;
    v.v[1] = in_V.v[1] * invSQRT;
    v.v[2] = in_V.v[2] * invSQRT;
    v.v[3] = in_V.v[3] * invSQRT;
    return v;
}



static inline mat4 mat4MakeOrtho(const float in_L, const float in_R, const float in_B, const float in_T, const float in_N, const float in_F) {
    mat4 m;
    const float rl = 1.0f / (in_R - in_L);
    const float tb = 1.0f / (in_T - in_B);
    const float fn = 1.0f / (in_N - in_F);
    m.m[0] = 2 * rl;
    m.m[4] = 0;
    m.m[8]  = 0;
    m.m[12] = -(in_L + in_R) * rl;
    m.m[1] = 0;
    m.m[5] = 2 * tb;
    m.m[9]  = 0;
    m.m[13] = -(in_B + in_T) * tb;
    m.m[2] = 0;
    m.m[6] = 0;
    m.m[10] = 2 * fn;
    m.m[14] = (in_N + in_F) * fn;
    m.m[3] = 0;
    m.m[7] = 0;
    m.m[11] = 0;
    m.m[15] = 1;
    return m;
}


static inline mat4 mat4MakeFrustum(const float in_L, const float in_R, const float in_B, const float in_T, const float in_N, const float in_F) {
    mat4 m;
    const float rl = 1.0f / (in_R - in_L);
    const float tb = 1.0f / (in_T - in_B);
    const float fn = 1.0f / (in_N - in_F);
    const float n2 = 2 * in_N;
    m.m[0] = n2 * rl;
    m.m[4] = 0;
    m.m[8]  = (in_L + in_R) * rl;
    m.m[12] = 0;
    m.m[1] = 0;
    m.m[5] = n2 * tb;
    m.m[9]  = (in_B + in_T) * tb;
    m.m[13] = 0;
    m.m[2] = 0;
    m.m[6] = 0;
    m.m[10] = (in_N + in_F) * fn;
    m.m[14] = 2.0f * in_F * in_N * fn;
    m.m[3] = 0;
    m.m[7] = 0;
    m.m[11] = -1;
    m.m[15] = 0;
    return m;
}


static inline mat4 mat4MakePerspective(const float in_FOV_RADIANS, const float in_AR, const float in_N, const float in_F) {
    mat4 m;
    const float f = tanf(2.0f / in_FOV_RADIANS);
    const float fn = 1.0f / (in_N - in_F);
    m.m[0] = f / in_AR;
    m.m[4] = 0;
    m.m[8]  = 0;
    m.m[12] = 0;
    m.m[1] = 0;
    m.m[5] = f;
    m.m[9]  = 0;
    m.m[13] = 0;
    m.m[2] = 0;
    m.m[6] = 0;
    m.m[10] = (in_N + in_F) * fn;
    m.m[14] = 2.0f * in_F * in_N * fn;
    m.m[3] = 0;
    m.m[7] = 0;
    m.m[11] = -1;
    m.m[15] = 0;
    return m;
}


static inline mat4 mat4MakeRotate(const float in_RADIANS, const vec3 in_V) {
    mat4 m;
    const float c = cosf(in_RADIANS);
    const float s = sinf(in_RADIANS);
    vec3 n = normalizef3(in_V);
    m.m[0] = n.x * n.x * (1.0f - c) + c;
    m.m[4] = n.y * n.x * (1.0f - c) - n.z * s;
    m.m[8]  = n.z * n.x * (1.0f - c) + n.y * s;
    m.m[12] = 0;
    m.m[1] = n.x * n.y * (1.0f - c) + n.z * s;
    m.m[5] = n.y * n.y * (1.0f - c) + c;
    m.m[9]  = n.z * n.y * (1.0f - c) - n.x * s;
    m.m[13] = 0;
    m.m[2] = n.x * n.z * (1.0f - c) - n.y * s;
    m.m[6] = n.y * n.z * (1.0f - c) + n.x * s;
    m.m[10] = n.z * n.z * (1.0f - c) + c;
    m.m[14] = 0;
    m.m[3] = 0;
    m.m[7] = 0;
    m.m[11] =  0;
    m.m[15] = 1;
    return m;
}


static inline mat4 mat4MakeRotateX(const float in_RADIANS) {
    mat4 m;
    const float c = cosf(in_RADIANS);
    const float s = sinf(in_RADIANS);
    m.m[0] = 1;
    m.m[4] = 0;
    m.m[8]  =  0;
    m.m[12] = 0;
    m.m[1] = 0;
    m.m[5] = c;
    m.m[9]  = -s;
    m.m[13] = 0;
    m.m[2] = 0;
    m.m[6] = s;
    m.m[10] =  c;
    m.m[14] = 0;
    m.m[3] = 0;
    m.m[7] = 0;
    m.m[11] =  0;
    m.m[15] = 1;
    return m;
}


static inline mat4 mat4MakeRotateY(const float in_RADIANS) {
    mat4 m;
    const float c = cosf(in_RADIANS);
    const float s = sinf(in_RADIANS);
    m.m[0] =  c;
    m.m[4] = 0;
    m.m[8]  = s;
    m.m[12] = 0;
    m.m[1] =  0;
    m.m[5] = 1;
    m.m[9]  = 0;
    m.m[13] = 0;
    m.m[2] = -s;
    m.m[6] = 0;
    m.m[10] = c;
    m.m[14] = 0;
    m.m[3] =  0;
    m.m[7] = 0;
    m.m[11] = 0;
    m.m[15] = 1;
    return m;
}


static inline mat2 mat2MakeRotateZ(const float in_RADIANS) {
    mat2 m;
    const float c = cosf(in_RADIANS);
    const float s = sinf(in_RADIANS);
    m.m[0] = c;
    m.m[2] = -s;
    m.m[1] = s;
    m.m[3] =  c;
    return m;
}


static inline mat4 mat4MakeRotateZ(const float in_RADIANS) {
    mat4 m;
    const float c = cosf(in_RADIANS);
    const float s = sinf(in_RADIANS);
    m.m[0] = c;
    m.m[4] = -s;
    m.m[8]  = 0;
    m.m[12] = 0;
    m.m[1] = s;
    m.m[5] =  c;
    m.m[9]  = 0;
    m.m[13] = 0;
    m.m[2] = 0;
    m.m[6] =  0;
    m.m[10] = 1;
    m.m[14] = 0;
    m.m[3] = 0;
    m.m[7] =  0;
    m.m[11] = 0;
    m.m[15] = 1;
    return m;
}


static inline mat4 mat4MakeTranslate(const vec3 in_V) {
    mat4 m;
    m.m[0] = 1;
    m.m[4] = 0;
    m.m[8]  = 0;
    m.m[12] = in_V.v[0];
    m.m[1] = 0;
    m.m[5] = 1;
    m.m[9]  = 0;
    m.m[13] = in_V.v[1];
    m.m[2] = 0;
    m.m[6] = 0;
    m.m[10] = 1;
    m.m[14] = in_V.v[2];
    m.m[3] = 0;
    m.m[7] = 0;
    m.m[11] = 0;
    m.m[15] = 1;
    return m;
}


static inline mat2 mat2FromMat3(const mat3 in_M) {
    mat2 m;
    m.m[0] = in_M.m[0];
    m.m[2] = in_M.m[3];
    m.m[1] = in_M.m[1];
    m.m[3] = in_M.m[4];
    return m;
}


static inline mat3 mat3FromMat4(const mat4 in_M) {
    mat3 m;
    m.m[0] = in_M.m[0];
    m.m[3] = in_M.m[4];
    m.m[6] = in_M.m[8];
    m.m[1] = in_M.m[1];
    m.m[4] = in_M.m[5];
    m.m[7] = in_M.m[9];
    m.m[2] = in_M.m[2];
    m.m[5] = in_M.m[6];
    m.m[8] = in_M.m[10];
    return m;
}


static inline mat2 mat2Transpose(const mat2 in_M) {
    mat2 m;
    m.m[0] = in_M.m[0];
    m.m[2] = in_M.m[1];
    m.m[1] = in_M.m[2];
    m.m[3] = in_M.m[3];
    return m;
}


static inline mat3 mat3Transpose(const mat3 in_M) {
    mat3 m;
    m.m[0] = in_M.m[0];
    m.m[3] = in_M.m[1];
    m.m[6] = in_M.m[2];
    m.m[1] = in_M.m[3];
    m.m[4] = in_M.m[4];
    m.m[7] = in_M.m[5];
    m.m[2] = in_M.m[6];
    m.m[5] = in_M.m[7];
    m.m[8] = in_M.m[8];
    return m;
}


static inline mat4 mat4Transpose(const mat4 in_M) {
    mat4 m;
    m.m[0] = in_M.m[0];
    m.m[4] = in_M.m[1];
    m.m[8]  = in_M.m[2];
    m.m[12] = in_M.m[3];
    m.m[1] = in_M.m[4];
    m.m[5] = in_M.m[5];
    m.m[9]  = in_M.m[6];
    m.m[13] = in_M.m[7];
    m.m[2] = in_M.m[8];
    m.m[6] = in_M.m[9];
    m.m[10] = in_M.m[10];
    m.m[14] = in_M.m[11];
    m.m[3] = in_M.m[12];
    m.m[7] = in_M.m[13];
    m.m[11] = in_M.m[14];
    m.m[15] = in_M.m[15];
    return m;
}


static inline mat2 mat2InverseTranspose(const mat2 in_M) {
    mat2 m;
    float invDet = 1.0f / (in_M.m[0] * in_M.m[3] - in_M.m[1] * in_M.m[2]);
    m.m[0] = invDet * in_M.m[3];
    m.m[1] = invDet * -in_M.m[2];
    m.m[2] = invDet * -in_M.m[1];
    m.m[3] = invDet * in_M.m[1];
    return m;
}

static inline mat3 mat3InverseTranspose(const mat3 in_M) {
    mat3 m;
    float invDet = 1.0f / (in_M.m[0] * in_M.m[4] * in_M.m[8]
                           + in_M.m[3] * in_M.m[7] * in_M.m[2]
                           + in_M.m[6] * in_M.m[1] * in_M.m[5]
                           - in_M.m[2] * in_M.m[4] * in_M.m[6]
                           - in_M.m[5] * in_M.m[7] * in_M.m[0]
                           - in_M.m[8] * in_M.m[1] * in_M.m[3]);
    m.m[0] = invDet * (in_M.m[4] * in_M.m[8] - in_M.m[5] * in_M.m[7]);
    m.m[1] = invDet * (in_M.m[5] * in_M.m[6] - in_M.m[3] * in_M.m[8]);
    m.m[2] = invDet * (in_M.m[3] * in_M.m[7] - in_M.m[4] * in_M.m[6]);
    m.m[3] = invDet * (in_M.m[7] * in_M.m[2] - in_M.m[8] * in_M.m[1]);
    m.m[4] = invDet * (in_M.m[0] * in_M.m[8] - in_M.m[2] * in_M.m[6]);
    m.m[5] = invDet * (in_M.m[6] * in_M.m[1] - in_M.m[7] * in_M.m[0]);
    m.m[6] = invDet * (in_M.m[1] * in_M.m[5] - in_M.m[2] * in_M.m[4]);
    m.m[7] = invDet * (in_M.m[2] * in_M.m[3] - in_M.m[0] * in_M.m[5]);
    m.m[8] = invDet * (in_M.m[0] * in_M.m[4] - in_M.m[1] * in_M.m[3]);
    return m;
}

static inline mat4 mat4InverseTranspose(const mat4 in_M);

static inline mat2 mat2Inverse(const mat2 in_M) {
    mat2 m;
    float invDet = 1.0f / (in_M.m[0] * in_M.m[3] - in_M.m[1] * in_M.m[2]);
    m.m[0] = invDet * in_M.m[3];
    m.m[1] = invDet * -in_M.m[1];
    m.m[2] = invDet * -in_M.m[2];
    m.m[3] = invDet * in_M.m[1];
    return m;
}

static inline mat3 mat3Inverse(const mat3 in_M) {
    mat3 m;
    float invDet = 1.0f / (in_M.m[0] * in_M.m[4] * in_M.m[8]
                           + in_M.m[3] * in_M.m[7] * in_M.m[2]
                           + in_M.m[6] * in_M.m[1] * in_M.m[5]
                           - in_M.m[2] * in_M.m[4] * in_M.m[6]
                           - in_M.m[5] * in_M.m[7] * in_M.m[0]
                           - in_M.m[8] * in_M.m[1] * in_M.m[3]);
    m.m[0] = invDet * (in_M.m[4] * in_M.m[8] - in_M.m[5] * in_M.m[7]);
    m.m[1] = invDet * (in_M.m[7] * in_M.m[2] - in_M.m[8] * in_M.m[1]);
    m.m[2] = invDet * (in_M.m[1] * in_M.m[5] - in_M.m[2] * in_M.m[4]);
    m.m[3] = invDet * (in_M.m[5] * in_M.m[6] - in_M.m[3] * in_M.m[8]);
    m.m[4] = invDet * (in_M.m[0] * in_M.m[8] - in_M.m[2] * in_M.m[6]);
    m.m[5] = invDet * (in_M.m[2] * in_M.m[3] - in_M.m[0] * in_M.m[5]);
    m.m[6] = invDet * (in_M.m[3] * in_M.m[7] - in_M.m[4] * in_M.m[6]);
    m.m[7] = invDet * (in_M.m[6] * in_M.m[1] - in_M.m[7] * in_M.m[0]);
    m.m[8] = invDet * (in_M.m[0] * in_M.m[4] - in_M.m[1] * in_M.m[3]);
    return m;
}

static inline mat4 mat4Inverse(const mat4 in_M);


#endif
