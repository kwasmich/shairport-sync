//
//  Shader.vsh
//  OpenGLES2Reference
//
//  Created by Michael Kwasnicki on 04.08.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

attribute vec4 a_position;
attribute vec2 a_texCoord;

varying lowp vec2 v_texCoord;

uniform float u_textureSize;
uniform mat4 u_projectionMatrix;

void main()
{
    v_texCoord = a_texCoord / u_textureSize;
    gl_Position = u_projectionMatrix * a_position;
}
