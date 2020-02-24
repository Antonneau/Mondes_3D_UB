#include "viewer.h"
#include "camera.h"

using namespace Eigen;

Viewer::Viewer()
  : _winWidth(0), 
  _winHeight(0), 
  _offset(0., 0.), 
  _enableWire(-1), 
  _enableView(-1), 
  _thetaX(0), 
  _thetaY(0), 
  _thetaYEarth(0), 
  _thetaYEarthRotate(0),
  _thetaYMoon(0),
  _thetaYMoonRotate(0),
  _thetaZ(0)
{
}

Viewer::~Viewer()
{
}

////////////////////////////////////////////////////////////////////////////////
// GL stuff

// initialize OpenGL context
void Viewer::init(int w, int h){
    loadShaders();

    if(!_mesh.load(DATA_DIR"/models/sphere.off")) exit(1);
    // Compute normals
    _mesh.computeNormals();
    _mesh.initVBA();

    reshape(w,h);
    _trackball.setCamera(&_cam);

    _cam.lookAt(Vector3f(0, 0, 1.0), Vector3f(0, 0, 0), Vector3f(0, 1, 0));
}

void Viewer::reshape(int w, int h){
    _winWidth = w;
    _winHeight = h;
    _cam.setViewport(w,h);
}


/*!
   callback to draw graphic primitives
 */
void Viewer::drawScene()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.5, 0.5, 0.5, 1);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_LINE_SMOOTH);

    _shader.activate();   
    glUniformMatrix4fv(_shader.getUniformLocation("mat_cam"), 1, GL_FALSE, _cam.viewMatrix().data());

    /* SIMPLE DISPLAY */

    Vector3f t(0, 0.5, 0);
    Affine3f A = Translation3f(t)
                * AngleAxisf(_thetaX, Vector3f::UnitX())
                * AngleAxisf(_thetaY, Vector3f::UnitY())
                * AngleAxisf(_thetaZ, Vector3f::UnitZ())
                * Translation3f(-t);

    A(0, 3) += _offset.x();
    A(1, 3) += _offset.y();

    Matrix3f L = _cam.viewMatrix().topLeftCorner(3, 3) * A.matrix().topLeftCorner(3, 3);
    Matrix3f N = (L.inverse()).transpose();

    glUniformMatrix3fv(_shader.getUniformLocation("mat_normal"), 1, GL_FALSE, N.data());
    glUniformMatrix4fv(_shader.getUniformLocation("mat_obj"), 1, GL_FALSE, A.matrix().data());
    _mesh.draw(_shader);
    if(_enableWire == 1){
      glUniform1f(_shader.getUniformLocation("white_filled"), 1);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      _mesh.draw(_shader);
      glUniform1f(_shader.getUniformLocation("white_filled"), 0);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    /* SOLAR SYSTEM */ 
    
    // Earth
    Affine3f AEarth = A;

    _thetaYEarth += 0.02;
    AEarth *= AngleAxisf(_thetaYEarth, Vector3f::UnitY());
    AEarth *= Translation3f(Vector3f(2.0, 0.0, 0.0));
    AEarth *= AngleAxisf(-_thetaYEarth, Vector3f::UnitY());
    _thetaYEarthRotate += 0.1;
    AEarth *= AngleAxisf((23.44 * (M_PI/180.0)), Vector3f::UnitZ());
    AEarth *= AngleAxisf(_thetaYEarthRotate, Vector3f::UnitY());
    AEarth = AEarth * Scaling(Vector3f(0.4, 0.4, 0.4));
    
    Matrix3f LEarth = _cam.viewMatrix().topLeftCorner(3, 3) * AEarth.matrix().topLeftCorner(3, 3);
    Matrix3f NEarth = (LEarth.inverse()).transpose();

    glUniformMatrix3fv(_shader.getUniformLocation("mat_normal"), 1, GL_FALSE, NEarth.data());
    glUniformMatrix4fv(_shader.getUniformLocation("mat_obj"), 1, GL_FALSE, AEarth.matrix().data());
    _mesh.draw(_shader);
    if(_enableWire == 1){
      glUniform1f(_shader.getUniformLocation("white_filled"), 1);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      _mesh.draw(_shader);
      glUniform1f(_shader.getUniformLocation("white_filled"), 0);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Moon
    Affine3f AMoon = A * AngleAxisf(_thetaYEarth, Vector3f::UnitY());
    AMoon *= Translation3f(Vector3f(2.0, 0.0, 0.0));
    _thetaYMoon += 0.2;
    AMoon *= AngleAxisf(_thetaYMoon, Vector3f::UnitY());
    AMoon *= Translation3f(Vector3f(0.75, 0.0, 0.0));

    //AMoon *= AngleAxisf(-(23.44* (M_PI/180.0)), Vector3f::UnitZ());
    AMoon *= AngleAxisf(-_thetaYMoon, Vector3f::UnitY());
    _thetaYMoonRotate += 0.1;
    AMoon *= AngleAxisf(_thetaYMoonRotate, Vector3f::UnitY());
    AMoon *= AngleAxisf((6.68* (M_PI/180.0)), Vector3f::UnitZ());
    AMoon = AMoon * Scaling(Vector3f(0.1, 0.1, 0.1));

    Matrix3f LMoon = _cam.viewMatrix().topLeftCorner(3, 3) * AMoon.matrix().topLeftCorner(3, 3);
    Matrix3f NMoon = (LMoon.inverse()).transpose();
    glUniformMatrix3fv(_shader.getUniformLocation("mat_normal"), 1, GL_FALSE, NMoon.data());
    glUniformMatrix4fv(_shader.getUniformLocation("mat_obj"), 1, GL_FALSE, AMoon.matrix().data());
    _mesh.draw(_shader);
    if(_enableWire == 1){
      glUniform1f(_shader.getUniformLocation("white_filled"), 1);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      _mesh.draw(_shader);
      glUniform1f(_shader.getUniformLocation("white_filled"), 0);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glUniformMatrix4fv(_shader.getUniformLocation("mat_persp"), 1, GL_FALSE, _cam.projectionMatrix().data());
    _shader.deactivate();
}


void Viewer::updateAndDrawScene()
{
    drawScene();
}

void Viewer::loadShaders()
{
    // Here we can load as many shaders as we want, currently we have only one:
    _shader.loadFromFiles(DATA_DIR"/shaders/simple.vert", DATA_DIR"/shaders/simple.frag");
    checkError();
}

////////////////////////////////////////////////////////////////////////////////
// Events

/*!
   callback to manage keyboard interactions
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::keyPressed(int key, int action, int /*mods*/)
{
  if(key == GLFW_KEY_R && action == GLFW_PRESS)
  {
    loadShaders();
  }

  if(action == GLFW_PRESS || action == GLFW_REPEAT )
  {
    // Position
    if (key==GLFW_KEY_UP)
    {
       _offset(1) += 0.1;
    }
    else if (key==GLFW_KEY_DOWN)
    {
       _offset(1) -= 0.1;
    }
    else if (key==GLFW_KEY_LEFT)
    {
       _offset(0) -= 0.1;
    }
    else if (key==GLFW_KEY_RIGHT)
    {
       _offset(0) += 0.1;
    }
    // Enable wire mode
    else if (key == GLFW_KEY_Z)
    {
      _enableWire *= -1;
    }
    // Enable multiple viewports
    else if (key == GLFW_KEY_V)
    {
      _enableView *= -1;
    }
    // Rotation
    else if (key == GLFW_KEY_W)
    {
      _thetaX += M_PI/10;
    }
    else if (key == GLFW_KEY_S)
    {
      _thetaX -= M_PI/10;
    }
    else if (key == GLFW_KEY_D)
    {
      _thetaY += M_PI/10;
    }
    else if (key == GLFW_KEY_A)
    {
      _thetaY -= M_PI/10;
    }
    else if (key == GLFW_KEY_Y)
    {
      _thetaZ += M_PI/10;
    }
    else if (key == GLFW_KEY_T)
    {
      _thetaZ -= M_PI/10;
    }
  }
}

/*!
   callback to manage mouse : called when user press or release mouse button
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::mousePressed(GLFWwindow */*window*/, int /*button*/, int action)
{
  if(action == GLFW_PRESS)
  {
      _trackingMode = TM_ROTATE_AROUND;
      _trackball.start();
      _trackball.track(_lastMousePos);
  }
  else if(action == GLFW_RELEASE)
  {
      _trackingMode = TM_NO_TRACK;
  }
}


/*!
   callback to manage mouse : called when user move mouse with button pressed
   You can change in this function the way the user
   interact with the application.
 */
void Viewer::mouseMoved(int x, int y)
{
    if(_trackingMode == TM_ROTATE_AROUND)
    {
        _trackball.track(Vector2i(x,y));
    }

    _lastMousePos = Vector2i(x,y);
}

void Viewer::mouseScroll(double /*x*/, double y)
{
  _cam.zoom(-0.1*y);
}

void Viewer::charPressed(int /*key*/)
{
}
