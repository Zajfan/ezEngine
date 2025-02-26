#include <AsteroidsPlugin/Components/AsteroidComponent.h>
#include <AsteroidsPlugin/Components/ShipComponent.h>
#include <AsteroidsPlugin/GameState/Level.h>
#include <Foundation/Configuration/CVar.h>
#include <Foundation/Time/Clock.h>

// clang-format off
EZ_BEGIN_COMPONENT_TYPE(AsteroidComponent, 1, ezComponentMode::Dynamic)
EZ_END_COMPONENT_TYPE
// clang-format on

ezCVarFloat CVar_AsteroidMaxDist("g_AsteroidMaxDist", 4.0f, ezCVarFlags::Default, "The radius at which an asteroid pushes ships away.");
ezCVarFloat CVar_AsteroidPush("g_AsteroidPush", 0.06f, ezCVarFlags::Default, "The strength with which an asteroid pushes a ship away.");

AsteroidComponent::AsteroidComponent() = default;

void AsteroidComponent::OnSimulationStarted()
{
  m_fRotationSpeed = (float)GetWorld()->GetRandomNumberGenerator().DoubleMinMax(-1.0, 1.0);
}

void AsteroidComponent::Update()
{
  const float fTimeDiff = (float)GetWorld()->GetClock().GetTimeDiff().GetSeconds();

  ezQuat qRot = ezQuat::MakeFromAxisAndAngle(ezVec3(0, 0, 1), ezAngle::MakeFromRadian(m_fRotationSpeed * fTimeDiff));

  GetOwner()->SetLocalRotation(qRot * GetOwner()->GetLocalRotation());

  const ezVec3 vOwnPos = GetOwner()->GetLocalPosition();

  ShipComponentManager* pShipManager = GetWorld()->GetOrCreateComponentManager<ShipComponentManager>();

  for (auto it = pShipManager->GetComponents(); it.IsValid(); ++it)
  {
    ShipComponent& ship = *it;

    if (!ship.IsActiveAndSimulating())
      continue;

    ezGameObject* pObject = ship.GetOwner();

    const ezVec3 vDir = pObject->GetLocalPosition() - vOwnPos;
    const float fDist = vDir.GetLength();
    const float fMaxDist = CVar_AsteroidMaxDist;

    if (fDist > fMaxDist)
      continue;

    const float fFactor = 1.0f - fDist / fMaxDist;
    const float fScaledFactor = ezMath::Pow(fFactor, 2.0f);
    const ezVec3 vPull = vDir * fScaledFactor;

    ship.AddExternalForce(vPull * (float)CVar_AsteroidPush);
  }
}
