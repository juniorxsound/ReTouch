#version 150 core

//gl_FragColor
out vec4 color_out;

//Texture
uniform sampler2D topBottomMap;

//States
uniform int colorDepthToogle;
uniform int isDOF;
uniform float iDOF;
uniform vec2 ScreenSize;
uniform vec2 texSize;
uniform int depthOrder;
uniform int isColorRetouching;

uniform vec3 frontColor;
uniform vec3 backColor;

uniform float backgroundIntensity;
uniform float foregroundIntensity;

//Varying from vertex
in vec3 vPos;
in vec2 vUv;

//UV displacements
vec2 topUV;
vec2 bottomUV;

void main(){

  //Take the top 0.5
  topUV = vUv;
  topUV.y *= 0.5;

  //Take the bottom 0.5
  bottomUV = vUv;
  bottomUV.y *= 0.5;
  bottomUV.y += 0.5;

  //Read the color texture
  vec4 colorTex = texture(topBottomMap, topUV);

  //Read the depth map
  vec4 depthTex = texture(topBottomMap, bottomUV);

  vec4 blurMixer = vec4(0.0);

  if(isDOF == 1){
    //Single pass blur here we go!
    vec4 blurryColor = vec4(0.0);

    //UV Displacements
    vec2 off1 = vec2(1.411764705882353) * vec2(0.5, 1.0);
    vec2 off2 = vec2(3.2941176470588234) * vec2(0.5, 1.0);
    vec2 off3 = vec2(5.176470588235294) * vec2(0.5, 1.0);

    blurryColor += texture(topBottomMap, topUV) * 0.1964825501511404;
    blurryColor += texture(topBottomMap, topUV + (off1 / ScreenSize)) * 0.2969069646728344;
    blurryColor += texture(topBottomMap, topUV - (off1 / ScreenSize)) * 0.2969069646728344;
    blurryColor += texture(topBottomMap, topUV + (off2 / ScreenSize)) * 0.09447039785044732;
    blurryColor += texture(topBottomMap, topUV - (off2 / ScreenSize)) * 0.09447039785044732;
    blurryColor += texture(topBottomMap, topUV + (off3 / ScreenSize)) * 0.010381362401148057;
    blurryColor += texture(topBottomMap, topUV - (off3 / ScreenSize)) * 0.010381362401148057;

    //Depth order based on a pixel sample from the depth map
    float order = depthTex.r;

    //Pick one of the two "modes"
    if(depthOrder == 1){
      order = depthTex.r * -1.0 + 1.0;
    }



    blurMixer = mix(colorTex, blurryColor, order * iDOF);

  } else {
    blurMixer = colorTex;
  }

  //Lighting modifications
  vec4 backLight = (vec4(backColor, 1.0) * (depthTex.r * -1.0 + 1.0)) * vec4(backgroundIntensity);
  vec4 frontLight = (vec4(frontColor, 1.0) * depthTex.r) * vec4(foregroundIntensity);

  blurMixer += backLight * isColorRetouching;
  blurMixer += frontLight * isColorRetouching;

  if(colorDepthToogle == 1) color_out = depthTex;
  else color_out = blurMixer;
}
