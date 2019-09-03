#version 410

uniform sampler2DRect inTex;
uniform float intensity;

out vec4 fragColor;


vec4 GetKernelMeanAndVariance(vec2 UV, vec4 Range, mat2 RotationMatrix)
{
    vec2 TexelSize = vec2(intensity/4);
    vec3 Mean = vec3(0);
    vec3 Variance = vec3(00);
    float Samples = 0;
    
    for (int x = Range.x; x <= Range.y; x++)
    {
        for (int y = Range.z; y <= Range.w; y++)
        {
            vec2 Offset = mul(vec2(x, y) * TexelSize, RotationMatrix);
            vec3 PixelColor = texture(inTex, UV + Offset).rgb;
            Mean += PixelColor;
            Variance += PixelColor * PixelColor;
            Samples++;
        }
    }
    
    Mean /= Samples;
    Variance = Variance / Samples - Mean * Mean;
    float TotalVariance = Variance.r + Variance.g + Variance.b;
    return vec4(Mean.r, Mean.g, Mean.b, TotalVariance);
}

float GetPixelAngle(vec2 UV)
{
    vec2 TexelSize = vec2(intensity/4);
    float GradientX = 0;
    float GradientY = 0;
    float SobelX[9] = float[9](-1, -2, -1, 0, 0, 0, 1, 2, 1);
    float SobelY[9] = float[9](-1, 0, 1, -2, 0, 2, -1, 0, 1);
    int i = 0;
    
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            // 1
            vec2 Offset = vec2(x, y) * TexelSize;
            vec3 PixelColor = texture(inTex, UV + Offset).rgb;
            float PixelValue = dot(PixelColor, vec3(0.3,0.59,0.11));
            
            // 2
            GradientX += PixelValue * SobelX[i];
            GradientY += PixelValue * SobelY[i];
            i++;
        }
    }
    
    return atan(GradientY / GradientX);
}

void main(){
    vec2 UV = gl_FragCoord.xy;
    vec4 texColor = texture(inTex, UV);

    vec4 MeanAndVariance = vec4(0.);
    vec4 Range  = vec4(0.);
    float Angle = GetPixelAngle(UV);
    mat2 RotationMatrix = mat2(cos(Angle), -sin(Angle), sin(Angle), cos(Angle));

    Range = vec4(-XRadius, 0, -YRadius, 0);
    MeanAndVariance[0] = GetKernelMeanAndVariance(UV, Range, RotationMatrix);

    Range = vec4(0, XRadius, -YRadius, 0);
    MeanAndVariance[1] = GetKernelMeanAndVariance(UV, Range, RotationMatrix);

    Range = vec4(-XRadius, 0, 0, YRadius);
    MeanAndVariance[2] = GetKernelMeanAndVariance(UV, Range, RotationMatrix);

    Range = vec4(0, XRadius, 0, YRadius);
    MeanAndVariance[3] = GetKernelMeanAndVariance(UV, Range, RotationMatrix);

    // 1
    vec3 FinalColor = MeanAndVariance[0].rgb;
    float MinimumVariance = MeanAndVariance[0].a;

    // 2
    for (int i = 1; i < 4; i++)
    {
        if (MeanAndVariance[i].a < MinimumVariance)
        {
            FinalColor = MeanAndVariance[i].rgb;
            MinimumVariance = MeanAndVariance[i].a;
        }
    }

    return FinalColor;
}