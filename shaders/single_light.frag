
#version 330 core

in vec4 fs_color;
in vec3 fs_normal;

out vec4 fragColor;

// @TODO:
// view normal
// view position
// frag position
// Phong Shading
// 

void main(void){
    vec3 ambientColor = (fs_color * 0.2).rgb;
        
    vec3 lightDir = normalize(vec3(0, -1, 1));
    vec3 lightColor = vec3(0.7f, 0.4f, 0.2f);

    //lightColor = vec3(1);
    
    float d = max(dot(lightDir, fs_normal), 0);
    
    vec3 diffuse = lightColor * d;

    fragColor.rgb = diffuse + ambientColor;
    //fragColor = diffuse + ambientColor;

    fragColor.a = 1;

    vec3 normalColor = (vec3(1) + fs_normal) * 0.5;
    fragColor.rgb = normalColor;
}

