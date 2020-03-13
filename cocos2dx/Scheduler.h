#pragma once
#include "ccMacros.h"
#include "cocoa/Object.h"


NS_CC_BEGIN

#define kCCPrioritySystem INT_MIN


class CC_DLL CCTimer : public Object
{
public:
    CCTimer(void);

    static CCTimer* timerWithTarget(Object* pTarget, SEL_SCHEDULE pfnSelector);
    static CCTimer* timerWithTarget(Object* pTarget, SEL_SCHEDULE pfnSelector, float fSeconds);

public:
    void update(float dt);

    float getInterval(void) const;
    void  setInterval(float fInterval);

    SEL_SCHEDULE getSelector() const;

    bool initWithTarget(Object* pTarget, SEL_SCHEDULE pfnSelector);
    bool initWithTarget(Object* pTarget, SEL_SCHEDULE pfnSelector, float fSeconds, unsigned int nRepeat, float fDelay);



protected:
    Object* m_pTarget;

    float m_fElapsed;
    float m_fDelay;
    float m_fInterval;

    bool m_bRunForever;
    bool m_bUseDelay;

    unsigned int m_uTimesExecuted;
    unsigned int m_uRepeat; //0 = once, 1 is 2 x executed

    SEL_SCHEDULE m_pfnSelector;
};



class CC_DLL Scheduler : public Object
{
public:
    Scheduler();
    ~Scheduler(void);

public:
    void         update(float dt);

    inline void  setTimeScale(float fTimeScale) { m_fTimeScale = fTimeScale; }
    inline float getTimeScale(void) { return m_fTimeScale; }


public:
    void    scheduleUpdateForTarget(Object* pTarget, int nPriority, bool bPaused);
    void    scheduleSelector(SEL_SCHEDULE pfnSelector, Object* pTarget, float fInterval, bool bPaused);
    void    scheduleSelector(SEL_SCHEDULE pfnSelector, Object* pTarget, float fInterval, unsigned int repeat, float delay, bool bPaused);

    void    unscheduleUpdateForTarget(const Object* pTarget);
    void    unscheduleSelector(SEL_SCHEDULE pfnSelector, Object* pTarget);

    void    unscheduleAllWithMinPriority(int nMinPriority);
    void    unscheduleAllForTarget(Object* pTarget);
    void    unscheduleAll(void);


private:
	void    priorityIn(struct _listEntry** ppList, Object* pTarget, int nPriority, bool bPaused);
	void    appendIn(struct _listEntry** ppList, Object* pTarget, bool bPaused);

	void    removeHashElement(struct _hashSelectorEntry* pElement);
	void    removeUpdateFromHash(struct _listEntry* entry);


public:
    void    pauseTarget(Object* pTarget);
    void    resumeTarget(Object* pTarget);
    bool    isTargetPaused(Object* pTarget);

    Set*    pauseAllTargets();
    Set*    pauseAllTargetsWithMinPriority(int nMinPriority);

    void    resumeTargets(Set* targetsToResume);


protected:
    float   m_fTimeScale;

	struct _listEntry* m_pUpdatesNegList;        // list of priority < 0
	struct _listEntry* m_pUpdates0List;            // list priority == 0
	struct _listEntry* m_pUpdatesPosList;        // list priority > 0
	struct _hashUpdateEntry* m_pHashForUpdates; // hash used to fetch quickly the list entries for pause,delete,etc

	struct _hashSelectorEntry* m_pHashForTimers;
	struct _hashSelectorEntry* m_pCurrentTarget;

    bool m_bUpdateHashLocked;
    bool m_bCurrentTargetSalvaged;
};




NS_CC_END