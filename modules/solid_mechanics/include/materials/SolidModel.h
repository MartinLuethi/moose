#ifndef SOLIDMODEL_H
#define SOLIDMODEL_H

#include "Material.h"

#include "SymmTensor.h"

// Forward declarations
class SolidModel;
class SymmElasticityTensor;
class VolumetricModel;
namespace Elk
{
namespace SolidMechanics
{
class Element;
}
}


template<>
InputParameters validParams<SolidModel>();

/**
 * SolidModel is the base class for all solid mechanics material models in Elk.
 */
class SolidModel : public Material
{
public:
  SolidModel( const std::string & name,
              InputParameters parameters );
  virtual ~SolidModel();

  static void rotateSymmetricTensor( const ColumnMajorMatrix & R, const SymmTensor & T,
                                     SymmTensor & result );

protected:

  bool _bulk_modulus_set;
  bool _lambda_set;
  bool _poissons_ratio_set;
  bool _shear_modulus_set;
  bool _youngs_modulus_set;

  Real _bulk_modulus;
  Real _lambda;
  Real _poissons_ratio;
  Real _shear_modulus;
  Real _youngs_modulus;

  const Real _cracking_stress;
  std::vector<unsigned int> _active_crack_planes;
  const unsigned int _max_cracks;

  const bool _has_temp;
  VariableValue & _temperature;
  VariableValue & _temperature_old;
  const Real _alpha;

  std::vector<VolumetricModel*> _volumetric_models;

  MaterialProperty<SymmTensor> & _stress;
private:
  MaterialProperty<SymmTensor> & _stress_old_prop;
protected:
  SymmTensor _stress_old;

  MaterialProperty<SymmTensor> & _total_strain;
  MaterialProperty<SymmTensor> & _total_strain_old;

  MaterialProperty<SymmTensor> & _elastic_strain;
  MaterialProperty<SymmTensor> & _elastic_strain_old;

  MaterialProperty<RealVectorValue> * _crack_flags;
  MaterialProperty<RealVectorValue> * _crack_flags_old;
  RealVectorValue _crack_flags_local;
  MaterialProperty<ColumnMajorMatrix> * _crack_rotation;
  MaterialProperty<ColumnMajorMatrix> * _crack_rotation_old;

  MaterialProperty<SymmElasticityTensor> & _elasticity_tensor;
  MaterialProperty<SymmElasticityTensor> & _Jacobian_mult;

  // Accumulate derivatives of strain tensors with respect to Temperature into this
  SymmTensor _d_strain_dT;

  // The derivative of the stress with respect to Temperature
  MaterialProperty<SymmTensor> & _d_stress_dT;

  SymmTensor _total_strain_increment;
  SymmTensor _strain_increment;


  virtual void initialSetup();

  virtual void computeProperties();

  virtual void elementInit() {}

  /// Modify increment for things like thermal strain
  virtual void modifyStrainIncrement();

  /// Determine cracking directions.  Rotate elasticity tensor.
  virtual void crackingStrainDirections();

  virtual unsigned int getNumKnownCrackDirs() const;

  /// Compute the stress (sigma += deltaSigma)
  virtual void computeStress() = 0;

  /*
   * Determine whether new cracks have formed.
   * Rotate old and new stress to global, if cracking active
   */
  virtual void crackingStressRotation();

  /// Rotate stress to current configuration
  virtual void finalizeStress();


  virtual void computePreconditioning();

  void applyCracksToTensor( SymmTensor & tensor );

  void
  elasticityTensor( SymmElasticityTensor * e );

  SymmElasticityTensor *
  elasticityTensor() const
  {
    return _local_elasticity_tensor;
  }

  const Elk::SolidMechanics::Element * element() const
  {
    return _element;
  }

  int delta(int i, int j) const
  {
    return i == j;
  }

private:

  void computeCrackStrainAndOrientation( ColumnMajorMatrix & principal_strain );

  Elk::SolidMechanics::Element * createElement( const std::string & name,
                                                InputParameters & parameters );

  void createElasticityTensor();

  Elk::SolidMechanics::Element * _element;

  SymmElasticityTensor * _local_elasticity_tensor;

};



#endif
