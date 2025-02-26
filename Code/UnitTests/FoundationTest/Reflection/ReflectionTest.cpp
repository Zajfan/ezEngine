#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/IO/MemoryStream.h>
#include <Foundation/Reflection/ReflectionUtils.h>
#include <Foundation/Serialization/ReflectionSerializer.h>
#include <FoundationTest/Reflection/ReflectionTestClasses.h>


template <typename T>
void TestSerialization(const T& source)
{
  ezDefaultMemoryStreamStorage StreamStorage;

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "WriteObjectToDDL")
  {
    ezMemoryStreamWriter FileOut(&StreamStorage);

    ezReflectionSerializer::WriteObjectToDDL(FileOut, ezGetStaticRTTI<T>(), &source, false, ezOpenDdlWriter::TypeStringMode::Compliant);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ReadObjectPropertiesFromDDL")
  {
    ezMemoryStreamReader FileIn(&StreamStorage);
    T data;
    ezReflectionSerializer::ReadObjectPropertiesFromDDL(FileIn, *ezGetStaticRTTI<T>(), &data);

    EZ_TEST_BOOL(data == source);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ReadObjectFromDDL")
  {
    ezMemoryStreamReader FileIn(&StreamStorage);

    const ezRTTI* pRtti;
    void* pObject = ezReflectionSerializer::ReadObjectFromDDL(FileIn, pRtti);

    T& c2 = *((T*)pObject);

    EZ_TEST_BOOL(c2 == source);

    if (pObject)
    {
      pRtti->GetAllocator()->Deallocate(pObject);
    }
  }

  ezDefaultMemoryStreamStorage StreamStorageBinary;
  EZ_TEST_BLOCK(ezTestBlock::Enabled, "WriteObjectToBinary")
  {
    ezMemoryStreamWriter FileOut(&StreamStorageBinary);

    ezReflectionSerializer::WriteObjectToBinary(FileOut, ezGetStaticRTTI<T>(), &source);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ReadObjectPropertiesFromBinary")
  {
    ezMemoryStreamReader FileIn(&StreamStorageBinary);
    T data;
    ezReflectionSerializer::ReadObjectPropertiesFromBinary(FileIn, *ezGetStaticRTTI<T>(), &data);

    EZ_TEST_BOOL(data == source);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ReadObjectFromBinary")
  {
    ezMemoryStreamReader FileIn(&StreamStorageBinary);

    const ezRTTI* pRtti;
    void* pObject = ezReflectionSerializer::ReadObjectFromBinary(FileIn, pRtti);

    T& c2 = *((T*)pObject);

    EZ_TEST_BOOL(c2 == source);

    if (pObject)
    {
      pRtti->GetAllocator()->Deallocate(pObject);
    }
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Clone")
  {
    {
      T clone;
      ezReflectionSerializer::Clone(&source, &clone, ezGetStaticRTTI<T>());
      EZ_TEST_BOOL(clone == source);
      EZ_TEST_BOOL(ezReflectionUtils::IsEqual(&clone, &source, ezGetStaticRTTI<T>()));
    }

    {
      T* pClone = ezReflectionSerializer::Clone(&source);
      EZ_TEST_BOOL(*pClone == source);
      EZ_TEST_BOOL(ezReflectionUtils::IsEqual(pClone, &source));
      ezGetStaticRTTI<T>()->GetAllocator()->Deallocate(pClone);
    }
  }
}


EZ_CREATE_SIMPLE_TEST_GROUP(Reflection);

EZ_CREATE_SIMPLE_TEST(Reflection, Types)
{
  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Iterate All")
  {
    bool bFoundStruct = false;
    bool bFoundClass1 = false;
    bool bFoundClass2 = false;

    ezRTTI::ForEachType([&](const ezRTTI* pRtti)
      {
        if (pRtti->GetTypeName() == "ezTestStruct")
          bFoundStruct = true;
        if (pRtti->GetTypeName() == "ezTestClass1")
          bFoundClass1 = true;
        if (pRtti->GetTypeName() == "ezTestClass2")
          bFoundClass2 = true;

        EZ_TEST_STRING(pRtti->GetPluginName(), "Static"); });

    EZ_TEST_BOOL(bFoundStruct);
    EZ_TEST_BOOL(bFoundClass1);
    EZ_TEST_BOOL(bFoundClass2);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "IsDerivedFrom")
  {
    ezDynamicArray<const ezRTTI*> allTypes;
    ezRTTI::ForEachType([&](const ezRTTI* pRtti)
      { allTypes.PushBack(pRtti); });

    // ground truth - traversing up the parent list
    auto ManualIsDerivedFrom = [](const ezRTTI* t, const ezRTTI* pBaseType) -> bool
    {
      while (t != nullptr)
      {
        if (t == pBaseType)
          return true;

        t = t->GetParentType();
      }

      return false;
    };

    // test each type against every other:
    for (const ezRTTI* typeA : allTypes)
    {
      for (const ezRTTI* typeB : allTypes)
      {
        bool derived = typeA->IsDerivedFrom(typeB);
        bool manualCheck = ManualIsDerivedFrom(typeA, typeB);
        EZ_TEST_BOOL(derived == manualCheck);
      }
    }
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "PropertyFlags")
  {
    EZ_TEST_BOOL(ezPropertyFlags::GetParameterFlags<void>() == (ezPropertyFlags::Void));
    EZ_TEST_BOOL(ezPropertyFlags::GetParameterFlags<const char*>() == (ezPropertyFlags::StandardType | ezPropertyFlags::Const));
    EZ_TEST_BOOL(ezPropertyFlags::GetParameterFlags<int>() == ezPropertyFlags::StandardType);
    EZ_TEST_BOOL(ezPropertyFlags::GetParameterFlags<int&>() == (ezPropertyFlags::StandardType | ezPropertyFlags::Reference));
    EZ_TEST_BOOL(ezPropertyFlags::GetParameterFlags<int*>() == (ezPropertyFlags::StandardType | ezPropertyFlags::Pointer));

    EZ_TEST_BOOL(ezPropertyFlags::GetParameterFlags<const int>() == (ezPropertyFlags::StandardType | ezPropertyFlags::Const));
    EZ_TEST_BOOL(
      ezPropertyFlags::GetParameterFlags<const int&>() == (ezPropertyFlags::StandardType | ezPropertyFlags::Reference | ezPropertyFlags::Const));
    EZ_TEST_BOOL(
      ezPropertyFlags::GetParameterFlags<const int*>() == (ezPropertyFlags::StandardType | ezPropertyFlags::Pointer | ezPropertyFlags::Const));

    EZ_TEST_BOOL(ezPropertyFlags::GetParameterFlags<ezVariant>() == (ezPropertyFlags::StandardType));

    EZ_TEST_BOOL(ezPropertyFlags::GetParameterFlags<ezExampleEnum::Enum>() == ezPropertyFlags::IsEnum);
    EZ_TEST_BOOL(ezPropertyFlags::GetParameterFlags<ezEnum<ezExampleEnum>>() == ezPropertyFlags::IsEnum);
    EZ_TEST_BOOL(ezPropertyFlags::GetParameterFlags<ezBitflags<ezExampleBitflags>>() == ezPropertyFlags::Bitflags);

    EZ_TEST_BOOL(ezPropertyFlags::GetParameterFlags<ezTestStruct3>() == ezPropertyFlags::Class);
    EZ_TEST_BOOL(ezPropertyFlags::GetParameterFlags<ezTestClass2>() == ezPropertyFlags::Class);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "TypeFlags")
  {
    EZ_TEST_INT(ezGetStaticRTTI<bool>()->GetTypeFlags().GetValue(), ezTypeFlags::StandardType);
    EZ_TEST_INT(ezGetStaticRTTI<ezUuid>()->GetTypeFlags().GetValue(), ezTypeFlags::StandardType);
    EZ_TEST_INT(ezGetStaticRTTI<const char*>()->GetTypeFlags().GetValue(), ezTypeFlags::StandardType);
    EZ_TEST_INT(ezGetStaticRTTI<ezString>()->GetTypeFlags().GetValue(), ezTypeFlags::StandardType);
    EZ_TEST_INT(ezGetStaticRTTI<ezMat4>()->GetTypeFlags().GetValue(), ezTypeFlags::StandardType);
    EZ_TEST_INT(ezGetStaticRTTI<ezVariant>()->GetTypeFlags().GetValue(), ezTypeFlags::StandardType);

    EZ_TEST_INT(ezGetStaticRTTI<ezAbstractTestClass>()->GetTypeFlags().GetValue(), (ezTypeFlags::Class | ezTypeFlags::Abstract).GetValue());
    EZ_TEST_INT(ezGetStaticRTTI<ezAbstractTestStruct>()->GetTypeFlags().GetValue(), (ezTypeFlags::Class | ezTypeFlags::Abstract).GetValue());

    EZ_TEST_INT(ezGetStaticRTTI<ezTestStruct3>()->GetTypeFlags().GetValue(), ezTypeFlags::Class);
    EZ_TEST_INT(ezGetStaticRTTI<ezTestClass2>()->GetTypeFlags().GetValue(), ezTypeFlags::Class);

    EZ_TEST_INT(ezGetStaticRTTI<ezExampleEnum>()->GetTypeFlags().GetValue(), ezTypeFlags::IsEnum);
    EZ_TEST_INT(ezGetStaticRTTI<ezExampleBitflags>()->GetTypeFlags().GetValue(), ezTypeFlags::Bitflags);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "FindTypeByName")
  {
    const ezRTTI* pFloat = ezRTTI::FindTypeByName("float");
    if (EZ_TEST_BOOL(pFloat != nullptr))
    {
      EZ_ANALYSIS_ASSUME(pFloat != nullptr);
      EZ_TEST_STRING(pFloat->GetTypeName(), "float");
    }

    const ezRTTI* pStruct = ezRTTI::FindTypeByName("ezTestStruct");
    if (EZ_TEST_BOOL(pStruct != nullptr))
    {
      EZ_ANALYSIS_ASSUME(pStruct != nullptr);
      EZ_TEST_STRING(pStruct->GetTypeName(), "ezTestStruct");
    }

    const ezRTTI* pClass2 = ezRTTI::FindTypeByName("ezTestClass2");
    if (EZ_TEST_BOOL(pClass2 != nullptr))
    {
      EZ_ANALYSIS_ASSUME(pClass2 != nullptr);
      EZ_TEST_STRING(pClass2->GetTypeName(), "ezTestClass2");
    }
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "FindTypeByNameHash")
  {
    const ezRTTI* pFloat = ezRTTI::FindTypeByName("float");
    const ezRTTI* pFloat2 = ezRTTI::FindTypeByNameHash(pFloat->GetTypeNameHash());
    EZ_TEST_BOOL(pFloat == pFloat2);

    const ezRTTI* pStruct = ezRTTI::FindTypeByName("ezTestStruct");
    const ezRTTI* pStruct2 = ezRTTI::FindTypeByNameHash(pStruct->GetTypeNameHash());
    EZ_TEST_BOOL(pStruct == pStruct2);

    const ezRTTI* pClass = ezRTTI::FindTypeByName("ezTestClass2");
    const ezRTTI* pClass2 = ezRTTI::FindTypeByNameHash(pClass->GetTypeNameHash());
    EZ_TEST_BOOL(pClass == pClass2);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "GetProperties")
  {
    {
      const ezRTTI* pType = ezRTTI::FindTypeByName("ezTestStruct");

      auto Props = pType->GetProperties();
      EZ_TEST_INT(Props.GetCount(), 9);
      EZ_TEST_STRING(Props[0]->GetPropertyName(), "Float");
      EZ_TEST_STRING(Props[1]->GetPropertyName(), "Vector");
      EZ_TEST_STRING(Props[2]->GetPropertyName(), "Int");
      EZ_TEST_STRING(Props[3]->GetPropertyName(), "UInt8");
      EZ_TEST_STRING(Props[4]->GetPropertyName(), "Variant");
      EZ_TEST_STRING(Props[5]->GetPropertyName(), "Angle");
      EZ_TEST_STRING(Props[6]->GetPropertyName(), "DataBuffer");
      EZ_TEST_STRING(Props[7]->GetPropertyName(), "vVec3I");
      EZ_TEST_STRING(Props[8]->GetPropertyName(), "VarianceAngle");
    }

    {
      const ezRTTI* pType = ezRTTI::FindTypeByName("ezTestClass2");

      auto Props = pType->GetProperties();
      EZ_TEST_INT(Props.GetCount(), 8);
      EZ_TEST_STRING(Props[0]->GetPropertyName(), "CharPtr");
      EZ_TEST_STRING(Props[1]->GetPropertyName(), "String");
      EZ_TEST_STRING(Props[2]->GetPropertyName(), "StringView");
      EZ_TEST_STRING(Props[3]->GetPropertyName(), "Time");
      EZ_TEST_STRING(Props[4]->GetPropertyName(), "Enum");
      EZ_TEST_STRING(Props[5]->GetPropertyName(), "Bitflags");
      EZ_TEST_STRING(Props[6]->GetPropertyName(), "Array");
      EZ_TEST_STRING(Props[7]->GetPropertyName(), "Variant");

      ezHybridArray<const ezAbstractProperty*, 32> AllProps;
      pType->GetAllProperties(AllProps);

      EZ_TEST_INT(AllProps.GetCount(), 11);
      EZ_TEST_STRING(AllProps[0]->GetPropertyName(), "SubStruct");
      EZ_TEST_STRING(AllProps[1]->GetPropertyName(), "Color");
      EZ_TEST_STRING(AllProps[2]->GetPropertyName(), "SubVector");
      EZ_TEST_STRING(AllProps[3]->GetPropertyName(), "CharPtr");
      EZ_TEST_STRING(AllProps[4]->GetPropertyName(), "String");
      EZ_TEST_STRING(AllProps[5]->GetPropertyName(), "StringView");
      EZ_TEST_STRING(AllProps[6]->GetPropertyName(), "Time");
      EZ_TEST_STRING(AllProps[7]->GetPropertyName(), "Enum");
      EZ_TEST_STRING(AllProps[8]->GetPropertyName(), "Bitflags");
      EZ_TEST_STRING(AllProps[9]->GetPropertyName(), "Array");
      EZ_TEST_STRING(AllProps[10]->GetPropertyName(), "Variant");
    }
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Casts")
  {
    ezTestClass2 test;
    ezTestClass1* pTestClass1 = &test;
    const ezTestClass1* pConstTestClass1 = &test;

    ezTestClass2* pTestClass2 = ezStaticCast<ezTestClass2*>(pTestClass1);
    const ezTestClass2* pConstTestClass2 = ezStaticCast<const ezTestClass2*>(pConstTestClass1);

    pTestClass2 = ezDynamicCast<ezTestClass2*>(pTestClass1);
    pConstTestClass2 = ezDynamicCast<const ezTestClass2*>(pConstTestClass1);
    EZ_TEST_BOOL(pTestClass2 != nullptr);
    EZ_TEST_BOOL(pConstTestClass2 != nullptr);

    ezTestClass1 otherTest;
    pTestClass1 = &otherTest;
    pConstTestClass1 = &otherTest;

    pTestClass2 = ezDynamicCast<ezTestClass2*>(pTestClass1);
    pConstTestClass2 = ezDynamicCast<const ezTestClass2*>(pConstTestClass1);
    EZ_TEST_BOOL(pTestClass2 == nullptr);
    EZ_TEST_BOOL(pConstTestClass2 == nullptr);
  }

#if EZ_ENABLED(EZ_SUPPORTS_DYNAMIC_PLUGINS) && EZ_ENABLED(EZ_COMPILE_ENGINE_AS_DLL)

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Types From Plugin")
  {
    ezResult loadPlugin = ezPlugin::LoadPlugin(ezFoundationTest_Plugin1);
    EZ_TEST_BOOL(loadPlugin == EZ_SUCCESS);

    if (loadPlugin.Failed())
      return;

    const ezRTTI* pStruct2 = ezRTTI::FindTypeByName("ezTestStruct2");
    EZ_TEST_BOOL(pStruct2 != nullptr);

    if (pStruct2)
    {
      EZ_TEST_STRING(pStruct2->GetTypeName(), "ezTestStruct2");
    }

    bool bFoundStruct2 = false;

    ezRTTI::ForEachType(
      [&](const ezRTTI* pRtti)
      {
        if (pRtti->GetTypeName() == "ezTestStruct2")
        {
          bFoundStruct2 = true;

          EZ_TEST_STRING(pRtti->GetPluginName(), ezFoundationTest_Plugin1);

          void* pInstance = pRtti->GetAllocator()->Allocate<void>();
          EZ_TEST_BOOL(pInstance != nullptr);

          const ezAbstractProperty* pProp = pRtti->FindPropertyByName("Float2");

          if (EZ_TEST_BOOL(pProp != nullptr))
          {
            EZ_ANALYSIS_ASSUME(pProp != nullptr);
            EZ_TEST_BOOL(pProp->GetCategory() == ezPropertyCategory::Member);
            auto pAbsMember = static_cast<const ezAbstractMemberProperty*>(pProp);

            EZ_TEST_BOOL(pAbsMember->GetSpecificType() == ezGetStaticRTTI<float>());

            auto pMember = static_cast<const ezTypedMemberProperty<float>*>(pAbsMember);

            EZ_TEST_FLOAT(pMember->GetValue(pInstance), 42.0f, 0);
            pMember->SetValue(pInstance, 43.0f);
            EZ_TEST_FLOAT(pMember->GetValue(pInstance), 43.0f, 0);
          }

          pRtti->GetAllocator()->Deallocate(pInstance);
        }
        else
        {
          EZ_TEST_STRING(pRtti->GetPluginName(), "Static");
        }
      });

    EZ_TEST_BOOL(bFoundStruct2);

    ezPlugin::UnloadAllPlugins();
  }
#endif
}


EZ_CREATE_SIMPLE_TEST(Reflection, Hierarchies)
{
  ezTestClass2Allocator::m_iAllocs = 0;
  ezTestClass2Allocator::m_iDeallocs = 0;

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezTestStruct")
  {
    const ezRTTI* pRtti = ezGetStaticRTTI<ezTestStruct>();

    EZ_TEST_STRING(pRtti->GetTypeName(), "ezTestStruct");
    EZ_TEST_INT(pRtti->GetTypeSize(), sizeof(ezTestStruct));
    EZ_TEST_BOOL(pRtti->GetVariantType() == ezVariant::Type::Invalid);

    EZ_TEST_BOOL(pRtti->GetParentType() == nullptr);

    EZ_TEST_BOOL(pRtti->GetAllocator()->CanAllocate());
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezTestClass1")
  {
    const ezRTTI* pRtti = ezGetStaticRTTI<ezTestClass1>();

    EZ_TEST_STRING(pRtti->GetTypeName(), "ezTestClass1");
    EZ_TEST_INT(pRtti->GetTypeSize(), sizeof(ezTestClass1));
    EZ_TEST_BOOL(pRtti->GetVariantType() == ezVariant::Type::Invalid);

    EZ_TEST_BOOL(pRtti->GetParentType() == ezGetStaticRTTI<ezReflectedClass>());

    EZ_TEST_BOOL(pRtti->GetAllocator()->CanAllocate());

    ezTestClass1* pInstance = pRtti->GetAllocator()->Allocate<ezTestClass1>();
    if (EZ_TEST_BOOL(pInstance != nullptr))
    {
      EZ_ANALYSIS_ASSUME(pInstance != nullptr);
      EZ_TEST_BOOL(pInstance->GetDynamicRTTI() == ezGetStaticRTTI<ezTestClass1>());
      pInstance->GetDynamicRTTI()->GetAllocator()->Deallocate(pInstance);
    }

    EZ_TEST_BOOL(pRtti->IsDerivedFrom<ezReflectedClass>());
    EZ_TEST_BOOL(pRtti->IsDerivedFrom(ezGetStaticRTTI<ezReflectedClass>()));

    EZ_TEST_BOOL(pRtti->IsDerivedFrom<ezTestClass1>());
    EZ_TEST_BOOL(pRtti->IsDerivedFrom(ezGetStaticRTTI<ezTestClass1>()));

    EZ_TEST_BOOL(!pRtti->IsDerivedFrom<ezVec3>());
    EZ_TEST_BOOL(!pRtti->IsDerivedFrom(ezGetStaticRTTI<ezVec3>()));
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezTestClass2")
  {
    const ezRTTI* pRtti = ezGetStaticRTTI<ezTestClass2>();

    EZ_TEST_STRING(pRtti->GetTypeName(), "ezTestClass2");
    EZ_TEST_INT(pRtti->GetTypeSize(), sizeof(ezTestClass2));
    EZ_TEST_BOOL(pRtti->GetVariantType() == ezVariant::Type::Invalid);

    EZ_TEST_BOOL(pRtti->GetParentType() == ezGetStaticRTTI<ezTestClass1>());

    EZ_TEST_BOOL(pRtti->GetAllocator()->CanAllocate());

    EZ_TEST_INT(ezTestClass2Allocator::m_iAllocs, 0);
    EZ_TEST_INT(ezTestClass2Allocator::m_iDeallocs, 0);

    ezTestClass2* pInstance = pRtti->GetAllocator()->Allocate<ezTestClass2>();
    if (EZ_TEST_BOOL(pInstance != nullptr))
    {
      EZ_ANALYSIS_ASSUME(pInstance != nullptr);
      EZ_TEST_BOOL(pInstance->GetDynamicRTTI() == ezGetStaticRTTI<ezTestClass2>());

      EZ_TEST_INT(ezTestClass2Allocator::m_iAllocs, 1);
      EZ_TEST_INT(ezTestClass2Allocator::m_iDeallocs, 0);

      pInstance->GetDynamicRTTI()->GetAllocator()->Deallocate(pInstance);

      EZ_TEST_INT(ezTestClass2Allocator::m_iAllocs, 1);
      EZ_TEST_INT(ezTestClass2Allocator::m_iDeallocs, 1);
    }

    EZ_TEST_BOOL(pRtti->IsDerivedFrom<ezTestClass1>());
    EZ_TEST_BOOL(pRtti->IsDerivedFrom(ezGetStaticRTTI<ezTestClass1>()));

    EZ_TEST_BOOL(pRtti->IsDerivedFrom<ezTestClass2>());
    EZ_TEST_BOOL(pRtti->IsDerivedFrom(ezGetStaticRTTI<ezTestClass2>()));

    EZ_TEST_BOOL(pRtti->IsDerivedFrom<ezReflectedClass>());
    EZ_TEST_BOOL(pRtti->IsDerivedFrom(ezGetStaticRTTI<ezReflectedClass>()));

    EZ_TEST_BOOL(!pRtti->IsDerivedFrom<ezVec3>());
    EZ_TEST_BOOL(!pRtti->IsDerivedFrom(ezGetStaticRTTI<ezVec3>()));
  }
}


template <typename T, typename T2>
void TestMemberProperty(const char* szPropName, void* pObject, const ezRTTI* pRtti, ezBitflags<ezPropertyFlags> expectedFlags, T2 expectedValue, T2 testValue, bool bTestDefaultValue = true)
{
  const ezAbstractProperty* pProp = pRtti->FindPropertyByName(szPropName);
  if (!EZ_TEST_BOOL(pProp != nullptr))
    return;

  EZ_ANALYSIS_ASSUME(pProp != nullptr);

  EZ_TEST_BOOL(pProp->GetCategory() == ezPropertyCategory::Member);

  EZ_TEST_BOOL(pProp->GetSpecificType() == ezGetStaticRTTI<T>());
  auto pMember = static_cast<const ezTypedMemberProperty<T>*>(pProp);

  EZ_TEST_INT(pMember->GetFlags().GetValue(), expectedFlags.GetValue());

  T value = pMember->GetValue(pObject);
  EZ_TEST_BOOL(expectedValue == value);

  if (bTestDefaultValue)
  {
    // Default value
    ezVariant defaultValue = ezReflectionUtils::GetDefaultValue(pProp);
    EZ_TEST_BOOL(ezVariant(expectedValue) == defaultValue);
  }

  if (!pMember->GetFlags().IsSet(ezPropertyFlags::ReadOnly))
  {
    pMember->SetValue(pObject, testValue);

    EZ_TEST_BOOL(testValue == pMember->GetValue(pObject));

    ezReflectionUtils::SetMemberPropertyValue(pMember, pObject, ezVariant(expectedValue));
    ezVariant res = ezReflectionUtils::GetMemberPropertyValue(pMember, pObject);

    EZ_TEST_BOOL(res == ezVariant(expectedValue));
    EZ_TEST_BOOL(res != ezVariant(testValue));

    ezReflectionUtils::SetMemberPropertyValue(pMember, pObject, ezVariant(testValue));
    res = ezReflectionUtils::GetMemberPropertyValue(pMember, pObject);

    EZ_TEST_BOOL(res != ezVariant(expectedValue));
    EZ_TEST_BOOL(res == ezVariant(testValue));
  }
}

EZ_CREATE_SIMPLE_TEST(Reflection, MemberProperties)
{
  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezTestStruct")
  {
    ezTestStruct data;
    const ezRTTI* pRtti = ezGetStaticRTTI<ezTestStruct>();

    TestMemberProperty<float>("Float", &data, pRtti, ezPropertyFlags::StandardType, 1.1f, 5.0f);
    TestMemberProperty<ezInt32>("Int", &data, pRtti, ezPropertyFlags::StandardType, 2, -8);
    TestMemberProperty<ezVec3>("Vector", &data, pRtti, ezPropertyFlags::StandardType | ezPropertyFlags::ReadOnly, ezVec3(3, 4, 5),
      ezVec3(0, -1.0f, 3.14f));
    TestMemberProperty<ezVariant>("Variant", &data, pRtti, ezPropertyFlags::StandardType, ezVariant("Test"),
      ezVariant(ezVec3(0, -1.0f, 3.14f)));
    TestMemberProperty<ezAngle>("Angle", &data, pRtti, ezPropertyFlags::StandardType, ezAngle::MakeFromDegree(0.5f), ezAngle::MakeFromDegree(1.0f));
    ezVarianceTypeAngle expectedVA = {0.5f, ezAngle::MakeFromDegree(90.0f)};
    ezVarianceTypeAngle testVA = {0.1f, ezAngle::MakeFromDegree(45.0f)};
    TestMemberProperty<ezVarianceTypeAngle>("VarianceAngle", &data, pRtti, ezPropertyFlags::Class, expectedVA, testVA);

    ezDataBuffer expected;
    expected.PushBack(255);
    expected.PushBack(0);
    expected.PushBack(127);

    ezDataBuffer newValue;
    newValue.PushBack(1);
    newValue.PushBack(2);

    TestMemberProperty<ezDataBuffer>("DataBuffer", &data, pRtti, ezPropertyFlags::StandardType, expected, newValue);
    TestMemberProperty<ezVec3I32>("vVec3I", &data, pRtti, ezPropertyFlags::StandardType, ezVec3I32(1, 2, 3), ezVec3I32(5, 6, 7));

    TestSerialization<ezTestStruct>(data);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezTestClass2")
  {
    ezTestClass2 Instance;
    const ezRTTI* pRtti = ezGetStaticRTTI<ezTestClass2>();

    {
      TestMemberProperty<const char*>("CharPtr", &Instance, pRtti, ezPropertyFlags::StandardType | ezPropertyFlags::Const, ezString("AAA"), ezString("aaaa"));

      TestMemberProperty<ezString>("String", &Instance, pRtti, ezPropertyFlags::StandardType, ezString("BBB"), ezString("bbbb"));

      TestMemberProperty<ezStringView>("StringView", &Instance, pRtti, ezPropertyFlags::StandardType, "CCC"_ezsv, "cccc"_ezsv);

      Instance.SetStringView("CCC");
      TestMemberProperty<ezStringView>("StringView", &Instance, pRtti, ezPropertyFlags::StandardType, ezString("CCC"), ezString("cccc"));

      const ezAbstractProperty* pProp = pRtti->FindPropertyByName("SubVector", false);
      EZ_TEST_BOOL(pProp == nullptr);
    }

    {
      TestMemberProperty<ezVec3>("SubVector", &Instance, pRtti, ezPropertyFlags::StandardType | ezPropertyFlags::ReadOnly, ezVec3(3, 4, 5), ezVec3(3, 4, 5));
      const ezAbstractProperty* pProp = pRtti->FindPropertyByName("SubStruct", false);
      EZ_TEST_BOOL(pProp == nullptr);
    }

    {
      const ezAbstractProperty* pProp = pRtti->FindPropertyByName("SubStruct");
      if (EZ_TEST_BOOL(pProp != nullptr))
      {
        EZ_ANALYSIS_ASSUME(pProp != nullptr);
        EZ_TEST_BOOL(pProp->GetCategory() == ezPropertyCategory::Member);
        ezAbstractMemberProperty* pAbs = (ezAbstractMemberProperty*)pProp;

        const ezRTTI* pStruct = pAbs->GetSpecificType();
        void* pSubStruct = pAbs->GetPropertyPointer(&Instance);

        EZ_TEST_BOOL(pSubStruct != nullptr);

        TestMemberProperty<float>("Float", pSubStruct, pStruct, ezPropertyFlags::StandardType, 33.3f, 44.4f, false);
      }
    }

    TestSerialization<ezTestClass2>(Instance);
  }
}


EZ_CREATE_SIMPLE_TEST(Reflection, Enum)
{
  const ezRTTI* pEnumRTTI = ezGetStaticRTTI<ezExampleEnum>();
  const ezRTTI* pRTTI = ezGetStaticRTTI<ezTestEnumStruct>();

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Enum Constants")
  {
    EZ_TEST_BOOL(pEnumRTTI->IsDerivedFrom<ezEnumBase>());
    auto props = pEnumRTTI->GetProperties();
    EZ_TEST_INT(props.GetCount(), 4); // Default + 3

    for (auto pProp : props)
    {
      EZ_TEST_BOOL(pProp->GetCategory() == ezPropertyCategory::Constant);
      EZ_TEST_BOOL(pProp->GetSpecificType() == ezGetStaticRTTI<ezInt8>());
    }
    EZ_TEST_INT(ezExampleEnum::Default, ezReflectionUtils::DefaultEnumerationValue(pEnumRTTI));

    EZ_TEST_STRING(props[0]->GetPropertyName(), "ezExampleEnum::Default");
    EZ_TEST_STRING(props[1]->GetPropertyName(), "ezExampleEnum::Value1");
    EZ_TEST_STRING(props[2]->GetPropertyName(), "ezExampleEnum::Value2");
    EZ_TEST_STRING(props[3]->GetPropertyName(), "ezExampleEnum::Value3");

    auto pTypedConstantProp0 = static_cast<const ezTypedConstantProperty<ezInt8>*>(props[0]);
    auto pTypedConstantProp1 = static_cast<const ezTypedConstantProperty<ezInt8>*>(props[1]);
    auto pTypedConstantProp2 = static_cast<const ezTypedConstantProperty<ezInt8>*>(props[2]);
    auto pTypedConstantProp3 = static_cast<const ezTypedConstantProperty<ezInt8>*>(props[3]);
    EZ_TEST_INT(pTypedConstantProp0->GetValue(), ezExampleEnum::Default);
    EZ_TEST_INT(pTypedConstantProp1->GetValue(), ezExampleEnum::Value1);
    EZ_TEST_INT(pTypedConstantProp2->GetValue(), ezExampleEnum::Value2);
    EZ_TEST_INT(pTypedConstantProp3->GetValue(), ezExampleEnum::Value3);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Enum Property")
  {
    ezTestEnumStruct data;
    auto props = pRTTI->GetProperties();
    EZ_TEST_INT(props.GetCount(), 4);

    for (auto pProp : props)
    {
      EZ_TEST_BOOL(pProp->GetCategory() == ezPropertyCategory::Member);
      EZ_TEST_INT(pProp->GetFlags().GetValue(), ezPropertyFlags::IsEnum);
      EZ_TEST_BOOL(pProp->GetSpecificType() == pEnumRTTI);
      auto pEnumProp = static_cast<const ezAbstractEnumerationProperty*>(pProp);
      EZ_TEST_BOOL(pEnumProp->GetValue(&data) == ezExampleEnum::Value1);

      const ezRTTI* pEnumPropertyRTTI = pEnumProp->GetSpecificType();
      // Set and get all valid enum values.
      for (auto pProp2 : pEnumPropertyRTTI->GetProperties().GetSubArray(1))
      {
        auto pConstantProp = static_cast<const ezTypedConstantProperty<ezInt8>*>(pProp2);
        pEnumProp->SetValue(&data, pConstantProp->GetValue());
        EZ_TEST_INT(pEnumProp->GetValue(&data), pConstantProp->GetValue());

        // Enum <-> string
        ezStringBuilder sValue;
        EZ_TEST_BOOL(ezReflectionUtils::EnumerationToString(pEnumPropertyRTTI, pConstantProp->GetValue(), sValue));
        EZ_TEST_STRING(sValue, pConstantProp->GetPropertyName());

        // Setting the value via a string also works.
        pEnumProp->SetValue(&data, ezExampleEnum::Value1);
        ezReflectionUtils::SetMemberPropertyValue(pEnumProp, &data, sValue.GetData());
        EZ_TEST_INT(pEnumProp->GetValue(&data), pConstantProp->GetValue());

        ezInt64 iValue = 0;
        EZ_TEST_BOOL(ezReflectionUtils::StringToEnumeration(pEnumPropertyRTTI, sValue, iValue));
        EZ_TEST_INT(iValue, pConstantProp->GetValue());

        // Testing the short enum name version
        EZ_TEST_BOOL(ezReflectionUtils::EnumerationToString(
          pEnumPropertyRTTI, pConstantProp->GetValue(), sValue, ezReflectionUtils::EnumConversionMode::ValueNameOnly));
        EZ_TEST_BOOL(sValue.IsEqual(pConstantProp->GetPropertyName()) ||
                     sValue.IsEqual(ezStringUtils::FindLastSubString(pConstantProp->GetPropertyName(), "::") + 2));

        EZ_TEST_BOOL(ezReflectionUtils::StringToEnumeration(pEnumPropertyRTTI, sValue, iValue));
        EZ_TEST_INT(iValue, pConstantProp->GetValue());

        // Testing the short enum name version
        EZ_TEST_BOOL(ezReflectionUtils::EnumerationToString(
          pEnumPropertyRTTI, pConstantProp->GetValue(), sValue, ezReflectionUtils::EnumConversionMode::ValueNameOnly));
        EZ_TEST_BOOL(sValue.IsEqual(pConstantProp->GetPropertyName()) ||
                     sValue.IsEqual(ezStringUtils::FindLastSubString(pConstantProp->GetPropertyName(), "::") + 2));

        EZ_TEST_BOOL(ezReflectionUtils::StringToEnumeration(pEnumPropertyRTTI, sValue, iValue));
        EZ_TEST_INT(iValue, pConstantProp->GetValue());

        EZ_TEST_INT(iValue, ezReflectionUtils::MakeEnumerationValid(pEnumPropertyRTTI, iValue));
        EZ_TEST_INT(ezExampleEnum::Default, ezReflectionUtils::MakeEnumerationValid(pEnumPropertyRTTI, iValue + 666));
      }
    }

    EZ_TEST_BOOL(data.m_enum == ezExampleEnum::Value3);
    EZ_TEST_BOOL(data.m_enumClass == ezExampleEnum::Value3);

    EZ_TEST_BOOL(data.GetEnum() == ezExampleEnum::Value3);
    EZ_TEST_BOOL(data.GetEnumClass() == ezExampleEnum::Value3);

    TestSerialization<ezTestEnumStruct>(data);
  }
}


EZ_CREATE_SIMPLE_TEST(Reflection, Bitflags)
{
  const ezRTTI* pBitflagsRTTI = ezGetStaticRTTI<ezExampleBitflags>();
  const ezRTTI* pRTTI = ezGetStaticRTTI<ezTestBitflagsStruct>();

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Bitflags Constants")
  {
    EZ_TEST_BOOL(pBitflagsRTTI->IsDerivedFrom<ezBitflagsBase>());
    auto props = pBitflagsRTTI->GetProperties();
    EZ_TEST_INT(props.GetCount(), 4); // Default + 3

    for (auto pProp : props)
    {
      EZ_TEST_BOOL(pProp->GetCategory() == ezPropertyCategory::Constant);
      EZ_TEST_BOOL(pProp->GetSpecificType() == ezGetStaticRTTI<ezUInt64>());
    }
    EZ_TEST_INT(ezExampleBitflags::Default, ezReflectionUtils::DefaultEnumerationValue(pBitflagsRTTI));

    EZ_TEST_STRING(props[0]->GetPropertyName(), "ezExampleBitflags::Default");
    EZ_TEST_STRING(props[1]->GetPropertyName(), "ezExampleBitflags::Value1");
    EZ_TEST_STRING(props[2]->GetPropertyName(), "ezExampleBitflags::Value2");
    EZ_TEST_STRING(props[3]->GetPropertyName(), "ezExampleBitflags::Value3");

    auto pTypedConstantProp0 = static_cast<const ezTypedConstantProperty<ezUInt64>*>(props[0]);
    auto pTypedConstantProp1 = static_cast<const ezTypedConstantProperty<ezUInt64>*>(props[1]);
    auto pTypedConstantProp2 = static_cast<const ezTypedConstantProperty<ezUInt64>*>(props[2]);
    auto pTypedConstantProp3 = static_cast<const ezTypedConstantProperty<ezUInt64>*>(props[3]);
    EZ_TEST_BOOL(pTypedConstantProp0->GetValue() == ezExampleBitflags::Default);
    EZ_TEST_BOOL(pTypedConstantProp1->GetValue() == ezExampleBitflags::Value1);
    EZ_TEST_BOOL(pTypedConstantProp2->GetValue() == ezExampleBitflags::Value2);
    EZ_TEST_BOOL(pTypedConstantProp3->GetValue() == ezExampleBitflags::Value3);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Bitflags Property")
  {
    ezTestBitflagsStruct data;
    auto props = pRTTI->GetProperties();
    EZ_TEST_INT(props.GetCount(), 2);

    for (auto pProp : props)
    {
      EZ_TEST_BOOL(pProp->GetCategory() == ezPropertyCategory::Member);
      EZ_TEST_BOOL(pProp->GetSpecificType() == pBitflagsRTTI);
      EZ_TEST_INT(pProp->GetFlags().GetValue(), ezPropertyFlags::Bitflags);
      auto pBitflagsProp = static_cast<const ezAbstractEnumerationProperty*>(pProp);
      EZ_TEST_BOOL(pBitflagsProp->GetValue(&data) == ezExampleBitflags::Value1);

      const ezRTTI* pBitflagsPropertyRTTI = pBitflagsProp->GetSpecificType();

      // Set and get all valid bitflags values. (skip default value)
      ezUInt64 constants[] = {
        static_cast<const ezTypedConstantProperty<ezUInt64>*>(pBitflagsPropertyRTTI->GetProperties()[1])->GetValue(),
        static_cast<const ezTypedConstantProperty<ezUInt64>*>(pBitflagsPropertyRTTI->GetProperties()[2])->GetValue(),
        static_cast<const ezTypedConstantProperty<ezUInt64>*>(pBitflagsPropertyRTTI->GetProperties()[3])->GetValue(),
      };

      const char* stringValues[] = {"",
        "ezExampleBitflags::Value1",
        "ezExampleBitflags::Value2",
        "ezExampleBitflags::Value1|ezExampleBitflags::Value2",
        "ezExampleBitflags::Value3",
        "ezExampleBitflags::Value1|ezExampleBitflags::Value3",
        "ezExampleBitflags::Value2|ezExampleBitflags::Value3",
        "ezExampleBitflags::Value1|ezExampleBitflags::Value2|ezExampleBitflags::Value3"};

      const char* stringValuesShort[] = {"",
        "Value1",
        "Value2",
        "Value1|Value2",
        "Value3",
        "Value1|Value3",
        "Value2|Value3",
        "Value1|Value2|Value3"};
      for (ezInt32 i = 0; i < 8; ++i)
      {
        ezUInt64 uiBitflagValue = 0;
        uiBitflagValue |= (i & EZ_BIT(0)) != 0 ? constants[0] : 0;
        uiBitflagValue |= (i & EZ_BIT(1)) != 0 ? constants[1] : 0;
        uiBitflagValue |= (i & EZ_BIT(2)) != 0 ? constants[2] : 0;

        pBitflagsProp->SetValue(&data, uiBitflagValue);
        EZ_TEST_INT(pBitflagsProp->GetValue(&data), uiBitflagValue);

        // Bitflags <-> string
        ezStringBuilder sValue;
        EZ_TEST_BOOL(ezReflectionUtils::EnumerationToString(pBitflagsPropertyRTTI, uiBitflagValue, sValue));
        EZ_TEST_STRING(sValue, stringValues[i]);

        // Setting the value via a string also works.
        pBitflagsProp->SetValue(&data, 0);
        ezReflectionUtils::SetMemberPropertyValue(pBitflagsProp, &data, sValue.GetData());
        EZ_TEST_INT(pBitflagsProp->GetValue(&data), uiBitflagValue);

        ezInt64 iValue = 0;
        EZ_TEST_BOOL(ezReflectionUtils::StringToEnumeration(pBitflagsPropertyRTTI, sValue, iValue));
        EZ_TEST_INT(iValue, uiBitflagValue);

        // Testing the short enum name version
        EZ_TEST_BOOL(ezReflectionUtils::EnumerationToString(
          pBitflagsPropertyRTTI, uiBitflagValue, sValue, ezReflectionUtils::EnumConversionMode::ValueNameOnly));
        EZ_TEST_BOOL(sValue.IsEqual(stringValuesShort[i]));

        EZ_TEST_BOOL(ezReflectionUtils::StringToEnumeration(pBitflagsPropertyRTTI, sValue, iValue));
        EZ_TEST_INT(iValue, uiBitflagValue);

        // Testing the short enum name version
        EZ_TEST_BOOL(ezReflectionUtils::EnumerationToString(
          pBitflagsPropertyRTTI, uiBitflagValue, sValue, ezReflectionUtils::EnumConversionMode::ValueNameOnly));
        EZ_TEST_BOOL(sValue.IsEqual(stringValuesShort[i]));

        EZ_TEST_BOOL(ezReflectionUtils::StringToEnumeration(pBitflagsPropertyRTTI, sValue, iValue));
        EZ_TEST_INT(iValue, uiBitflagValue);

        EZ_TEST_INT(iValue, ezReflectionUtils::MakeEnumerationValid(pBitflagsPropertyRTTI, iValue));
        EZ_TEST_INT(iValue, ezReflectionUtils::MakeEnumerationValid(pBitflagsPropertyRTTI, iValue | EZ_BIT(16)));
      }
    }

    EZ_TEST_BOOL(data.m_bitflagsClass == (ezExampleBitflags::Value1 | ezExampleBitflags::Value2 | ezExampleBitflags::Value3));
    EZ_TEST_BOOL(data.GetBitflagsClass() == (ezExampleBitflags::Value1 | ezExampleBitflags::Value2 | ezExampleBitflags::Value3));
    TestSerialization<ezTestBitflagsStruct>(data);
  }
}


template <typename T>
void TestArrayPropertyVariant(const ezAbstractArrayProperty* pArrayProp, void* pObject, const ezRTTI* pRtti, T& value)
{
  T temp = {};

  // Reflection Utils
  ezVariant value0 = ezReflectionUtils::GetArrayPropertyValue(pArrayProp, pObject, 0);
  EZ_TEST_BOOL(value0 == ezVariant(value));
  // insert
  ezReflectionUtils::InsertArrayPropertyValue(pArrayProp, pObject, ezVariant(temp), 2);
  EZ_TEST_INT(pArrayProp->GetCount(pObject), 3);
  ezVariant value2 = ezReflectionUtils::GetArrayPropertyValue(pArrayProp, pObject, 2);
  EZ_TEST_BOOL(value0 != value2);
  ezReflectionUtils::SetArrayPropertyValue(pArrayProp, pObject, 2, value);
  value2 = ezReflectionUtils::GetArrayPropertyValue(pArrayProp, pObject, 2);
  EZ_TEST_BOOL(value0 == value2);
  // remove again
  ezReflectionUtils::RemoveArrayPropertyValue(pArrayProp, pObject, 2);
  EZ_TEST_INT(pArrayProp->GetCount(pObject), 2);
}

template <>
void TestArrayPropertyVariant<ezTestArrays>(const ezAbstractArrayProperty* pArrayProp, void* pObject, const ezRTTI* pRtti, ezTestArrays& value)
{
}

template <>
void TestArrayPropertyVariant<ezTestStruct3>(const ezAbstractArrayProperty* pArrayProp, void* pObject, const ezRTTI* pRtti, ezTestStruct3& value)
{
}

template <typename T>
void TestArrayProperty(const char* szPropName, void* pObject, const ezRTTI* pRtti, T& value)
{
  const ezAbstractProperty* pProp = pRtti->FindPropertyByName(szPropName);
  EZ_TEST_BOOL(pProp != nullptr);
  if (pProp == nullptr)
    return;

  EZ_TEST_BOOL(pProp->GetCategory() == ezPropertyCategory::Array);
  auto pArrayProp = static_cast<const ezAbstractArrayProperty*>(pProp);
  const ezRTTI* pElemRtti = pProp->GetSpecificType();
  EZ_TEST_BOOL(pElemRtti == ezGetStaticRTTI<T>());
  if (!pArrayProp->GetFlags().IsSet(ezPropertyFlags::ReadOnly))
  {
    // If we don't know the element type T but we can allocate it, we can handle it anyway.
    if (pElemRtti->GetAllocator()->CanAllocate())
    {
      void* pData = pElemRtti->GetAllocator()->Allocate<void>();

      pArrayProp->SetCount(pObject, 2);
      EZ_TEST_INT(pArrayProp->GetCount(pObject), 2);
      // Push default constructed object in both slots.
      pArrayProp->SetValue(pObject, 0, pData);
      pArrayProp->SetValue(pObject, 1, pData);

      // Retrieve it again and compare to function parameter, they should be different.
      pArrayProp->GetValue(pObject, 0, pData);
      EZ_TEST_BOOL(*static_cast<T*>(pData) != value);
      pArrayProp->GetValue(pObject, 1, pData);
      EZ_TEST_BOOL(*static_cast<T*>(pData) != value);

      pElemRtti->GetAllocator()->Deallocate(pData);
    }

    pArrayProp->Clear(pObject);
    EZ_TEST_INT(pArrayProp->GetCount(pObject), 0);
    pArrayProp->SetCount(pObject, 2);
    pArrayProp->SetValue(pObject, 0, &value);
    pArrayProp->SetValue(pObject, 1, &value);

    // Insert default init values
    T temp = {};
    pArrayProp->Insert(pObject, 2, &temp);
    EZ_TEST_INT(pArrayProp->GetCount(pObject), 3);
    pArrayProp->Insert(pObject, 0, &temp);
    EZ_TEST_INT(pArrayProp->GetCount(pObject), 4);

    // Remove them again
    pArrayProp->Remove(pObject, 3);
    EZ_TEST_INT(pArrayProp->GetCount(pObject), 3);
    pArrayProp->Remove(pObject, 0);
    EZ_TEST_INT(pArrayProp->GetCount(pObject), 2);

    TestArrayPropertyVariant<T>(pArrayProp, pObject, pRtti, value);
  }

  // Assumes this function gets called first by a writeable property, and then immediately by the same data as a read-only property.
  // So the checks are valid for the read-only version, too.
  EZ_TEST_INT(pArrayProp->GetCount(pObject), 2);

  T v1 = {};
  pArrayProp->GetValue(pObject, 0, &v1);
  if constexpr (std::is_same<const char*, T>::value)
  {
    EZ_TEST_BOOL(ezStringUtils::IsEqual(v1, value));
  }
  else
  {
    EZ_TEST_BOOL(v1 == value);
  }

  T v2 = {};
  pArrayProp->GetValue(pObject, 1, &v2);
  if constexpr (std::is_same<const char*, T>::value)
  {
    EZ_TEST_BOOL(ezStringUtils::IsEqual(v2, value));
  }
  else
  {
    EZ_TEST_BOOL(v2 == value);
  }

  if (pElemRtti->GetAllocator()->CanAllocate())
  {
    // Current values should be different from default constructed version.
    void* pData = pElemRtti->GetAllocator()->Allocate<void>();

    EZ_TEST_BOOL(*static_cast<T*>(pData) != v1);
    EZ_TEST_BOOL(*static_cast<T*>(pData) != v2);

    pElemRtti->GetAllocator()->Deallocate(pData);
  }
}

EZ_CREATE_SIMPLE_TEST(Reflection, Arrays)
{
  ezTestArrays containers;
  const ezRTTI* pRtti = ezGetStaticRTTI<ezTestArrays>();
  EZ_TEST_BOOL(pRtti != nullptr);

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "POD Array")
  {
    double fValue = 5;
    TestArrayProperty<double>("Hybrid", &containers, pRtti, fValue);
    TestArrayProperty<double>("HybridRO", &containers, pRtti, fValue);

    TestArrayProperty<double>("AcHybrid", &containers, pRtti, fValue);
    TestArrayProperty<double>("AcHybridRO", &containers, pRtti, fValue);

    const char* szValue = "Bla";
    const char* szValue2 = "LongString------------------------------------------------------------------------------------";
    ezString sValue = szValue;
    ezString sValue2 = szValue2;

    TestArrayProperty<ezString>("HybridChar", &containers, pRtti, sValue);
    TestArrayProperty<ezString>("HybridCharRO", &containers, pRtti, sValue);

    TestArrayProperty<const char*>("AcHybridChar", &containers, pRtti, szValue);
    TestArrayProperty<const char*>("AcHybridCharRO", &containers, pRtti, szValue);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Struct Array")
  {
    ezTestStruct3 data;
    data.m_fFloat1 = 99.0f;
    data.m_UInt8 = 127;

    TestArrayProperty<ezTestStruct3>("Dynamic", &containers, pRtti, data);
    TestArrayProperty<ezTestStruct3>("DynamicRO", &containers, pRtti, data);

    TestArrayProperty<ezTestStruct3>("AcDynamic", &containers, pRtti, data);
    TestArrayProperty<ezTestStruct3>("AcDynamicRO", &containers, pRtti, data);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezReflectedClass Array")
  {
    ezTestArrays data;
    data.m_Hybrid.PushBack(42.0);

    TestArrayProperty<ezTestArrays>("Deque", &containers, pRtti, data);
    TestArrayProperty<ezTestArrays>("DequeRO", &containers, pRtti, data);

    TestArrayProperty<ezTestArrays>("AcDeque", &containers, pRtti, data);
    TestArrayProperty<ezTestArrays>("AcDequeRO", &containers, pRtti, data);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Custom Variant Array")
  {
    ezVarianceTypeAngle data{0.1f, ezAngle::MakeFromDegree(45.0f)};

    TestArrayProperty<ezVarianceTypeAngle>("Custom", &containers, pRtti, data);
    TestArrayProperty<ezVarianceTypeAngle>("CustomRO", &containers, pRtti, data);

    TestArrayProperty<ezVarianceTypeAngle>("AcCustom", &containers, pRtti, data);
    TestArrayProperty<ezVarianceTypeAngle>("AcCustomRO", &containers, pRtti, data);
  }

  TestSerialization<ezTestArrays>(containers);
}

/// \brief Determines whether a type is a pointer.
template <typename T>
struct ezIsPointer
{
  static constexpr bool value = false;
};

template <typename T>
struct ezIsPointer<T*>
{
  static constexpr bool value = true;
};

template <typename T>
void TestSetProperty(const char* szPropName, void* pObject, const ezRTTI* pRtti, T& ref_value1, T& ref_value2)
{
  const ezAbstractProperty* pProp = pRtti->FindPropertyByName(szPropName);
  if (!EZ_TEST_BOOL(pProp != nullptr))
    return;

  EZ_ANALYSIS_ASSUME(pProp != nullptr);

  EZ_TEST_BOOL(pProp->GetCategory() == ezPropertyCategory::Set);
  auto pSetProp = static_cast<const ezAbstractSetProperty*>(pProp);
  const ezRTTI* pElemRtti = pProp->GetSpecificType();
  EZ_TEST_BOOL(pElemRtti == ezGetStaticRTTI<T>());

  if (!pSetProp->GetFlags().IsSet(ezPropertyFlags::ReadOnly))
  {
    pSetProp->Clear(pObject);
    EZ_TEST_BOOL(pSetProp->IsEmpty(pObject));
    pSetProp->Insert(pObject, &ref_value1);
    EZ_TEST_BOOL(!pSetProp->IsEmpty(pObject));
    EZ_TEST_BOOL(pSetProp->Contains(pObject, &ref_value1));
    EZ_TEST_BOOL(!pSetProp->Contains(pObject, &ref_value2));
    pSetProp->Insert(pObject, &ref_value2);
    EZ_TEST_BOOL(!pSetProp->IsEmpty(pObject));
    EZ_TEST_BOOL(pSetProp->Contains(pObject, &ref_value1));
    EZ_TEST_BOOL(pSetProp->Contains(pObject, &ref_value2));

    // Insert default init value
    if (!ezIsPointer<T>::value)
    {
      T temp = T{};
      pSetProp->Insert(pObject, &temp);
      EZ_TEST_BOOL(!pSetProp->IsEmpty(pObject));
      EZ_TEST_BOOL(pSetProp->Contains(pObject, &ref_value1));
      EZ_TEST_BOOL(pSetProp->Contains(pObject, &ref_value2));
      EZ_TEST_BOOL(pSetProp->Contains(pObject, &temp));

      // Remove it again
      pSetProp->Remove(pObject, &temp);
      EZ_TEST_BOOL(!pSetProp->IsEmpty(pObject));
      EZ_TEST_BOOL(!pSetProp->Contains(pObject, &temp));
    }
  }

  // Assumes this function gets called first by a writeable property, and then immediately by the same data as a read-only property.
  // So the checks are valid for the read-only version, too.
  EZ_TEST_BOOL(!pSetProp->IsEmpty(pObject));
  EZ_TEST_BOOL(pSetProp->Contains(pObject, &ref_value1));
  EZ_TEST_BOOL(pSetProp->Contains(pObject, &ref_value2));


  ezHybridArray<ezVariant, 16> keys;
  pSetProp->GetValues(pObject, keys);
  EZ_TEST_INT(keys.GetCount(), 2);
}

EZ_CREATE_SIMPLE_TEST(Reflection, Sets)
{
  ezTestSets containers;
  const ezRTTI* pRtti = ezGetStaticRTTI<ezTestSets>();
  EZ_TEST_BOOL(pRtti != nullptr);

  // Disabled because MSVC 2017 has code generation issues in Release builds
  EZ_TEST_BLOCK(ezTestBlock::Disabled, "ezSet")
  {
    ezInt8 iValue1 = -5;
    ezInt8 iValue2 = 127;
    TestSetProperty<ezInt8>("Set", &containers, pRtti, iValue1, iValue2);
    TestSetProperty<ezInt8>("SetRO", &containers, pRtti, iValue1, iValue2);

    double fValue1 = 5;
    double fValue2 = -3;
    TestSetProperty<double>("AcSet", &containers, pRtti, fValue1, fValue2);
    TestSetProperty<double>("AcSetRO", &containers, pRtti, fValue1, fValue2);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezHashSet")
  {
    ezInt32 iValue1 = -5;
    ezInt32 iValue2 = 127;
    TestSetProperty<ezInt32>("HashSet", &containers, pRtti, iValue1, iValue2);
    TestSetProperty<ezInt32>("HashSetRO", &containers, pRtti, iValue1, iValue2);

    ezInt64 fValue1 = 5;
    ezInt64 fValue2 = -3;
    TestSetProperty<ezInt64>("HashAcSet", &containers, pRtti, fValue1, fValue2);
    TestSetProperty<ezInt64>("HashAcSetRO", &containers, pRtti, fValue1, fValue2);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezDeque Pseudo Set")
  {
    int iValue1 = -5;
    int iValue2 = 127;

    TestSetProperty<int>("AcPseudoSet", &containers, pRtti, iValue1, iValue2);
    TestSetProperty<int>("AcPseudoSetRO", &containers, pRtti, iValue1, iValue2);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezSetPtr Pseudo Set")
  {
    ezString sValue1 = "TestString1";
    ezString sValue2 = "Test String Deus";

    TestSetProperty<ezString>("AcPseudoSet2", &containers, pRtti, sValue1, sValue2);
    TestSetProperty<ezString>("AcPseudoSet2RO", &containers, pRtti, sValue1, sValue2);

    const char* szValue1 = "TestString1";
    const char* szValue2 = "Test String Deus";
    TestSetProperty<const char*>("AcPseudoSet2b", &containers, pRtti, szValue1, szValue2);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Custom Variant HashSet")
  {
    ezVarianceTypeAngle value1{-0.1f, ezAngle::MakeFromDegree(-45.0f)};
    ezVarianceTypeAngle value2{0.1f, ezAngle::MakeFromDegree(45.0f)};

    TestSetProperty<ezVarianceTypeAngle>("CustomHashSet", &containers, pRtti, value1, value2);
    TestSetProperty<ezVarianceTypeAngle>("CustomHashSetRO", &containers, pRtti, value1, value2);

    ezVarianceTypeAngle value3{-0.2f, ezAngle::MakeFromDegree(-90.0f)};
    ezVarianceTypeAngle value4{0.2f, ezAngle::MakeFromDegree(90.0f)};
    TestSetProperty<ezVarianceTypeAngle>("CustomHashAcSet", &containers, pRtti, value3, value4);
    TestSetProperty<ezVarianceTypeAngle>("CustomHashAcSetRO", &containers, pRtti, value3, value4);
  }
  TestSerialization<ezTestSets>(containers);
}

template <typename T>
void TestMapProperty(const char* szPropName, void* pObject, const ezRTTI* pRtti, T& ref_value1, T& ref_value2)
{
  const ezAbstractProperty* pProp = pRtti->FindPropertyByName(szPropName);
  if (!EZ_TEST_BOOL(pProp != nullptr))
    return;
  EZ_ANALYSIS_ASSUME(pProp != nullptr);
  EZ_TEST_BOOL(pProp->GetCategory() == ezPropertyCategory::Map);
  auto pMapProp = static_cast<const ezAbstractMapProperty*>(pProp);
  const ezRTTI* pElemRtti = pProp->GetSpecificType();
  EZ_TEST_BOOL(pElemRtti == ezGetStaticRTTI<T>());
  EZ_TEST_BOOL(ezReflectionUtils::IsBasicType(pElemRtti) || pElemRtti == ezGetStaticRTTI<ezVariant>() || pElemRtti == ezGetStaticRTTI<ezVarianceTypeAngle>());

  if (!pMapProp->GetFlags().IsSet(ezPropertyFlags::ReadOnly))
  {
    pMapProp->Clear(pObject);
    EZ_TEST_BOOL(pMapProp->IsEmpty(pObject));
    pMapProp->Insert(pObject, "value1", &ref_value1);
    EZ_TEST_BOOL(!pMapProp->IsEmpty(pObject));
    EZ_TEST_BOOL(pMapProp->Contains(pObject, "value1"));
    EZ_TEST_BOOL(!pMapProp->Contains(pObject, "value2"));
    T getValue;
    EZ_TEST_BOOL(!pMapProp->GetValue(pObject, "value2", &getValue));
    EZ_TEST_BOOL(pMapProp->GetValue(pObject, "value1", &getValue));
    EZ_TEST_BOOL(getValue == ref_value1);

    pMapProp->Insert(pObject, "value2", &ref_value2);
    EZ_TEST_BOOL(!pMapProp->IsEmpty(pObject));
    EZ_TEST_BOOL(pMapProp->Contains(pObject, "value1"));
    EZ_TEST_BOOL(pMapProp->Contains(pObject, "value2"));
    EZ_TEST_BOOL(pMapProp->GetValue(pObject, "value1", &getValue));
    EZ_TEST_BOOL(getValue == ref_value1);
    EZ_TEST_BOOL(pMapProp->GetValue(pObject, "value2", &getValue));
    EZ_TEST_BOOL(getValue == ref_value2);
  }

  // Assumes this function gets called first by a writeable property, and then immediately by the same data as a read-only property.
  // So the checks are valid for the read-only version, too.
  T getValue2;
  EZ_TEST_BOOL(!pMapProp->IsEmpty(pObject));
  EZ_TEST_BOOL(pMapProp->Contains(pObject, "value1"));
  EZ_TEST_BOOL(pMapProp->Contains(pObject, "value2"));
  EZ_TEST_BOOL(pMapProp->GetValue(pObject, "value1", &getValue2));
  EZ_TEST_BOOL(getValue2 == ref_value1);
  EZ_TEST_BOOL(pMapProp->GetValue(pObject, "value2", &getValue2));
  EZ_TEST_BOOL(getValue2 == ref_value2);

  ezHybridArray<ezString, 16> keys;
  pMapProp->GetKeys(pObject, keys);
  EZ_TEST_INT(keys.GetCount(), 2);
  keys.Sort();
  EZ_TEST_BOOL(keys[0] == "value1");
  EZ_TEST_BOOL(keys[1] == "value2");
}

EZ_CREATE_SIMPLE_TEST(Reflection, Maps)
{
  ezTestMaps containers;
  const ezRTTI* pRtti = ezGetStaticRTTI<ezTestMaps>();
  EZ_TEST_BOOL(pRtti != nullptr);

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezMap")
  {
    int iValue1 = -5;
    int iValue2 = 127;
    TestMapProperty<int>("Map", &containers, pRtti, iValue1, iValue2);
    TestMapProperty<int>("MapRO", &containers, pRtti, iValue1, iValue2);

    ezInt64 iValue1b = 5;
    ezInt64 iValue2b = -3;
    TestMapProperty<ezInt64>("AcMap", &containers, pRtti, iValue1b, iValue2b);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "ezHashMap")
  {
    double fValue1 = -5;
    double fValue2 = 127;
    TestMapProperty<double>("HashTable", &containers, pRtti, fValue1, fValue2);
    TestMapProperty<double>("HashTableRO", &containers, pRtti, fValue1, fValue2);

    ezString sValue1 = "Bla";
    ezString sValue2 = "Test";
    TestMapProperty<ezString>("AcHashTable", &containers, pRtti, sValue1, sValue2);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Accessor")
  {
    ezVariant sValue1 = "Test";
    ezVariant sValue2 = ezVec4(1, 2, 3, 4);
    TestMapProperty<ezVariant>("Accessor", &containers, pRtti, sValue1, sValue2);
    TestMapProperty<ezVariant>("AccessorRO", &containers, pRtti, sValue1, sValue2);
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "CustomVariant")
  {
    ezVarianceTypeAngle value1{-0.1f, ezAngle::MakeFromDegree(-45.0f)};
    ezVarianceTypeAngle value2{0.1f, ezAngle::MakeFromDegree(45.0f)};

    TestMapProperty<ezVarianceTypeAngle>("CustomVariant", &containers, pRtti, value1, value2);
    TestMapProperty<ezVarianceTypeAngle>("CustomVariantRO", &containers, pRtti, value1, value2);
  }
  TestSerialization<ezTestMaps>(containers);
}


template <typename T>
void TestPointerMemberProperty(const char* szPropName, void* pObject, const ezRTTI* pRtti, ezBitflags<ezPropertyFlags> expectedFlags, T* pExpectedValue)
{
  const ezAbstractProperty* pProp = pRtti->FindPropertyByName(szPropName);
  if (!EZ_TEST_BOOL(pProp != nullptr))
    return;
  EZ_ANALYSIS_ASSUME(pProp != nullptr);
  EZ_TEST_BOOL(pProp->GetCategory() == ezPropertyCategory::Member);
  auto pAbsMember = static_cast<const ezAbstractMemberProperty*>(pProp);
  EZ_TEST_INT(pProp->GetFlags().GetValue(), expectedFlags.GetValue());
  EZ_TEST_BOOL(pProp->GetSpecificType() == ezGetStaticRTTI<T>());
  void* pData = nullptr;
  pAbsMember->GetValuePtr(pObject, &pData);
  EZ_TEST_BOOL(pData == pExpectedValue);

  // Set value to null.
  {
    void* pDataNull = nullptr;
    pAbsMember->SetValuePtr(pObject, &pDataNull);
    void* pDataNull2 = nullptr;
    pAbsMember->GetValuePtr(pObject, &pDataNull2);
    EZ_TEST_BOOL(pDataNull == pDataNull2);
  }

  // Set value to new instance.
  {
    void* pNewData = pAbsMember->GetSpecificType()->GetAllocator()->Allocate<void>();
    pAbsMember->SetValuePtr(pObject, &pNewData);
    void* pData2 = nullptr;
    pAbsMember->GetValuePtr(pObject, &pData2);
    EZ_TEST_BOOL(pNewData == pData2);
  }

  // Delete old value
  pAbsMember->GetSpecificType()->GetAllocator()->Deallocate(pData);
}

EZ_CREATE_SIMPLE_TEST(Reflection, Pointer)
{
  const ezRTTI* pRtti = ezGetStaticRTTI<ezTestPtr>();
  if (!EZ_TEST_BOOL(pRtti != nullptr))
    return;
  EZ_ANALYSIS_ASSUME(pRtti != nullptr);

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Member Property Ptr")
  {
    ezTestPtr containers;
    {
      const ezAbstractProperty* pProp = pRtti->FindPropertyByName("ConstCharPtr");
      if (EZ_TEST_BOOL(pProp != nullptr))
      {
        EZ_ANALYSIS_ASSUME(pProp != nullptr);
        EZ_TEST_BOOL(pProp->GetCategory() == ezPropertyCategory::Member);
        EZ_TEST_INT(pProp->GetFlags().GetValue(), (ezPropertyFlags::StandardType | ezPropertyFlags::Const).GetValue());
        EZ_TEST_BOOL(pProp->GetSpecificType() == ezGetStaticRTTI<const char*>());
      }
    }

    TestPointerMemberProperty<ezTestArrays>(
      "ArraysPtr", &containers, pRtti, ezPropertyFlags::Class | ezPropertyFlags::Pointer | ezPropertyFlags::PointerOwner, containers.m_pArrays);
    TestPointerMemberProperty<ezTestArrays>("ArraysPtrDirect", &containers, pRtti,
      ezPropertyFlags::Class | ezPropertyFlags::Pointer | ezPropertyFlags::PointerOwner, containers.m_pArraysDirect);
  }

  ezTestPtr containers;
  ezDefaultMemoryStreamStorage StreamStorage;

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Serialize Property Ptr")
  {
    containers.m_sString = "Test";

    containers.m_pArrays = EZ_DEFAULT_NEW(ezTestArrays);
    containers.m_pArrays->m_Deque.PushBack(ezTestArrays());

    containers.m_ArrayPtr.PushBack(EZ_DEFAULT_NEW(ezTestArrays));
    containers.m_ArrayPtr[0]->m_Hybrid.PushBack(5.0);

    containers.m_SetPtr.Insert(EZ_DEFAULT_NEW(ezTestSets));
    containers.m_SetPtr.GetIterator().Key()->m_Array.PushBack("BLA");
  }

  TestSerialization<ezTestPtr>(containers);
}
