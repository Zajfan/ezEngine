#include <CoreTest/CoreTestPCH.h>

#include <Core/World/World.h>
#include <Foundation/Memory/FrameAllocator.h>
#include <Foundation/Time/Clock.h>

namespace
{
  struct ezMsgTest : public ezMessage
  {
    EZ_DECLARE_MESSAGE_TYPE(ezMsgTest, ezMessage);
  };

  // clang-format off
  EZ_IMPLEMENT_MESSAGE_TYPE(ezMsgTest);
  EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(ezMsgTest, 1, ezRTTIDefaultAllocator<ezMsgTest>)
  EZ_END_DYNAMIC_REFLECTED_TYPE;
  // clang-format on

  struct TestMessage1 : public ezMsgTest
  {
    EZ_DECLARE_MESSAGE_TYPE(TestMessage1, ezMsgTest);

    int m_iValue;
  };

  struct TestMessage2 : public ezMsgTest
  {
    EZ_DECLARE_MESSAGE_TYPE(TestMessage2, ezMsgTest);

    virtual ezInt32 GetSortingKey() const override { return 2; }

    int m_iValue;
  };

  // clang-format off
  EZ_IMPLEMENT_MESSAGE_TYPE(TestMessage1);
  EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(TestMessage1, 1, ezRTTIDefaultAllocator<TestMessage1>)
  EZ_END_DYNAMIC_REFLECTED_TYPE;

  EZ_IMPLEMENT_MESSAGE_TYPE(TestMessage2);
  EZ_BEGIN_DYNAMIC_REFLECTED_TYPE(TestMessage2, 1, ezRTTIDefaultAllocator<TestMessage2>)
  EZ_END_DYNAMIC_REFLECTED_TYPE;
  // clang-format on

  class TestComponentMsg;
  using TestComponentMsgManager = ezComponentManager<TestComponentMsg, ezBlockStorageType::FreeList>;

  class TestComponentMsg : public ezComponent
  {
    EZ_DECLARE_COMPONENT_TYPE(TestComponentMsg, ezComponent, TestComponentMsgManager);

  public:
    TestComponentMsg()

      = default;
    ~TestComponentMsg() = default;

    virtual void SerializeComponent(ezWorldWriter& inout_stream) const override {}
    virtual void DeserializeComponent(ezWorldReader& inout_stream) override {}

    void OnTestMessage(TestMessage1& ref_msg) { m_iSomeData += ref_msg.m_iValue; }

    void OnTestMessage2(TestMessage2& ref_msg) { m_iSomeData2 += 2 * ref_msg.m_iValue; }

    ezInt32 m_iSomeData = 1;
    ezInt32 m_iSomeData2 = 2;
  };

  // clang-format off
  EZ_BEGIN_COMPONENT_TYPE(TestComponentMsg, 1, ezComponentMode::Static)
  {
    EZ_BEGIN_MESSAGEHANDLERS
    {
      EZ_MESSAGE_HANDLER(TestMessage1, OnTestMessage),
      EZ_MESSAGE_HANDLER(TestMessage2, OnTestMessage2),
    }
    EZ_END_MESSAGEHANDLERS;
  }
  EZ_END_COMPONENT_TYPE;
  // clang-format on

  void ResetComponents(ezGameObject& ref_object)
  {
    TestComponentMsg* pComponent = nullptr;
    if (ref_object.TryGetComponentOfBaseType(pComponent))
    {
      pComponent->m_iSomeData = 1;
      pComponent->m_iSomeData2 = 2;
    }

    for (auto it = ref_object.GetChildren(); it.IsValid(); ++it)
    {
      ResetComponents(*it);
    }
  }
} // namespace

EZ_CREATE_SIMPLE_TEST(World, Messaging)
{
  ezWorldDesc worldDesc("Test");
  ezWorld world(worldDesc);
  EZ_LOCK(world.GetWriteMarker());

  TestComponentMsgManager* pManager = world.GetOrCreateComponentManager<TestComponentMsgManager>();

  ezGameObjectDesc desc;
  desc.m_sName.Assign("Root");
  ezGameObject* pRoot = nullptr;
  world.CreateObject(desc, pRoot);
  TestComponentMsg* pComponent = nullptr;
  pManager->CreateComponent(pRoot, pComponent);

  ezGameObject* pParents[2];
  desc.m_hParent = pRoot->GetHandle();
  desc.m_sName.Assign("Parent1");
  world.CreateObject(desc, pParents[0]);
  pManager->CreateComponent(pParents[0], pComponent);

  desc.m_sName.Assign("Parent2");
  world.CreateObject(desc, pParents[1]);
  pManager->CreateComponent(pParents[1], pComponent);

  for (ezUInt32 i = 0; i < 2; ++i)
  {
    desc.m_hParent = pParents[i]->GetHandle();
    for (ezUInt32 j = 0; j < 4; ++j)
    {
      ezStringBuilder sb;
      sb.AppendFormat("Parent{0}_Child{1}", i + 1, j + 1);
      desc.m_sName.Assign(sb.GetData());

      ezGameObject* pObject = nullptr;
      world.CreateObject(desc, pObject);
      pManager->CreateComponent(pObject, pComponent);
    }
  }

  // one update step so components are initialized
  world.Update();

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Direct Routing")
  {
    ResetComponents(*pRoot);

    TestMessage1 msg;
    msg.m_iValue = 4;
    pParents[0]->SendMessage(msg);

    TestMessage2 msg2;
    msg2.m_iValue = 4;
    pParents[0]->SendMessage(msg2);

    TestComponentMsg* pComponent2 = nullptr;
    EZ_TEST_BOOL(pParents[0]->TryGetComponentOfBaseType(pComponent2));
    EZ_TEST_INT(pComponent2->m_iSomeData, 5);
    EZ_TEST_INT(pComponent2->m_iSomeData2, 10);

    // siblings, parent and children should not be affected
    EZ_TEST_BOOL(pParents[1]->TryGetComponentOfBaseType(pComponent2));
    EZ_TEST_INT(pComponent2->m_iSomeData, 1);
    EZ_TEST_INT(pComponent2->m_iSomeData2, 2);

    EZ_TEST_BOOL(pRoot->TryGetComponentOfBaseType(pComponent2));
    EZ_TEST_INT(pComponent2->m_iSomeData, 1);
    EZ_TEST_INT(pComponent2->m_iSomeData2, 2);

    for (auto it = pParents[0]->GetChildren(); it.IsValid(); ++it)
    {
      EZ_TEST_BOOL(it->TryGetComponentOfBaseType(pComponent2));
      EZ_TEST_INT(pComponent2->m_iSomeData, 1);
      EZ_TEST_INT(pComponent2->m_iSomeData2, 2);
    }
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Queuing")
  {
    ResetComponents(*pRoot);

    for (ezUInt32 i = 0; i < 10; ++i)
    {
      TestMessage1 msg;
      msg.m_iValue = i;
      pRoot->PostMessage(msg, ezTime::MakeZero(), ezObjectMsgQueueType::NextFrame);

      TestMessage2 msg2;
      msg2.m_iValue = i;
      pRoot->PostMessage(msg2, ezTime::MakeZero(), ezObjectMsgQueueType::NextFrame);
    }

    world.Update();

    TestComponentMsg* pComponent2 = nullptr;
    EZ_TEST_BOOL(pRoot->TryGetComponentOfBaseType(pComponent2));
    EZ_TEST_INT(pComponent2->m_iSomeData, 46);
    EZ_TEST_INT(pComponent2->m_iSomeData2, 92);

    ezFrameAllocator::Reset();
  }

  EZ_TEST_BLOCK(ezTestBlock::Enabled, "Queuing with delay")
  {
    ResetComponents(*pRoot);

    for (ezUInt32 i = 0; i < 10; ++i)
    {
      TestMessage1 msg;
      msg.m_iValue = i;
      pRoot->PostMessage(msg, ezTime::MakeFromSeconds(i + 1));

      TestMessage2 msg2;
      msg2.m_iValue = i;
      pRoot->PostMessage(msg2, ezTime::MakeFromSeconds(i + 1));
    }

    world.GetClock().SetFixedTimeStep(ezTime::MakeFromSeconds(1.001f));

    int iDesiredValue = 1;
    int iDesiredValue2 = 2;

    for (ezUInt32 i = 0; i < 10; ++i)
    {
      iDesiredValue += i;
      iDesiredValue2 += i * 2;

      world.Update();

      TestComponentMsg* pComponent2 = nullptr;
      EZ_TEST_BOOL(pRoot->TryGetComponentOfBaseType(pComponent2));
      EZ_TEST_INT(pComponent2->m_iSomeData, iDesiredValue);
      EZ_TEST_INT(pComponent2->m_iSomeData2, iDesiredValue2);
    }

    ezFrameAllocator::Reset();
  }
}
