#include <ProcGenPlugin/ProcGenPluginPCH.h>

EZ_STATICLINK_LIBRARY(ProcGenPlugin)
{
  if (bReturn)
    return;

  EZ_STATICLINK_REFERENCE(ProcGenPlugin_Components_Implementation_ProcPlacementComponent);
  EZ_STATICLINK_REFERENCE(ProcGenPlugin_Components_Implementation_ProcVertexColorComponent);
  EZ_STATICLINK_REFERENCE(ProcGenPlugin_Components_Implementation_ProcVertexColorRenderer);
  EZ_STATICLINK_REFERENCE(ProcGenPlugin_Components_Implementation_ProcVolumeComponent);
  EZ_STATICLINK_REFERENCE(ProcGenPlugin_Components_Implementation_VolumeCollection);
  EZ_STATICLINK_REFERENCE(ProcGenPlugin_Declarations);
  EZ_STATICLINK_REFERENCE(ProcGenPlugin_Resources_Implementation_ProcGenGraphResource);
  EZ_STATICLINK_REFERENCE(ProcGenPlugin_Startup);
  EZ_STATICLINK_REFERENCE(ProcGenPlugin_Tasks_Implementation_FindPlacementTilesTask);
}
