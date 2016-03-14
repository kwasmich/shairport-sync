//
//  Shader.fsh
//  OpenGLES2Reference
//
//  Created by Michael Kwasnicki on 04.08.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

varying lowp vec2 v_texCoord;

uniform sampler2D u_texture;

void main()
{
    gl_FragColor = texture2D( u_texture, v_texCoord );
}
