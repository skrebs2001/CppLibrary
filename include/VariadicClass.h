#pragma once

class MyBase
{
public:
    MyBase() { Mercury::Trace("MyBase ctor\n"); }

    explicit MyBase(const MyBase&)
    {
        Mercury::Trace("MyBase explicit ctor\n");
    }

    ~MyBase() { Mercury::Trace("MyBase dtor\n"); }

    void TheBaseFunction() const { Mercury::Trace("TheBaseFunction()\n"); }
};

template <typename T>
class AdvancedHelper : virtual public T
{
public:
    //AdvancedHelper() { Mercury::Trace("AdvancedHelper ctor\n"); }

    explicit AdvancedHelper(const T& p)
        : T(p)
    {
        Mercury::Trace("AdvancedHelper explicit ctor\n");
    }
    ~AdvancedHelper()
    {
        Mercury::Trace("AdvancedHelper dtor\n");
    }
};

template <typename T>
class LightshowHelper : virtual public T
{
public:
    LightshowHelper() { Mercury::Trace("LightshowHelper ctor\n"); }

    ~LightshowHelper() { Mercury::Trace("LightshowHelper dtor\n"); }

    void TheLightshowHelperFunction() const { Mercury::Trace("TheLightshowHelperFunction()\n"); }
};

template <typename T>
class SequenceHelper : virtual public T
{
public:
    SequenceHelper() { Mercury::Trace("SequenceHelper ctor\n"); }

    ~SequenceHelper() { Mercury::Trace("SequenceHelper dtor\n"); }

    void TheSequenceHelperFunction() const { Mercury::Trace("TheSequenceHelperFunction()\n"); }
};

template <typename T>
class MessageHelper : virtual public T
{
public:
    MessageHelper() { Mercury::Trace("MessageHelper ctor\n"); }

    ~MessageHelper() { Mercury::Trace("MessageHelper dtor\n"); }

    void TheMessageHelperFunction() const { Mercury::Trace("TheMessageHelperFunction()\n"); }
};

//template <typename T, template <class> typename M1, template <class> typename M2, template <class> typename M3>
//class CompositeScene : public M1<T>, public M2<T>, public M3<T>
//{
//public:
//    CompositeScene() { Mercury::Trace("CompositeScene ctor\n"); }
//
//    ~CompositeScene() { Mercury::Trace("*** CompositeScene dtor\n"); }
//
//    void TheCompositeSceneFunction() const { Mercury::Trace("TheCompositeSceneFunction()\n"); }
//};

//template <typename Tbase, template <class> typename... Mixins>
//class CompositeScene : virtual public Tbase, public Mixins<Tbase>...
//{
//public:
//    CompositeScene()
//    : Tbase()
//    , Mixins<Tbase>()...
//    {
//        Mercury::Trace("CompositeScene ctor\n");
//    }
//
//    ~CompositeScene() { Mercury::Trace("*** CompositeScene dtor\n"); }
//
//    void TheCompositeSceneFunction() const { Mercury::Trace("TheCompositeSceneFunction()\n"); }
//};

template <typename Tbase, template <class> typename Tmixin, template <class> typename... Mixins>
class CompositeScene : public Tmixin<Tbase>, public Mixins<Tbase>...
{
public:
    CompositeScene()
        : Tmixin<Tbase>()
        , Mixins<Tbase>()...
    {
        Mercury::Trace("CompositeScene ctor\n");
    }

    ~CompositeScene() { Mercury::Trace("*** CompositeScene dtor\n"); }

    void TheCompositeSceneFunction() const { Mercury::Trace("TheCompositeSceneFunction()\n"); }
};

class MyGameScene : public CompositeScene<MyBase, SequenceHelper, MessageHelper, LightshowHelper>
{
public:
    MyGameScene() { Mercury::Trace("MyGameScene ctor\n"); }

    ~MyGameScene() { Mercury::Trace("*** MyGameScene dtor\n"); }

    void someFunc()
    {
        TheBaseFunction();
        TheLightshowHelperFunction();
        TheMessageHelperFunction();
        TheSequenceHelperFunction();
        TheCompositeSceneFunction();
    }
};

void MyClassTest()
{
    {
        MyGameScene scene;
        scene.someFunc();
        scene.TheBaseFunction();
        scene.TheLightshowHelperFunction();
        scene.TheMessageHelperFunction();
        scene.TheSequenceHelperFunction();
        scene.TheCompositeSceneFunction();
    }
    Mercury::Trace("Done\n");

    {
        CompositeScene<MyBase, MessageHelper, LightshowHelper> scene;
    }
    Mercury::Trace("Done\n");

    {
        CompositeScene<MyBase, LightshowHelper> scene;
    }
    Mercury::Trace("Done\n");
}

//
class AbstractScene : public Slot::MercuryScene
{
public:
    virtual ~AbstractScene() = default;

    std::string GetDescription() const { return m_sDescription; }

    Mercury::Timeline* GetTimeline() const { return m_pTimeline; }
    Mercury::Sequence* GetSequence() { return &m_Sequence; }

protected:
    void SetDescription(const std::string& s) { m_sDescription = s; }
    AbstractScene() = default;

private:
    std::string m_sDescription;
};

// Abstract Decorator class, adds functionality to Scene
class AbstractDecorator : public AbstractScene
{
protected:
    explicit AbstractDecorator(std::unique_ptr<AbstractScene> pScene)
        : pScene(std::move(pScene))
    {
    }

    AbstractDecorator() = delete;

    std::unique_ptr<AbstractScene> pScene;

public:
    ~AbstractDecorator() = default;
};

// Decorate sequence operations
class SequenceDecorator : public AbstractDecorator
{
public:
    explicit SequenceDecorator(std::unique_ptr<AbstractScene> pScene)
        : AbstractDecorator(std::move(pScene))
    {
        SetDescription(pScene->GetDescription() + ", SequenceDecorator");
    }

    ~SequenceDecorator() { Mercury::Trace("*** SequenceDecorator dtor\n"); }

    void Clear(const eastl::string& sName) const { pScene->GetSequence()->Clear(sName); }

    void Complete(const eastl::string& sName) const { pScene->GetSequence()->Complete(sName); }

    bool IsEmpty(const eastl::string& sName) const { return pScene->GetSequence()->IsEmpty(sName); }
};

// Decorate message operations
class MessageDecorator : public AbstractDecorator
{
public:
    explicit MessageDecorator(std::unique_ptr<AbstractScene> scene)
        : AbstractDecorator(std::move(scene))
    {
        SetDescription(pScene->GetDescription() + ", MessageDecorator");
    }

    ~MessageDecorator() { Mercury::Trace("*** MessageDecorator dtor\n"); }
};

class TimelineDecorator : public AbstractDecorator
{
public:
    explicit TimelineDecorator(std::unique_ptr<AbstractScene> scene)
        : AbstractDecorator(std::move(scene))
    {
        SetDescription(pScene->GetDescription() + ", TimelineDecorator");
    }

    ~TimelineDecorator() { Mercury::Trace("*** TimelineDecorator dtor\n"); }

    Mercury::Timeline* FindTimeline(const eastl::string& sName) const { return FindTimeline(pScene->GetTimeline(), sName); }

    Mercury::Timeline* FindTimeline(Mercury::Timeline* pTimeline, const eastl::string& sName) const
    {
        Mercury::Timeline* pDestTimeline = nullptr;
        EXECUTE_THEN_CHECK(Aristocrat::GDK::Slot::FindTimeline(pTimeline, sName, &pDestTimeline));
        return pDestTimeline;
    }

    Mercury::Text* FindText(const eastl::string& sName) const { return FindText(pScene->GetTimeline(), sName); }

    Mercury::Text* FindText(Mercury::Timeline* pTimeline, const eastl::string& sName) const
    {
        Mercury::Text* pDestText = nullptr;
        EXECUTE_THEN_CHECK(Aristocrat::GDK::Slot::FindText(pTimeline, sName, &pDestText));
        return pDestText;
    }
};

// Concrete scene class
class BaseScene : public AbstractScene
{
public:
    BaseScene() { SetDescription(GetDescription() + "BaseScene"); }

    ~BaseScene() { Mercury::Trace("*** BaseScene dtor\n"); }
};

void DoDecoratorTest()
{
    auto pBaseScene = std::make_unique<BaseScene>();
    Mercury::Trace("%s\n", pBaseScene->GetDescription().c_str());

    // Add Message decorator
    auto pSequenceDecorator = std::make_unique<SequenceDecorator>(std::move(pBaseScene));
    Mercury::Trace("%s\n", pSequenceDecorator->GetDescription().c_str());

    // Add Timeline decorator
    auto pTimelineDecorator = std::make_unique<TimelineDecorator>(std::move(pSequenceDecorator));
    Mercury::Trace("%s\n", pTimelineDecorator->GetDescription().c_str());

    pTimelineDecorator->FindTimeline(L"test");
    pSequenceDecorator->Clear("test");

    auto MyScene = std::make_unique<TimelineDecorator>(std::make_unique<MessageDecorator>(std::make_unique<BaseScene>()));
    Mercury::Trace("%s\n", MyScene->GetDescription().c_str());
}
