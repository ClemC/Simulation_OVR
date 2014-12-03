// Version du GLSL

#version 150 core


// Entree

in vec2 coordTexture;


// Uniform

uniform sampler2D mtexture;
uniform vec4 dimensions;


// Sortie

out vec4 out_Color;


// Fonction main

void main()
{
    // Couleur du pixel

    // light = 2
    // dimensions.x is the mass : 0 for low mass and 1 for high mass
    // dimensions.y is the age : 0 for low age and 1 for high age
    out_Color = 2 * 10 * dimensions.x * texture(mtexture, coordTexture);
    
    // a high age object is more red than a low age object
    out_Color.x += dimensions.y; 	
}
