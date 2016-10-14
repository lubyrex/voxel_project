/**
  \file App.h

  The G3D 10.00 default starter app is configured for OpenGL 4.1 and
  relatively recent GPUs.
 */
#pragma once
#include <G3D/G3DAll.h>

class Mesh {
public:
    Array<Vector3> vertexArray;
    Array<Vector2> textureCoords;
    Array<Vector3> normalArray;
    Array<int>     indexArray;

    void initFromVoxels(const ParseVOX& voxelGrid);
    void addQuad(Vector3::Axis axis, const Point3& center, float sign, const Point2& texCoord);
    void addQuad(const Point3& A, const Point3& B, const Point3& C, const Vector3& n, const Point2& texCoord);
    void weldVertices();
    void saveToOBJ();
};

/** \brief Application framework. */
class App : public GApp {
protected:

    /** Called from onInit */
    void makeGUI();
    bool m_remove_voxels = true;
    bool m_remove_surfaces = true;
    String m_filesource;
    int m_resolutionChoice = 1;

public:
    
    App(const GApp::Settings& settings = GApp::Settings());

    virtual void onInit() override;
    virtual void onAI() override;
    virtual void onNetwork() override;
    virtual void onSimulation(RealTime rdt, SimTime sdt, SimTime idt) override;
    virtual void onPose(Array<shared_ptr<Surface> >& posed3D, Array<shared_ptr<Surface2D> >& posed2D) override;

    // You can override onGraphics if you want more control over the rendering loop.
    // virtual void onGraphics(RenderDevice* rd, Array<shared_ptr<Surface> >& surface, Array<shared_ptr<Surface2D> >& surface2D) override;

    virtual void onGraphics3D(RenderDevice* rd, Array<shared_ptr<Surface> >& surface3D) override;
    virtual void onGraphics2D(RenderDevice* rd, Array<shared_ptr<Surface2D> >& surface2D) override;

    virtual bool onEvent(const GEvent& e) override;
    virtual void onUserInput(UserInput* ui) override;
    virtual void onCleanup() override;
    //virtual void testMesh( G3D::ParseVOX );
    virtual void culling( G3D::ParseVOX );
    /*virtual void culling2(G3D::ParseVOX);*/
    virtual void savePNG(G3D::ParseVOX);
    virtual void saveMTL(G3D::ParseVOX);
    virtual void message(const String& msg) const;
};
