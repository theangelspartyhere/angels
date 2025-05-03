#include "MainComponent.h"
#include "EditorContent.h"
#include "PluginEditor.h"
#include "PluginProcessor.h"



using namespace juce::gl;

//==============================================================================
MainComponent::MainComponent()
    : cubeSize(1.0f),
    cubeColor(juce::Colours::red),
    dampValue(0.0f),
    widthValue(0.0f),
    greenCubeAlpha(0.5f) // Default alpha value for the green cube
{
    setSize(350, 600);
    setOpaque(false);  // Ensure the background is transparent
    setVisible(true);


    // Add mouse listener for rotation
    addMouseListener(this, true);
    setMouseClickGrabsKeyboardFocus(false);
    setMouseCursor(juce::MouseCursor());

    //Start the OpenGL context
    openGLContext.setContinuousRepainting(true);
    openGLContext.attachTo(*this);
     /*openGLContext.setContinuousRepainting(false); 
     openGLContext.detach(); */


}

MainComponent::~MainComponent()
{
    openGLContext.detach();
    shutdownOpenGL();

    /*openGLContext.setRenderer(nullptr); 
    cubeShaderProgram.reset();
    motionBlurShaderProgram.reset();
    blurShaderProgram.reset();*/

}

void MainComponent::setCubeSize(float newSize)
{
    cubeSize = newSize;
}

void MainComponent::setCubeColor(juce::Colour newColor)
{
    cubeColor = newColor;
}

void MainComponent::setDampValue(float newDamp)
{
    dampValue = juce::jlimit(0.0f, 1.0f, newDamp);
}

void MainComponent::setWidthValue(float newWidth)
{
    widthValue = juce::jlimit(0.0f, 1.0f, newWidth);  // Ensure widthValue is between 0 and 1
}

void MainComponent::setGreenCubeAlpha(float newAlpha)
{
    greenCubeAlpha = juce::jlimit(0.0f, 1.0f, newAlpha); // Ensure alpha is between 0 and 1
}

void MainComponent::initialise()
{
    // Enable depth testing and set clear color
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);  // Accept fragments closer to the camera
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Clear background to black

    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // === Define Shaders ===

    // Cube Vertex Shader
    const char* cubeVertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec3 aPosition;

    uniform mat4 uProjectionMatrix;
    uniform mat4 uModelViewMatrix;
    uniform mat4 uPrevModelViewMatrix;

    out vec4 vCurrentPos;
    out vec4 vPrevPos;

    void main()
    {
        vCurrentPos = uProjectionMatrix * uModelViewMatrix * vec4(aPosition, 1.0);
        vPrevPos = uProjectionMatrix * uPrevModelViewMatrix * vec4(aPosition, 1.0);
        gl_Position = vCurrentPos;
    }
    )";

    // Cube Fragment Shader
    const char* cubeFragmentShaderSource = R"(
    #version 330 core
    in vec4 vCurrentPos;
    in vec4 vPrevPos;

    out vec4 FragColor;
    layout(location = 1) out vec2 MotionVector;

    uniform vec3 uColor;
    uniform float uAlpha; // Alpha value for transparency

    void main()
    {
        // Compute normalized device coordinates
        vec2 currentPos = vCurrentPos.xy / vCurrentPos.w;
        vec2 prevPos = vPrevPos.xy / vPrevPos.w;

        // Store motion vector
        MotionVector = (currentPos - prevPos) * 0.5; // Scale to enhance visibility

        FragColor = vec4(uColor, uAlpha);
    }
    )";

    // Create and compile the cube shader program
    cubeShaderProgram.reset(new juce::OpenGLShaderProgram(openGLContext));

    if (!cubeShaderProgram->addVertexShader(cubeVertexShaderSource))
    {
        juce::Logger::writeToLog("Failed to compile cube vertex shader:");
        juce::Logger::writeToLog(cubeShaderProgram->getLastError());
        return;
    }

    if (!cubeShaderProgram->addFragmentShader(cubeFragmentShaderSource))
    {
        juce::Logger::writeToLog("Failed to compile cube fragment shader:");
        juce::Logger::writeToLog(cubeShaderProgram->getLastError());
        return;
    }

    if (!cubeShaderProgram->link())
    {
        juce::Logger::writeToLog("Failed to link cube shader program:");
        juce::Logger::writeToLog(cubeShaderProgram->getLastError());
        return;
    }

    // Get attribute and uniform locations for the cube shader
    cubePositionAttribute.reset(new juce::OpenGLShaderProgram::Attribute(*cubeShaderProgram, "aPosition"));

    cubeProjectionMatrixUniform.reset(new juce::OpenGLShaderProgram::Uniform(*cubeShaderProgram, "uProjectionMatrix"));
    cubeModelViewMatrixUniform.reset(new juce::OpenGLShaderProgram::Uniform(*cubeShaderProgram, "uModelViewMatrix"));
    cubePrevModelViewMatrixUniform.reset(new juce::OpenGLShaderProgram::Uniform(*cubeShaderProgram, "uPrevModelViewMatrix"));
    cubeColorUniform.reset(new juce::OpenGLShaderProgram::Uniform(*cubeShaderProgram, "uColor"));
    cubeAlphaUniform.reset(new juce::OpenGLShaderProgram::Uniform(*cubeShaderProgram, "uAlpha"));

    // Motion Blur Shader
    const char* motionBlurVertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec2 aPosition;
    out vec2 vTexCoord;

    void main()
    {
        vTexCoord = aPosition * 0.5 + 0.5;
        gl_Position = vec4(aPosition, 0.0, 1.0);
    }
    )";

    const char* motionBlurFragmentShaderSource = R"(
    #version 330 core
    in vec2 vTexCoord;
    out vec4 FragColor;

    uniform sampler2D uColorTexture;
    uniform sampler2D uMotionTexture;

    void main()
    {
        vec2 motion = texture(uMotionTexture, vTexCoord).xy;

        int samples = 16;
        vec4 color = vec4(0.0);
        float totalWeight = 0.0;

        for (int i = 0; i < samples; ++i)
        {
            float t = float(i) / float(samples - 1);
            vec2 samplePos = vTexCoord - motion * t;
            vec4 sampleColor = texture(uColorTexture, samplePos);
            float weight = exp(-3.0 * t * t);
            color += sampleColor * weight;
            totalWeight += weight;
        }

        color /= totalWeight;
        FragColor = color;
    }
    )";

    // Create and compile the motion blur shader program
    motionBlurShaderProgram.reset(new juce::OpenGLShaderProgram(openGLContext));

    if (!motionBlurShaderProgram->addVertexShader(motionBlurVertexShaderSource))
    {
        juce::Logger::writeToLog("Failed to compile motion blur vertex shader:");
        juce::Logger::writeToLog(motionBlurShaderProgram->getLastError());
        return;
    }

    if (!motionBlurShaderProgram->addFragmentShader(motionBlurFragmentShaderSource))
    {
        juce::Logger::writeToLog("Failed to compile motion blur fragment shader:");
        juce::Logger::writeToLog(motionBlurShaderProgram->getLastError());
        return;
    }

    if (!motionBlurShaderProgram->link())
    {
        juce::Logger::writeToLog("Failed to link motion blur shader program:");
        juce::Logger::writeToLog(motionBlurShaderProgram->getLastError());
        return;
    }

    // Get attribute and uniform locations for the motion blur shader
    quadPositionAttribute.reset(new juce::OpenGLShaderProgram::Attribute(*motionBlurShaderProgram, "aPosition"));
    motionBlurColorTextureUniform.reset(new juce::OpenGLShaderProgram::Uniform(*motionBlurShaderProgram, "uColorTexture"));
    motionBlurMotionTextureUniform.reset(new juce::OpenGLShaderProgram::Uniform(*motionBlurShaderProgram, "uMotionTexture"));

    // Basic Blur Shader (Enhanced Blur with DAMP)
    const char* blurVertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec2 aPosition;
    out vec2 vTexCoord;

    void main()
    {
        vTexCoord = aPosition * 0.5 + 0.5;
        gl_Position = vec4(aPosition, 0.0, 1.0);
    }
    )";

    const char* blurFragmentShaderSource = R"(
    #version 330 core
    in vec2 vTexCoord;
    out vec4 FragColor;

    uniform sampler2D uTexture;
    uniform float uDamp;

    void main()
    {
        float offset = (1.0 + uDamp * 2.0) / 300.0; // Adjusted blur strength
        vec3 result = vec3(0.0);

        // Expanded 9-tap Gaussian blur kernel
        float kernel[9] = float[](0.05, 0.09, 0.12, 0.15, 0.18, 0.15, 0.12, 0.09, 0.05);
        vec2 offsets[9] = vec2[](
            vec2(-4.0, 0.0), vec2(-3.0, 0.0), vec2(-2.0, 0.0),
            vec2(-1.0, 0.0), vec2(0.0, 0.0), vec2(1.0, 0.0),
            vec2(2.0, 0.0), vec2(3.0, 0.0), vec2(4.0, 0.0)
        );

        for(int i = 0; i < 9; i++)
        {
            vec2 samplePos = vTexCoord + offsets[i] * offset;
            result += texture(uTexture, samplePos).rgb * kernel[i];
        }

        FragColor = vec4(result, 1.0);
    }
    )";

    // Create and compile the basic blur shader program
    blurShaderProgram.reset(new juce::OpenGLShaderProgram(openGLContext));

    if (!blurShaderProgram->addVertexShader(blurVertexShaderSource))
    {
        juce::Logger::writeToLog("Failed to compile blur vertex shader:");
        juce::Logger::writeToLog(blurShaderProgram->getLastError());
        return;
    }

    if (!blurShaderProgram->addFragmentShader(blurFragmentShaderSource))
    {
        juce::Logger::writeToLog("Failed to compile blur fragment shader:");
        juce::Logger::writeToLog(blurShaderProgram->getLastError());
        return;
    }

    if (!blurShaderProgram->link())
    {
        juce::Logger::writeToLog("Failed to link blur shader program:");
        juce::Logger::writeToLog(blurShaderProgram->getLastError());
        return;
    }

    // Get uniform locations for the blur shader
    blurTextureUniform.reset(new juce::OpenGLShaderProgram::Uniform(*blurShaderProgram, "uTexture"));
    blurDampUniform.reset(new juce::OpenGLShaderProgram::Uniform(*blurShaderProgram, "uDamp"));

    // === Set Up Cube Geometry ===

    // Define the cube vertices
    static const GLfloat cubeVertices[] = {
        // Positions
        -1.0f, -1.0f, -1.0f, // 0
         1.0f, -1.0f, -1.0f, // 1
         1.0f,  1.0f, -1.0f, // 2
        -1.0f,  1.0f, -1.0f, // 3
        -1.0f, -1.0f,  1.0f, // 4
         1.0f, -1.0f,  1.0f, // 5
         1.0f,  1.0f,  1.0f, // 6
        -1.0f,  1.0f,  1.0f  // 7
    };

    // Define the indices for the cube lines (wireframe)
    static const GLuint cubeIndices[] = {
        // Front face
        0, 1, 1, 2, 2, 3, 3, 0,
        // Back face
        4, 5, 5, 6, 6, 7, 7, 4,
        // Edges between front and back faces
        0, 4, 1, 5, 2, 6, 3, 7
    };

    numIndices = sizeof(cubeIndices) / sizeof(GLuint);

    // Generate and bind the VAO
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);

    // Generate and bind the VBO
    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // Generate and bind the EBO (indices)
    glGenBuffers(1, &cubeEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

    // Configure vertex attributes
    glEnableVertexAttribArray(cubePositionAttribute->attributeID);
    glVertexAttribPointer(cubePositionAttribute->attributeID, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);

    // Unbind VAO
    glBindVertexArray(0);

    // === Set Up Quad Geometry ===

    // Quad vertices (position)
    GLfloat quadVertices[] = {
        // Positions
        -1.0f, -1.0f,   // Bottom-left
         1.0f, -1.0f,   // Bottom-right
        -1.0f,  1.0f,   // Top-left
         1.0f,  1.0f    // Top-right
    };

    // Generate and bind the VAO and VBO for the quad
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // Configure vertex attributes
    glEnableVertexAttribArray(0); // Assuming layout(location = 0) in your shaders
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);

    glBindVertexArray(0);

    // === Set Up Framebuffers and Textures ===

    // Scene FBO and textures
    glGenFramebuffers(1, &sceneFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);

    // Color texture
    glGenTextures(1, &colorTexture);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, getWidth(), getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture, 0);

    // Motion vector texture
    glGenTextures(1, &motionTexture);
    glBindTexture(GL_TEXTURE_2D, motionTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, getWidth(), getHeight(), 0, GL_RG, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, motionTexture, 0);

    // Specify the list of draw buffers
    GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, buffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        juce::Logger::writeToLog("Scene framebuffer is not complete!");
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Blur FBO and texture
    glGenFramebuffers(1, &blurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);

    glGenTextures(1, &blurTexture);
    glBindTexture(GL_TEXTURE_2D, blurTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, getWidth(), getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurTexture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        juce::Logger::writeToLog("Blur framebuffer is not complete!");
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Initialize the previous model-view matrices
    prevModelViewMatrix = juce::Matrix3D<float>(); // Identity matrix
    prevRedCubeModelViewMatrix = juce::Matrix3D<float>(); // Identity matrix
    prevGreenCubeModelViewMatrix = juce::Matrix3D<float>(); // Identity matrix
}

void MainComponent::render()
{
    jassert(juce::OpenGLHelpers::isContextActive());


    // --- Animate Cube Rotation ---
   // Increment rotation angles for a slow, continuous spin
    rotationX += 0.1f;  // Adjust this value to control speed on the X-axis
    rotationY += 0.2f;  // Adjust this value to control speed on the Y-axis
    if (rotationX > 360.0f)
        rotationX -= 360.0f;
    if (rotationY > 360.0f)
        rotationY -= 360.0f;





    // === Update Transformations ===

    // Store the previous model-view matrices for each cube
    prevRedCubeModelViewMatrix = redCubeModelViewMatrix;
    prevGreenCubeModelViewMatrix = greenCubeModelViewMatrix;

    // Calculate the aspect ratio
    float aspect = static_cast<float>(getWidth()) / getHeight();

    // Manually create the projection matrix (perspective projection)
    float fov = 60.0f; // Field of view in degrees
    float near = 1.0f;
    float far = 100.0f;
    float f = 1.0f / std::tan(juce::degreesToRadians(fov) / 2.0f);

    float projectionMatrixValues[16] = {
        f / aspect, 0.0f, 0.0f,                               0.0f,
        0.0f,       f,      0.0f,                             0.0f,
        0.0f,       0.0f,   (far + near) / (near - far),     -1.0f,
        0.0f,       0.0f,   (2.0f * far * near) / (near - far), 0.0f
    };
    juce::Matrix3D<float> projectionMatrix(projectionMatrixValues);

    // Create rotation matrix
    juce::Matrix3D<float> rotationMatrix = juce::Matrix3D<float>::rotation(
        { juce::degreesToRadians(rotationX), juce::degreesToRadians(rotationY), 0.0f });

    // Create translation matrix
    juce::Matrix3D<float> translationMatrix(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, -10.0f, 1.0f
    );

    // === Define Scaling Factors ===
        // Create a common adjusted base size. 
// This remaps cubeSize from [1.0, 2.0] to [1.0, 1.6] if using a factor of 0.6.
    float adjustedBaseSize = 1.0f + ((cubeSize - 1.0f) * 0.6f);
    // Red Cube uses cubeSize from the size slider/dial (unaffected by widthValue)
    float cubeSize1 = cubeSize;

    // Green Cube scales based on widthValue
    float minScale = 0.5f; // Minimum scale factor
    float maxScale = 1.5f; // Maximum scale factor
    float greenScaleFactor = minScale + (maxScale - minScale) * widthValue;
    float whiteCubeSize = adjustedBaseSize * greenScaleFactor;




    // === Render the Cubes to Scene FBO ===

    // Bind and clear the scene framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
    glViewport(0, 0, getWidth(), getHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    cubeShaderProgram->use();

    // Set uniforms
    if (cubeProjectionMatrixUniform != nullptr)
        cubeProjectionMatrixUniform->setMatrix4(projectionMatrix.mat, 1, false);

    // Set line width
    glLineWidth(5.0f);

    // --- Render Red Cube --- //
    {
        // Set color to the red cube's color
        if (cubeColorUniform != nullptr)
            cubeColorUniform->set(cubeColor.getFloatRed(),
                cubeColor.getFloatGreen(),
                cubeColor.getFloatBlue());

        // Set alpha to 1.0 (fully opaque)
        if (cubeAlphaUniform != nullptr)
            cubeAlphaUniform->set(1.0f);

        float adjustedRedCubeSize = adjustedBaseSize;
        juce::Matrix3D<float> scalingMatrix1(
            adjustedRedCubeSize, 0.0f, 0.0f, 0.0f,
            0.0f, adjustedRedCubeSize, 0.0f, 0.0f,
            0.0f, 0.0f, adjustedRedCubeSize, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
        redCubeModelViewMatrix = translationMatrix * rotationMatrix * scalingMatrix1;

        // Set model-view matrices
        if (cubeModelViewMatrixUniform != nullptr)
            cubeModelViewMatrixUniform->setMatrix4(redCubeModelViewMatrix.mat, 1, false);
        if (cubePrevModelViewMatrixUniform != nullptr)
            cubePrevModelViewMatrixUniform->setMatrix4(prevRedCubeModelViewMatrix.mat, 1, false);

        // Bind VAO and draw red cube
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_LINES, numIndices, GL_UNSIGNED_INT, 0);
    }

    // --- Render Green Cube --- //
    {
        // Set color to green
        if (cubeColorUniform != nullptr)
            cubeColorUniform->set(1.0f, 1.0f, 1.0f);  // Green color CHANGED TO WHITE

        // Set alpha to make the cube less visible
        if (cubeAlphaUniform != nullptr)
            cubeAlphaUniform->set(greenCubeAlpha);  // Adjust alpha value as needed

        // Create scaling matrix for the green cube
        juce::Matrix3D<float> scalingMatrix2(
            whiteCubeSize, 0.0f, 0.0f, 0.0f,
            0.0f, whiteCubeSize, 0.0f, 0.0f,
            0.0f, 0.0f, whiteCubeSize, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        );
        greenCubeModelViewMatrix = translationMatrix * rotationMatrix * scalingMatrix2;


        // Set model-view matrices
        if (cubeModelViewMatrixUniform != nullptr)
            cubeModelViewMatrixUniform->setMatrix4(greenCubeModelViewMatrix.mat, 1, false);
        if (cubePrevModelViewMatrixUniform != nullptr)
            cubePrevModelViewMatrixUniform->setMatrix4(prevGreenCubeModelViewMatrix.mat, 1, false);

        // Draw the green cube
        glDrawElements(GL_LINES, numIndices, GL_UNSIGNED_INT, 0);
    }

    // Unbind VAO after drawing
    glBindVertexArray(0);

    // Unbind the scene framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // === Apply Motion Blur ===

    // Use motion blur shader program
    motionBlurShaderProgram->use();

    // Bind the necessary textures and set uniforms
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorTexture);
    if (motionBlurColorTextureUniform != nullptr)
        motionBlurColorTextureUniform->set(0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, motionTexture);
    if (motionBlurMotionTextureUniform != nullptr)
        motionBlurMotionTextureUniform->set(1);

    // Bind framebuffer and clear
    glBindFramebuffer(GL_FRAMEBUFFER, blurFBO);
    glViewport(0, 0, getWidth(), getHeight());
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);

    // Unbind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // === Apply Basic Blur (Gaussian Blur) ===

    // Use blur shader program
    blurShaderProgram->use();

    // Bind the blurred texture from the motion blur pass
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, blurTexture);
    if (blurTextureUniform != nullptr)
        blurTextureUniform->set(0);

    if (blurDampUniform != nullptr)
        blurDampUniform->set(dampValue);

    // Clear the default framebuffer
    glViewport(0, 0, getWidth(), getHeight());
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glBindVertexArray(0);
}

void MainComponent::shutdown()
{
    // Delete buffers and resources

    // Delete cube VAO and VBO
    if (cubeVAO != 0)
    {
        glDeleteVertexArrays(1, &cubeVAO);
        cubeVAO = 0;
    }

    if (cubeVBO != 0)
    {
        glDeleteBuffers(1, &cubeVBO);
        cubeVBO = 0;
    }

    if (cubeEBO != 0)
    {
        glDeleteBuffers(1, &cubeEBO);
        cubeEBO = 0;
    }

    // Delete quad VAO and VBO
    if (quadVAO != 0)
    {
        glDeleteVertexArrays(1, &quadVAO);
        quadVAO = 0;
    }

    if (quadVBO != 0)
    {
        glDeleteBuffers(1, &quadVBO);
        quadVBO = 0;
    }

    // Delete textures and framebuffers
    if (colorTexture != 0)
    {
        glDeleteTextures(1, &colorTexture);
        colorTexture = 0;
    }

    if (motionTexture != 0)
    {
        glDeleteTextures(1, &motionTexture);
        motionTexture = 0;
    }

    if (sceneFBO != 0)
    {
        glDeleteFramebuffers(1, &sceneFBO);
        sceneFBO = 0;
    }

    if (blurTexture != 0)
    {
        glDeleteTextures(1, &blurTexture);
        blurTexture = 0;
    }

    if (blurFBO != 0)
    {
        glDeleteFramebuffers(1, &blurFBO);
        blurFBO = 0;
    }

    // Reset shader programs
    cubeShaderProgram.reset();
    motionBlurShaderProgram.reset();
    blurShaderProgram.reset();
}

void MainComponent::paint(juce::Graphics& g)
{
    // OpenGL rendering is handled in render()
}

void MainComponent::resized()
{
    // Handle window resizing
    // Update textures and framebuffers to match new size

    if (colorTexture != 0)
    {
        glBindTexture(GL_TEXTURE_2D, colorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, getWidth(), getHeight(),
            0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }

    if (motionTexture != 0)
    {
        glBindTexture(GL_TEXTURE_2D, motionTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, getWidth(), getHeight(),
            0, GL_RG, GL_FLOAT, nullptr);
    }

    if (blurTexture != 0)
    {
        glBindTexture(GL_TEXTURE_2D, blurTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, getWidth(), getHeight(),
            0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}

void MainComponent::mouseMove(const juce::MouseEvent& event)
{
    lastMouseX = event.x;
    lastMouseY = event.y;
}

void MainComponent::mouseDown(const juce::MouseEvent& event)
{
    isDragging = true;
    lastMouseX = event.getPosition().getX();
    lastMouseY = event.getPosition().getY();
}

void MainComponent::mouseDrag(const juce::MouseEvent& event)
{
    if (isDragging)
    {
        float deltaX = event.getPosition().getX() - lastMouseX;
        float deltaY = event.getPosition().getY() - lastMouseY;

        rotationY += deltaX * 0.5f;  // Adjust sensitivity as needed
        rotationX += deltaY * 0.5f;  // Adjust sensitivity as needed

        lastMouseX = event.getPosition().getX();
        lastMouseY = event.getPosition().getY();
    }
}

void MainComponent::mouseUp(const juce::MouseEvent& event)
{
    isDragging = false;
}

juce::Colour MainComponent::interpolateColor(const juce::Colour& startColor,
    const juce::Colour& endColor,
    float ratio)
{
    return startColor.interpolatedWith(endColor, ratio);
}