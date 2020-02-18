#include "viewer.h"
#include "camera.h"

using namespace Eigen;

Viewer::Viewer()
  : _winWidth(0), _winHeight(0), _offset(0., 0.), _enableWire(-1), _enableView(-1), _thetaX(0), _thetaY(0), _thetaZ(0)
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

    if(!_mesh.load(DATA_DIR"/models/sphere.obj")) exit(1);
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


    _shader.activate();    
    Vector3f t(0, 0.5, 0);
    Affine3f A = Translation3f(t)
                 * AngleAxisf(_thetaX, Vector3f::UnitX())
                 * AngleAxisf(_thetaY, Vector3f::UnitY())
                 * AngleAxisf(_thetaZ, Vector3f::UnitZ())
                 * Translation3f(-t);

    A(0, 3) += _offset.x();
    A(1, 3) += _offset.y();

    glUniformMatrix4fv(_shader.getUniformLocation("mat_obj"), 1, GL_FALSE, A.matrix().data());
    glUniformMatrix4fv(_shader.getUniformLocation("mat_cam"), 1, GL_FALSE, _cam.viewMatrix().data());
    glUniformMatrix4fv(_shader.getUniformLocation("mat_persp"), 1, GL_FALSE, _cam.projectionMatrix().data());

    glUniform3f(_shader.getUniformLocation("vtx_light"), -0.5, -0.5, -1.0);
    glUniform3f(_shader.getUniformLocation("vtx_spec"), 1.0, 1.0, 1.0);
    glUniform1f(_shader.getUniformLocation("exponent"), 10);
    glUniform1f(_shader.getUniformLocation("intensity"), 1);

    Matrix3f L = _cam.viewMatrix().topLeftCorner(3, 3) * A.matrix().topLeftCorner(3, 3);
    Matrix3f N = (L.inverse()).transpose();

    glUniformMatrix3fv(_shader.getUniformLocation("mat_normal"), 1, GL_FALSE, N.data());

    /* LEMMING */

    /*
    glUniform1f(_shader.getUniformLocation("is_solar"), 0);
    int i = 1;
    if(_enableView > 0){
      glUniform1f(_shader.getUniformLocation("views"), 1);
      glViewport(-_winWidth/2, 0, _winWidth, _winHeight);
      i = 2;
    } else {
      glUniform1f(_shader.getUniformLocation("views"), 0);
      glViewport(0, 0, _winWidth, _winHeight);
    }
    while(i > 0){
      // Drawing the wire version
      if (_enableWire > 0){
        glEnable(GL_LINE_SMOOTH);
        glUniform1f(_shader.getUniformLocation("white_filled"), 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDepthFunc(GL_LEQUAL);
        _mesh.draw(_shader);
      }
      // Drawing the full model
      glUniform1f(_shader.getUniformLocation("white_filled"), 0);
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      _mesh.draw(_shader);
      glViewport(_winWidth/2, 0, _winWidth, _winHeight);
      i--;
    }
    */

    /* SOLAR SYSTEM */

    glUniform1f(_shader.getUniformLocation("is_solar"), 0);

    // CHANGER LA MATRICE MAT_OBJ POUR LES DEPLACEMENTS
    // Sun
    _mesh.draw(_shader);
    // Earth
    /*A(0, 3) += 0.5;
    A(1, 3) += 0;
    glUniformMatrix4fv(_shader.getUniformLocation("mat_obj"), 1, GL_FALSE, A.matrix().data());
    _mesh.draw(_shader);
    // Moon
    A(0, 3) += 0;
    A(1, 3) += 0.5;
    glUniformMatrix4fv(_shader.getUniformLocation("mat_obj"), 1, GL_FALSE, A.matrix().data());
    _mesh.draw(_shader);*/


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
    else if (key == GLFW_KEY_T)
    {
      _thetaZ += M_PI/10;
    }
    else if (key == GLFW_KEY_R)
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
