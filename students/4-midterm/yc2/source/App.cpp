/** \file App.cpp */
#include "App.h"
#include <iostream>
#include <fstream>
using namespace std;
#include <stdio.h>


// Tells C++ to invoke command-line main() function even on OS X and Win32.
G3D_START_AT_MAIN();

int main(int argc, const char* argv[]) {
    {
        G3DSpecification g3dSpec;
        g3dSpec.audio = false;
        initGLG3D(g3dSpec);
    }

    GApp::Settings settings(argc, argv);

    // Change the window and other startup parameters by modifying the
    // settings class.  For example:
    settings.window.caption             = argv[0];

    // Set enable to catch more OpenGL errors
    // settings.window.debugContext     = true;

    // Some common resolutions:
    // settings.window.width            =  854; settings.window.height       = 480;
    // settings.window.width            = 1024; settings.window.height       = 768;
    settings.window.width               = 1280; settings.window.height       = 720;
    //settings.window.width             = 1920; settings.window.height       = 1080;
    // settings.window.width            = OSWindow::primaryDisplayWindowSize().x; settings.window.height = OSWindow::primaryDisplayWindowSize().y;
    settings.window.fullScreen          = false;
    settings.window.resizable           = ! settings.window.fullScreen;
    settings.window.framed              = ! settings.window.fullScreen;

    // Set to true for a significant performance boost if your app can't render at 60fps, or if
    // you *want* to render faster than the display.
    settings.window.asynchronous        = false;

    settings.hdrFramebuffer.depthGuardBandThickness = Vector2int16(64, 64);
    settings.hdrFramebuffer.colorGuardBandThickness = Vector2int16(0, 0);
    settings.dataDir                    = FileSystem::currentDirectory();
    settings.screenshotDirectory        = "../journal/";

    settings.renderer.deferredShading = true;
    settings.renderer.orderIndependentTransparency = false;

    return App(settings).run();
}


App::App(const GApp::Settings& settings) : GApp(settings) {
}


// Called before the application loop begins.  Load data here and
// not in the constructor so that common exceptions will be
// automatically caught.
void App::onInit() {
    GApp::onInit();
    //setFrameDuration(1.0f / 120.0f);

    // Call setScene(shared_ptr<Scene>()) or setScene(MyScene::create()) to replace
    // the default scene here.
    
    showRenderingStats      = false;

    makeGUI();
    // For higher-quality screenshots:
    // developerWindow->videoRecordDialog->setScreenShotFormat("PNG");
    // developerWindow->videoRecordDialog->setCaptureGui(false);
    developerWindow->cameraControlWindow->moveTo(Point2(developerWindow->cameraControlWindow->rect().x0(), 0));
    BinaryInput voxInput("untitled.vox",G3D_LITTLE_ENDIAN);
    G3D::ParseVOX s;
    s.ParseVOX::parse(voxInput);
    culling2(s);
    loadScene(
        //"G3D Sponza"
        "Test Scene" // Load something simple
        //developerWindow->sceneEditorWindow->selectedSceneName()  // Load the first scene encountered 
        );
   
   
   /* shared_ptr<ArticulatedModel> test;
    test->createEmpty("ya");
    
    ArticulatedModel::Part* part(test -> addPart("part",NULL));
    ArticulatedModel::Geometry* geom(test -> addGeometry("geom"));
    ArticulatedModel::Mesh* mesh(test->addMesh("first",part,geom));*/
    
}


void App::testMesh( G3D::ParseVOX s ){
    TextOutput myfile("../data-files/model/test.obj");
    //bad upper bound
    
    Array<ParseVOX::Voxel> testarray = s.voxel;
    int l = testarray.length();
    
    myfile.printf("vn -1 0 0\nvn 1 0 0\nvn 0 0 1\nvn 0 0 -1\nvn 0 -1 0\nvn 0 1 0\n");
    for (int i = 0; i < l; ++i) {
        ParseVOX::Voxel testvoxel = s.voxel[i];
        Point3uint8 position = testvoxel.position;
        myfile.printf("v %d %d %d \n",position.x,position.z,position.y);
        myfile.printf("v %d %d %d \n",position.x+1,position.z,position.y);
        myfile.printf("v %d %d %d \n",position.x+1,position.z,position.y+1);
        myfile.printf("v %d %d %d \n",position.x,position.z,position.y+1);
        myfile.printf("v %d %d %d \n",position.x,position.z+1,position.y);
        myfile.printf("v %d %d %d \n",position.x+1,position.z+1,position.y);
        myfile.printf("v %d %d %d \n",position.x+1,position.z+1,position.y+1);
        myfile.printf("v %d %d %d \n",position.x,position.z+1,position.y+1);
    }
      for (int i = 0; i < l; ++i) {
        ParseVOX::Voxel testvoxel = s.voxel[i];
        Point3uint8 position = testvoxel.position;
        //top
        myfile.printf("f %d %d %d %d \n",5+8*i,8+8*i,7+8*i,6+8*i);
        //left
        myfile.printf("f %d %d %d %d \n",6+8*i,7+8*i,3+8*i,2+8*i);
        //bot
        myfile.printf("f %d %d %d %d \n",2+8*i,3+8*i,4+8*i,1+8*i);
        //right
        myfile.printf("f %d %d %d %d \n",1+8*i,4+8*i,8+8*i,5+8*i);
        //back
        myfile.printf("f %d %d %d %d \n",8+8*i,4+8*i,3+8*i,7+8*i);
        //front
        myfile.printf("f %d %d %d %d \n",6+8*i,2+8*i,1+8*i,5+8*i);
    }
    TextOutput testfile("../data-files/model/test.txt");
    for (int i = 0; i < l; ++i) {
        ParseVOX::Voxel testvoxel = s.voxel[i];
        Point3uint8 position = testvoxel.position;
        testfile.printf("v %d %d %d \n",position.x,position.y,position.z);
    }
    myfile.commit();
    testfile.commit();
}

void App::culling( G3D::ParseVOX s ){
    TextOutput myfile("../data-files/model/culling.obj");
    //bad upper bound
    shared_ptr<Table<G3D::Point3uint8,int>> table(new G3D::Table<G3D::Point3uint8,int>());
    Array<ParseVOX::Voxel> testarray = s.voxel;
    ParseVOX::Voxel testvoxel;
    int l = testarray.length();
    for (int i = 0; i < l; ++i) {
        testvoxel = s.voxel[i];
        Point3uint8 position = testvoxel.position;
        table->set(position,1);
    }
    for (int i = 0; i < l; ++i) {
        testvoxel = testarray[i];
        Point3uint8 position = testvoxel.position;
        if(table->containsKey(Vector3uint8(position.x,position.y,position.z+1))&&
        table->containsKey(Vector3uint8(position.x+1,position.y,position.z))&&
        table->containsKey(Vector3uint8(position.x,position.y,position.z-1))&&
        table->containsKey(Vector3uint8(position.x-1,position.y,position.z))&&
        table->containsKey(Vector3uint8(position.x,position.y+1,position.z))&&
        table->containsKey(Vector3uint8(position.x,position.y-1,position.z))){
            testarray.remove(i);
            --i;
            --l;
        }
    }
    

    myfile.printf("vn -1 0 0\nvn 1 0 0\nvn 0 0 1\nvn 0 0 -1\nvn 0 -1 0\nvn 0 1 0\n");
    
    for (int i = 0; i < l; ++i) {
        testvoxel = testarray[i];
        Point3uint8 position = testvoxel.position;
        //1
        myfile.printf("v %d %d %d \n",position.x,position.z,position.y);
        //2
        myfile.printf("v %d %d %d \n",position.x+1,position.z,position.y);
        //3
        myfile.printf("v %d %d %d \n",position.x+1,position.z,position.y+1);
        //4
        myfile.printf("v %d %d %d \n",position.x,position.z,position.y+1);
        //5
        myfile.printf("v %d %d %d \n",position.x,position.z+1,position.y);
        //6
        myfile.printf("v %d %d %d \n",position.x+1,position.z+1,position.y);
        //7
        myfile.printf("v %d %d %d \n",position.x+1,position.z+1,position.y+1);
        //8
        myfile.printf("v %d %d %d \n",position.x,position.z+1,position.y+1);
    }
      for (int i = 0; i < l; ++i) {
        testvoxel =testarray[i];
        Point3uint8 position = testvoxel.position;
        if(!table->containsKey(Vector3uint8(position.x,position.y,position.z+1))){
        myfile.printf("f %d %d %d %d \n",5+8*i,8+8*i,7+8*i,6+8*i);
        }
        if(!table->containsKey(Vector3uint8(position.x+1,position.y,position.z))){
        myfile.printf("f %d %d %d %d \n",6+8*i,7+8*i,3+8*i,2+8*i);
        }
        if(!table->containsKey(Vector3uint8(position.x,position.y,position.z-1))){
        myfile.printf("f %d %d %d %d \n",2+8*i,3+8*i,4+8*i,1+8*i);
        }
        if(!table->containsKey(Vector3uint8(position.x-1,position.y,position.z))){
        myfile.printf("f %d %d %d %d \n",1+8*i,4+8*i,8+8*i,5+8*i);
        }
        if(!table->containsKey(Vector3uint8(position.x,position.y+1,position.z))){
        myfile.printf("f %d %d %d %d \n",8+8*i,4+8*i,3+8*i,7+8*i);
        }
        if(!table->containsKey(Vector3uint8(position.x,position.y-1,position.z))){
        myfile.printf("f %d %d %d %d \n",6+8*i,2+8*i,1+8*i,5+8*i);
        }
    }
    TextOutput testfile("../data-files/model/test.txt");
    for (int i = 0; i < l; ++i) {
        ParseVOX::Voxel testvoxel = s.voxel[i];
        Point3uint8 position = testvoxel.position;
        testfile.printf("v %d %d %d \n",position.x,position.y,position.z);
    }
    myfile.commit();
    testfile.commit();
}



void App::culling2( G3D::ParseVOX s ){
    TextOutput myfile("../data-files/model/culling.obj");
    //bad upper bound
    shared_ptr<Table<G3D::Point3uint8,int>> table(new G3D::Table<G3D::Point3uint8,int>());
    Array<ParseVOX::Voxel> testarray = s.voxel;
    ParseVOX::Voxel testvoxel;
    int l = testarray.length();
    for (int i = 0; i < l; ++i) {
        testvoxel = s.voxel[i];
        Point3uint8 position = testvoxel.position;
        table->set(position,1);
    }
    for (int i = 0; i < l; ++i) {
        testvoxel = testarray[i];
        Point3uint8 position = testvoxel.position;
        if(table->containsKey(Vector3uint8(position.x,position.y,position.z+1))&&
        table->containsKey(Vector3uint8(position.x+1,position.y,position.z))&&
        table->containsKey(Vector3uint8(position.x,position.y,position.z-1))&&
        table->containsKey(Vector3uint8(position.x-1,position.y,position.z))&&
        table->containsKey(Vector3uint8(position.x,position.y+1,position.z))&&
        table->containsKey(Vector3uint8(position.x,position.y-1,position.z))){
            testarray.remove(i);
            --i;
            --l;
        }
    }
    

    myfile.printf("vn -1 0 0\nvn 1 0 0\nvn 0 0 1\nvn 0 0 -1\nvn 0 -1 0\nvn 0 1 0\n");
    
    for (int i = 0; i < l; ++i) {
        testvoxel = testarray[i];
        Point3uint8 position = testvoxel.position;
        //1
        myfile.printf("v %d %d %d \n",position.x,position.z,position.y);
        //2
        myfile.printf("v %d %d %d \n",position.x+1,position.z,position.y);
        //3
        myfile.printf("v %d %d %d \n",position.x+1,position.z,position.y+1);
        //4
        myfile.printf("v %d %d %d \n",position.x,position.z,position.y+1);
        //5
        myfile.printf("v %d %d %d \n",position.x,position.z+1,position.y);
        //6
        myfile.printf("v %d %d %d \n",position.x+1,position.z+1,position.y);
        //7
        myfile.printf("v %d %d %d \n",position.x+1,position.z+1,position.y+1);
        //8
        myfile.printf("v %d %d %d \n",position.x,position.z+1,position.y+1);
    }
      for (int i = 0; i < l; ++i) {
        testvoxel =testarray[i];
        Point3uint8 position = testvoxel.position;
      
        myfile.printf("f %d %d %d %d \n",5+8*i,8+8*i,7+8*i,6+8*i);
    
        myfile.printf("f %d %d %d %d \n",6+8*i,7+8*i,3+8*i,2+8*i);
    
        myfile.printf("f %d %d %d %d \n",2+8*i,3+8*i,4+8*i,1+8*i);
        
        myfile.printf("f %d %d %d %d \n",1+8*i,4+8*i,8+8*i,5+8*i);
      
        myfile.printf("f %d %d %d %d \n",8+8*i,4+8*i,3+8*i,7+8*i);
       
        myfile.printf("f %d %d %d %d \n",6+8*i,2+8*i,1+8*i,5+8*i);
       
    }
    TextOutput testfile("../data-files/model/test.txt");
    for (int i = 0; i < 256; ++i) {
        Color4unorm8 color(s.palette[i]);
        
        testfile.printf("v %d %d %d \n",color.r,color.g,color.b);
    }
    myfile.commit();
    testfile.commit();
}




void App::makeGUI() {
    // Initialize the developer HUD
    createDeveloperHUD();

    debugWindow->setVisible(true);
    developerWindow->videoRecordDialog->setEnabled(true);

    GuiPane* infoPane = debugPane->addPane("Info", GuiTheme::ORNATE_PANE_STYLE);
    
    // Example of how to add debugging controls
    infoPane->addLabel("You can add GUI controls");
    infoPane->addLabel("in App::onInit().");
    infoPane->addButton("Exit", [this]() { m_endProgram = true; });
    infoPane->pack();

    // More examples of debugging GUI controls:
    // debugPane->addCheckBox("Use explicit checking", &explicitCheck);
    // debugPane->addTextBox("Name", &myName);
    // debugPane->addNumberBox("height", &height, "m", GuiTheme::LINEAR_SLIDER, 1.0f, 2.5f);
    // button = debugPane->addButton("Run Simulator");
    // debugPane->addButton("Generate Heightfield", [this](){ generateHeightfield(); });
    // debugPane->addButton("Generate Heightfield", [this](){ makeHeightfield(imageName, scale, "model/heightfield.off"); });

    debugWindow->pack();
    debugWindow->setRect(Rect2D::xywh(0, 0, (float)window()->width(), debugWindow->rect().height()));
}


// This default implementation is a direct copy of GApp::onGraphics3D to make it easy
// for you to modify. If you aren't changing the hardware rendering strategy, you can
// delete this override entirely.
void App::onGraphics3D(RenderDevice* rd, Array<shared_ptr<Surface> >& allSurfaces) {
    if (!scene()) {
        if ((submitToDisplayMode() == SubmitToDisplayMode::MAXIMIZE_THROUGHPUT) && (!rd->swapBuffersAutomatically())) {
            swapBuffers();
        }
        rd->clear();
        rd->pushState(); {
            rd->setProjectionAndCameraMatrix(activeCamera()->projection(), activeCamera()->frame());
            drawDebugShapes();
        } rd->popState();
        return;
    }

    GBuffer::Specification gbufferSpec = m_gbufferSpecification;
    extendGBufferSpecification(gbufferSpec);
    m_gbuffer->setSpecification(gbufferSpec);
    m_gbuffer->resize(m_framebuffer->width(), m_framebuffer->height());
    m_gbuffer->prepare(rd, activeCamera(), 0, -(float)previousSimTimeStep(), m_settings.hdrFramebuffer.depthGuardBandThickness, m_settings.hdrFramebuffer.colorGuardBandThickness);

    m_renderer->render(rd, m_framebuffer, scene()->lightingEnvironment().ambientOcclusionSettings.enabled ? m_depthPeelFramebuffer : shared_ptr<Framebuffer>(),
        scene()->lightingEnvironment(), m_gbuffer, allSurfaces);

    // Debug visualizations and post-process effects
    rd->pushState(m_framebuffer); {
        // Call to make the App show the output of debugDraw(...)
        rd->setProjectionAndCameraMatrix(activeCamera()->projection(), activeCamera()->frame());
        drawDebugShapes();
        const shared_ptr<Entity>& selectedEntity = (notNull(developerWindow) && notNull(developerWindow->sceneEditorWindow)) ? developerWindow->sceneEditorWindow->selectedEntity() : shared_ptr<Entity>();
        scene()->visualize(rd, selectedEntity, allSurfaces, sceneVisualizationSettings(), activeCamera());

        // Post-process special effects
        m_depthOfField->apply(rd, m_framebuffer->texture(0), m_framebuffer->texture(Framebuffer::DEPTH), activeCamera(), m_settings.hdrFramebuffer.depthGuardBandThickness - m_settings.hdrFramebuffer.colorGuardBandThickness);

        m_motionBlur->apply(rd, m_framebuffer->texture(0), m_gbuffer->texture(GBuffer::Field::SS_EXPRESSIVE_MOTION),
            m_framebuffer->texture(Framebuffer::DEPTH), activeCamera(),
            m_settings.hdrFramebuffer.depthGuardBandThickness - m_settings.hdrFramebuffer.colorGuardBandThickness);
    } rd->popState();

    // We're about to render to the actual back buffer, so swap the buffers now.
    // This call also allows the screenshot and video recording to capture the
    // previous frame just before it is displayed.
    if (submitToDisplayMode() == SubmitToDisplayMode::MAXIMIZE_THROUGHPUT) {
        swapBuffers();
    }

    // Clear the entire screen (needed even though we'll render over it, since
    // AFR uses clear() to detect that the buffer is not re-used.)
    rd->clear();

    // Perform gamma correction, bloom, and SSAA, and write to the native window frame buffer
    m_film->exposeAndRender(rd, activeCamera()->filmSettings(), m_framebuffer->texture(0), settings().hdrFramebuffer.colorGuardBandThickness.x + settings().hdrFramebuffer.depthGuardBandThickness.x, settings().hdrFramebuffer.depthGuardBandThickness.x);
}


void App::onAI() {
    GApp::onAI();
    // Add non-simulation game logic and AI code here
}


void App::onNetwork() {
    GApp::onNetwork();
    // Poll net messages here
}


void App::onSimulation(RealTime rdt, SimTime sdt, SimTime idt) {
    GApp::onSimulation(rdt, sdt, idt);

    // Example GUI dynamic layout code.  Resize the debugWindow to fill
    // the screen horizontally.
    debugWindow->setRect(Rect2D::xywh(0, 0, (float)window()->width(), debugWindow->rect().height()));
}


bool App::onEvent(const GEvent& event) {
    // Handle super-class events
    if (GApp::onEvent(event)) { return true; }

    // If you need to track individual UI events, manage them here.
    // Return true if you want to prevent other parts of the system
    // from observing this specific event.
    //
    // For example,
    // if ((event.type == GEventType::GUI_ACTION) && (event.gui.control == m_button)) { ... return true; }
    // if ((event.type == GEventType::KEY_DOWN) && (event.key.keysym.sym == GKey::TAB)) { ... return true; }
    // if ((event.type == GEventType::KEY_DOWN) && (event.key.keysym.sym == 'p')) { ... return true; }

    if ((event.type == GEventType::KEY_DOWN) && (event.key.keysym.sym == 'p')) { 
        shared_ptr<DefaultRenderer> r = dynamic_pointer_cast<DefaultRenderer>(m_renderer);
        r->setDeferredShading(! r->deferredShading());
        return true; 
    }

    return false;
}


void App::onUserInput(UserInput* ui) {
    GApp::onUserInput(ui);
    (void)ui;
    // Add key handling here based on the keys currently held or
    // ones that changed in the last frame.
}


void App::onPose(Array<shared_ptr<Surface> >& surface, Array<shared_ptr<Surface2D> >& surface2D) {
    GApp::onPose(surface, surface2D);

    // Append any models to the arrays that you want to later be rendered by onGraphics()
}


void App::onGraphics2D(RenderDevice* rd, Array<shared_ptr<Surface2D> >& posed2D) {
    // Render 2D objects like Widgets.  These do not receive tone mapping or gamma correction.
    Surface2D::sortAndRender(rd, posed2D);
}


void App::onCleanup() {
    // Called after the application loop ends.  Place a majority of cleanup code
    // here instead of in the constructor so that exceptions can be caught.
}
