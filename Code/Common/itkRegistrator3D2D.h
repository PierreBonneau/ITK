/*==========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkRegistrator3D2D.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) 2000 National Library of Medicine
  All rights reserved.

  See COPYRIGHT.txt for copyright details.
  
==========================================================================*/

/**
 *  Registrator3D2D performs 3D rigid registration 
 *  between a set of 3D points and a set of 2D points.
 *  
 *  Each 2D point is associated with an unique point in the 3D set. 
 *  The 2D point and is assumed to be the projection of the 3D one
 *  after and unkown 3D rigid transformation. The projections parameters
 *  should be provided encapsulated in a 'intrinsic' transformation.
 *  An extrinsic tranformation accounts for ridig movements of the 3D set
 *  of points before the projection.
 *
 *  The method should return a 3D rigid transformation that after 
 *  projection maps the the set of 3D point on top of the 2D set.
 *  
 *  This method was part of a registration method developed by Alan Liu 
 *  during his Ph.D. Thesis at the University of North Carolina at Chapel Hill
 *  with the advisory of Elizabeth Bullit, MD.  The code was modified and
 *  adapted for Insight toolkit by Luis Ibanez.
 *
 *  liu@cs.unc.edu
 *  bullitt@med.unc.edu
 *  ibanez@cs.unc.edu
 */
 
#ifndef __itkRegistrator3D2D_h
#define __itkRegistrator3D2D_h

#include <vector>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <itkExceptionObject.h>


class itkRegistrator3D2D  
{

public:  
  /**
   *  Type used internally to represent a homogeneous transformation in 3D
   *  space. It is basically a 4x4 matrix but any internal representaition is
   *  valid as long as it provides an interface for composing (concatenate)
   *  transformation, and for transforming 3D points and 3D vectors.
   */ 
  typedef vnl_matrix_fixed<float,4,4> Transform3D;

  /**
   *  Type used to define the 3D point class
   */
  typedef vnl_vector_fixed<float,3> Point3D;

  /**
   *  Type used to define the 3D vector class
   */
  typedef vnl_vector_fixed<float,3> Vector3D;

  /**
   *  Type used to define the 2D point class
   */
  typedef vnl_vector_fixed<float,2> Point2D;
  
  /**
   *   PairPoint3D2D  groups a 3D point with a 2D point.
   *
   *   This class is intended to provide an efficient
   *   support for passing data to a 3D/2D registration method
   *   \sa Point2D
   *   \sa Point3D
   */
  class PairPoint3D2D
  {
    Point2D p2;
    Point3D p3;
  public:
    PairPoint3D2D() { }
    PairPoint3D2D(const Point2D & ip2, const Point3D & ip3 ) 
    { 
      p2=ip2; p3=ip3; 
    }
    const Point2D & GetPoint2D(void) const { return p2; }
    const Point3D & GetPoint3D(void) const { return p3; }

  };



private:  
  /** 
   * 3D rigid transformation representing the current position of 3D points
   * in space.
   *
   * \sa cumulatedCorrectionMatrix
   * \sa intrinsicTransform
   *
   */
  Transform3D	extrinsicTransform;	     


  /** 
   * This matrix contains the cumulated corrections generated by the 
   * successive iterations of the registration process. At the end of the
   * registration this matrix is the transformation that should be applied
   * to the 3D points in order to be mapped on top of their associated 2D
   * points
   *
   * \sa extrinsicTransform
   * \sa intrinsicTransform
   *
   */
  Transform3D cumulatedCorrectionTransform;

  
  /**
   * This transformation matrix contains the parameters of the projection it
   * includes the focal distance and the viewing area
   *
   * \sa extrinsicTransform
   * \sa cumulatedCorrectionMatrix
   *
   */ 
  Transform3D	intrinsicTransform;	      


  /**
   * MeanSquareError mesaures the mean square error of the registration
   * it is computed in 2D as the difference between a 2D point and the
   * the projection of its associated 3D point
   */
  double		MeanSquareError;


  /** 
   * When the registration MeanSquareError is lower than the Tolerance
   * the registration is considered to be successful and the process stops
   *
   * \sa MeanSquareError
   * \sa inTolerance
   *
   */
  double		Tolerance;                

  /** 
   * When the registration MeanSquareError is higer than the inTolerance
   * the registration is considered to be successful and the process stops
   *
   * \sa MeanSquareError
   * \sa Tolerance
   *
   */
  double		InTolerance;


  /**
   * Potential is an alternative measure to the MeanSquareError. The
   * MeanSquareError is insensitive to the number of points. The Potential 
   * is the sum   -1/(1+dist^2). It is decreasing with the number of points
   * as well as with the closer they are.
   * \sa potentialRange
   */
  double    Potential;

  /**
   * Potential range is a scale factor that defines the width of the potential 
   * well around a point. It is useful for regulating the extent to which a
   * point is considered to be part of a curve.
   *
   * \sa Potential
   */
  double    potentialRange;


  /**
   * Lambda coefficient of the Levenger Marquart modified Least Square Method
   * a value of Lambda zero is equivalent to a Newton root finder.
   * other values of lambda can be related to the step size in a steppest
   * descent method.
   *
   */
  double    lambda;

  /**
   * This boolean is set whenever a test condition indicates that the 
   * registration iteration should be stopped. It can be because the
   * registration has succed under the current Tolerance or because it
   * has exced the inTolerance or the maximum number of iterations.
   * it can also be set by a external event (like user interaction) 
   * through the method StopRegistration()
   *
   * \sa StopRegistration
   * \sa Tolerance
   * \sa inTolerance
   * \sa maxNumberOfIterations
   *
   */   
  bool      stopRegistration;


  /** 
   * Maximum number of iterations to run the registration process
   * if the number is exceded the registration is stopped.
   *
   * \sa stopRegistration
   *
   */
  unsigned int maxNumberOfIterations;
  
  /** 
   * Real number of iterations used in the registration process
   *
   * \sa maxNumberOfIterations
   *
   */
  unsigned int numberOfIterations;

  /** 
   * Differential correction of the pose (extrinsic) parameter. This six
   * dimensional vector contains the translation along the orthognal axis and
   * rotation aroud them.
   *
   */
	vnl_vector_fixed<double,6> Delta;

  /** 
   * Uncertainty in the values corresponding to pose parameters.
   * This values are related with the step size used in the iterative
   * method to look for a new extrinsic parameter for registration.
   * Big uncertainties lead to small steps.
   *
   * \sa Delta
   *
   */
	vnl_vector_fixed<double,6> Uncertain;

  /** 
   * The Jacobian contains the dependencies between the registration
   * parameters and the 2D coordinates of points
   *
   */
  vnl_matrix<double> * Jacobian; 
  

  /** 
   * PlanarError is the vector containing the differences of 2D coordinates
   * between the 2D points and the projections of 3D points.
   *
   */
  vnl_vector<double> * PlanarError; 
 


private: 
  /**
   * Set of associated pairs of 3D/2D points to register 
   * 
   * \sa PairPoint3D2D
   *
   */
  std::vector<PairPoint3D2D>  AssociatedPoints;


public:

  /**
   * Constructor of a registrator object
   */
  itkRegistrator3D2D();


  /**
   * Destructor of a registrator object
   */
  ~itkRegistrator3D2D();

  /**
   * Initialize the registration process
   */
  void PerformRegistration( void );

  /**
   * This method computes the Jacobian that relates the 2D coordinates of a 3D
   * point projection with the parameters of a rigid transformation. These
   * parameteres are represented as translations along the 3 orthogonal axis
   * and rotations around the 3 orthogonal axis. 
   */
  void ComputeJacobian(void);

  /**
   * This method set a bool flag that stops the iterative process of
   * registration. 
   */
  void StopRegistration( void );

  /**
   * Set the tolerance on the 2D coordinates error. When the MeanSquareError
   * is under this value, the registration stops.
   * \sa Tolerance
   * \sa InTolerance
   */
  void SetTolerance( double );


  /**
   * Set the intolerance on the 2D coordinates error. When the MeanSquareError
   * is over this value, the registration fails. This is intended to avoid
   * pursuing a registration that for some reason is not converging.
   * \sa InTolerance
   * \sa Tolerance
   */
  void SetInTolerance( double );


  /** 
   * Set the value of the range for computing a potential.
   * \sa potentialRange
   */
  void SetPotentialRange( double );


  /**
   * Defines the maximum number of iterations that the method sould run before
   * considering to have failed
   * \sa maxNumberOfIterations
   */
  void SetMaximumNumberOfIterations( unsigned int );


  /**
   * Set the extrinsic rigid transformation on the set of 3D points
   * \sa extrinsicTransform
   */  
  void SetExtrinsicTransform( const Transform3D & );


  /**
   * Set the intrinsic transformation on the set of 3D points. This
   * transformation is expected to contain the projection parameters, 
   * but in practice any transformation can be used.
   * \sa intrinsicTransform
   */  
  void SetIntrinsicTransform( const Transform3D & );


  /**
   * This method set the uncertainty in the translation correction needed for
   * registration. The value is used for stabilization of the
   * Levenberg-Marquard method used to solve the least square estimation of the
   * registration parameters
   */ 
  void SetTranslationUncertainty( const Vector3D & variance);

  /**
   * This method set the uncertainty in the rotation correction needed for
   * registration. The value is used for stabilization of the
   * Levenberg-Marquard method used to solve the least square estimation of the
   * registration parameters
   */ 
  void SetRotationUncertainty( const Vector3D & variance);

  /**
   * This method returns the Mean Square Error computed between the coordinates
   * of the 2D points and the projections of the 3D points. This value is
   * dependent on the scale used for the particular set of points.
   * \sa MeanSquareError
   */  
  double GetMeanSquareError( void ) const;

  /**
   * This method returns the potential computed over the differences between
   * the 2D coordinates of the 2D points and the projection coordinates of the
   * 3D points. The value is dependent on the scale used to represent the
   * points.
   * \sa potential
   */
  double GetPotential( void ) const;


  /**
   * Returns the range used to compute the potential. It defines the range of 
   * influence of a point distance when considering how well the projections of
   * 3D points fit the set of 2D points.
   * \sa potentialRange
   */
  double GetPotentialRange( void ) const;

  /**
   * Returns the number of iteration performed so far. This is intended to be
   * used in interactive user interfaces.
   * \sa numberOfIterations
   */
  unsigned int GetNumberOfIterationsPerformed( void ) const;

  /**
   * Returns the rigid transformation that should be applied to the set of 3D
   * points to register their projections with the set of 2D points.
   * \sa Delta
   */
  const Transform3D & GetTransformation( void ) const;


  /**
   * Set the list of associated pairs of 3D point and 2D points.
   * \sa AssociatedPoints
   */
  void LoadAssociatedPoints( const vector<PairPoint3D2D> & externalList );

  /**
   * Set cumulatedCorrectionMatrix to an identity matrix. This method is useful when
   * the registration doesn't converges and needs to be reinitialized
   */ 
  void ResetRegistration( void );


private:  
  /**
   * Allocate memory for internal arrays containg data used for solving the
   * least square problem
   */
  void Allocate(void);

  /**
   * This method solve the Least Square Problem using Levenberg-Marquard
   * method.
   */
  void LeastSquareSolution(void); 



};




/**
 *
 *  itkRegistrator3D2DException manages the errors produced during the 
 *  registration process. It derives from itkExeptionObject.
 *  
 */
class itkRegistrator3D2DException : public itkExceptionObject {
public:

  /**
   * Creator of a Registration Exception. The message should contain a brief
   * descrption of the problem.
   * \sa itkExeptionObject
   */
	itkRegistrator3D2DException(const char * message) 
  {
    SetLocation("itkRegistrator3D2D");
    SetDescription(message);
  }


  /**
   * Virtual destructor of the exception class. Does nothing.
   */
  virtual ~itkRegistrator3D2DException() 
  {
  }


};




#endif



