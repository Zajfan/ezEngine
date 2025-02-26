#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Math/Intersection.h>
#include <Foundation/Math/Mat4.h>

EZ_CREATE_SIMPLE_TEST(Math, Intersection)
{
  EZ_TEST_BLOCK(ezTestBlock::Enabled, "RayPolygonIntersection")
  {
    for (ezUInt32 i = 0; i < 100; ++i)
    {
      ezMat4 m;
      m = ezMat4::MakeAxisRotation(ezVec3(i + 1.0f, i * 3.0f, i * 7.0f).GetNormalized(), ezAngle::MakeFromDegree((float)i));
      m.SetTranslationVector(ezVec3((float)i, i * 2.0f, i * 3.0f));

      ezVec3 Vertices[8] = {m.TransformPosition(ezVec3(-10, -10, 0)), ezVec3(-10, -10, 0), m.TransformPosition(ezVec3(10, -10, 0)),
        ezVec3(10, -10, 0), m.TransformPosition(ezVec3(10, 10, 0)), ezVec3(10, 10, 0), m.TransformPosition(ezVec3(-10, 10, 0)), ezVec3(-10, 10, 0)};

      for (float y = -14.5; y <= 14.5f; y += 2.0f)
      {
        for (float x = -14.5; x <= 14.5f; x += 2.0f)
        {
          const ezVec3 vRayDir = m.TransformDirection(ezVec3(x, y, -10.0f));
          const ezVec3 vRayStart = m.TransformPosition(ezVec3(x, y, 0.0f)) - vRayDir * 3.0f;

          const bool bIntersects = (x >= -10.0f && x <= 10.0f && y >= -10.0f && y <= 10.0f);

          float fIntersection;
          ezVec3 vIntersection;
          EZ_TEST_BOOL(ezIntersectionUtils::RayPolygonIntersection(vRayStart, vRayDir, Vertices, 4, &fIntersection, &vIntersection, sizeof(ezVec3) * 2) == bIntersects);

          if (bIntersects)
          {
            EZ_TEST_FLOAT(fIntersection, 3.0f, 0.0001f);
            EZ_TEST_VEC3(vIntersection, m.TransformPosition(ezVec3(x, y, 0.0f)), 0.0001f);
          }
        }
      }
    }
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ClosestPoint_PointLineSegment")
  {
    for (ezUInt32 i = 0; i < 100; ++i)
    {
      ezMat4 m;
      m = ezMat4::MakeAxisRotation(ezVec3(i + 1.0f, i * 3.0f, i * 7.0f).GetNormalized(), ezAngle::MakeFromDegree((float)i));
      m.SetTranslationVector(ezVec3((float)i, i * 2.0f, i * 3.0f));

      ezVec3 vSegment0 = m.TransformPosition(ezVec3(-10, 1, 2));
      ezVec3 vSegment1 = m.TransformPosition(ezVec3(10, 1, 2));

      for (float f = -20; f <= -10; f += 0.5f)
      {
        const ezVec3 vPos = m.TransformPosition(ezVec3(f, 10.0f, 20.0f));

        float fFraction = -1.0f;
        const ezVec3 vClosest = ezIntersectionUtils::ClosestPoint_PointLineSegment(vPos, vSegment0, vSegment1, &fFraction);

        EZ_TEST_FLOAT(fFraction, 0.0f, 0.0001f);
        EZ_TEST_VEC3(vClosest, vSegment0, 0.0001f);
      }

      for (float f = -10; f <= 10; f += 0.5f)
      {
        const ezVec3 vPos = m.TransformPosition(ezVec3(f, 10.0f, 20.0f));

        float fFraction = -1.0f;
        const ezVec3 vClosest = ezIntersectionUtils::ClosestPoint_PointLineSegment(vPos, vSegment0, vSegment1, &fFraction);

        EZ_TEST_FLOAT(fFraction, (f + 10.0f) / 20.0f, 0.0001f);
        EZ_TEST_VEC3(vClosest, m.TransformPosition(ezVec3(f, 1, 2)), 0.0001f);
      }

      for (float f = 10; f <= 20; f += 0.5f)
      {
        const ezVec3 vPos = m.TransformPosition(ezVec3(f, 10.0f, 20.0f));

        float fFraction = -1.0f;
        const ezVec3 vClosest = ezIntersectionUtils::ClosestPoint_PointLineSegment(vPos, vSegment0, vSegment1, &fFraction);

        EZ_TEST_FLOAT(fFraction, 1.0f, 0.0001f);
        EZ_TEST_VEC3(vClosest, vSegment1, 0.0001f);
      }
    }
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Ray2DLine2D")
  {
    for (ezUInt32 i = 0; i < 100; ++i)
    {
      ezMat4 m;
      m = ezMat4::MakeRotationZ(ezAngle::MakeFromDegree((float)i));
      m.SetTranslationVector(ezVec3((float)i, i * 2.0f, i * 3.0f));

      const ezVec2 vSegment0 = m.TransformPosition(ezVec3(23, 42, 0)).GetAsVec2();
      const ezVec2 vSegmentDir = m.TransformDirection(ezVec3(13, 15, 0)).GetAsVec2();

      const ezVec2 vSegment1 = vSegment0 + vSegmentDir;

      for (float f = -1.1f; f < 2.0f; f += 0.2f)
      {
        const bool bIntersection = (f >= 0.0f && f <= 1.0f);
        const ezVec2 vSegmentPos = vSegment0 + f * vSegmentDir;

        const ezVec2 vRayDir = ezVec2(2.0f, f);
        const ezVec2 vRayStart = vSegmentPos - vRayDir * 5.0f;

        float fIntersection;
        ezVec2 vIntersection;
        EZ_TEST_BOOL(ezIntersectionUtils::Ray2DLine2D(vRayStart, vRayDir, vSegment0, vSegment1, &fIntersection, &vIntersection) == bIntersection);

        if (bIntersection)
        {
          EZ_TEST_FLOAT(fIntersection, 5.0f, 0.0001f);
          EZ_TEST_VEC2(vIntersection, vSegmentPos, 0.0001f);
        }
      };
    }
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "RayTriangleIntersection")
  {
    for (ezUInt32 i = 0; i < 100; ++i)
    {
      ezMat4 m;
      m = ezMat4::MakeAxisRotation(ezVec3(i + 1.0f, i * 3.0f, i * 7.0f).GetNormalized(), ezAngle::MakeFromDegree((float)i));
      m.SetTranslationVector(ezVec3((float)i, i * 2.0f, i * 3.0f));

      ezVec3 Vertices[4] = {m.TransformPosition(ezVec3(-10, -10, 0)), m.TransformPosition(ezVec3(10, -10, 0)),
        m.TransformPosition(ezVec3(10, 10, 0)), m.TransformPosition(ezVec3(-10, 10, 0))};

      for (float y = -14.5; y <= 14.5f; y += 2.0f)
      {
        for (float x = -14.5; x <= 14.5f; x += 2.0f)
        {
          const ezVec3 vRayDir = m.TransformDirection(ezVec3(x, y, -10.0f));
          const ezVec3 vRayStart = m.TransformPosition(ezVec3(x, y, 0.0f)) - vRayDir * 3.0f;

          const bool bInRect = (x >= -10.0f && x <= 10.0f && y >= -10.0f && y <= 10.0f);

          ezVec3 vPlaneIntersection;
          ezPlane plane = ezPlane::MakeFromPoints(Vertices[0], Vertices[1], Vertices[2]);
          EZ_TEST_BOOL(plane.GetRayIntersection(vRayStart, vRayDir, nullptr, &vPlaneIntersection));
          const bool bIsOnEdge = ezIntersectionUtils::IsPointOnLine(Vertices[0], Vertices[2], vPlaneIntersection);

          float fIntersection1 = 0;
          ezVec3 vIntersection1(0.0f);
          float fIntersection2 = 0;
          ezVec3 vIntersection2(0.0f);

          const bool bHit1a = ezIntersectionUtils::RayPolygonIntersection(vRayStart, vRayDir, Vertices, 3);

          const bool bHit1 = ezIntersectionUtils::RayTriangleIntersection(vRayStart, vRayDir, Vertices[0], Vertices[1], Vertices[2], &fIntersection1, &vIntersection1);

          EZ_TEST_BOOL(bHit1 == bHit1a);

          const bool bHit2 = ezIntersectionUtils::RayTriangleIntersection(vRayStart, vRayDir, Vertices[0], Vertices[2], Vertices[3], &fIntersection2, &vIntersection2);

          if (!bInRect)
          {
            // outside rect, neither should hit
            EZ_TEST_BOOL(bHit1 == false);
            EZ_TEST_BOOL(bHit2 == false);
            EZ_TEST_BOOL(bHit1a == false);
          }
          else
          {
            if (!bIsOnEdge) // anything can happen close to the edge
            {
              // inside rect, exactly one triangle should hit
              EZ_TEST_BOOL(bHit1 || bHit2);
              EZ_TEST_BOOL(bHit1 != bHit2);
            }

            if (bHit1)
            {
              EZ_TEST_FLOAT(fIntersection1, 3.0f, 0.0001f);
              EZ_TEST_VEC3(vIntersection1, m.TransformPosition(ezVec3(x, y, 0.0f)), 0.0001f);
            }

            if (bHit2)
            {
              EZ_TEST_FLOAT(fIntersection2, 3.0f, 0.0001f);
              EZ_TEST_VEC3(vIntersection2, m.TransformPosition(ezVec3(x, y, 0.0f)), 0.0001f);
            }
          }
        }
      }
    }
  }
}
