#include <RendererCore/RendererCorePCH.h>

#include <Core/WorldSerializer/WorldReader.h>
#include <Core/WorldSerializer/WorldWriter.h>
#include <RendererCore/Lights/Implementation/ShadowPool.h>
#include <RendererCore/Lights/PointLightComponent.h>
#include <RendererCore/Pipeline/View.h>

// clang-format off
EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezPointLightRenderData, 1, ezRTTIDefaultAllocator<ezPointLightRenderData>)
EZ_END_DYNAMIC_REFLECTED_TYPE;

EZ_BEGIN_COMPONENT_TYPE(ezPointLightComponent, 3, ezComponentMode::Static)
{
  EZ_BEGIN_PROPERTIES
  {
    EZ_ACCESSOR_PROPERTY("Range", GetRange, SetRange)->AddAttributes(new ezClampValueAttribute(0.0f, ezVariant()), new ezSuffixAttribute(" m"), new ezMinValueTextAttribute("Auto")),
    EZ_ACCESSOR_PROPERTY("ShadowFadeOutRange", GetShadowFadeOutRange, SetShadowFadeOutRange)->AddAttributes(new ezClampValueAttribute(0.0f, ezVariant()), new ezSuffixAttribute(" m"), new ezMinValueTextAttribute("Auto")),
    //EZ_ACCESSOR_PROPERTY("ProjectedTexture", GetProjectedTextureFile, SetProjectedTextureFile)->AddAttributes(new ezAssetBrowserAttribute("CompatibleAsset_Texture_Cube")),
  }
  EZ_END_PROPERTIES;
  EZ_BEGIN_MESSAGEHANDLERS
  {
    EZ_MESSAGE_HANDLER(ezMsgExtractRenderData, OnMsgExtractRenderData),
  }
  EZ_END_MESSAGEHANDLERS;
  EZ_BEGIN_ATTRIBUTES
  {
    new ezSphereManipulatorAttribute("Range"),
    new ezPointLightVisualizerAttribute("Range", "Intensity", "LightColor"),
  }
  EZ_END_ATTRIBUTES;
}
EZ_END_COMPONENT_TYPE
// clang-format on

ezPointLightComponent::ezPointLightComponent() = default;
ezPointLightComponent::~ezPointLightComponent() = default;

ezResult ezPointLightComponent::GetLocalBounds(ezBoundingBoxSphere& ref_bounds, bool& ref_bAlwaysVisible, ezMsgUpdateLocalBounds& ref_msg)
{
  m_fEffectiveRange = CalculateEffectiveRange(m_fRange, m_fIntensity);

  ref_bounds = ezBoundingSphere::MakeFromCenterAndRadius(ezVec3::MakeZero(), m_fEffectiveRange);
  return EZ_SUCCESS;
}

void ezPointLightComponent::SetRange(float fRange)
{
  m_fRange = ezMath::Max(fRange, 0.0f);

  TriggerLocalBoundsUpdate();
}

float ezPointLightComponent::GetRange() const
{
  return m_fRange;
}

float ezPointLightComponent::GetEffectiveRange() const
{
  return m_fEffectiveRange;
}

void ezPointLightComponent::SetShadowFadeOutRange(float fRange)
{
  m_fShadowFadeOutRange = ezMath::Max(fRange, 0.0f);

  InvalidateCachedRenderData();
}

float ezPointLightComponent::GetShadowFadeOutRange() const
{
  return m_fShadowFadeOutRange;
}

// void ezPointLightComponent::SetProjectedTexture(const ezTextureCubeResourceHandle& hProjectedTexture)
//{
//   m_hProjectedTexture = hProjectedTexture;
//
//   InvalidateCachedRenderData();
// }
//
// const ezTextureCubeResourceHandle& ezPointLightComponent::GetProjectedTexture() const
//{
//   return m_hProjectedTexture;
// }

// void ezPointLightComponent::SetProjectedTextureFile(const char* szFile)
//{
//   ezTextureCubeResourceHandle hProjectedTexture;
//
//   if (!ezStringUtils::IsNullOrEmpty(szFile))
//   {
//     hProjectedTexture = ezResourceManager::LoadResource<ezTextureCubeResource>(szFile);
//   }
//
//   SetProjectedTexture(hProjectedTexture);
// }
//
// const char* ezPointLightComponent::GetProjectedTextureFile() const
//{
//   if (!m_hProjectedTexture.IsValid())
//     return "";
//
//   return m_hProjectedTexture.GetResourceID();
// }

void ezPointLightComponent::OnMsgExtractRenderData(ezMsgExtractRenderData& msg) const
{
  // Don't extract light render data for selection or in shadow views.
  if (msg.m_OverrideCategory != ezInvalidRenderDataCategory || msg.m_pView->GetCameraUsageHint() == ezCameraUsageHint::Shadow)
    return;

  if (m_fIntensity <= 0.0f || m_fEffectiveRange <= 0.0f)
    return;

  const ezTransform t = GetOwner()->GetGlobalTransform();
  const ezBoundingSphere bounds = ezBoundingSphere::MakeFromCenterAndRadius(t.m_vPosition, m_fEffectiveRange);
  const float fScreenSpaceSize = CalculateScreenSpaceSize(bounds, *msg.m_pView->GetCullingCamera());
  float fShadowScreenSize = 0.0f;
  const float fShadowFadeOut = CalculateShadowFadeOut(bounds, m_fShadowFadeOutRange, *msg.m_pView->GetCullingCamera(), fShadowScreenSize);

#if EZ_ENABLED(EZ_COMPILE_FOR_DEVELOPMENT)
  VisualizeScreenSpaceSize(msg.m_pView->GetHandle(), bounds, fScreenSpaceSize, fShadowScreenSize, fShadowFadeOut);
#endif

  auto pRenderData = ezCreateRenderDataForThisFrame<ezPointLightRenderData>(GetOwner());

  pRenderData->m_GlobalTransform = t;
  pRenderData->m_LightColor = GetEffectiveColor();
  pRenderData->m_fIntensity = m_fIntensity;
  pRenderData->m_fSpecularMultiplier = m_fSpecularMultiplier;
  pRenderData->m_fRange = m_fEffectiveRange;
  // pRenderData->m_hProjectedTexture = m_hProjectedTexture;

  if (m_bCastShadows && fShadowFadeOut > 0.0f)
  {
    pRenderData->FillShadowDataOffsetAndFadeOut(ezShadowPool::AddPointLight(this, fScreenSpaceSize, msg.m_pView), fShadowFadeOut);
  }
  else
  {
    pRenderData->m_uiShadowDataOffsetAndFadeOut = 0;
  }

  pRenderData->FillBatchIdAndSortingKey(fScreenSpaceSize);

  ezRenderData::Caching::Enum caching = m_bCastShadows ? ezRenderData::Caching::Never : ezRenderData::Caching::IfStatic;
  msg.AddRenderData(pRenderData, ezDefaultRenderDataCategories::Light, caching);
}

void ezPointLightComponent::SerializeComponent(ezWorldWriter& inout_stream) const
{
  SUPER::SerializeComponent(inout_stream);

  ezStreamWriter& s = inout_stream.GetStream();

  ezTextureCubeResourceHandle m_hProjectedTexture;

  s << m_fRange;
  s << m_fShadowFadeOutRange;
  s << m_hProjectedTexture;
}

void ezPointLightComponent::DeserializeComponent(ezWorldReader& inout_stream)
{
  SUPER::DeserializeComponent(inout_stream);
  const ezUInt32 uiVersion = inout_stream.GetComponentTypeVersion(GetStaticRTTI());
  ezStreamReader& s = inout_stream.GetStream();

  ezTextureCubeResourceHandle m_hProjectedTexture;

  s >> m_fRange;
  if (uiVersion >= 3)
  {
    s >> m_fShadowFadeOutRange;
  }
  s >> m_hProjectedTexture;
}

//////////////////////////////////////////////////////////////////////////

EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezPointLightVisualizerAttribute, 1, ezRTTIDefaultAllocator<ezPointLightVisualizerAttribute>)
EZ_END_DYNAMIC_REFLECTED_TYPE;

ezPointLightVisualizerAttribute::ezPointLightVisualizerAttribute()
  : ezVisualizerAttribute(nullptr)
{
}

ezPointLightVisualizerAttribute::ezPointLightVisualizerAttribute(
  const char* szRangeProperty, const char* szIntensityProperty, const char* szColorProperty)
  : ezVisualizerAttribute(szRangeProperty, szIntensityProperty, szColorProperty)
{
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include <Foundation/Serialization/AbstractObjectGraph.h>
#include <Foundation/Serialization/GraphPatch.h>

class ezPointLightComponentPatch_1_2 : public ezGraphPatch
{
public:
  ezPointLightComponentPatch_1_2()
    : ezGraphPatch("ezPointLightComponent", 2)
  {
  }

  virtual void Patch(ezGraphPatchContext& ref_context, ezAbstractObjectGraph* pGraph, ezAbstractObjectNode* pNode) const override
  {
    ref_context.PatchBaseClass("ezLightComponent", 2, true);
  }
};

ezPointLightComponentPatch_1_2 g_ezPointLightComponentPatch_1_2;

EZ_STATICLINK_FILE(RendererCore, RendererCore_Lights_Implementation_PointLightComponent);
