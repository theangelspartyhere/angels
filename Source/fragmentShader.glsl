#version 330 core
out vec4 FragColor;
uniform float blurAmount;

void main()
{
    vec4 color = vec4(1.0); // Default to white background
    
    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <= 2; y++) {
            color += texture(offset + vec2(x, y));
        }
    }

    FragColor = color / 25.0;
}
