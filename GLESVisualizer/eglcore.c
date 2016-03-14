//
//  eglcore.c
//  GLESCompute
//
//  Created by Michael Kwasnicki on 01.12.13.
//
//

#include "eglcore.h"

#include "glcore.h"

#include <assert.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <stdio.h>

//#define printf(x...)
//#define puts(x...)

static void eglCheckError(void);


static EGLDisplay s_display;
static EGLSurface s_surface;
static EGLContext s_context;



// config you use OpenGL ES2.0 by default
static const EGLint s_contextAttributes[] = {
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
};


static const EGLint s_configAttribs[] = {
    EGL_CONFIG_ID,		7,
    EGL_NONE
};


//static const char* stringFromConst( const EGLint in_I ) {
//	switch ( in_I ) {
//		case EGL_NONE:
//			return "EGL_NONE";
//
//		default:
//			return "UNKNOWN";
//	}
//}


static void printConfig(const EGLConfig in_CONFIG) {
    EGLint attributes[32] = {
        EGL_CONFIG_ID,
        EGL_RED_SIZE,
        EGL_GREEN_SIZE,
        EGL_BLUE_SIZE,
        EGL_ALPHA_SIZE,
        EGL_BUFFER_SIZE,
        EGL_DEPTH_SIZE,
        EGL_STENCIL_SIZE,
        EGL_SAMPLES,
        EGL_SAMPLE_BUFFERS,
        EGL_BIND_TO_TEXTURE_RGB,
        EGL_BIND_TO_TEXTURE_RGBA,
        EGL_SURFACE_TYPE,
        EGL_CONFIG_CAVEAT,
        EGL_CONFORMANT,
        EGL_LEVEL,
        EGL_MAX_PBUFFER_HEIGHT,
        EGL_MAX_PBUFFER_WIDTH,
        EGL_MAX_PBUFFER_PIXELS,
        EGL_NATIVE_RENDERABLE,
        EGL_NATIVE_VISUAL_ID,
        EGL_NATIVE_VISUAL_TYPE,
        EGL_TRANSPARENT_TYPE,
        EGL_TRANSPARENT_BLUE_VALUE,
        EGL_TRANSPARENT_GREEN_VALUE,
        EGL_TRANSPARENT_RED_VALUE,
        EGL_LUMINANCE_SIZE,
        EGL_ALPHA_MASK_SIZE,
        EGL_COLOR_BUFFER_TYPE,
        EGL_RENDERABLE_TYPE,
        EGL_MIN_SWAP_INTERVAL,
        EGL_MAX_SWAP_INTERVAL,
    };

    EGLBoolean result;
    int i;

    for (i = 0; i < 32; i++) {
        result = eglGetConfigAttrib(s_display, in_CONFIG, attributes[i], &attributes[i]);
        assert(result != EGL_FALSE);
        eglCheckError();

        //printf( "%d	", attributes[i] );
    }

    //puts( "" );

    printf("%2i: RGBA%i%i%i%i (%i), Depth %2i, Stencil %i, Samples %i (Buffers %i), BindToTexture RGB:%i RGBA:%i\n", attributes[0], attributes[1], attributes[2], attributes[3], attributes[4],
           attributes[5], attributes[6], attributes[7], attributes[8], attributes[9], attributes[10], attributes[11]);


}


void initEGL(const uint32_t in_DISPLAY_NUMBER, const uint32_t in_X, const uint32_t in_Y, const uint32_t in_W, const uint32_t in_H) {
    // this code does the main window creation
    EGLBoolean result;
    EGLConfig config;
    EGLint numConfig;

    static EGL_DISPMANX_WINDOW_T nativeWindow;
    // our source and destination rect for the screen
    VC_RECT_T dstRect;
    VC_RECT_T srcRect;

    // get an EGL display connection
    s_display = eglGetDisplay((EGLNativeDisplayType)in_DISPLAY_NUMBER);   // EGL_DEFAULT_DISPLAY
    eglCheckError();
    assert(s_display != EGL_NO_DISPLAY);

    // initialize the EGL display connection
    int major, minor;
    result = eglInitialize(s_display, &major, &minor);
    eglCheckError();
    assert(result != EGL_FALSE);
    printf("EGL init version %i.%i\n", major, minor);

    puts(eglQueryString(s_display, EGL_CLIENT_APIS));
    eglCheckError();
    puts(eglQueryString(s_display, EGL_EXTENSIONS));
    eglCheckError();
    puts(eglQueryString(s_display, EGL_VENDOR));
    eglCheckError();
    puts(eglQueryString(s_display, EGL_VERSION));
    eglCheckError();


    EGLint configsSize = 1024;
    EGLConfig configs[1024];
    result = eglGetConfigs(s_display, configs, configsSize, &numConfig);
    assert(result != EGL_FALSE);
    eglCheckError();

    int i = 0;

    for (i = 0; i < numConfig; i++) {
        printConfig(configs[i]);
    }

    printf("%d configs\n", numConfig);

    result = eglChooseConfig(s_display, s_configAttribs, &config, 1, &numConfig);
    eglCheckError();
    assert(result != EGL_FALSE);


    // create an EGL window surface the way this works is we set the dimensions of the srec
    // and destination rectangles.
    // if these are the same size there is no scaling, else the window will auto scale

    dstRect.x = in_X;
    dstRect.y = in_Y;
    dstRect.width = in_W;
    dstRect.height = in_H;

    srcRect.x = 0;
    srcRect.y = 0;
    srcRect.width = in_W << 16;
    srcRect.height = in_H << 16;

    DISPMANX_ELEMENT_HANDLE_T dispmanElement;
    DISPMANX_DISPLAY_HANDLE_T dispmanDisplay;
    DISPMANX_UPDATE_HANDLE_T dispmanUpdate;

    dispmanDisplay = vc_dispmanx_display_open(in_DISPLAY_NUMBER);
    dispmanUpdate = vc_dispmanx_update_start(0);
    dispmanElement = vc_dispmanx_element_add(dispmanUpdate, dispmanDisplay, 0, &dstRect, 0, &srcRect, DISPMANX_PROTECTION_NONE, 0, 0, DISPMANX_NO_ROTATE);
    // now we have created this element we pass it to the native window structure ready
    // no create our new EGL surface
    nativeWindow.element = dispmanElement;
    nativeWindow.width = in_W;
    nativeWindow.height = in_H;
    // we now tell the vc we have finished our update
    vc_dispmanx_update_submit_sync(dispmanUpdate);

    // finally we can create a new surface using this config and window
    s_surface = eglCreateWindowSurface(s_display, config, &nativeWindow, NULL);
    //s_surface = eglCreatePbufferSurface( s_display, config, ATTRIBS );
    //s_surface = eglCreatePixmapSurface( s_display, config, EGLNativePixmapType pixmap, ATTRIBS );
    eglCheckError();
    assert(s_surface != EGL_NO_SURFACE);


    EGLint height, width;
    result = eglQuerySurface(s_display, s_surface, EGL_HEIGHT, &height);
    assert(result != EGL_FALSE);
    result = eglQuerySurface(s_display, s_surface, EGL_WIDTH, &width);
    assert(result != EGL_FALSE);

    printf("%i %i\n", width, height);


    // bind the OpenGL API to the EGL
    result = eglBindAPI(EGL_OPENGL_ES_API);
    eglCheckError();
    assert(result != EGL_FALSE);

    // create an EGL rendering context
    s_context = eglCreateContext(s_display, config, EGL_NO_CONTEXT, s_contextAttributes);
    eglCheckError();
    assert(s_context != EGL_NO_CONTEXT);

    //////////////eglQueryContext( s_display, s_context, EGLint attribute, EGLint *value)




    // connect the context to the surface
    result = eglMakeCurrent(s_display, s_surface, s_surface, s_context);
    eglCheckError();
    assert(EGL_FALSE != result);

    eglSwapInterval(s_display, 1);   // only draw every n-th frame with n=1 : 50/60 FPS (depending on display) 0 = unsynchronized
    eglCheckError();

    initGL(in_W, in_H);
}



void drawEGL() {
    EGLBoolean result;
    result = eglMakeCurrent(s_display, s_surface, s_surface, s_context);
    eglCheckError();
    assert(EGL_FALSE != result);

    drawGL();

    // to update we need to swap the buffers
    eglSwapBuffers(s_display, s_surface);
    eglCheckError();
}



void destroyEGL() {
    puts("destroyEGL");
    EGLBoolean result;

    result = eglMakeCurrent(s_display, s_surface, s_surface, s_context);
    eglCheckError();
    assert(EGL_FALSE != result);
    destroyGL();
    eglSwapBuffers(s_display, s_surface);
    eglCheckError();
    eglMakeCurrent(s_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglCheckError();
    eglDestroyContext(s_display, s_context);
    eglCheckError();
    eglDestroySurface(s_display, s_surface);
    eglCheckError();
    result = eglTerminate(s_display);
    eglCheckError();
    assert(result != EGL_FALSE);
    //eglMakeCurrent( s_display, s_surface, s_surface, s_context);
    eglReleaseThread();
}


void eglCheckError() {
    EGLint error = eglGetError();

    switch (error) {
        case EGL_SUCCESS:
            //puts( "EGL_SUCCESS" ); // don't print that
            break;

        case EGL_NOT_INITIALIZED:
            puts("EGL_NOT_INITIALIZED");
            break;

        case EGL_BAD_ACCESS:
            puts("EGL_BAD_ACCESS");
            break;

        case EGL_BAD_ALLOC:
            puts("EGL_BAD_ALLOC");
            break;

        case EGL_BAD_ATTRIBUTE:
            puts("EGL_BAD_ATTRIBUTE");
            break;

        case EGL_BAD_CONFIG:
            puts("EGL_BAD_CONFIG");
            break;

        case EGL_BAD_CONTEXT:
            puts("EGL_BAD_CONTEXT");
            break;

        case EGL_BAD_CURRENT_SURFACE:
            puts("EGL_BAD_CURRENT_SURFACE");
            break;

        case EGL_BAD_DISPLAY:
            puts("EGL_BAD_DISPLAY");
            break;

        case EGL_BAD_MATCH:
            puts("EGL_BAD_MATCH");
            break;

        case EGL_BAD_NATIVE_PIXMAP:
            puts("EGL_BAD_NATIVE_PIXMAP");
            break;

        case EGL_BAD_NATIVE_WINDOW:
            puts("EGL_BAD_NATIVE_WINDOW");
            break;

        case EGL_BAD_PARAMETER:
            puts("EGL_BAD_PARAMETER");
            break;

        case EGL_BAD_SURFACE:
            puts("EGL_BAD_SURFACE");
            break;

        case EGL_CONTEXT_LOST:
            puts("EGL_CONTEXT_LOST");
            break;

        default:
            puts("UNKNOWN EGL ERROR");
            break;
    }
}
