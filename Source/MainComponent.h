

#pragma once

#include <JuceHeader.h>

class MainComponent : public juce::OpenGLAppComponent
{
public:
    MainComponent();
    ~MainComponent() override;

    void setCubeSize(float newSize);
    void setCubeColor(juce::Colour newColor);
    void setDampValue(float newDamp);
    void setWidthValue(float newWidth);
    void setGreenCubeAlpha(float newAlpha); // Setter for green cube alpha

    void initialise() override;
    void shutdown() override;
    void render() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void mouseMove(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

    bool openglDisabled = false;


private:
    
    float rotationX = 0.0f;
    float rotationY = 0.0f;
    float lastMouseX = 0.0f;
    float lastMouseY = 0.0f;
    bool isDragging = false;

    float cubeSize = 1.0f;                          // default cube size
    juce::Colour cubeColor = juce::Colours::red;    // def cube color
    float dampValue = 0.0f;                         // DAMP parameter (0.0 to 1.0)
    float widthValue = 0.0f;                        // WIDTH parameter (0.0 to 1.0)
    float greenCubeAlpha = 0.5f;                    // alpha value for green cube  (0.0 to 1.0)

    // shader programs
    std::unique_ptr<juce::OpenGLShaderProgram> cubeShaderProgram;        // rendering the cube
    std::unique_ptr<juce::OpenGLShaderProgram> motionBlurShaderProgram;  //  applying motion blur
    std::unique_ptr<juce::OpenGLShaderProgram> blurShaderProgram;        //  applying basic blur

    // cube shader uniforms and attributes
    std::unique_ptr<juce::OpenGLShaderProgram::Attribute> cubePositionAttribute;
    std::unique_ptr<juce::OpenGLShaderProgram::Uniform> cubeProjectionMatrixUniform;
    std::unique_ptr<juce::OpenGLShaderProgram::Uniform> cubeModelViewMatrixUniform;
    std::unique_ptr<juce::OpenGLShaderProgram::Uniform> cubePrevModelViewMatrixUniform;
    std::unique_ptr<juce::OpenGLShaderProgram::Uniform> cubeColorUniform;
    std::unique_ptr<juce::OpenGLShaderProgram::Uniform> cubeAlphaUniform; // alpha uniform for transparency

    // motion blur shader uniforms and attributes
    std::unique_ptr<juce::OpenGLShaderProgram::Attribute> quadPositionAttribute;
    std::unique_ptr<juce::OpenGLShaderProgram::Uniform> motionBlurColorTextureUniform;
    std::unique_ptr<juce::OpenGLShaderProgram::Uniform> motionBlurMotionTextureUniform;

    // blur shader uniforms and attributes
    std::unique_ptr<juce::OpenGLShaderProgram::Uniform> blurTextureUniform;
    std::unique_ptr<juce::OpenGLShaderProgram::Uniform> blurDampUniform;
    std::unique_ptr<juce::OpenGLShaderProgram::Uniform> blurDirectionUniform;

    GLuint cubeVAO = 0;
    GLuint cubeVBO = 0;
    GLuint cubeEBO = 0;
    GLuint numIndices = 0;

    // framebuffers and textures
    GLuint sceneFBO = 0;
    GLuint colorTexture = 0;
    GLuint motionTexture = 0;
    GLuint blurFBO = 0;
    GLuint blurTexture = 0;

    // quad for rendering
    GLuint quadVAO = 0;
    GLuint quadVBO = 0;

    // transformation matrices
    juce::Matrix3D<float> modelViewMatrix;                // current model-view matrix
    juce::Matrix3D<float> prevModelViewMatrix;            // previous model-view matrix
    juce::Matrix3D<float> redCubeModelViewMatrix;         //model-view matrix for red cube
    juce::Matrix3D<float> greenCubeModelViewMatrix;       //model-view matrix for green cube
    juce::Matrix3D<float> prevRedCubeModelViewMatrix;     // previous model-view matrix for red cube
    juce::Matrix3D<float> prevGreenCubeModelViewMatrix;   //previous model-view matrix for green cube

    juce::Colour interpolateColor(const juce::Colour& startColor,
        const juce::Colour& endColor,
        float ratio);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
