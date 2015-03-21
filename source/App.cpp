#include <G3D/G3DAll.h>

/** Application framework. */
class App : public GApp {
protected:

    shared_ptr<Texture> m_environmentMap;

    Array<shared_ptr<Texture> > m_texture;

    bool debugEnabled;
    bool cloudsEnabled;
    bool mieEnabled;
    bool rayleighEnabled;
    bool shadowsEnabled;
    bool complexNoise;
    bool proceduralNoise;
    bool skipNight;

    float frame;
    float cloudSpeed;
    float sunSpeed;
    float rayleighScale;
    float mieScale;
    float cloudAmplitude;

public:
    
    App(const GApp::Settings& settings = GApp::Settings());

    virtual void onInit() override;
    virtual void onGraphics3D(RenderDevice* rd, Array<shared_ptr<Surface> >& surface3D) override;
    virtual void onUserInput(UserInput* ui) override;

};

//////////////////////////////////////////////////////////////////////////////


App::App(const GApp::Settings& settings) : GApp(settings) {
}

void App::onInit() {
    GApp::onInit();

    debugEnabled = false;
    cloudsEnabled = true;
    complexNoise = true;
    shadowsEnabled = true;
    mieEnabled = true;
    rayleighEnabled = true;
    proceduralNoise = false;
    skipNight = true;

    frame = 0.0;
    cloudSpeed = 0.1;
    sunSpeed = 0.05;
    rayleighScale = 1.0;
    mieScale = 1.0;
    cloudAmplitude = 4.0;

    setFrameDuration(1.0f / 60.0f);
    showRenderingStats  = false;

    // Example of how to add debugging controls
    // infoPane->addLabel("You can add GUI controls");
    // infoPane->addLabel("in App::onInit().");
    // infoPane->addButton("Exit", this, &App::endProgram);
    // infoPane->pack();

    // More examples of debugging GUI controls:
    // debugPane->addCheckBox("Use explicit checking", &explicitCheck);
    // debugPane->addTextBox("Name", &myName);
    // debugPane->addNumberBox("height", &height, "m", GuiTheme::LINEAR_SLIDER, 1.0f, 2.5f);
    // button = debugPane->addButton("Run Simulator");

    createDeveloperHUD();
    developerWindow->sceneEditorWindow->setVisible(false);
    developerWindow->cameraControlWindow->setVisible(false);
    developerWindow->setVisible(false);
    developerWindow->videoRecordDialog->setEnabled(true);


    debugWindow->setVisible(false);
    
    debugPane->addNumberBox("Cloud speed", &cloudSpeed);
    debugPane->addNumberBox("Sun speed", &sunSpeed);
    debugPane->addNumberBox("Rayleigh scale", &rayleighScale);
    debugPane->addNumberBox("Mie scale", &mieScale);
    debugPane->addNumberBox("Cloud amplitude", &cloudAmplitude);
    debugPane->addCheckBox("Clouds", &cloudsEnabled);
    debugPane->addCheckBox("Shadows", &shadowsEnabled);
    debugPane->addCheckBox("Rayleigh scattering", &rayleighEnabled);
    debugPane->addCheckBox("Mie scattering", &mieEnabled);
    debugPane->addCheckBox("Complex noise", &complexNoise);
    debugPane->addCheckBox("Procedural noise", &proceduralNoise);
    debugPane->addCheckBox("Skip night", &skipNight);
    
    debugWindow->pack();
    debugWindow->setRect(Rect2D::xywh(0, 0, (float)window()->width(), debugWindow->rect().height()));


    Texture::Specification e;
    e.filename = System::findDataFile("cubemap/islands/islands_*.jpg");
    e.encoding.format = ImageFormat::SRGB8();
    m_environmentMap = Texture::create(e);

    m_texture.append(Texture::fromFile("noise.png"));
    for (int i = 0; i < 4; ++i) {
        m_texture.append(Texture::fromFile(format("iChannel%d.jpg", i)));
    }

    shared_ptr<Camera> videoCamera = Camera::create("video");
    videoCamera->filmSettings().setBloomStrength(0.0);
    videoCamera->setFieldOfViewAngle(80 * units::degrees());
    videoCamera->setPosition(Point3(0.0, 1.9, 0.0));
    videoCamera->lookAt(Point3(0.0, 2.3, 1.0));

    setActiveCamera(videoCamera);

    debugCamera()->setFieldOfViewAngle(40 * units::degrees());
}

        
void App::onGraphics3D(RenderDevice* rd, Array<shared_ptr<Surface> >& allSurfaces) {
    // Skip night
    if (frame > 290.0 && skipNight) {
        frame = -50.0;
    }
    else {
        frame += 1.0;
    }
    // Bind the main framebuffer
    rd->push2D(m_framebuffer); {
        rd->clear();

        Args args;

        args.setUniform("frame", frame);
        args.setUniform("cloudsEnabled", cloudsEnabled);
        args.setUniform("cloudSpeed", cloudSpeed);
        args.setUniform("sunSpeed", sunSpeed);
        args.setUniform("rayleighScale", rayleighScale);
        args.setUniform("mieScale", mieScale);
        args.setUniform("cloudAmplitude", cloudAmplitude);
        args.setUniform("shadowsEnabled", shadowsEnabled);
        args.setUniform("mieEnabled", mieEnabled);
        args.setUniform("rayleighEnabled", rayleighEnabled);
        args.setUniform("complexNoise", complexNoise);
        args.setUniform("proceduralNoise", proceduralNoise);

        // Prepare the arguments for the shader function invoked below
        args.setUniform("cameraToWorldMatrix",    activeCamera()->frame());

        m_environmentMap->setShaderArgs(args, "environmentMap_", Sampler::cubeMap());
        args.setUniform("environmentMap_MIPConstant", log2(float(m_environmentMap->width() * sqrt(3.0f))));

        args.setUniform("tanHalfFieldOfViewY",  float(tan(activeCamera()->projection().fieldOfViewAngle() / 2.0f)));

        // Projection matrix, for writing to the depth buffer. This
        // creates the input that allows us to use the depth of field
        // effect below.
        Matrix4 projectionMatrix;
        activeCamera()->getProjectUnitMatrix(rd->viewport(), projectionMatrix);
        args.setUniform("projectionMatrix22", projectionMatrix[2][2]);
        args.setUniform("projectionMatrix23", projectionMatrix[2][3]);

        // Textures
        for (int i = 0; i < m_texture.size(); ++i) {
            args.setUniform(format("iChannel%d", i), m_texture[i], Sampler::defaults());
        }

        // Set the domain of the shader to the viewport rectangle
        args.setRect(rd->viewport());

        // Call the program in trace.pix for every pixel within the
        // domain, using many threads on the GPU. This call returns
        // immediately on the CPU and the code executes asynchronously
        // on the GPU.
        LAUNCH_SHADER("sky.pix", args);

        // Post-process special effects
        // m_depthOfField->apply(rd, m_framebuffer->texture(0), m_framebuffer->texture(Framebuffer::DEPTH), activeCamera(), Vector2int16());
        
    } rd->pop2D();

    swapBuffers();

    rd->clear();

    // Perform gamma correction, bloom, and SSAA, and write to the native window frame buffer
    m_film->exposeAndRender(rd, activeCamera()->filmSettings(), m_framebuffer->texture(0));

}


void App::onUserInput(UserInput* ui) {
    GApp::onUserInput(ui);
    if (ui->keyReleased(GKey::BACKQUOTE)) {
        debugEnabled = !debugEnabled;
        developerWindow->setVisible(debugEnabled);
        debugWindow->setVisible(debugEnabled);
    }
    if (ui->keyReleased(GKey::LEFTBRACKET)) {
        cloudsEnabled = false;
        shadowsEnabled = false;
        mieEnabled = false;
        rayleighEnabled = false;
    }
    if (ui->keyReleased(GKey::RIGHTBRACKET)) {
        cloudsEnabled = true;
        shadowsEnabled = true;
        mieEnabled = true;
        rayleighEnabled = true;
    }
    if (ui->keyReleased(GKey::COMMA)) {
        mieEnabled = !mieEnabled;
    }
    if (ui->keyReleased(GKey::PERIOD)) {
        rayleighEnabled = !rayleighEnabled;
    }
    if (ui->keyReleased(GKey::SEMICOLON)) {
        cloudsEnabled = !cloudsEnabled;
    }
    if (ui->keyReleased(GKey::QUOTE)) {
        shadowsEnabled = !shadowsEnabled;
    }
}

//////////////////////////////////////////////////////////////////////

G3D_START_AT_MAIN();

int main(int argc, const char* argv[]) {
    {
        G3DSpecification g3dSpec;
        g3dSpec.audio = false;
        initGLG3D(g3dSpec);
    }

    GApp::Settings settings(argc, argv);

    settings.window.caption = "Sky";
    settings.window.width   = 800;
    settings.window.height  = 600;
    settings.window.fullScreen = true;
    settings.window.framed = false;

    settings.screenshotDirectory = "../journal";

    return App(settings).run();
}

