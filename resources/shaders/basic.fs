#version 330

in vec4 color;
in vec3 worldpos;
in vec3 worldnormal;

uniform vec3 cam_pos;
uniform vec2 viewport_size;

out vec4 out_color;

void main()
{
    vec3 col = vec3(1.0,0.9,0.7);
    
    
    vec2 p = (2.0*gl_FragCoord.xy-viewport_size.xy)/viewport_size.y;

    //vec3 col = render( p );
    {
        vec3 rd = -normalize(cam_pos - worldpos);
        vec3 pos = worldpos;
        vec3 nor = normalize(worldnormal);
        vec3 ref = reflect( rd, nor );
        float fre= clamp( 1.0 + dot(nor,rd), 0.0, 1.0 );
        	
        float occ = 0.45;
        	
        col = vec3(0.9,0.02,0.01);
        col = col*0.72 + 0.2*fre*vec3(1.0,0.8,0.2);
        float max_n = max(nor.y,nor.y);
        max_n = max(nor.y,max_n);
        vec3 lin  = 4.0*vec3(0.7,0.80,1.00)*(0.5+0.5*max_n)*pow(occ,0.5);
                lin += 0.8*fre*vec3(1.0,1.0,1.00)*(0.6+0.4*occ);
        col = col * lin;
        col += 2.5*vec3(0.8,0.9,1.00)*smoothstep(0.0,0.4,ref.y)*(0.06+0.94*pow(fre,5.0))*occ;
    		
    	col = pow(col,vec3(0.4545));
    }
    col = clamp(col,0.0,1.0);
        
    vec2 q = gl_FragCoord.xy/viewport_size.xy;
    col *= 0.2 + 0.8*pow(16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y),0.2);
    out_color = vec4(col,1.0);
}