
#include "camera.h"

using namespace Eigen;

Camera::Camera()
{
  mViewMatrix.setIdentity();
  setPerspective(M_PI/2,0.1,10000);
  setViewport(0,0);
}

void Camera::lookAt(const Vector3f& position, const Vector3f& target, const Vector3f& up)
{
  mTarget = target;

  Vector3f zc = (position - target).normalized();
  Vector3f xc = (up.cross(zc)).normalized();
  Vector3f yc = zc.cross(xc);

  Matrix4f mc;
  mc << xc, yc, zc, position,
         0,  0,  0,        1;

  mViewMatrix = mc.inverse();
}

void Camera::setPerspective(float fovY, float near, float far)
{
  m_fovY = fovY;
  m_near = near;
  m_far = far;
}

void Camera::setViewport(int width, int height)
{
  mVpWidth = width;
  mVpHeight = height;
}

void Camera::zoom(float x)
{
  Vector3f t = Affine3f(mViewMatrix) * mTarget;
  mViewMatrix = Affine3f(Translation3f(Vector3f(0,0,x*t.norm())).inverse()) * mViewMatrix;
}

void Camera::rotateAroundTarget(float angle, Vector3f axis)
{
  Affine3f a = Translation3f(mTarget)
                   * AngleAxisf(angle, axis)
                   * Translation3f(-mTarget);

  Matrix4f mat;
  mat << a(0, 0), a(0, 1), a(0, 2), a(0, 3),
         a(1, 0), a(1, 1), a(1, 2), a(1, 3),
         a(2, 0), a(2, 1), a(2, 2), a(2, 3),
         a(3, 0), a(3, 1), a(3, 2), a(3, 3);
  mViewMatrix = mat;
}

Camera::~Camera()
{
}

const Matrix4f& Camera::viewMatrix() const
{
  return mViewMatrix;
}

Matrix4f Camera::projectionMatrix() const
{
  float aspect = float(mVpWidth)/float(mVpHeight);
  float theta = m_fovY*0.5;
  float range = m_far - m_near;
  float invtan = 1./std::tan(theta);

  Matrix4f projMat;
  projMat.setZero();
  projMat(0,0) = invtan / aspect;
  projMat(1,1) = invtan;
  projMat(2,2) = -(m_near + m_far) / range;
  projMat(2,3) = -2 * m_near * m_far / range;
  projMat(3,2) = -1;

  return projMat;
}
