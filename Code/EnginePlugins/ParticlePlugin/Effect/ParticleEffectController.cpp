#include <ParticlePlugin/ParticlePluginPCH.h>

#include <ParticlePlugin/Components/ParticleFinisherComponent.h>
#include <ParticlePlugin/Effect/ParticleEffectController.h>
#include <ParticlePlugin/WorldModule/ParticleWorldModule.h>

ezParticleEffectController::ezParticleEffectController()
{
  m_hEffect.Invalidate();
}

ezParticleEffectController::ezParticleEffectController(const ezParticleEffectController& rhs)
{
  m_pModule = rhs.m_pModule;
  m_hEffect = rhs.m_hEffect;
  m_pSharedInstanceOwner = rhs.m_pSharedInstanceOwner;
}

ezParticleEffectController::ezParticleEffectController(ezParticleWorldModule* pModule, ezParticleEffectHandle hEffect)
{
  m_pModule = pModule;
  m_hEffect = hEffect;
}

void ezParticleEffectController::operator=(const ezParticleEffectController& rhs)
{
  m_pModule = rhs.m_pModule;
  m_hEffect = rhs.m_hEffect;
  m_pSharedInstanceOwner = rhs.m_pSharedInstanceOwner;
}

ezParticleEffectInstance* ezParticleEffectController::GetInstance() const
{
  if (m_pModule == nullptr)
    return nullptr;

  ezParticleEffectInstance* pEffect = nullptr;
  m_pModule->TryGetEffectInstance(m_hEffect, pEffect);
  return pEffect;
}

void ezParticleEffectController::Create(const ezParticleEffectResourceHandle& hEffectResource, ezParticleWorldModule* pModule, ezUInt64 uiRandomSeed,
  const char* szSharedName, const void* pSharedInstanceOwner, ezArrayPtr<ezParticleEffectFloatParam> floatParams,
  ezArrayPtr<ezParticleEffectColorParam> colorParams)
{
  m_pSharedInstanceOwner = pSharedInstanceOwner;

  // first get the new effect, to potentially increase a refcount to the same effect instance, before we decrease the refcount of our
  // current one
  ezParticleEffectHandle hNewEffect;
  if (pModule != nullptr && hEffectResource.IsValid())
  {
    hNewEffect = pModule->CreateEffectInstance(hEffectResource, uiRandomSeed, szSharedName, m_pSharedInstanceOwner, floatParams, colorParams);
  }

  Invalidate();

  m_hEffect = hNewEffect;

  if (!m_hEffect.IsInvalidated())
    m_pModule = pModule;
}

bool ezParticleEffectController::IsValid() const
{
  return (m_pModule != nullptr && !m_hEffect.IsInvalidated());
}

bool ezParticleEffectController::IsAlive() const
{
  ezParticleEffectInstance* pEffect = GetInstance();
  return pEffect != nullptr;
}

void ezParticleEffectController::SetTransform(const ezTransform& t, const ezVec3& vParticleStartVelocity) const
{
  ezParticleEffectInstance* pEffect = GetInstance();

  // shared effects are always simulated at the origin
  if (pEffect && m_pSharedInstanceOwner == nullptr)
  {
    pEffect->SetTransform(t, vParticleStartVelocity);
  }
}

void ezParticleEffectController::CombineSystemBoundingVolumes()
{
  if (ezParticleEffectInstance* pEffect = GetInstance())
  {
    pEffect->CombineSystemBoundingVolumes();
  }
}

void ezParticleEffectController::Tick(const ezTime& diff) const
{
  ezParticleEffectInstance* pEffect = GetInstance();

  if (pEffect)
  {
    pEffect->PreSimulate();
    pEffect->Update(diff);
  }
}

void ezParticleEffectController::ExtractRenderData(ezMsgExtractRenderData& ref_msg, const ezTransform& systemTransform) const
{
  if (const ezParticleEffectInstance* pEffect = GetInstance())
  {
    pEffect->SetIsVisible();

    m_pModule->ExtractEffectRenderData(pEffect, ref_msg, systemTransform);
  }
}

void ezParticleEffectController::StopImmediate()
{
  if (m_pModule)
  {
    m_pModule->DestroyEffectInstance(m_hEffect, true, m_pSharedInstanceOwner);

    m_pModule = nullptr;
    m_hEffect.Invalidate();
  }
}

void ezParticleEffectController::GetBoundingVolume(ezBoundingBoxSphere& ref_volume) const
{
  if (ezParticleEffectInstance* pEffect = GetInstance())
  {
    pEffect->GetBoundingVolume(ref_volume);
  }
}

void ezParticleEffectController::UpdateWindSamples(ezTime diff)
{
  if (ezParticleEffectInstance* pEffect = GetInstance())
  {
    pEffect->UpdateWindSamples(diff);
  }
}

void ezParticleEffectController::ForceVisible()
{
  if (ezParticleEffectInstance* pEffect = GetInstance())
  {
    pEffect->SetIsVisible();
  }
}

ezUInt64 ezParticleEffectController::GetNumActiveParticles() const
{
  if (ezParticleEffectInstance* pEffect = GetInstance())
  {
    return pEffect->GetNumActiveParticles();
  }

  return 0;
}

void ezParticleEffectController::SetParameter(const ezTempHashedString& sName, float value)
{
  ezParticleEffectInstance* pEffect = GetInstance();

  if (pEffect)
  {
    pEffect->SetParameter(sName, value);
  }
}

void ezParticleEffectController::SetParameter(const ezTempHashedString& sName, const ezColor& value)
{
  ezParticleEffectInstance* pEffect = GetInstance();

  if (pEffect)
  {
    pEffect->SetParameter(sName, value);
  }
}

void ezParticleEffectController::Invalidate()
{
  if (m_pModule)
  {
    m_pModule->DestroyEffectInstance(m_hEffect, false, m_pSharedInstanceOwner);

    m_pModule = nullptr;
    m_hEffect.Invalidate();
  }
}
