//
//  OpenGLES2Core.c
//  GLESCompute
//
//  Created by Michael Kwasnicki on 06.12.13.
//
//


#include "OpenGLES2Core.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


#define free_s( X ) free( X ); X = NULL


static void detectRAWTextureSize(const int in_SIZE, GLsizei *out_dim, GLenum *out_format, GLint *out_mipMaps);
static void detectETC1TextureSize(const int in_SIZE, GLsizei *out_dim, GLint *out_mipMaps);


#pragma mark - Shader



Shader_t createProgramObject(const GLuint in_VERTEX_SHADER, const GLuint in_FRAGMENT_SHADER) {
    GLint i;
    Shader_t shader;
    GLuint programObject = glCreateProgram();
    assert(programObject != 0);

    glAttachShader(programObject, in_VERTEX_SHADER);
    glAttachShader(programObject, in_FRAGMENT_SHADER);
    glLinkProgram(programObject);

    GLint linkStatus = 0;
    glGetProgramiv(programObject, GL_LINK_STATUS, &linkStatus);

    // check the link status
    GLsizei infoLogLength = 0;
    glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLogLength);
    GLchar *infoLog = malloc(sizeof(GLchar) * infoLogLength);
    glGetProgramInfoLog(programObject, infoLogLength, &infoLogLength, infoLog);
    puts(infoLog);
    free_s(infoLog);

    assert(linkStatus == GL_TRUE);

    // validate shader program
    glValidateProgram(programObject);
    GLint validateStatus = 0;
    glGetProgramiv(programObject, GL_VALIDATE_STATUS, &validateStatus);

    infoLogLength = 0;
    glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLogLength);
    infoLog = malloc(sizeof(GLchar) * infoLogLength);
    glGetProgramInfoLog(programObject, infoLogLength, &infoLogLength, infoLog);
    puts(infoLog);
    free_s(infoLog);

    assert(validateStatus == GL_TRUE);

    // check the uniforms
    GLint activeUniforms = 0;
    GLint activeUniformsMaxLength = 0;
    glGetProgramiv(programObject, GL_ACTIVE_UNIFORMS, &activeUniforms);
    glGetProgramiv(programObject, GL_ACTIVE_UNIFORM_MAX_LENGTH, &activeUniformsMaxLength);
    GLint activeUniformSize = 0;
    GLenum activeUniformType = 0;
    GLchar *activeUniform = malloc(sizeof(GLchar) * activeUniformsMaxLength);

    for (i = 0; i < activeUniforms; i++) {
        glGetActiveUniform(programObject, i, activeUniformsMaxLength, NULL, &activeUniformSize, &activeUniformType, activeUniform);
        printf("%-20s %-15s %i\n", activeUniform, glEnumToCString(activeUniformType), activeUniformSize);
    }

    free_s(activeUniform);

    // check the attributes
    GLint activeAttributes = 0;
    GLint activeAttributesMaxLength = 0;
    glGetProgramiv(programObject, GL_ACTIVE_ATTRIBUTES, &activeAttributes);
    glGetProgramiv(programObject, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &activeAttributesMaxLength);
    GLint activeAttributeSize = 0;
    GLenum activeAttributeType = 0;
    GLchar *activeAttributeName = malloc(sizeof(GLchar) * activeAttributesMaxLength);

    for (i = 0; i < activeAttributes; i++) {
        glGetActiveAttrib(programObject, i, activeAttributesMaxLength, NULL, &activeAttributeSize, &activeAttributeType, activeAttributeName);
        printf("%-20s %-15s %i\n", activeAttributeName, glEnumToCString(activeAttributeType), activeAttributeSize);
    }

    free_s(activeAttributeName);

    //    GLint activeUniformBlocks = 0;
    //    glGetProgramiv( programObject, GL_ACTIVE_UNIFORM_BLOCKS, &activeUniformBlocks );
    //    printf( "GL_ACTIVE_UNIFORM_BLOCKS = %i\n", activeUniformBlocks );

    shader.programObject = programObject;

    // get uniform locations
    shader.uniformLocations[UNIFORM_MODEL_MATRIX] = glGetUniformLocation(programObject, "u_modelMatrix");
    shader.uniformLocations[UNIFORM_VIEW_MATRIX] = glGetUniformLocation(programObject, "u_viewMatrix");
    shader.uniformLocations[UNIFORM_PROJECTION_MATRIX] = glGetUniformLocation(programObject, "u_projectionMatrix");
    shader.uniformLocations[UNIFORM_NORMAL_MATRIX] = glGetUniformLocation(programObject, "u_normalMatrix");
    shader.uniformLocations[UNIFORM_EC_LIGHT_POSITION] = glGetUniformLocation(programObject, "u_ecLightPos");
    shader.uniformLocations[UNIFORM_COLOR] = glGetUniformLocation(programObject, "u_color");
    shader.uniformLocations[UNIFORM_TEXTURE] = glGetUniformLocation(programObject, "u_texture");
    shader.uniformLocations[UNIFORM_TEXTURE_SIZE] = glGetUniformLocation(programObject, "u_textureSize");

    // get attribute locations
    shader.attribLocations[ATTRIB_POSITION] = glGetAttribLocation(programObject, "a_position");
    shader.attribLocations[ATTRIB_NORMAL] = glGetAttribLocation(programObject, "a_normal");
    shader.attribLocations[ATTRIB_TANGENT] = glGetAttribLocation(programObject, "a_tangent");
    shader.attribLocations[ATTRIB_BI_TANGENT] = glGetAttribLocation(programObject, "a_biTangent");
    shader.attribLocations[ATTRIB_COLOR] = glGetAttribLocation(programObject, "a_color");
    shader.attribLocations[ATTRIB_TEX_COORD] = glGetAttribLocation(programObject, "a_texCoord");

    return shader;
}



GLuint createShaderObject(const char *in_FILE_NAME, const GLenum in_SHADER_TYPE) {
    GLchar *shaderSource = createStringFromFileContents(in_FILE_NAME);
    assert(shaderSource != NULL);

    GLuint shaderObject = glCreateShader(in_SHADER_TYPE);
    assert(shaderObject != 0);

    glShaderSource(shaderObject, 1, (const GLchar **)&shaderSource, NULL);
    glCompileShader(shaderObject);

    GLint compileStatus = 0;
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &compileStatus);

    // check the compile status
    GLsizei infoLogLength = 0;
    glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
    GLchar *infoLog = malloc(sizeof(GLchar) * infoLogLength);
    glGetShaderInfoLog(shaderObject, infoLogLength, &infoLogLength, infoLog);
    puts(infoLog);
    free_s(infoLog);

    assert(compileStatus == GL_TRUE);

    free_s(shaderSource);
    return shaderObject;
}



#pragma mark - Texture



GLuint createTextureObject(const char *in_FILE_NAME, const GLenum in_INTERNAL_FORMAT) {
    int rawSize = 0;
    unsigned char *rawImageData = rawFromFileContents(in_FILE_NAME, false, &rawSize);
    GLuint textureObject = 0;
    glGenTextures(1, &textureObject);

    glBindTexture(GL_TEXTURE_2D, textureObject);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLsizei dim = 0;
    GLenum format = GL_RGB;
    GLint mipMaps = 0;

    if (in_INTERNAL_FORMAT == GL_ETC1_RGB8_OES) {
        detectETC1TextureSize(rawSize, &dim, &mipMaps);

        if (mipMaps) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            unsigned char *subImageData = rawImageData;
            int subSize = 0;
            int level;

            for (level = 0; level <= mipMaps; level++) {
                subSize = 8 * ((dim + 3) >> 2) * ((dim + 3) >> 2);         // mips smaller than 4x4 still take 8 bytes
                glCompressedTexImage2D(GL_TEXTURE_2D, level, GL_ETC1_RGB8_OES, dim, dim, 0, subSize, subImageData);
                subImageData += subSize;
                dim >>= 1;
            }
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_ETC1_RGB8_OES, dim, dim, 0, rawSize, rawImageData);
        }
    } else if (in_INTERNAL_FORMAT == GL_RGB) {
        detectRAWTextureSize(rawSize, &dim, &format, &mipMaps);

        if (mipMaps) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            assert(false);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, format, dim, dim, 0, format, GL_UNSIGNED_BYTE, rawImageData);
        }
    } else if (in_INTERNAL_FORMAT == GL_LUMINANCE) {
        detectRAWTextureSize(rawSize, &dim, &format, &mipMaps);
        dim *= 2;
        format = GL_LUMINANCE;

        if (mipMaps) {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            assert(false);
        } else {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexImage2D(GL_TEXTURE_2D, 0, format, dim, dim, 0, format, GL_UNSIGNED_BYTE, rawImageData);
        }
    }

    free_s(rawImageData);
    return textureObject;
}


GLuint createTextureObjectRAW(const uint8_t *const in_IMAGE, const uint32_t in_WIDTH, const uint32_t in_HEIGHT, const uint32_t in_NUM_COMPONENTS) {
    GLuint textureObject = 0;
    GLenum format = GL_RGB;
    
    switch (in_NUM_COMPONENTS) {
        case 4:
            format = GL_RGBA;
            break;
            
        case 3:
            format = GL_RGB;
            break;
            
        case 1:
            format = GL_LUMINANCE;
            break;
            
        default:
            assert(false);
            return textureObject;
    }
    glGenTextures(1, &textureObject);
    glBindTexture(GL_TEXTURE_2D, textureObject);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, format, in_WIDTH, in_HEIGHT, 0, format, GL_UNSIGNED_BYTE, in_IMAGE);
    
    return textureObject;
}



#pragma mark - Helpers



void detectRAWTextureSize(const int in_SIZE, GLsizei *out_dim, GLenum *out_format, GLint *out_mipMaps) {
    static const int squares[] = { 1, 4, 16, 64, 256, 1024, 4096, 16384, 65536, 262144, 1048576, 4194304, 16777216, 67108864, 268435456, 1073741824 };
    static const int squaresMIP[] = { 1, 5, 21, 85, 341, 1365, 5461, 21845, 87381, 349525, 1398101, 5592405, 22369621, 89478485, 357913941, 1431655765 };
    const int div2 = in_SIZE >> 1;
    const int div3 = in_SIZE / 3;
    const int div4 = in_SIZE >> 2;
    unsigned int i = 1;
    GLenum format = GL_LUMINANCE;
    GLsizei dim = 0;
    GLint mipMaps = 0;

    for (i = 0; i < 16; i++) {
        if (squaresMIP[i] == div2) {
            format = GL_LUMINANCE_ALPHA;
            dim = 1 << i;
            mipMaps = i;
        }

        if (squaresMIP[i] == div3) {
            format = GL_RGB;
            dim = 1 << i;
            mipMaps = i;
        }

        if (squaresMIP[i] == div4) {
            format = GL_RGBA;
            dim = 1 << i;
            mipMaps = i;
        }

        if (squares[i] == div2) {
            format = GL_LUMINANCE_ALPHA;
            dim = 1 << i;
            mipMaps = 0;
        }

        if (squares[i] == div3) {
            format = GL_RGB;
            dim = 1 << i;
            mipMaps = 0;
        }

        if (squares[i] == div4) {
            format = GL_RGBA;
            dim = 1 << i;
            mipMaps = 0;
        }
    }

    if (dim == 0) {
        fprintf(stderr, "ERROR : Texture is not square!\n");
    } else {
        printf("%i x %i @ %x - %i\n", dim, dim, format, mipMaps);
    }

    *out_dim = dim;
    *out_format = format;
    *out_mipMaps = mipMaps;
}



void detectETC1TextureSize(const int in_SIZE, GLsizei *out_dim, GLint *out_mipMaps) {
    static const int squares[] = { 1, 1, 1, 4, 16, 64, 256, 1024, 4096, 16384, 65536, 262144, 1048576, 4194304, 16777216, 67108864 };
    static const int squaresMIP[] = { 1, 2, 3, 7, 23, 87, 343, 1367, 5463, 21847, 87383, 349527, 1398103, 5592407, 22369623, 89478487 };
    const int div8 = in_SIZE >> 3;
    unsigned int i = 1;
    GLsizei dim = 0;
    GLint mipMaps = false;

    for (i = 0; i < 16; i++) {
        if (squaresMIP[i] == div8) {
            dim = 1 << i;
            mipMaps = i;
        }

        if (squares[i] == div8) {
            dim = 1 << i;
            mipMaps = 0;
        }
    }

    if (dim == 0) {
        fprintf(stderr, "ERROR : Texture is not square!\n");
    } else {
        printf("%i x %i - %i\n", dim, dim, mipMaps);
    }

    *out_dim = dim;
    *out_mipMaps = mipMaps;
}



unsigned char *rawFromFileContents(const char *in_FILE_NAME, const bool in_ZERO_TERMINATE, int *out_fileSize) {
    unsigned char *fileContents = NULL;
    FILE *fileStream = fopen(in_FILE_NAME, "rb");

    if (!fileStream) {
        perror(in_FILE_NAME);
        *out_fileSize = 0;
        return fileContents;
    }

    int err = fseek(fileStream, 0L, SEEK_END);

    if (err != 0) {
        perror(in_FILE_NAME);
        *out_fileSize = 0;
        err = fclose(fileStream);

        if (err != 0) {
            perror(in_FILE_NAME);
        }

        fileStream = NULL;
        return fileContents;
    }

    long length = ftell(fileStream);

    if (length == -1) {
        perror(in_FILE_NAME);
        *out_fileSize = 0;
        err = fclose(fileStream);

        if (err != 0) {
            perror(in_FILE_NAME);
        }

        fileStream = NULL;
        return fileContents;
    }

    rewind(fileStream);

    size_t mallocCount = (in_ZERO_TERMINATE) ? length + 1 : length;
    fileContents = malloc(sizeof(unsigned char) * mallocCount);

    if (!fileContents) {
        perror(in_FILE_NAME);
        *out_fileSize = 0;
        err = fclose(fileStream);

        if (err != 0) {
            perror(in_FILE_NAME);
        }

        fileStream = NULL;
        return fileContents;
    }

    long readLength = fread(fileContents, sizeof(unsigned char), length, fileStream);
    assert(readLength == length);

    err = fclose(fileStream);

    if (err != 0) {
        perror(in_FILE_NAME);
    }

    fileStream = NULL;

    if (in_ZERO_TERMINATE) {
        fileContents[length] = 0;
    }

    *out_fileSize = length;
    return fileContents;
}



char *createStringFromFileContents(const char *in_FILE_NAME) {
    int dummy;
    return (char *)rawFromFileContents(in_FILE_NAME, true, &dummy);
}



char *glEnumToCString(const GLenum in_ENUM) {
    char *enumString = NULL;

    switch (in_ENUM) {
        case GL_FLOAT:
            enumString = "GL_FLOAT";
            break;

        case GL_FLOAT_VEC2:
            enumString = "GL_FLOAT_VEC2";
            break;

        case GL_FLOAT_VEC3:
            enumString = "GL_FLOAT_VEC3";
            break;

        case GL_FLOAT_VEC4:
            enumString = "GL_FLOAT_VEC4";
            break;

        case GL_FLOAT_MAT2:
            enumString = "GL_FLOAT_MAT2";
            break;

        case GL_FLOAT_MAT3:
            enumString = "GL_FLOAT_MAT3";
            break;

        case GL_FLOAT_MAT4:
            enumString = "GL_FLOAT_MAT4";
            break;

        case GL_INT:
            enumString = "GL_INT";
            break;

        case GL_INT_VEC2:
            enumString = "GL_INT_VEC2";
            break;

        case GL_INT_VEC3:
            enumString = "GL_INT_VEC3";
            break;

        case GL_INT_VEC4:
            enumString = "GL_INT_VEC4";
            break;

        case GL_UNSIGNED_INT:
            enumString = "GL_UNSIGNED_INT";
            break;

        case GL_BOOL:
            enumString = "GL_BOOL";
            break;

        case GL_BOOL_VEC2:
            enumString = "GL_BOOL_VEC2";
            break;

        case GL_BOOL_VEC3:
            enumString = "GL_BOOL_VEC3";
            break;

        case GL_BOOL_VEC4:
            enumString = "GL_BOOL_VEC4";
            break;

        case GL_SAMPLER_2D:
            enumString = "GL_SAMPLER_2D";
            break;

        case GL_SAMPLER_CUBE:
            enumString = "GL_SAMPLER_CUBE";
            break;

        default:
            enumString = "(UNKNOWN)";
            break;
    }

    return enumString;
}

