#pragma once

#include <Foundation/Strings/HashedString.h>
#include <Foundation/Threading/DelegateTask.h>
#include <Foundation/Types/SharedPtr.h>
#include <Foundation/Types/TagSet.h>
#include <RendererCore/Pipeline/RenderPipeline.h>
#include <RendererCore/Pipeline/RenderPipelineNode.h>
#include <RendererCore/Pipeline/RenderPipelineResource.h>
#include <RendererCore/Pipeline/ViewData.h>
#include <RendererFoundation/Device/SwapChain.h>
#include <RendererFoundation/Resources/RenderTargetSetup.h>

class ezFrustum;
class ezWorld;
class ezRenderPipeline;

/// \brief Encapsulates a view on the given world through the given camera
/// and rendered with the specified RenderPipeline into the given render target setup.
class EZ_RENDERERCORE_DLL ezView : public ezRenderPipelineNode
{
  EZ_ADD_DYNAMIC_REFLECTION(ezView, ezRenderPipelineNode);

private:
  /// \brief Use ezRenderLoop::CreateView to create a view.
  ezView();
  ~ezView();

public:
  ezViewHandle GetHandle() const;

  void SetName(ezStringView sName);
  ezStringView GetName() const;

  void SetWorld(ezWorld* pWorld);
  ezWorld* GetWorld();
  const ezWorld* GetWorld() const;

  /// \brief Sets the swapchain that this view will be rendering into. Can be invalid in case the render target is an off-screen buffer in which case SetRenderTargets needs to be called.
  /// Setting the swap-chain is necessary in order to acquire and present the image to the window.
  /// SetSwapChain and SetRenderTargets are mutually exclusive. Calling this function will reset the render targets.
  void SetSwapChain(ezGALSwapChainHandle hSwapChain);
  ezGALSwapChainHandle GetSwapChain() const;

  /// \brief Sets the off-screen render targets. Use SetSwapChain if rendering to a window.
  /// SetSwapChain and SetRenderTargets are mutually exclusive. Calling this function will reset the swap chain.
  void SetRenderTargets(const ezGALRenderTargets& renderTargets);
  const ezGALRenderTargets& GetRenderTargets() const;

  /// \brief Returns the render targets that were either set via the swapchain or via the manually set render targets.
  const ezGALRenderTargets& GetActiveRenderTargets() const;

  void SetRenderPipelineResource(ezRenderPipelineResourceHandle hPipeline);
  ezRenderPipelineResourceHandle GetRenderPipelineResource() const;

  void SetCamera(ezCamera* pCamera);
  ezCamera* GetCamera();
  const ezCamera* GetCamera() const;

  void SetCullingCamera(const ezCamera* pCamera);
  const ezCamera* GetCullingCamera() const;

  void SetLodCamera(const ezCamera* pCamera);
  const ezCamera* GetLodCamera() const;

  /// \brief Returns the camera usage hint for the view.
  ezEnum<ezCameraUsageHint> GetCameraUsageHint() const;
  /// \brief Sets the camera usage hint for the view. If not 'None', the camera component of the same usage will be auto-connected
  ///   to this view.
  void SetCameraUsageHint(ezEnum<ezCameraUsageHint> val);

  void SetViewRenderMode(ezEnum<ezViewRenderMode> value);
  ezEnum<ezViewRenderMode> GetViewRenderMode() const;

  void SetViewport(const ezRectFloat& viewport);
  const ezRectFloat& GetViewport() const;

  /// \brief Forces the render pipeline to be rebuilt.
  void ForceUpdate();

  const ezViewData& GetData() const;

  bool IsValid() const;

  /// \brief Extracts all relevant data from the world to render the view.
  void ExtractData();

  /// \brief Returns a task implementation that calls ExtractData on this view.
  const ezSharedPtr<ezTask>& GetExtractTask();


  /// \brief Calculates the start position and direction (in world space) of the picking ray through the screen position in this view.
  ///
  /// fNormalizedScreenPosX and fNormalizedScreenPosY are expected to be in [0; 1] range (normalized screen coordinates).
  /// If no ray can be computed, EZ_FAILURE is returned.
  ezResult ComputePickingRay(float fNormalizedScreenPosX, float fNormalizedScreenPosY, ezVec3& out_vRayStartPos, ezVec3& out_vRayDir) const;

  /// \brief Calculates the normalized screen-space coordinate ([0; 1] range) that the given world-space point projects to.
  ///
  /// Returns EZ_FAILURE, if the point could not be projected into screen-space.
  ezResult ComputeScreenSpacePos(const ezVec3& vWorldPos, ezVec3& out_vScreenPosNormalized) const;

  /// \brief Calculates the world-space position that the given normalized screen-space coordinate maps to
  ezResult ComputeWorldSpacePos(float fNormalizedScreenPosX, float fNormalizedScreenPosY, ezVec3& out_vWorldPos) const;

  /// \brief Converts a screen-space position from pixel coordinates to normalized coordinates.
  void ConvertScreenPixelPosToNormalizedPos(ezVec3& inout_vPixelPos);

  /// \brief Converts a screen-space position from normalized coordinates to pixel coordinates.
  void ConvertScreenNormalizedPosToPixelPos(ezVec3& inout_vNormalizedPos);


  /// \brief Returns the current projection matrix.
  const ezMat4& GetProjectionMatrix(ezCameraEye eye) const;

  /// \brief Returns the current inverse projection matrix.
  const ezMat4& GetInverseProjectionMatrix(ezCameraEye eye) const;

  /// \brief Returns the current view matrix (camera orientation).
  const ezMat4& GetViewMatrix(ezCameraEye eye) const;

  /// \brief Returns the current inverse view matrix (inverse camera orientation).
  const ezMat4& GetInverseViewMatrix(ezCameraEye eye) const;

  /// \brief Returns the current view-projection matrix.
  const ezMat4& GetViewProjectionMatrix(ezCameraEye eye) const;

  /// \brief Returns the current inverse view-projection matrix.
  const ezMat4& GetInverseViewProjectionMatrix(ezCameraEye eye) const;

  /// \brief Returns the frustum that should be used for determine visible objects for this view.
  void ComputeCullingFrustum(ezFrustum& out_frustum) const;

  void SetShaderPermutationVariable(const char* szName, const char* szValue);

  void SetRenderPassProperty(const char* szPassName, const char* szPropertyName, const ezVariant& value);
  void SetExtractorProperty(const char* szPassName, const char* szPropertyName, const ezVariant& value);

  void ResetRenderPassProperties();
  void ResetExtractorProperties();

  void SetRenderPassReadBackProperty(const char* szPassName, const char* szPropertyName, const ezVariant& value);
  ezVariant GetRenderPassReadBackProperty(const char* szPassName, const char* szPropertyName);
  bool IsRenderPassReadBackPropertyExisting(const char* szPassName, const char* szPropertyName) const;

  /// \brief Pushes the view and camera data into the extracted data of the pipeline.
  ///
  /// Use ezRenderWorld::GetDataIndexForExtraction() to update the data from the extraction thread. Can't be used if this view is currently extracted.
  /// Use ezRenderWorld::GetDataIndexForRendering() to update the data from the render thread.
  void UpdateViewData(ezUInt32 uiDataIndex);

  ezTagSet m_IncludeTags;
  ezTagSet m_ExcludeTags;

private:
  friend class ezRenderWorld;
  friend class ezMemoryUtils;

  ezViewId m_InternalId;
  ezHashedString m_sName;

  ezSharedPtr<ezTask> m_pExtractTask;

  ezWorld* m_pWorld = nullptr;

  ezRenderPipelineResourceHandle m_hRenderPipeline;
  ezUInt32 m_uiRenderPipelineResourceDescriptionCounter = 0;
  ezSharedPtr<ezRenderPipeline> m_pRenderPipeline;
  ezCamera* m_pCamera = nullptr;
  const ezCamera* m_pCullingCamera = nullptr;
  const ezCamera* m_pLodCamera = nullptr;


private:
  ezRenderPipelineNodeInputPin m_PinRenderTarget0;
  ezRenderPipelineNodeInputPin m_PinRenderTarget1;
  ezRenderPipelineNodeInputPin m_PinRenderTarget2;
  ezRenderPipelineNodeInputPin m_PinRenderTarget3;
  ezRenderPipelineNodeInputPin m_PinDepthStencil;

private:
  void UpdateCachedMatrices() const;

  /// \brief Rebuilds pipeline if necessary and pushes double-buffered settings into the pipeline.
  void EnsureUpToDate();

  mutable ezUInt32 m_uiLastCameraSettingsModification = 0;
  mutable ezUInt32 m_uiLastCameraOrientationModification = 0;
  mutable float m_fLastViewportAspectRatio = 1.0f;

  mutable ezViewData m_Data;

  ezInternal::RenderDataCache* m_pRenderDataCache = nullptr;

  ezDynamicArray<ezPermutationVar> m_PermutationVars;
  bool m_bPermutationVarsDirty = false;

  void ApplyPermutationVars();

  struct PropertyValue
  {
    ezString m_sObjectName;
    ezString m_sPropertyName;
    ezVariant m_DefaultValue;
    ezVariant m_CurrentValue;
    bool m_bIsValid;
    bool m_bIsDirty;
  };

  void SetProperty(ezMap<ezString, PropertyValue>& map, const char* szPassName, const char* szPropertyName, const ezVariant& value);
  void SetReadBackProperty(ezMap<ezString, PropertyValue>& map, const char* szPassName, const char* szPropertyName, const ezVariant& value);

  void ReadBackPassProperties();

  void ResetAllPropertyStates(ezMap<ezString, PropertyValue>& map);

  void ApplyRenderPassProperties();
  void ApplyExtractorProperties();

  void ApplyProperty(ezReflectedClass* pObject, PropertyValue& data, const char* szTypeName);

  ezMap<ezString, PropertyValue> m_PassProperties;
  ezMap<ezString, PropertyValue> m_PassReadBackProperties;
  ezMap<ezString, PropertyValue> m_ExtractorProperties;
};

#include <RendererCore/Pipeline/Implementation/View_inl.h>
