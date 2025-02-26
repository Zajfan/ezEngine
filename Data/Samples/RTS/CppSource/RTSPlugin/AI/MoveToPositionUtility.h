#pragma once

#include <RTSPlugin/AI/AiUtilitySystem.h>

class RtsMoveToPositionAiUtility : public RtsUnitComponentUtility
{
public:
  virtual void Activate(ezGameObject* pOwnerObject, ezComponent* pOwnerComponent) override;
  virtual void Deactivate(ezGameObject* pOwnerObject, ezComponent* pOwnerComponent) override;
  virtual void Execute(ezGameObject* pOwnerObject, ezComponent* pOwnerComponent, ezTime now) override;
  virtual double ComputePriority(ezGameObject* pOwnerObject, ezComponent* pOwnerComponent) const override;
};
