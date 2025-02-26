#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Types/VarianceTypes.h>
#include <Foundation/Types/Variant.h>
#include <FoundationTest/Reflection/ReflectionTestClasses.h>

// this file takes ages to compile in a Release build
// since we don't care for runtime performance, just disable all optimizations
#pragma optimize("", off)

class Blubb : public ezReflectedClass
{
  EZ_ADD_DYNAMIC_REFLECTION(Blubb, ezReflectedClass);

public:
  float u;
  float v;
};

// clang-format off
EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(Blubb, 1, ezRTTINoAllocator)
{
  EZ_BEGIN_PROPERTIES
  {
    EZ_MEMBER_PROPERTY("u", u),
    EZ_MEMBER_PROPERTY("v", v),
  }
  EZ_END_PROPERTIES;
}
EZ_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

template <typename T>
void TestVariant(ezVariant& v, ezVariantType::Enum type)
{
  EZ_TEST_BOOL(v.IsValid());
  EZ_TEST_BOOL(v.GetType() == type);
  EZ_TEST_BOOL(v.CanConvertTo<T>());
  EZ_TEST_BOOL(v.IsA<T>());
  EZ_TEST_BOOL(v.GetReflectedType() == ezGetStaticRTTI<T>());

  ezTypedPointer ptr = v.GetWriteAccess();
  EZ_TEST_BOOL(ptr.m_pObject == &v.Get<T>());
  EZ_TEST_BOOL(ptr.m_pObject == &v.GetWritable<T>());
  EZ_TEST_BOOL(ptr.m_pType == ezGetStaticRTTI<T>());

  EZ_TEST_BOOL(ptr.m_pObject == v.GetData());

  ezVariant vCopy = v;
  ezTypedPointer ptr2 = vCopy.GetWriteAccess();
  EZ_TEST_BOOL(ptr2.m_pObject == &vCopy.Get<T>());
  EZ_TEST_BOOL(ptr2.m_pObject == &vCopy.GetWritable<T>());

  EZ_TEST_BOOL(ptr2.m_pObject != ptr.m_pObject);
  EZ_TEST_BOOL(ptr2.m_pType == ezGetStaticRTTI<T>());

  EZ_TEST_BOOL(v.Get<T>() == vCopy.Get<T>());

  EZ_TEST_BOOL(v.ComputeHash(0) != 0);
}

template <typename T>
inline void TestIntegerVariant(ezVariant::Type::Enum type)
{
  ezVariant b((T)23);
  TestVariant<T>(b, type);

  EZ_TEST_BOOL(b.Get<T>() == 23);

  EZ_TEST_BOOL(b == ezVariant(23));
  EZ_TEST_BOOL(b != ezVariant(11));
  EZ_TEST_BOOL(b == ezVariant((T)23));
  EZ_TEST_BOOL(b != ezVariant((T)11));

  EZ_TEST_BOOL(b == 23);
  EZ_TEST_BOOL(b != 24);
  EZ_TEST_BOOL(b == (T)23);
  EZ_TEST_BOOL(b != (T)24);

  b = (T)17;
  EZ_TEST_BOOL(b == (T)17);

  b = ezVariant((T)19);
  EZ_TEST_BOOL(b == (T)19);

  EZ_TEST_BOOL(b.IsNumber());
  EZ_TEST_BOOL(b.IsFloatingPoint() == false);
  EZ_TEST_BOOL(!b.IsString());
}

inline void TestNumberCanConvertTo(const ezVariant& v)
{
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Invalid) == false);
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Bool));
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Int8));
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::UInt8));
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Int16));
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::UInt16));
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Int32));
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::UInt32));
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Int64));
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::UInt64));
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Float));
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Double));
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Color) == false);
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector2) == false);
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector3) == false);
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector4) == false);
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector2I) == false);
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector3I) == false);
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector4I) == false);
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Quaternion) == false);
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Matrix3) == false);
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Matrix4) == false);
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Transform) == false);
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::String));
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::StringView) == false);
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::DataBuffer) == false);
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Time) == false);
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Uuid) == false);
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Angle) == false);
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::ColorGamma) == false);
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::HashedString));
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::TempHashedString));
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::VariantArray) == false);
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::VariantDictionary) == false);
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::TypedPointer) == false);
  EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::TypedObject) == false);

  ezResult conversionResult = EZ_FAILURE;
  EZ_TEST_BOOL(v.ConvertTo<bool>(&conversionResult) == true);
  EZ_TEST_BOOL(conversionResult.Succeeded());

  EZ_TEST_BOOL(v.ConvertTo<ezInt8>(&conversionResult) == 3);
  EZ_TEST_BOOL(conversionResult.Succeeded());

  EZ_TEST_BOOL(v.ConvertTo<ezUInt8>(&conversionResult) == 3);
  EZ_TEST_BOOL(conversionResult.Succeeded());

  EZ_TEST_BOOL(v.ConvertTo<ezInt16>(&conversionResult) == 3);
  EZ_TEST_BOOL(conversionResult.Succeeded());

  EZ_TEST_BOOL(v.ConvertTo<ezUInt16>(&conversionResult) == 3);
  EZ_TEST_BOOL(conversionResult.Succeeded());

  EZ_TEST_BOOL(v.ConvertTo<ezInt32>(&conversionResult) == 3);
  EZ_TEST_BOOL(conversionResult.Succeeded());

  EZ_TEST_BOOL(v.ConvertTo<ezUInt32>(&conversionResult) == 3);
  EZ_TEST_BOOL(conversionResult.Succeeded());

  EZ_TEST_BOOL(v.ConvertTo<ezInt64>(&conversionResult) == 3);
  EZ_TEST_BOOL(conversionResult.Succeeded());

  EZ_TEST_BOOL(v.ConvertTo<ezUInt64>(&conversionResult) == 3);
  EZ_TEST_BOOL(conversionResult.Succeeded());

  EZ_TEST_BOOL(v.ConvertTo<float>(&conversionResult) == 3.0f);
  EZ_TEST_BOOL(conversionResult.Succeeded());

  EZ_TEST_BOOL(v.ConvertTo<double>(&conversionResult) == 3.0);
  EZ_TEST_BOOL(conversionResult.Succeeded());

  EZ_TEST_BOOL(v.ConvertTo<ezString>(&conversionResult) == "3");
  EZ_TEST_BOOL(conversionResult.Succeeded());

  EZ_TEST_BOOL(v.ConvertTo<ezHashedString>(&conversionResult) == ezMakeHashedString("3"));
  EZ_TEST_BOOL(conversionResult.Succeeded());

  EZ_TEST_BOOL(v.ConvertTo<ezTempHashedString>(&conversionResult) == ezTempHashedString("3"));
  EZ_TEST_BOOL(conversionResult.Succeeded());

  EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Bool).Get<bool>() == true);
  EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Int8).Get<ezInt8>() == 3);
  EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::UInt8).Get<ezUInt8>() == 3);
  EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Int16).Get<ezInt16>() == 3);
  EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::UInt16).Get<ezUInt16>() == 3);
  EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Int32).Get<ezInt32>() == 3);
  EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::UInt32).Get<ezUInt32>() == 3);
  EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Int64).Get<ezInt64>() == 3);
  EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::UInt64).Get<ezUInt64>() == 3);
  EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Float).Get<float>() == 3.0f);
  EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Double).Get<double>() == 3.0);
  EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::String).Get<ezString>() == "3");
  EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::HashedString).Get<ezHashedString>() == ezMakeHashedString("3"));
  EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::TempHashedString).Get<ezTempHashedString>() == ezTempHashedString("3"));
}

inline void TestCanOnlyConvertToID(const ezVariant& v, ezVariant::Type::Enum type)
{
  for (int iType = ezVariant::Type::FirstStandardType; iType < ezVariant::Type::LastExtendedType; ++iType)
  {
    if (iType == ezVariant::Type::LastStandardType)
      iType = ezVariant::Type::FirstExtendedType;

    if (iType == type)
    {
      EZ_TEST_BOOL(v.CanConvertTo(type));
    }
    else
    {
      EZ_TEST_BOOL(v.CanConvertTo((ezVariant::Type::Enum)iType) == false);
    }
  }
}

inline void TestCanOnlyConvertToStringAndID(const ezVariant& v, ezVariant::Type::Enum type, ezVariant::Type::Enum type2 = ezVariant::Type::Invalid,
  ezVariant::Type::Enum type3 = ezVariant::Type::Invalid)
{
  if (type2 == ezVariant::Type::Invalid)
    type2 = type;

  for (int iType = ezVariant::Type::FirstStandardType; iType < ezVariant::Type::LastExtendedType; ++iType)
  {
    if (iType == ezVariant::Type::LastStandardType)
      iType = ezVariant::Type::FirstExtendedType;

    if (iType == ezVariant::Type::String || iType == ezVariant::Type::HashedString || iType == ezVariant::Type::TempHashedString)
    {
      EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::String));
    }
    else if (iType == type || iType == type2 || iType == type3)
    {
      EZ_TEST_BOOL(v.CanConvertTo(type));
    }
    else
    {
      EZ_TEST_BOOL(v.CanConvertTo((ezVariant::Type::Enum)iType) == false);
    }
  }
}

EZ_CREATE_SIMPLE_TEST(Basics, Variant)
{
  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Invalid")
  {
    ezVariant b;
    EZ_TEST_BOOL(b.GetType() == ezVariant::Type::Invalid);
    EZ_TEST_BOOL(b == ezVariant());
    EZ_TEST_BOOL(b != ezVariant(0));
    EZ_TEST_BOOL(!b.IsValid());
    EZ_TEST_BOOL(!b[0].IsValid());
    EZ_TEST_BOOL(!b["x"].IsValid());
    EZ_TEST_BOOL(b.GetReflectedType() == nullptr);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "bool")
  {
    ezVariant b(true);
    TestVariant<bool>(b, ezVariantType::Bool);

    EZ_TEST_BOOL(b.Get<bool>() == true);

    EZ_TEST_BOOL(b == ezVariant(true));
    EZ_TEST_BOOL(b != ezVariant(false));

    EZ_TEST_BOOL(b == true);
    EZ_TEST_BOOL(b != false);

    b = false;
    EZ_TEST_BOOL(b == false);

    b = ezVariant(true);
    EZ_TEST_BOOL(b == true);
    EZ_TEST_BOOL(!b[0].IsValid());

    EZ_TEST_BOOL(b.IsNumber());
    EZ_TEST_BOOL(!b.IsString());
    EZ_TEST_BOOL(b.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezInt8")
  {
    TestIntegerVariant<ezInt8>(ezVariant::Type::Int8);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezUInt8")
  {
    TestIntegerVariant<ezUInt8>(ezVariant::Type::UInt8);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezInt16")
  {
    TestIntegerVariant<ezInt16>(ezVariant::Type::Int16);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezUInt16")
  {
    TestIntegerVariant<ezUInt16>(ezVariant::Type::UInt16);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezInt32")
  {
    TestIntegerVariant<ezInt32>(ezVariant::Type::Int32);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezUInt32")
  {
    TestIntegerVariant<ezUInt32>(ezVariant::Type::UInt32);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezInt64")
  {
    TestIntegerVariant<ezInt64>(ezVariant::Type::Int64);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezUInt64")
  {
    TestIntegerVariant<ezUInt64>(ezVariant::Type::UInt64);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "float")
  {
    ezVariant b(42.0f);
    TestVariant<float>(b, ezVariantType::Float);

    EZ_TEST_BOOL(b.Get<float>() == 42.0f);

    EZ_TEST_BOOL(b == ezVariant(42));
    EZ_TEST_BOOL(b != ezVariant(11));
    EZ_TEST_BOOL(b == ezVariant(42.0));
    EZ_TEST_BOOL(b != ezVariant(11.0));
    EZ_TEST_BOOL(b == ezVariant(42.0f));
    EZ_TEST_BOOL(b != ezVariant(11.0f));

    EZ_TEST_BOOL(b == 42);
    EZ_TEST_BOOL(b != 41);
    EZ_TEST_BOOL(b == 42.0);
    EZ_TEST_BOOL(b != 41.0);
    EZ_TEST_BOOL(b == 42.0f);
    EZ_TEST_BOOL(b != 41.0f);

    b = 17.0f;
    EZ_TEST_BOOL(b == 17.0f);

    b = ezVariant(19.0f);
    EZ_TEST_BOOL(b == 19.0f);

    EZ_TEST_BOOL(b.IsNumber());
    EZ_TEST_BOOL(!b.IsString());
    EZ_TEST_BOOL(b.IsFloatingPoint());
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "double")
  {
    ezVariant b(42.0);
    TestVariant<double>(b, ezVariantType::Double);
    EZ_TEST_BOOL(b.Get<double>() == 42.0);

    EZ_TEST_BOOL(b == ezVariant(42));
    EZ_TEST_BOOL(b != ezVariant(11));
    EZ_TEST_BOOL(b == ezVariant(42.0));
    EZ_TEST_BOOL(b != ezVariant(11.0));
    EZ_TEST_BOOL(b == ezVariant(42.0f));
    EZ_TEST_BOOL(b != ezVariant(11.0f));

    EZ_TEST_BOOL(b == 42);
    EZ_TEST_BOOL(b != 41);
    EZ_TEST_BOOL(b == 42.0);
    EZ_TEST_BOOL(b != 41.0);
    EZ_TEST_BOOL(b == 42.0f);
    EZ_TEST_BOOL(b != 41.0f);

    b = 17.0;
    EZ_TEST_BOOL(b == 17.0);

    b = ezVariant(19.0);
    EZ_TEST_BOOL(b == 19.0);

    EZ_TEST_BOOL(b.IsNumber());
    EZ_TEST_BOOL(!b.IsString());
    EZ_TEST_BOOL(b.IsFloatingPoint());
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezColor")
  {
    ezVariant v(ezColor(1, 2, 3, 1));
    TestVariant<ezColor>(v, ezVariantType::Color);

    EZ_TEST_BOOL(v.CanConvertTo<ezColorGammaUB>());
    EZ_TEST_BOOL(v.ConvertTo<ezColorGammaUB>() == static_cast<ezColorGammaUB>(ezColor(1, 2, 3, 1)));
    EZ_TEST_BOOL(v.Get<ezColor>() == ezColor(1, 2, 3, 1));

    EZ_TEST_BOOL(v == ezVariant(ezColor(1, 2, 3)));
    EZ_TEST_BOOL(v != ezVariant(ezColor(1, 1, 1)));

    EZ_TEST_BOOL(v == ezColor(1, 2, 3));
    EZ_TEST_BOOL(v != ezColor(1, 4, 3));

    v = ezColor(5, 8, 9);
    EZ_TEST_BOOL(v == ezColor(5, 8, 9));

    v = ezVariant(ezColor(7, 9, 4));
    EZ_TEST_BOOL(v == ezColor(7, 9, 4));
    EZ_TEST_BOOL(v[0] == 7);
    EZ_TEST_BOOL(v[1] == 9);
    EZ_TEST_BOOL(v[2] == 4);
    EZ_TEST_BOOL(v[3] == 1);
    EZ_TEST_BOOL(v[4] == ezVariant());
    EZ_TEST_BOOL(!v[4].IsValid());
    EZ_TEST_BOOL(v["r"] == 7);
    EZ_TEST_BOOL(v["g"] == 9);
    EZ_TEST_BOOL(v["b"] == 4);
    EZ_TEST_BOOL(v["a"] == 1);
    EZ_TEST_BOOL(v["x"] == ezVariant());
    EZ_TEST_BOOL(!v["x"].IsValid());

    EZ_TEST_BOOL(v.IsNumber() == false);
    EZ_TEST_BOOL(!v.IsString());
    EZ_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezColorGammaUB")
  {
    ezVariant v(ezColorGammaUB(64, 128, 255, 255));
    TestVariant<ezColorGammaUB>(v, ezVariantType::ColorGamma);

    EZ_TEST_BOOL(v.CanConvertTo<ezColor>());
    EZ_TEST_BOOL(v.Get<ezColorGammaUB>() == ezColorGammaUB(64, 128, 255, 255));

    EZ_TEST_BOOL(v == ezVariant(ezColorGammaUB(64, 128, 255, 255)));
    EZ_TEST_BOOL(v != ezVariant(ezColorGammaUB(255, 128, 255, 255)));

    EZ_TEST_BOOL(v == ezColorGammaUB(64, 128, 255, 255));
    EZ_TEST_BOOL(v != ezColorGammaUB(64, 42, 255, 255));

    v = ezColorGammaUB(10, 50, 200);
    EZ_TEST_BOOL(v == ezColorGammaUB(10, 50, 200));

    v = ezVariant(ezColorGammaUB(17, 120, 200));
    EZ_TEST_BOOL(v == ezColorGammaUB(17, 120, 200));
    EZ_TEST_BOOL(v[0] == 17);
    EZ_TEST_BOOL(v[1] == 120);
    EZ_TEST_BOOL(v[2] == 200);
    EZ_TEST_BOOL(v[3] == 255);
    EZ_TEST_BOOL(v[4] == ezVariant());
    EZ_TEST_BOOL(!v[4].IsValid());
    EZ_TEST_BOOL(v["r"] == 17);
    EZ_TEST_BOOL(v["g"] == 120);
    EZ_TEST_BOOL(v["b"] == 200);
    EZ_TEST_BOOL(v["a"] == 255);
    EZ_TEST_BOOL(v["x"] == ezVariant());
    EZ_TEST_BOOL(!v["x"].IsValid());

    EZ_TEST_BOOL(v.IsNumber() == false);
    EZ_TEST_BOOL(!v.IsString());
    EZ_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezVec2")
  {
    ezVariant v(ezVec2(1, 2));
    TestVariant<ezVec2>(v, ezVariantType::Vector2);

    EZ_TEST_BOOL(v.Get<ezVec2>() == ezVec2(1, 2));

    EZ_TEST_BOOL(v == ezVariant(ezVec2(1, 2)));
    EZ_TEST_BOOL(v != ezVariant(ezVec2(1, 1)));

    EZ_TEST_BOOL(v == ezVec2(1, 2));
    EZ_TEST_BOOL(v != ezVec2(1, 4));

    v = ezVec2(5, 8);
    EZ_TEST_BOOL(v == ezVec2(5, 8));

    v = ezVariant(ezVec2(7, 9));
    EZ_TEST_BOOL(v == ezVec2(7, 9));
    EZ_TEST_BOOL(v[0] == 7);
    EZ_TEST_BOOL(v[1] == 9);
    EZ_TEST_BOOL(v["x"] == 7);
    EZ_TEST_BOOL(v["y"] == 9);

    EZ_TEST_BOOL(v.IsNumber() == false);
    EZ_TEST_BOOL(!v.IsString());
    EZ_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezVec3")
  {
    ezVariant v(ezVec3(1, 2, 3));
    TestVariant<ezVec3>(v, ezVariantType::Vector3);

    EZ_TEST_BOOL(v.Get<ezVec3>() == ezVec3(1, 2, 3));

    EZ_TEST_BOOL(v == ezVariant(ezVec3(1, 2, 3)));
    EZ_TEST_BOOL(v != ezVariant(ezVec3(1, 1, 3)));

    EZ_TEST_BOOL(v == ezVec3(1, 2, 3));
    EZ_TEST_BOOL(v != ezVec3(1, 4, 3));

    v = ezVec3(5, 8, 9);
    EZ_TEST_BOOL(v == ezVec3(5, 8, 9));

    v = ezVariant(ezVec3(7, 9, 8));
    EZ_TEST_BOOL(v == ezVec3(7, 9, 8));
    EZ_TEST_BOOL(v[0] == 7);
    EZ_TEST_BOOL(v[1] == 9);
    EZ_TEST_BOOL(v[2] == 8);
    EZ_TEST_BOOL(v["x"] == 7);
    EZ_TEST_BOOL(v["y"] == 9);
    EZ_TEST_BOOL(v["z"] == 8);

    EZ_TEST_BOOL(v.IsNumber() == false);
    EZ_TEST_BOOL(!v.IsString());
    EZ_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezVec4")
  {
    ezVariant v(ezVec4(1, 2, 3, 4));
    TestVariant<ezVec4>(v, ezVariantType::Vector4);

    EZ_TEST_BOOL(v.Get<ezVec4>() == ezVec4(1, 2, 3, 4));

    EZ_TEST_BOOL(v == ezVariant(ezVec4(1, 2, 3, 4)));
    EZ_TEST_BOOL(v != ezVariant(ezVec4(1, 1, 3, 4)));

    EZ_TEST_BOOL(v == ezVec4(1, 2, 3, 4));
    EZ_TEST_BOOL(v != ezVec4(1, 4, 3, 4));

    v = ezVec4(5, 8, 9, 3);
    EZ_TEST_BOOL(v == ezVec4(5, 8, 9, 3));

    v = ezVariant(ezVec4(7, 9, 8, 4));
    EZ_TEST_BOOL(v == ezVec4(7, 9, 8, 4));
    EZ_TEST_BOOL(v[0] == 7);
    EZ_TEST_BOOL(v[1] == 9);
    EZ_TEST_BOOL(v[2] == 8);
    EZ_TEST_BOOL(v[3] == 4);
    EZ_TEST_BOOL(v["x"] == 7);
    EZ_TEST_BOOL(v["y"] == 9);
    EZ_TEST_BOOL(v["z"] == 8);
    EZ_TEST_BOOL(v["w"] == 4);

    EZ_TEST_BOOL(v.IsNumber() == false);
    EZ_TEST_BOOL(!v.IsString());
    EZ_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezVec2I32")
  {
    ezVariant v(ezVec2I32(1, 2));
    TestVariant<ezVec2I32>(v, ezVariantType::Vector2I);

    EZ_TEST_BOOL(v.Get<ezVec2I32>() == ezVec2I32(1, 2));

    EZ_TEST_BOOL(v == ezVariant(ezVec2I32(1, 2)));
    EZ_TEST_BOOL(v != ezVariant(ezVec2I32(1, 1)));

    EZ_TEST_BOOL(v == ezVec2I32(1, 2));
    EZ_TEST_BOOL(v != ezVec2I32(1, 4));

    v = ezVec2I32(5, 8);
    EZ_TEST_BOOL(v == ezVec2I32(5, 8));

    v = ezVariant(ezVec2I32(7, 9));
    EZ_TEST_BOOL(v == ezVec2I32(7, 9));
    EZ_TEST_BOOL(v[0] == 7);
    EZ_TEST_BOOL(v[1] == 9);
    EZ_TEST_BOOL(v["x"] == 7);
    EZ_TEST_BOOL(v["y"] == 9);

    EZ_TEST_BOOL(v.IsNumber() == false);
    EZ_TEST_BOOL(!v.IsString());
    EZ_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezVec3I32")
  {
    ezVariant v(ezVec3I32(1, 2, 3));
    TestVariant<ezVec3I32>(v, ezVariantType::Vector3I);

    EZ_TEST_BOOL(v.Get<ezVec3I32>() == ezVec3I32(1, 2, 3));

    EZ_TEST_BOOL(v == ezVariant(ezVec3I32(1, 2, 3)));
    EZ_TEST_BOOL(v != ezVariant(ezVec3I32(1, 1, 3)));

    EZ_TEST_BOOL(v == ezVec3I32(1, 2, 3));
    EZ_TEST_BOOL(v != ezVec3I32(1, 4, 3));

    v = ezVec3I32(5, 8, 9);
    EZ_TEST_BOOL(v == ezVec3I32(5, 8, 9));

    v = ezVariant(ezVec3I32(7, 9, 8));
    EZ_TEST_BOOL(v == ezVec3I32(7, 9, 8));
    EZ_TEST_BOOL(v[0] == 7);
    EZ_TEST_BOOL(v[1] == 9);
    EZ_TEST_BOOL(v[2] == 8);
    EZ_TEST_BOOL(v["x"] == 7);
    EZ_TEST_BOOL(v["y"] == 9);
    EZ_TEST_BOOL(v["z"] == 8);

    EZ_TEST_BOOL(v.IsNumber() == false);
    EZ_TEST_BOOL(!v.IsString());
    EZ_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezVec4I32")
  {
    ezVariant v(ezVec4I32(1, 2, 3, 4));
    TestVariant<ezVec4I32>(v, ezVariantType::Vector4I);

    EZ_TEST_BOOL(v.Get<ezVec4I32>() == ezVec4I32(1, 2, 3, 4));

    EZ_TEST_BOOL(v == ezVariant(ezVec4I32(1, 2, 3, 4)));
    EZ_TEST_BOOL(v != ezVariant(ezVec4I32(1, 1, 3, 4)));

    EZ_TEST_BOOL(v == ezVec4I32(1, 2, 3, 4));
    EZ_TEST_BOOL(v != ezVec4I32(1, 4, 3, 4));

    v = ezVec4I32(5, 8, 9, 3);
    EZ_TEST_BOOL(v == ezVec4I32(5, 8, 9, 3));

    v = ezVariant(ezVec4I32(7, 9, 8, 4));
    EZ_TEST_BOOL(v == ezVec4I32(7, 9, 8, 4));
    EZ_TEST_BOOL(v[0] == 7);
    EZ_TEST_BOOL(v[1] == 9);
    EZ_TEST_BOOL(v[2] == 8);
    EZ_TEST_BOOL(v[3] == 4);
    EZ_TEST_BOOL(v["x"] == 7);
    EZ_TEST_BOOL(v["y"] == 9);
    EZ_TEST_BOOL(v["z"] == 8);
    EZ_TEST_BOOL(v["w"] == 4);

    EZ_TEST_BOOL(v.IsNumber() == false);
    EZ_TEST_BOOL(!v.IsString());
    EZ_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezQuat")
  {
    ezVariant v(ezQuat(1, 2, 3, 4));
    TestVariant<ezQuat>(v, ezVariantType::Quaternion);

    EZ_TEST_BOOL(v.Get<ezQuat>() == ezQuat(1, 2, 3, 4));

    EZ_TEST_BOOL(v == ezQuat(1, 2, 3, 4));
    EZ_TEST_BOOL(v != ezQuat(1, 2, 3, 5));

    EZ_TEST_BOOL(v == ezQuat(1, 2, 3, 4));
    EZ_TEST_BOOL(v != ezQuat(1, 4, 3, 4));

    v = ezQuat(5, 8, 9, 3);
    EZ_TEST_BOOL(v == ezQuat(5, 8, 9, 3));

    v = ezVariant(ezQuat(7, 9, 8, 4));
    EZ_TEST_BOOL(v == ezQuat(7, 9, 8, 4));
    EZ_TEST_BOOL(v[0] == 7);
    EZ_TEST_BOOL(v[1] == 9);
    EZ_TEST_BOOL(v[2] == 8);
    EZ_TEST_BOOL(v[3] == 4);
    EZ_TEST_BOOL(v["x"] == 7);
    EZ_TEST_BOOL(v["y"] == 9);
    EZ_TEST_BOOL(v["z"] == 8);
    EZ_TEST_BOOL(v["w"] == 4);

    EZ_TEST_BOOL(v.IsNumber() == false);
    EZ_TEST_BOOL(!v.IsString());
    EZ_TEST_BOOL(v.IsFloatingPoint() == false);

    ezTypedPointer ptr = v.GetWriteAccess();
    EZ_TEST_BOOL(ptr.m_pObject == &v.Get<ezQuat>());
    EZ_TEST_BOOL(ptr.m_pObject == &v.GetWritable<ezQuat>());
    EZ_TEST_BOOL(ptr.m_pType == ezGetStaticRTTI<ezQuat>());
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezMat3")
  {
    ezVariant v(ezMat3::MakeFromValues(1, 2, 3, 4, 5, 6, 7, 8, 9));
    TestVariant<ezMat3>(v, ezVariantType::Matrix3);

    EZ_TEST_BOOL(v.Get<ezMat3>() == ezMat3::MakeFromValues(1, 2, 3, 4, 5, 6, 7, 8, 9));

    EZ_TEST_BOOL(v == ezVariant(ezMat3::MakeFromValues(1, 2, 3, 4, 5, 6, 7, 8, 9)));
    EZ_TEST_BOOL(v != ezVariant(ezMat3::MakeFromValues(1, 2, 3, 4, 5, 6, 7, 8, 8)));

    EZ_TEST_BOOL(v == ezMat3::MakeFromValues(1, 2, 3, 4, 5, 6, 7, 8, 9));
    EZ_TEST_BOOL(v != ezMat3::MakeFromValues(1, 2, 3, 4, 5, 6, 7, 8, 8));

    v = ezMat3::MakeFromValues(5, 8, 9, 3, 1, 2, 3, 4, 5);
    EZ_TEST_BOOL(v == ezMat3::MakeFromValues(5, 8, 9, 3, 1, 2, 3, 4, 5));

    v = ezVariant(ezMat3::MakeFromValues(5, 8, 9, 3, 1, 2, 3, 4, 4));
    EZ_TEST_BOOL(v == ezMat3::MakeFromValues(5, 8, 9, 3, 1, 2, 3, 4, 4));

    EZ_TEST_BOOL(v.IsNumber() == false);
    EZ_TEST_BOOL(!v.IsString());
    EZ_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezMat4")
  {
    ezVariant v(ezMat4::MakeFromValues(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));
    TestVariant<ezMat4>(v, ezVariantType::Matrix4);

    EZ_TEST_BOOL(v.Get<ezMat4>() == ezMat4::MakeFromValues(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));

    EZ_TEST_BOOL(v == ezVariant(ezMat4::MakeFromValues(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16)));
    EZ_TEST_BOOL(v != ezVariant(ezMat4::MakeFromValues(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 15)));

    EZ_TEST_BOOL(v == ezMat4::MakeFromValues(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16));
    EZ_TEST_BOOL(v != ezMat4::MakeFromValues(1, 2, 3, 4, 5, 6, 2, 8, 9, 10, 11, 12, 13, 14, 15, 16));

    v = ezMat4::MakeFromValues(5, 8, 9, 3, 1, 2, 3, 4, 5, 3, 7, 3, 6, 8, 6, 8);
    EZ_TEST_BOOL(v == ezMat4::MakeFromValues(5, 8, 9, 3, 1, 2, 3, 4, 5, 3, 7, 3, 6, 8, 6, 8));

    v = ezVariant(ezMat4::MakeFromValues(5, 8, 9, 3, 1, 2, 1, 4, 5, 3, 7, 3, 6, 8, 6, 8));
    EZ_TEST_BOOL(v == ezMat4::MakeFromValues(5, 8, 9, 3, 1, 2, 1, 4, 5, 3, 7, 3, 6, 8, 6, 8));

    EZ_TEST_BOOL(v.IsNumber() == false);
    EZ_TEST_BOOL(!v.IsString());
    EZ_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezTransform")
  {
    ezVariant v(ezTransform(ezVec3(1, 2, 3), ezQuat(4, 5, 6, 7), ezVec3(8, 9, 10)));
    TestVariant<ezTransform>(v, ezVariantType::Transform);

    EZ_TEST_BOOL(v.Get<ezTransform>() == ezTransform(ezVec3(1, 2, 3), ezQuat(4, 5, 6, 7), ezVec3(8, 9, 10)));

    EZ_TEST_BOOL(v == ezTransform(ezVec3(1, 2, 3), ezQuat(4, 5, 6, 7), ezVec3(8, 9, 10)));
    EZ_TEST_BOOL(v != ezTransform(ezVec3(1, 2, 3), ezQuat(4, 5, 6, 7), ezVec3(8, 9, 11)));

    v = ezTransform(ezVec3(5, 8, 9), ezQuat(3, 1, 2, 3), ezVec3(4, 5, 3));
    EZ_TEST_BOOL(v == ezTransform(ezVec3(5, 8, 9), ezQuat(3, 1, 2, 3), ezVec3(4, 5, 3)));

    EZ_TEST_BOOL(v.IsNumber() == false);
    EZ_TEST_BOOL(!v.IsString());
    EZ_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "const char*")
  {
    ezVariant v("This is a const char array");
    TestVariant<ezString>(v, ezVariantType::String);

    EZ_TEST_BOOL(v.IsA<const char*>());
    EZ_TEST_BOOL(v.IsA<char*>());
    EZ_TEST_BOOL(v.Get<ezString>() == ezString("This is a const char array"));

    EZ_TEST_BOOL(v == ezVariant("This is a const char array"));
    EZ_TEST_BOOL(v != ezVariant("This is something else"));

    EZ_TEST_BOOL(v == ezString("This is a const char array"));
    EZ_TEST_BOOL(v != ezString("This is another string"));

    EZ_TEST_BOOL(v == "This is a const char array");
    EZ_TEST_BOOL(v != "This is another string");

    EZ_TEST_BOOL(v == (const char*)"This is a const char array");
    EZ_TEST_BOOL(v != (const char*)"This is another string");

    v = "blurg!";
    EZ_TEST_BOOL(v == ezString("blurg!"));

    v = ezVariant("blärg!");
    EZ_TEST_BOOL(v == ezString("blärg!"));

    EZ_TEST_BOOL(v.IsNumber() == false);
    EZ_TEST_BOOL(v.IsString());
    EZ_TEST_BOOL(v.CanConvertTo<ezStringView>());
    ezStringView view = v.ConvertTo<ezStringView>();
    EZ_TEST_BOOL(view == v.Get<ezString>());
    EZ_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezString")
  {
    ezVariant v(ezString("This is an ezString"));
    TestVariant<ezString>(v, ezVariantType::String);

    EZ_TEST_BOOL(v.Get<ezString>() == ezString("This is an ezString"));

    EZ_TEST_BOOL(v == ezVariant(ezString("This is an ezString")));
    EZ_TEST_BOOL(v == ezVariant(ezStringView("This is an ezString"), false));
    EZ_TEST_BOOL(v != ezVariant(ezString("This is something else")));

    EZ_TEST_BOOL(v == ezString("This is an ezString"));
    EZ_TEST_BOOL(v != ezString("This is another ezString"));

    v = ezString("blurg!");
    EZ_TEST_BOOL(v == ezString("blurg!"));

    v = ezVariant(ezString("blärg!"));
    EZ_TEST_BOOL(v == ezString("blärg!"));

    EZ_TEST_BOOL(v.IsNumber() == false);
    EZ_TEST_BOOL(v.IsString());
    EZ_TEST_BOOL(v.CanConvertTo<ezStringView>());
    ezStringView view = v.ConvertTo<ezStringView>();
    EZ_TEST_BOOL(view == v.Get<ezString>());
    EZ_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezStringView")
  {
    const char* szTemp = "This is an ezStringView";
    ezStringView bla(szTemp);
    ezVariant v(bla, false);
    TestVariant<ezStringView>(v, ezVariantType::StringView);

    const ezString sCopy = szTemp;
    EZ_TEST_BOOL(v.Get<ezStringView>() == sCopy);

    EZ_TEST_BOOL(v == ezVariant(ezStringView(sCopy.GetData()), false));
    EZ_TEST_BOOL(v == ezVariant(ezString("This is an ezStringView")));
    EZ_TEST_BOOL(v != ezVariant(ezStringView("This is something else"), false));

    EZ_TEST_BOOL(v == ezStringView(sCopy.GetData()));
    EZ_TEST_BOOL(v != ezStringView("This is something else"));

    v = ezVariant(ezStringView("blurg!"), false);
    EZ_TEST_BOOL(v == ezStringView("blurg!"));

    EZ_TEST_BOOL(v.IsNumber() == false);
    EZ_TEST_BOOL(v.IsString());
    EZ_TEST_BOOL(v.CanConvertTo<ezString>());
    ezString sString = v.ConvertTo<ezString>();
    EZ_TEST_BOOL(sString == v.Get<ezStringView>());
    EZ_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezDataBuffer")
  {
    ezDataBuffer a, a2;
    a.PushBack(ezUInt8(1));
    a.PushBack(ezUInt8(2));
    a.PushBack(ezUInt8(255));

    ezVariant va(a);
    TestVariant<ezDataBuffer>(va, ezVariantType::DataBuffer);

    const ezDataBuffer& b = va.Get<ezDataBuffer>();
    ezArrayPtr<const ezUInt8> b2 = va.Get<ezDataBuffer>();

    EZ_TEST_BOOL(a == b);
    EZ_TEST_BOOL(a == b2);

    EZ_TEST_BOOL(a != a2);

    EZ_TEST_BOOL(va == a);
    EZ_TEST_BOOL(va != a2);

    EZ_TEST_BOOL(va.IsNumber() == false);
    EZ_TEST_BOOL(!va.IsString());
    EZ_TEST_BOOL(va.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezTime")
  {
    ezVariant v(ezTime::MakeFromSeconds(1337));
    TestVariant<ezTime>(v, ezVariantType::Time);

    EZ_TEST_BOOL(v.Get<ezTime>() == ezTime::MakeFromSeconds(1337));

    EZ_TEST_BOOL(v == ezVariant(ezTime::MakeFromSeconds(1337)));
    EZ_TEST_BOOL(v != ezVariant(ezTime::MakeFromSeconds(1336)));

    EZ_TEST_BOOL(v == ezTime::MakeFromSeconds(1337));
    EZ_TEST_BOOL(v != ezTime::MakeFromSeconds(1338));

    v = ezTime::MakeFromSeconds(8472);
    EZ_TEST_BOOL(v == ezTime::MakeFromSeconds(8472));

    v = ezVariant(ezTime::MakeFromSeconds(13));
    EZ_TEST_BOOL(v == ezTime::MakeFromSeconds(13));

    EZ_TEST_BOOL(v.IsNumber() == false);
    EZ_TEST_BOOL(!v.IsString());
    EZ_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezUuid")
  {
    ezUuid id;
    ezVariant v(id);
    TestVariant<ezUuid>(v, ezVariantType::Uuid);

    EZ_TEST_BOOL(v.Get<ezUuid>() == ezUuid());

    const ezUuid uuid = ezUuid::MakeUuid();
    EZ_TEST_BOOL(v != ezVariant(uuid));
    EZ_TEST_BOOL(ezVariant(uuid).Get<ezUuid>() == uuid);

    const ezUuid uuid2 = ezUuid::MakeUuid();
    EZ_TEST_BOOL(ezVariant(uuid) != ezVariant(uuid2));

    EZ_TEST_BOOL(v.IsNumber() == false);
    EZ_TEST_BOOL(!v.IsString());
    EZ_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezAngle")
  {
    ezVariant v(ezAngle::MakeFromDegree(1337));
    TestVariant<ezAngle>(v, ezVariantType::Angle);

    EZ_TEST_BOOL(v.Get<ezAngle>() == ezAngle::MakeFromDegree(1337));

    EZ_TEST_BOOL(v == ezVariant(ezAngle::MakeFromDegree(1337)));
    EZ_TEST_BOOL(v != ezVariant(ezAngle::MakeFromDegree(1336)));

    EZ_TEST_BOOL(v == ezAngle::MakeFromDegree(1337));
    EZ_TEST_BOOL(v != ezAngle::MakeFromDegree(1338));

    v = ezAngle::MakeFromDegree(8472);
    EZ_TEST_BOOL(v == ezAngle::MakeFromDegree(8472));

    v = ezVariant(ezAngle::MakeFromDegree(13));
    EZ_TEST_BOOL(v == ezAngle::MakeFromDegree(13));

    EZ_TEST_BOOL(v.IsNumber() == false);
    EZ_TEST_BOOL(!v.IsString());
    EZ_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezHashedString")
  {
    ezVariant v(ezMakeHashedString("ABCDE"));
    TestVariant<ezHashedString>(v, ezVariantType::HashedString);

    EZ_TEST_BOOL(v.Get<ezHashedString>() == ezMakeHashedString("ABCDE"));

    EZ_TEST_BOOL(v == ezVariant(ezMakeHashedString("ABCDE")));
    EZ_TEST_BOOL(v != ezVariant(ezMakeHashedString("ABCDK")));
    EZ_TEST_BOOL(v == ezVariant(ezTempHashedString("ABCDE")));
    EZ_TEST_BOOL(v != ezVariant(ezTempHashedString("ABCDK")));

    EZ_TEST_BOOL(v == ezMakeHashedString("ABCDE"));
    EZ_TEST_BOOL(v != ezMakeHashedString("ABCDK"));
    EZ_TEST_BOOL(v == ezTempHashedString("ABCDE"));
    EZ_TEST_BOOL(v != ezTempHashedString("ABCDK"));

    v = ezMakeHashedString("HHH");
    EZ_TEST_BOOL(v == ezMakeHashedString("HHH"));

    EZ_TEST_BOOL(v.IsNumber() == false);
    EZ_TEST_BOOL(v.IsString() == false);
    EZ_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezTempHashedString")
  {
    ezVariant v(ezTempHashedString("ABCDE"));
    TestVariant<ezTempHashedString>(v, ezVariantType::TempHashedString);

    EZ_TEST_BOOL(v.Get<ezTempHashedString>() == ezTempHashedString("ABCDE"));

    EZ_TEST_BOOL(v == ezVariant(ezTempHashedString("ABCDE")));
    EZ_TEST_BOOL(v != ezVariant(ezTempHashedString("ABCDK")));
    EZ_TEST_BOOL(v == ezVariant(ezMakeHashedString("ABCDE")));
    EZ_TEST_BOOL(v != ezVariant(ezMakeHashedString("ABCDK")));

    EZ_TEST_BOOL(v == ezTempHashedString("ABCDE"));
    EZ_TEST_BOOL(v != ezTempHashedString("ABCDK"));
    EZ_TEST_BOOL(v == ezMakeHashedString("ABCDE"));
    EZ_TEST_BOOL(v != ezMakeHashedString("ABCDK"));

    v = ezTempHashedString("HHH");
    EZ_TEST_BOOL(v == ezTempHashedString("HHH"));

    EZ_TEST_BOOL(v.IsNumber() == false);
    EZ_TEST_BOOL(v.IsString() == false);
    EZ_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezVariantArray")
  {
    ezVariantArray a, a2;
    a.PushBack("This");
    a.PushBack("is a");
    a.PushBack("test");

    ezVariant va(a);
    EZ_TEST_BOOL(va.IsValid());
    EZ_TEST_BOOL(va.GetType() == ezVariant::Type::VariantArray);
    EZ_TEST_BOOL(va.IsA<ezVariantArray>());
    EZ_TEST_BOOL(va.GetReflectedType() == nullptr);

    const ezArrayPtr<const ezVariant>& b = va.Get<ezVariantArray>();
    ezArrayPtr<const ezVariant> b2 = va.Get<ezVariantArray>();

    EZ_TEST_BOOL(a == b);
    EZ_TEST_BOOL(a == b2);

    EZ_TEST_BOOL(a != a2);

    EZ_TEST_BOOL(va == a);
    EZ_TEST_BOOL(va != a2);

    EZ_TEST_BOOL(va[0] == ezString("This"));
    EZ_TEST_BOOL(va[1] == ezString("is a"));
    EZ_TEST_BOOL(va[2] == ezString("test"));
    EZ_TEST_BOOL(va[4] == ezVariant());
    EZ_TEST_BOOL(!va[4].IsValid());

    EZ_TEST_BOOL(va.IsNumber() == false);
    EZ_TEST_BOOL(!va.IsString());
    EZ_TEST_BOOL(va.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezVariantDictionary")
  {
    ezVariantDictionary a, a2;
    a["my"] = true;
    a["luv"] = 4;
    a["pon"] = "ies";

    ezVariant va(a);
    EZ_TEST_BOOL(va.IsValid());
    EZ_TEST_BOOL(va.GetType() == ezVariant::Type::VariantDictionary);
    EZ_TEST_BOOL(va.IsA<ezVariantDictionary>());
    EZ_TEST_BOOL(va.GetReflectedType() == nullptr);

    const ezVariantDictionary& d1 = va.Get<ezVariantDictionary>();
    ezVariantDictionary d2 = va.Get<ezVariantDictionary>();

    EZ_TEST_BOOL(a == d1);
    EZ_TEST_BOOL(a == d2);
    EZ_TEST_BOOL(d1 == d2);

    EZ_TEST_BOOL(va == a);
    EZ_TEST_BOOL(va != a2);

    EZ_TEST_BOOL(va["my"] == true);
    EZ_TEST_BOOL(va["luv"] == 4);
    EZ_TEST_BOOL(va["pon"] == ezString("ies"));
    EZ_TEST_BOOL(va["x"] == ezVariant());
    EZ_TEST_BOOL(!va["x"].IsValid());

    EZ_TEST_BOOL(va.IsNumber() == false);
    EZ_TEST_BOOL(!va.IsString());
    EZ_TEST_BOOL(va.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezTypedPointer")
  {
    Blubb blubb;
    blubb.u = 1.0f;
    blubb.v = 2.0f;

    Blubb blubb2;

    ezVariant v(&blubb);

    EZ_TEST_BOOL(v.IsValid());
    EZ_TEST_BOOL(v.GetType() == ezVariant::Type::TypedPointer);
    EZ_TEST_BOOL(v.IsA<Blubb*>());
    EZ_TEST_BOOL(v.Get<Blubb*>() == &blubb);
    EZ_TEST_BOOL(v.IsA<ezReflectedClass*>());
    EZ_TEST_BOOL(v.Get<ezReflectedClass*>() == &blubb);
    EZ_TEST_BOOL(v.Get<ezReflectedClass*>() != &blubb2);
    EZ_TEST_BOOL(ezDynamicCast<Blubb*>(v) == &blubb);
    EZ_TEST_BOOL(ezDynamicCast<ezVec3*>(v) == nullptr);
    EZ_TEST_BOOL(v.IsA<void*>());
    EZ_TEST_BOOL(v.Get<void*>() == &blubb);
    EZ_TEST_BOOL(v.IsA<const void*>());
    EZ_TEST_BOOL(v.Get<const void*>() == &blubb);
    EZ_TEST_BOOL(v.GetData() == &blubb);
    EZ_TEST_BOOL(v.IsA<ezTypedPointer>());
    EZ_TEST_BOOL(v.GetReflectedType() == ezGetStaticRTTI<Blubb>());
    EZ_TEST_BOOL(!v.IsA<ezVec3*>());

    ezTypedPointer ptr = v.Get<ezTypedPointer>();
    EZ_TEST_BOOL(ptr.m_pObject == &blubb);
    EZ_TEST_BOOL(ptr.m_pType == ezGetStaticRTTI<Blubb>());

    ezTypedPointer ptr2 = v.GetWriteAccess();
    EZ_TEST_BOOL(ptr2.m_pObject == &blubb);
    EZ_TEST_BOOL(ptr2.m_pType == ezGetStaticRTTI<Blubb>());

    EZ_TEST_BOOL(v[0] == 1.0f);
    EZ_TEST_BOOL(v[1] == 2.0f);
    EZ_TEST_BOOL(v["u"] == 1.0f);
    EZ_TEST_BOOL(v["v"] == 2.0f);
    ezVariant v2 = &blubb;
    EZ_TEST_BOOL(v == v2);
    ezVariant v3 = ptr;
    EZ_TEST_BOOL(v == v3);

    EZ_TEST_BOOL(v.IsNumber() == false);
    EZ_TEST_BOOL(!v.IsString());
    EZ_TEST_BOOL(v.IsFloatingPoint() == false);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezTypedPointer nullptr")
  {
    ezTypedPointer ptr = {nullptr, ezGetStaticRTTI<Blubb>()};
    ezVariant v = ptr;
    EZ_TEST_BOOL(v.IsValid());
    EZ_TEST_BOOL(v.GetType() == ezVariant::Type::TypedPointer);
    EZ_TEST_BOOL(v.IsA<Blubb*>());
    EZ_TEST_BOOL(v.Get<Blubb*>() == nullptr);
    EZ_TEST_BOOL(v.IsA<ezReflectedClass*>());
    EZ_TEST_BOOL(v.Get<ezReflectedClass*>() == nullptr);
    EZ_TEST_BOOL(ezDynamicCast<Blubb*>(v) == nullptr);
    EZ_TEST_BOOL(ezDynamicCast<ezVec3*>(v) == nullptr);
    EZ_TEST_BOOL(v.IsA<void*>());
    EZ_TEST_BOOL(v.Get<void*>() == nullptr);
    EZ_TEST_BOOL(v.IsA<const void*>());
    EZ_TEST_BOOL(v.Get<const void*>() == nullptr);
    EZ_TEST_BOOL(v.IsA<ezTypedPointer>());
    EZ_TEST_BOOL(v.GetReflectedType() == ezGetStaticRTTI<Blubb>());
    EZ_TEST_BOOL(!v.IsA<ezVec3*>());

    ezTypedPointer ptr2 = v.Get<ezTypedPointer>();
    EZ_TEST_BOOL(ptr2.m_pObject == nullptr);
    EZ_TEST_BOOL(ptr2.m_pType == ezGetStaticRTTI<Blubb>());

    EZ_TEST_BOOL(!v[0].IsValid());
    EZ_TEST_BOOL(!v["u"].IsValid());
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezTypedObject inline")
  {
    // ezAngle::MakeFromDegree(90.0f) was replaced with radian as release builds generate a different float then debug.
    ezVarianceTypeAngle value = {0.1f, ezAngle::MakeFromRadian(1.57079637f)};
    ezVarianceTypeAngle value2 = {0.2f, ezAngle::MakeFromRadian(1.57079637f)};

    ezVariant v(value);
    TestVariant<ezVarianceTypeAngle>(v, ezVariantType::TypedObject);

    EZ_TEST_BOOL(v.IsA<ezTypedObject>());
    EZ_TEST_BOOL(!v.IsA<void*>());
    EZ_TEST_BOOL(!v.IsA<const void*>());
    EZ_TEST_BOOL(!v.IsA<ezVec3*>());
    EZ_TEST_BOOL(ezDynamicCast<ezVec3*>(v) == nullptr);

    const ezVarianceTypeAngle& valueGet = v.Get<ezVarianceTypeAngle>();
    EZ_TEST_BOOL(value == valueGet);

    ezVariant va = value;
    EZ_TEST_BOOL(v == va);

    ezVariant v2 = value2;
    EZ_TEST_BOOL(v != v2);

    ezUInt64 uiHash = v.ComputeHash(0);
    EZ_TEST_INT(uiHash, 8527525522777555267ul);

    ezVarianceTypeAngle* pTypedAngle = EZ_DEFAULT_NEW(ezVarianceTypeAngle, {0.1f, ezAngle::MakeFromRadian(1.57079637f)});
    ezVariant copy;
    copy.CopyTypedObject(pTypedAngle, ezGetStaticRTTI<ezVarianceTypeAngle>());
    ezVariant move;
    move.MoveTypedObject(pTypedAngle, ezGetStaticRTTI<ezVarianceTypeAngle>());
    EZ_TEST_BOOL(v == copy);
    EZ_TEST_BOOL(v == move);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezTypedObject shared")
  {
    ezTypedObjectStruct data;
    ezVariant v = data;
    EZ_TEST_BOOL(v.IsValid());
    EZ_TEST_BOOL(v.GetType() == ezVariant::Type::TypedObject);
    EZ_TEST_BOOL(v.IsA<ezTypedObject>());
    EZ_TEST_BOOL(v.IsA<ezTypedObjectStruct>());
    EZ_TEST_BOOL(!v.IsA<void*>());
    EZ_TEST_BOOL(!v.IsA<const void*>());
    EZ_TEST_BOOL(!v.IsA<ezVec3*>());
    EZ_TEST_BOOL(ezDynamicCast<ezVec3*>(v) == nullptr);
    EZ_TEST_BOOL(v.GetReflectedType() == ezGetStaticRTTI<ezTypedObjectStruct>());

    ezVariant v2 = v;

    ezTypedPointer ptr = v.GetWriteAccess();
    EZ_TEST_BOOL(ptr.m_pObject == &v.Get<ezTypedObjectStruct>());
    EZ_TEST_BOOL(ptr.m_pObject == &v.GetWritable<ezTypedObjectStruct>());
    EZ_TEST_BOOL(ptr.m_pObject != &v2.Get<ezTypedObjectStruct>());
    EZ_TEST_BOOL(ptr.m_pType == ezGetStaticRTTI<ezTypedObjectStruct>());

    EZ_TEST_BOOL(ezReflectionUtils::IsEqual(ptr.m_pObject, &v2.Get<ezTypedObjectStruct>(), ezGetStaticRTTI<ezTypedObjectStruct>()));
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (bool)")
  {
    ezVariant v(true);

    EZ_TEST_BOOL(v.CanConvertTo<bool>());
    EZ_TEST_BOOL(v.CanConvertTo<ezInt32>());

    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Invalid) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Bool));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Int8));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::UInt8));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Int16));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::UInt16));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Int32));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::UInt32));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Int64));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::UInt64));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Float));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Double));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Color) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector2) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector3) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector4) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector2I) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector3I) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector4I) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Quaternion) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Matrix3) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Matrix4) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::String));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::StringView) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::DataBuffer) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Time) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Angle) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::VariantArray) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::VariantDictionary) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::TypedPointer) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::TypedObject) == false);

    EZ_TEST_BOOL(v.ConvertTo<bool>() == true);
    EZ_TEST_BOOL(v.ConvertTo<ezInt8>() == 1);
    EZ_TEST_BOOL(v.ConvertTo<ezUInt8>() == 1);
    EZ_TEST_BOOL(v.ConvertTo<ezInt16>() == 1);
    EZ_TEST_BOOL(v.ConvertTo<ezUInt16>() == 1);
    EZ_TEST_BOOL(v.ConvertTo<ezInt32>() == 1);
    EZ_TEST_BOOL(v.ConvertTo<ezUInt32>() == 1);
    EZ_TEST_BOOL(v.ConvertTo<ezInt64>() == 1);
    EZ_TEST_BOOL(v.ConvertTo<ezUInt64>() == 1);
    EZ_TEST_BOOL(v.ConvertTo<float>() == 1.0f);
    EZ_TEST_BOOL(v.ConvertTo<double>() == 1.0);
    EZ_TEST_BOOL(v.ConvertTo<ezString>() == "true");
    EZ_TEST_BOOL(v.ConvertTo<ezHashedString>() == ezMakeHashedString("true"));
    EZ_TEST_BOOL(v.ConvertTo<ezTempHashedString>() == ezTempHashedString("true"));

    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Bool).Get<bool>() == true);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Int8).Get<ezInt8>() == 1);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::UInt8).Get<ezUInt8>() == 1);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Int16).Get<ezInt16>() == 1);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::UInt16).Get<ezUInt16>() == 1);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Int32).Get<ezInt32>() == 1);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::UInt32).Get<ezUInt32>() == 1);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Int64).Get<ezInt64>() == 1);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::UInt64).Get<ezUInt64>() == 1);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Float).Get<float>() == 1.0f);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Double).Get<double>() == 1.0);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::String).Get<ezString>() == "true");
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::HashedString).Get<ezHashedString>() == ezMakeHashedString("true"));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::TempHashedString).Get<ezTempHashedString>() == ezTempHashedString("true"));
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezInt8)")
  {
    ezVariant v((ezInt8)3);
    TestNumberCanConvertTo(v);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezUInt8)")
  {
    ezVariant v((ezUInt8)3);
    TestNumberCanConvertTo(v);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezInt16)")
  {
    ezVariant v((ezInt16)3);
    TestNumberCanConvertTo(v);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezUInt16)")
  {
    ezVariant v((ezUInt16)3);
    TestNumberCanConvertTo(v);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezInt32)")
  {
    ezVariant v((ezInt32)3);
    TestNumberCanConvertTo(v);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezUInt32)")
  {
    ezVariant v((ezUInt32)3);
    TestNumberCanConvertTo(v);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezInt64)")
  {
    ezVariant v((ezInt64)3);
    TestNumberCanConvertTo(v);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezUInt64)")
  {
    ezVariant v((ezUInt64)3);
    TestNumberCanConvertTo(v);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (float)")
  {
    ezVariant v((float)3.0f);
    TestNumberCanConvertTo(v);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (double)")
  {
    ezVariant v((double)3.0f);
    TestNumberCanConvertTo(v);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (Color)")
  {
    ezColor c(3, 3, 4, 0);
    ezVariant v(c);

    TestCanOnlyConvertToStringAndID(v, ezVariant::Type::Color, ezVariant::Type::ColorGamma);

    ezResult conversionResult = EZ_FAILURE;
    EZ_TEST_BOOL(v.ConvertTo<ezColor>(&conversionResult) == c);
    EZ_TEST_BOOL(conversionResult.Succeeded());

    EZ_TEST_BOOL(v.ConvertTo<ezString>(&conversionResult) == "{ r=3, g=3, b=4, a=0 }");
    EZ_TEST_BOOL(conversionResult.Succeeded());

    EZ_TEST_BOOL(v.ConvertTo<ezHashedString>() == ezMakeHashedString("{ r=3, g=3, b=4, a=0 }"));
    EZ_TEST_BOOL(v.ConvertTo<ezTempHashedString>() == ezTempHashedString("{ r=3, g=3, b=4, a=0 }"));

    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Color).Get<ezColor>() == c);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::String).Get<ezString>() == "{ r=3, g=3, b=4, a=0 }");
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::HashedString).Get<ezHashedString>() == ezMakeHashedString("{ r=3, g=3, b=4, a=0 }"));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::TempHashedString).Get<ezTempHashedString>() == ezTempHashedString("{ r=3, g=3, b=4, a=0 }"));
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ColorGamma)")
  {
    ezColorGammaUB c(0, 128, 64, 255);
    ezVariant v(c);

    TestCanOnlyConvertToStringAndID(v, ezVariant::Type::ColorGamma, ezVariant::Type::Color);

    ezResult conversionResult = EZ_FAILURE;
    EZ_TEST_BOOL(v.ConvertTo<ezColorGammaUB>(&conversionResult) == c);
    EZ_TEST_BOOL(conversionResult.Succeeded());

    ezString val = v.ConvertTo<ezString>(&conversionResult);
    EZ_TEST_BOOL(val == "{ r=0, g=128, b=64, a=255 }");
    EZ_TEST_BOOL(conversionResult.Succeeded());

    EZ_TEST_BOOL(v.ConvertTo<ezHashedString>() == ezMakeHashedString("{ r=0, g=128, b=64, a=255 }"));
    EZ_TEST_BOOL(v.ConvertTo<ezTempHashedString>() == ezTempHashedString("{ r=0, g=128, b=64, a=255 }"));

    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::ColorGamma).Get<ezColorGammaUB>() == c);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::String).Get<ezString>() == "{ r=0, g=128, b=64, a=255 }");
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::HashedString).Get<ezHashedString>() == ezMakeHashedString("{ r=0, g=128, b=64, a=255 }"));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::TempHashedString).Get<ezTempHashedString>() == ezTempHashedString("{ r=0, g=128, b=64, a=255 }"));
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezVec2)")
  {
    ezVec2 vec(3.0f, 4.0f);
    ezVariant v(vec);

    TestCanOnlyConvertToStringAndID(v, ezVariant::Type::Vector2, ezVariant::Type::Vector2I, ezVariant::Type::Vector2U);

    EZ_TEST_BOOL(v.ConvertTo<ezVec2>() == vec);
    EZ_TEST_BOOL(v.ConvertTo<ezVec2I32>() == ezVec2I32(3, 4));
    EZ_TEST_BOOL(v.ConvertTo<ezVec2U32>() == ezVec2U32(3, 4));
    EZ_TEST_BOOL(v.ConvertTo<ezString>() == "{ x=3, y=4 }");
    EZ_TEST_BOOL(v.ConvertTo<ezHashedString>() == ezMakeHashedString("{ x=3, y=4 }"));
    EZ_TEST_BOOL(v.ConvertTo<ezTempHashedString>() == ezTempHashedString("{ x=3, y=4 }"));

    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Vector2).Get<ezVec2>() == vec);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Vector2I).Get<ezVec2I32>() == ezVec2I32(3, 4));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Vector2U).Get<ezVec2U32>() == ezVec2U32(3, 4));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::String).Get<ezString>() == "{ x=3, y=4 }");
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::HashedString).Get<ezHashedString>() == ezMakeHashedString("{ x=3, y=4 }"));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::TempHashedString).Get<ezTempHashedString>() == ezTempHashedString("{ x=3, y=4 }"));
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezVec3)")
  {
    ezVec3 vec(3.0f, 4.0f, 6.0f);
    ezVariant v(vec);

    TestCanOnlyConvertToStringAndID(v, ezVariant::Type::Vector3, ezVariant::Type::Vector3I, ezVariant::Type::Vector3U);

    EZ_TEST_BOOL(v.ConvertTo<ezVec3>() == vec);
    EZ_TEST_BOOL(v.ConvertTo<ezVec3I32>() == ezVec3I32(3, 4, 6));
    EZ_TEST_BOOL(v.ConvertTo<ezVec3U32>() == ezVec3U32(3, 4, 6));
    EZ_TEST_BOOL(v.ConvertTo<ezString>() == "{ x=3, y=4, z=6 }");
    EZ_TEST_BOOL(v.ConvertTo<ezHashedString>() == ezMakeHashedString("{ x=3, y=4, z=6 }"));
    EZ_TEST_BOOL(v.ConvertTo<ezTempHashedString>() == ezTempHashedString("{ x=3, y=4, z=6 }"));

    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Vector3).Get<ezVec3>() == vec);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Vector3I).Get<ezVec3I32>() == ezVec3I32(3, 4, 6));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Vector3U).Get<ezVec3U32>() == ezVec3U32(3, 4, 6));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::String).Get<ezString>() == "{ x=3, y=4, z=6 }");
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::HashedString).Get<ezHashedString>() == ezMakeHashedString("{ x=3, y=4, z=6 }"));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::TempHashedString).Get<ezTempHashedString>() == ezTempHashedString("{ x=3, y=4, z=6 }"));
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezVec4)")
  {
    ezVec4 vec(3.0f, 4.0f, 3, 56);
    ezVariant v(vec);

    TestCanOnlyConvertToStringAndID(v, ezVariant::Type::Vector4, ezVariant::Type::Vector4I, ezVariant::Type::Vector4U);

    EZ_TEST_BOOL(v.ConvertTo<ezVec4>() == vec);
    EZ_TEST_BOOL(v.ConvertTo<ezVec4I32>() == ezVec4I32(3, 4, 3, 56));
    EZ_TEST_BOOL(v.ConvertTo<ezVec4U32>() == ezVec4U32(3, 4, 3, 56));
    EZ_TEST_BOOL(v.ConvertTo<ezString>() == "{ x=3, y=4, z=3, w=56 }");
    EZ_TEST_BOOL(v.ConvertTo<ezHashedString>() == ezMakeHashedString("{ x=3, y=4, z=3, w=56 }"));
    EZ_TEST_BOOL(v.ConvertTo<ezTempHashedString>() == ezTempHashedString("{ x=3, y=4, z=3, w=56 }"));

    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Vector4).Get<ezVec4>() == vec);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Vector4I).Get<ezVec4I32>() == ezVec4I32(3, 4, 3, 56));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Vector4U).Get<ezVec4U32>() == ezVec4U32(3, 4, 3, 56));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::String).Get<ezString>() == "{ x=3, y=4, z=3, w=56 }");
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::HashedString).Get<ezHashedString>() == ezMakeHashedString("{ x=3, y=4, z=3, w=56 }"));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::TempHashedString).Get<ezTempHashedString>() == ezTempHashedString("{ x=3, y=4, z=3, w=56 }"));
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezVec2I32)")
  {
    ezVec2I32 vec(3, 4);
    ezVariant v(vec);

    TestCanOnlyConvertToStringAndID(v, ezVariant::Type::Vector2I, ezVariant::Type::Vector2U, ezVariant::Type::Vector2);

    EZ_TEST_BOOL(v.ConvertTo<ezVec2I32>() == vec);
    EZ_TEST_BOOL(v.ConvertTo<ezVec2>() == ezVec2(3, 4));
    EZ_TEST_BOOL(v.ConvertTo<ezVec2U32>() == ezVec2U32(3, 4));
    EZ_TEST_BOOL(v.ConvertTo<ezString>() == "{ x=3, y=4 }");
    EZ_TEST_BOOL(v.ConvertTo<ezHashedString>() == ezMakeHashedString("{ x=3, y=4 }"));
    EZ_TEST_BOOL(v.ConvertTo<ezTempHashedString>() == ezTempHashedString("{ x=3, y=4 }"));

    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Vector2I).Get<ezVec2I32>() == vec);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Vector2).Get<ezVec2>() == ezVec2(3, 4));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Vector2U).Get<ezVec2U32>() == ezVec2U32(3, 4));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::String).Get<ezString>() == "{ x=3, y=4 }");
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::HashedString).Get<ezHashedString>() == ezMakeHashedString("{ x=3, y=4 }"));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::TempHashedString).Get<ezTempHashedString>() == ezTempHashedString("{ x=3, y=4 }"));
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezVec3I32)")
  {
    ezVec3I32 vec(3, 4, 6);
    ezVariant v(vec);

    TestCanOnlyConvertToStringAndID(v, ezVariant::Type::Vector3I, ezVariant::Type::Vector3U, ezVariant::Type::Vector3);

    EZ_TEST_BOOL(v.ConvertTo<ezVec3I32>() == vec);
    EZ_TEST_BOOL(v.ConvertTo<ezVec3>() == ezVec3(3, 4, 6));
    EZ_TEST_BOOL(v.ConvertTo<ezVec3U32>() == ezVec3U32(3, 4, 6));
    EZ_TEST_BOOL(v.ConvertTo<ezString>() == "{ x=3, y=4, z=6 }");
    EZ_TEST_BOOL(v.ConvertTo<ezHashedString>() == ezMakeHashedString("{ x=3, y=4, z=6 }"));
    EZ_TEST_BOOL(v.ConvertTo<ezTempHashedString>() == ezTempHashedString("{ x=3, y=4, z=6 }"));

    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Vector3I).Get<ezVec3I32>() == vec);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Vector3).Get<ezVec3>() == ezVec3(3, 4, 6));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Vector3U).Get<ezVec3U32>() == ezVec3U32(3, 4, 6));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::String).Get<ezString>() == "{ x=3, y=4, z=6 }");
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::HashedString).Get<ezHashedString>() == ezMakeHashedString("{ x=3, y=4, z=6 }"));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::TempHashedString).Get<ezTempHashedString>() == ezTempHashedString("{ x=3, y=4, z=6 }"));
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezVec4I32)")
  {
    ezVec4I32 vec(3, 4, 3, 56);
    ezVariant v(vec);

    TestCanOnlyConvertToStringAndID(v, ezVariant::Type::Vector4I, ezVariant::Type::Vector4U, ezVariant::Type::Vector4);

    EZ_TEST_BOOL(v.ConvertTo<ezVec4I32>() == vec);
    EZ_TEST_BOOL(v.ConvertTo<ezVec4>() == ezVec4(3, 4, 3, 56));
    EZ_TEST_BOOL(v.ConvertTo<ezVec4U32>() == ezVec4U32(3, 4, 3, 56));
    EZ_TEST_BOOL(v.ConvertTo<ezString>() == "{ x=3, y=4, z=3, w=56 }");
    EZ_TEST_BOOL(v.ConvertTo<ezHashedString>() == ezMakeHashedString("{ x=3, y=4, z=3, w=56 }"));
    EZ_TEST_BOOL(v.ConvertTo<ezTempHashedString>() == ezTempHashedString("{ x=3, y=4, z=3, w=56 }"));

    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Vector4I).Get<ezVec4I32>() == vec);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Vector4).Get<ezVec4>() == ezVec4(3, 4, 3, 56));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Vector4U).Get<ezVec4U32>() == ezVec4U32(3, 4, 3, 56));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::String).Get<ezString>() == "{ x=3, y=4, z=3, w=56 }");
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::HashedString).Get<ezHashedString>() == ezMakeHashedString("{ x=3, y=4, z=3, w=56 }"));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::TempHashedString).Get<ezTempHashedString>() == ezTempHashedString("{ x=3, y=4, z=3, w=56 }"));
  }
  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezQuat)")
  {
    ezQuat q(3.0f, 4.0f, 3, 56);
    ezVariant v(q);

    TestCanOnlyConvertToStringAndID(v, ezVariant::Type::Quaternion);

    EZ_TEST_BOOL(v.ConvertTo<ezQuat>() == q);
    EZ_TEST_BOOL(v.ConvertTo<ezString>() == "{ x=3, y=4, z=3, w=56 }");
    EZ_TEST_BOOL(v.ConvertTo<ezHashedString>() == ezMakeHashedString("{ x=3, y=4, z=3, w=56 }"));
    EZ_TEST_BOOL(v.ConvertTo<ezTempHashedString>() == ezTempHashedString("{ x=3, y=4, z=3, w=56 }"));

    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Quaternion).Get<ezQuat>() == q);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::String).Get<ezString>() == "{ x=3, y=4, z=3, w=56 }");
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::HashedString).Get<ezHashedString>() == ezMakeHashedString("{ x=3, y=4, z=3, w=56 }"));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::TempHashedString).Get<ezTempHashedString>() == ezTempHashedString("{ x=3, y=4, z=3, w=56 }"));
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezMat3)")
  {
    ezMat3 m = ezMat3::MakeFromValues(1, 2, 3, 4, 5, 6, 7, 8, 9);
    ezVariant v(m);

    TestCanOnlyConvertToStringAndID(v, ezVariant::Type::Matrix3);

    EZ_TEST_BOOL(v.ConvertTo<ezMat3>() == m);
    EZ_TEST_BOOL(v.ConvertTo<ezString>() == "{ c1r1=1, c2r1=2, c3r1=3, c1r2=4, c2r2=5, c3r2=6, c1r3=7, c2r3=8, c3r3=9 }");
    EZ_TEST_BOOL(v.ConvertTo<ezHashedString>() == ezMakeHashedString("{ c1r1=1, c2r1=2, c3r1=3, c1r2=4, c2r2=5, c3r2=6, c1r3=7, c2r3=8, c3r3=9 }"));
    EZ_TEST_BOOL(v.ConvertTo<ezTempHashedString>() == ezTempHashedString("{ c1r1=1, c2r1=2, c3r1=3, c1r2=4, c2r2=5, c3r2=6, c1r3=7, c2r3=8, c3r3=9 }"));

    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Matrix3).Get<ezMat3>() == m);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::String).Get<ezString>() == "{ c1r1=1, c2r1=2, c3r1=3, c1r2=4, c2r2=5, c3r2=6, c1r3=7, c2r3=8, c3r3=9 }");
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::HashedString).Get<ezHashedString>() == ezMakeHashedString("{ c1r1=1, c2r1=2, c3r1=3, c1r2=4, c2r2=5, c3r2=6, c1r3=7, c2r3=8, c3r3=9 }"));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::TempHashedString).Get<ezTempHashedString>() == ezTempHashedString("{ c1r1=1, c2r1=2, c3r1=3, c1r2=4, c2r2=5, c3r2=6, c1r3=7, c2r3=8, c3r3=9 }"));
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezMat4)")
  {
    ezMat4 m = ezMat4::MakeFromValues(1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6);
    ezVariant v(m);

    TestCanOnlyConvertToStringAndID(v, ezVariant::Type::Matrix4);

    EZ_TEST_BOOL(v.ConvertTo<ezMat4>() == m);
    EZ_TEST_BOOL(v.ConvertTo<ezString>() == "{ c1r1=1, c2r1=2, c3r1=3, c4r1=4, "
                                            "c1r2=5, c2r2=6, c3r2=7, c4r2=8, "
                                            "c1r3=9, c2r3=0, c3r3=1, c4r3=2, "
                                            "c1r4=3, c2r4=4, c3r4=5, c4r4=6 }");
    EZ_TEST_BOOL(v.ConvertTo<ezHashedString>() == ezMakeHashedString("{ c1r1=1, c2r1=2, c3r1=3, c4r1=4, "
                                                                     "c1r2=5, c2r2=6, c3r2=7, c4r2=8, "
                                                                     "c1r3=9, c2r3=0, c3r3=1, c4r3=2, "
                                                                     "c1r4=3, c2r4=4, c3r4=5, c4r4=6 }"));
    EZ_TEST_BOOL(v.ConvertTo<ezTempHashedString>() == ezTempHashedString("{ c1r1=1, c2r1=2, c3r1=3, c4r1=4, "
                                                                         "c1r2=5, c2r2=6, c3r2=7, c4r2=8, "
                                                                         "c1r3=9, c2r3=0, c3r3=1, c4r3=2, "
                                                                         "c1r4=3, c2r4=4, c3r4=5, c4r4=6 }"));

    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Matrix4).Get<ezMat4>() == m);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::String).Get<ezString>() == "{ c1r1=1, c2r1=2, c3r1=3, c4r1=4, "
                                                                         "c1r2=5, c2r2=6, c3r2=7, c4r2=8, "
                                                                         "c1r3=9, c2r3=0, c3r3=1, c4r3=2, "
                                                                         "c1r4=3, c2r4=4, c3r4=5, c4r4=6 }");
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::HashedString).Get<ezHashedString>() == ezMakeHashedString("{ c1r1=1, c2r1=2, c3r1=3, c4r1=4, "
                                                                                                        "c1r2=5, c2r2=6, c3r2=7, c4r2=8, "
                                                                                                        "c1r3=9, c2r3=0, c3r3=1, c4r3=2, "
                                                                                                        "c1r4=3, c2r4=4, c3r4=5, c4r4=6 }"));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::TempHashedString).Get<ezTempHashedString>() == ezTempHashedString("{ c1r1=1, c2r1=2, c3r1=3, c4r1=4, "
                                                                                                                "c1r2=5, c2r2=6, c3r2=7, c4r2=8, "
                                                                                                                "c1r3=9, c2r3=0, c3r3=1, c4r3=2, "
                                                                                                                "c1r4=3, c2r4=4, c3r4=5, c4r4=6 }"));
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezString)")
  {
    ezVariant v("ich hab keine Lust mehr");

    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Invalid) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Bool));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Int8));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::UInt8));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Int16));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::UInt16));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Int32));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::UInt32));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Int64));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::UInt64));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Float));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Double));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Color) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector2) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector3) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector4) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector2I) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector3I) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector4I) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Quaternion) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Matrix3) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Matrix4) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::String));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::StringView));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::DataBuffer) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Time) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Angle) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::ColorGamma) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::HashedString));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::TempHashedString));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::VariantArray) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::VariantDictionary) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::TypedPointer) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::TypedObject) == false);

    {
      ezResult ConversionStatus = EZ_SUCCESS;
      EZ_TEST_BOOL(v.ConvertTo<bool>(&ConversionStatus) == false);
      EZ_TEST_BOOL(ConversionStatus == EZ_FAILURE);

      ConversionStatus = EZ_SUCCESS;
      EZ_TEST_BOOL(v.ConvertTo<ezInt8>(&ConversionStatus) == 0);
      EZ_TEST_BOOL(ConversionStatus == EZ_FAILURE);

      ConversionStatus = EZ_SUCCESS;
      EZ_TEST_BOOL(v.ConvertTo<ezUInt8>(&ConversionStatus) == 0);
      EZ_TEST_BOOL(ConversionStatus == EZ_FAILURE);

      ConversionStatus = EZ_SUCCESS;
      EZ_TEST_BOOL(v.ConvertTo<ezInt16>(&ConversionStatus) == 0);
      EZ_TEST_BOOL(ConversionStatus == EZ_FAILURE);

      ConversionStatus = EZ_SUCCESS;
      EZ_TEST_BOOL(v.ConvertTo<ezUInt16>(&ConversionStatus) == 0);
      EZ_TEST_BOOL(ConversionStatus == EZ_FAILURE);

      ConversionStatus = EZ_SUCCESS;
      EZ_TEST_BOOL(v.ConvertTo<ezInt32>(&ConversionStatus) == 0);
      EZ_TEST_BOOL(ConversionStatus == EZ_FAILURE);

      ConversionStatus = EZ_SUCCESS;
      EZ_TEST_BOOL(v.ConvertTo<ezUInt32>(&ConversionStatus) == 0);
      EZ_TEST_BOOL(ConversionStatus == EZ_FAILURE);

      ConversionStatus = EZ_SUCCESS;
      EZ_TEST_BOOL(v.ConvertTo<ezInt64>(&ConversionStatus) == 0);
      EZ_TEST_BOOL(ConversionStatus == EZ_FAILURE);

      ConversionStatus = EZ_SUCCESS;
      EZ_TEST_BOOL(v.ConvertTo<ezUInt64>(&ConversionStatus) == 0);
      EZ_TEST_BOOL(ConversionStatus == EZ_FAILURE);

      ConversionStatus = EZ_SUCCESS;
      EZ_TEST_BOOL(v.ConvertTo<float>(&ConversionStatus) == 0.0f);
      EZ_TEST_BOOL(ConversionStatus == EZ_FAILURE);

      ConversionStatus = EZ_SUCCESS;
      EZ_TEST_BOOL(v.ConvertTo<double>(&ConversionStatus) == 0.0);
      EZ_TEST_BOOL(ConversionStatus == EZ_FAILURE);

      ConversionStatus = EZ_SUCCESS;
      EZ_TEST_BOOL(v.ConvertTo<ezHashedString>(&ConversionStatus) == ezMakeHashedString("ich hab keine Lust mehr"));
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

      ConversionStatus = EZ_SUCCESS;
      EZ_TEST_BOOL(v.ConvertTo<ezTempHashedString>(&ConversionStatus) == ezTempHashedString("ich hab keine Lust mehr"));
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);
    }

    {
      v = "true";
      ezResult ConversionStatus = EZ_FAILURE;
      EZ_TEST_BOOL(v.ConvertTo<bool>(&ConversionStatus) == true);
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

      ConversionStatus = EZ_FAILURE;
      EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Bool, &ConversionStatus).Get<bool>() == true);
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);
    }

    {
      v = "-128";
      ezResult ConversionStatus = EZ_FAILURE;
      EZ_TEST_BOOL(v.ConvertTo<ezInt8>(&ConversionStatus) == -128);
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

      ConversionStatus = EZ_FAILURE;
      EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Int8, &ConversionStatus).Get<ezInt8>() == -128);
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);
    }

    {
      v = "255";
      ezResult ConversionStatus = EZ_FAILURE;
      EZ_TEST_BOOL(v.ConvertTo<ezUInt8>(&ConversionStatus) == 255);
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

      ConversionStatus = EZ_FAILURE;
      EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::UInt8, &ConversionStatus).Get<ezUInt8>() == 255);
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);
    }

    {
      v = "-5643";
      ezResult ConversionStatus = EZ_FAILURE;
      EZ_TEST_BOOL(v.ConvertTo<ezInt16>(&ConversionStatus) == -5643);
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

      ConversionStatus = EZ_FAILURE;
      EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Int16, &ConversionStatus).Get<ezInt16>() == -5643);
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);
    }

    {
      v = "9001";
      ezResult ConversionStatus = EZ_FAILURE;
      EZ_TEST_BOOL(v.ConvertTo<ezUInt16>(&ConversionStatus) == 9001);
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

      ConversionStatus = EZ_FAILURE;
      EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::UInt16, &ConversionStatus).Get<ezUInt16>() == 9001);
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);
    }

    {
      v = "46";
      ezResult ConversionStatus = EZ_FAILURE;
      EZ_TEST_BOOL(v.ConvertTo<ezInt32>(&ConversionStatus) == 46);
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

      ConversionStatus = EZ_FAILURE;
      EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Int32, &ConversionStatus).Get<ezInt32>() == 46);
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);
    }

    {
      v = "356";
      ezResult ConversionStatus = EZ_FAILURE;
      EZ_TEST_BOOL(v.ConvertTo<ezUInt32>(&ConversionStatus) == 356);
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

      ConversionStatus = EZ_FAILURE;
      EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::UInt32, &ConversionStatus).Get<ezUInt32>() == 356);
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);
    }

    {
      v = "64";
      ezResult ConversionStatus = EZ_FAILURE;
      EZ_TEST_BOOL(v.ConvertTo<ezInt64>(&ConversionStatus) == 64);
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

      ConversionStatus = EZ_FAILURE;
      EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Int64, &ConversionStatus).Get<ezInt64>() == 64);
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);
    }

    {
      v = "6464";
      ezResult ConversionStatus = EZ_FAILURE;
      EZ_TEST_BOOL(v.ConvertTo<ezUInt64>(&ConversionStatus) == 6464);
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

      ConversionStatus = EZ_FAILURE;
      EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::UInt64, &ConversionStatus).Get<ezUInt64>() == 6464);
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);
    }

    {
      v = "0.07564f";
      ezResult ConversionStatus = EZ_FAILURE;
      EZ_TEST_BOOL(v.ConvertTo<float>(&ConversionStatus) == 0.07564f);
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

      ConversionStatus = EZ_FAILURE;
      EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Float, &ConversionStatus).Get<float>() == 0.07564f);
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);
    }

    {
      v = "0.4453";
      ezResult ConversionStatus = EZ_FAILURE;
      EZ_TEST_BOOL(v.ConvertTo<double>(&ConversionStatus) == 0.4453);
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

      ConversionStatus = EZ_FAILURE;
      EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Double, &ConversionStatus).Get<double>() == 0.4453);
      EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);
    }
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezStringView)")
  {
    ezStringView va0("Test String");
    ezVariant v(va0, false);

    TestCanOnlyConvertToStringAndID(v, ezVariant::Type::StringView);

    EZ_TEST_BOOL(v.ConvertTo<ezStringView>() == va0);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::StringView).Get<ezStringView>() == va0);

    {
      ezVariant va, va2;

      va = "Bla";
      EZ_TEST_BOOL(va.IsA<ezString>());
      EZ_TEST_BOOL(va.CanConvertTo<ezString>());
      EZ_TEST_BOOL(va.CanConvertTo<ezStringView>());

      va = ezVariant("Bla"_ezsv, false);
      EZ_TEST_BOOL(va.IsA<ezStringView>());
      EZ_TEST_BOOL(va.CanConvertTo<ezString>());
      EZ_TEST_BOOL(va.CanConvertTo<ezStringView>());

      va2 = va;
      EZ_TEST_BOOL(va2.IsA<ezStringView>());
      EZ_TEST_BOOL(va2.CanConvertTo<ezString>());
      EZ_TEST_BOOL(va2.CanConvertTo<ezStringView>());
      EZ_TEST_BOOL(va2.ConvertTo<ezStringView>() == "Bla");
      EZ_TEST_BOOL(va2.ConvertTo<ezString>() == "Bla");

      ezVariant va3 = va2.ConvertTo(ezVariantType::StringView);
      EZ_TEST_BOOL(va3.IsA<ezStringView>());
      EZ_TEST_BOOL(va3.ConvertTo<ezString>() == "Bla");

      va = "Blub";
      EZ_TEST_BOOL(va.IsA<ezString>());

      ezVariant va4 = va.ConvertTo(ezVariantType::StringView);
      EZ_TEST_BOOL(va4.IsA<ezStringView>());
      EZ_TEST_BOOL(va4.ConvertTo<ezString>() == "Blub");
    }
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezDataBuffer)")
  {
    ezDataBuffer va;
    va.PushBack(255);
    va.PushBack(4);
    ezVariant v(va);

    TestCanOnlyConvertToID(v, ezVariant::Type::DataBuffer);

    EZ_TEST_BOOL(v.ConvertTo<ezDataBuffer>() == va);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::DataBuffer).Get<ezDataBuffer>() == va);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezTime)")
  {
    ezTime t = ezTime::MakeFromSeconds(123.0);
    ezVariant v(t);

    TestCanOnlyConvertToStringAndID(v, ezVariant::Type::Time);

    EZ_TEST_BOOL(v.ConvertTo<ezTime>() == t);
    // EZ_TEST_BOOL(v.ConvertTo<ezString>() == "");

    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Time).Get<ezTime>() == t);
    // EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::String).Get<ezString>() == "");
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezUuid)")
  {
    const ezUuid uuid = ezUuid::MakeUuid();
    ezVariant v(uuid);

    TestCanOnlyConvertToStringAndID(v, ezVariant::Type::Uuid);

    EZ_TEST_BOOL(v.ConvertTo<ezUuid>() == uuid);
    // EZ_TEST_BOOL(v.ConvertTo<ezString>() == "");

    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Uuid).Get<ezUuid>() == uuid);
    // EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::String).Get<ezString>() == "");
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezAngle)")
  {
    ezAngle t = ezAngle::MakeFromDegree(123.0);
    ezVariant v(t);

    TestCanOnlyConvertToStringAndID(v, ezVariant::Type::Angle);

    EZ_TEST_BOOL(v.ConvertTo<ezAngle>() == t);
    EZ_TEST_BOOL(v.ConvertTo<ezString>() == "123.0°");
    // EZ_TEST_BOOL(v.ConvertTo<ezHashedString>() == ezMakeHashedString("123.0°")); // For some reason the compiler stumbles upon the degree sign, encoding weirdness most likely
    // EZ_TEST_BOOL(v.ConvertTo<ezTempHashedString>() == ezTempHashedString("123.0°"));

    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::Angle).Get<ezAngle>() == t);
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::String).Get<ezString>() == "123.0°");
    // EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::HashedString).Get<ezHashedString>() == ezMakeHashedString("123.0°"));
    // EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::TempHashedString).Get<ezTempHashedString>() == ezTempHashedString("123.0°"));
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezHashedString)")
  {
    ezVariant v(ezMakeHashedString("78"));

    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Invalid) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Bool));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Int8));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::UInt8));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Int16));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::UInt16));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Int32));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::UInt32));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Int64));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::UInt64));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Float));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Double));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Color) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector2) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector3) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector4) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector2I) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector3I) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Vector4I) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Quaternion) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Matrix3) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Matrix4) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::String));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::StringView));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::DataBuffer) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Time) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::Angle) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::ColorGamma) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::HashedString));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::TempHashedString));
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::VariantArray) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::VariantDictionary) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::TypedPointer) == false);
    EZ_TEST_BOOL(v.CanConvertTo(ezVariant::Type::TypedObject) == false);

    ezResult ConversionStatus = EZ_SUCCESS;
    EZ_TEST_BOOL(v.ConvertTo<bool>(&ConversionStatus) == false);
    EZ_TEST_BOOL(ConversionStatus == EZ_FAILURE);

    ConversionStatus = EZ_FAILURE;
    EZ_TEST_INT(v.ConvertTo<ezInt8>(&ConversionStatus), 78);
    EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

    ConversionStatus = EZ_FAILURE;
    EZ_TEST_INT(v.ConvertTo<ezUInt8>(&ConversionStatus), 78);
    EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

    ConversionStatus = EZ_FAILURE;
    EZ_TEST_INT(v.ConvertTo<ezInt16>(&ConversionStatus), 78);
    EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

    ConversionStatus = EZ_FAILURE;
    EZ_TEST_INT(v.ConvertTo<ezUInt16>(&ConversionStatus), 78);
    EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

    ConversionStatus = EZ_FAILURE;
    EZ_TEST_INT(v.ConvertTo<ezInt32>(&ConversionStatus), 78);
    EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

    ConversionStatus = EZ_FAILURE;
    EZ_TEST_INT(v.ConvertTo<ezUInt32>(&ConversionStatus), 78);
    EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

    ConversionStatus = EZ_FAILURE;
    EZ_TEST_INT(v.ConvertTo<ezInt64>(&ConversionStatus), 78);
    EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

    ConversionStatus = EZ_FAILURE;
    EZ_TEST_INT(v.ConvertTo<ezUInt64>(&ConversionStatus), 78);
    EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

    ConversionStatus = EZ_FAILURE;
    EZ_TEST_BOOL(v.ConvertTo<float>(&ConversionStatus) == 78.0f);
    EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

    ConversionStatus = EZ_FAILURE;
    EZ_TEST_BOOL(v.ConvertTo<double>(&ConversionStatus) == 78.0);
    EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

    ConversionStatus = EZ_FAILURE;
    EZ_TEST_STRING(v.ConvertTo<ezString>(&ConversionStatus), "78");
    EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

    ConversionStatus = EZ_FAILURE;
    EZ_TEST_BOOL(v.ConvertTo<ezStringView>(&ConversionStatus) == "78"_ezsv);
    EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);

    ConversionStatus = EZ_FAILURE;
    EZ_TEST_BOOL(v.ConvertTo<ezTempHashedString>(&ConversionStatus) == ezTempHashedString("78"));
    EZ_TEST_BOOL(ConversionStatus == EZ_SUCCESS);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezTempHashedString)")
  {
    ezTempHashedString s("VVVV");
    ezVariant v(s);

    TestCanOnlyConvertToStringAndID(v, ezVariant::Type::TempHashedString);

    EZ_TEST_BOOL(v.ConvertTo<ezTempHashedString>() == ezTempHashedString("VVVV"));
    EZ_TEST_BOOL(v.ConvertTo<ezString>() == "0x69d489c8b7fa5f47");

    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::TempHashedString).Get<ezTempHashedString>() == ezTempHashedString("VVVV"));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::String).Get<ezString>() == "0x69d489c8b7fa5f47");
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (VariantArray)")
  {
    ezVariantArray va;
    va.PushBack(2.5);
    va.PushBack("ABC");
    va.PushBack(ezVariant());
    ezVariant v(va);

    TestCanOnlyConvertToStringAndID(v, ezVariant::Type::VariantArray);

    EZ_TEST_BOOL(v.ConvertTo<ezVariantArray>() == va);
    EZ_TEST_STRING(v.ConvertTo<ezString>(), "[2.5, ABC, <Invalid>]");
    EZ_TEST_BOOL(v.ConvertTo<ezHashedString>() == ezMakeHashedString("[2.5, ABC, <Invalid>]"));
    EZ_TEST_BOOL(v.ConvertTo<ezTempHashedString>() == ezTempHashedString("[2.5, ABC, <Invalid>]"));

    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::VariantArray).Get<ezVariantArray>() == va);
    EZ_TEST_STRING(v.ConvertTo(ezVariant::Type::String).Get<ezString>(), "[2.5, ABC, <Invalid>]");
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::HashedString).Get<ezHashedString>() == ezMakeHashedString("[2.5, ABC, <Invalid>]"));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::TempHashedString).Get<ezTempHashedString>() == ezTempHashedString("[2.5, ABC, <Invalid>]"));
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "(Can)ConvertTo (ezVariantDictionary)")
  {
    ezVariantDictionary va;
    va.Insert("A", 2.5);
    va.Insert("B", "ABC");
    va.Insert("C", ezVariant());
    ezVariant v(va);

    TestCanOnlyConvertToStringAndID(v, ezVariant::Type::VariantDictionary);

    EZ_TEST_BOOL(v.ConvertTo<ezVariantDictionary>() == va);
    EZ_TEST_STRING(v.ConvertTo<ezString>(), "{A=2.5, C=<Invalid>, B=ABC}");
    EZ_TEST_BOOL(v.ConvertTo<ezHashedString>() == ezMakeHashedString("{A=2.5, C=<Invalid>, B=ABC}"));
    EZ_TEST_BOOL(v.ConvertTo<ezTempHashedString>() == ezTempHashedString("{A=2.5, C=<Invalid>, B=ABC}"));

    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::VariantDictionary).Get<ezVariantDictionary>() == va);
    EZ_TEST_STRING(v.ConvertTo(ezVariant::Type::String).Get<ezString>(), "{A=2.5, C=<Invalid>, B=ABC}");
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::HashedString).Get<ezHashedString>() == ezMakeHashedString("{A=2.5, C=<Invalid>, B=ABC}"));
    EZ_TEST_BOOL(v.ConvertTo(ezVariant::Type::TempHashedString).Get<ezTempHashedString>() == ezTempHashedString("{A=2.5, C=<Invalid>, B=ABC}"));
  }
}

#pragma optimize("", on)
