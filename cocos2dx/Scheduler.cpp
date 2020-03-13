#include "Scheduler.h"
#include "cocoa/data_support/uthash.h"
#include "cocoa/data_support/utlist.h"


NS_CC_BEGIN



//=====================================================
// CCTimer
//
CCTimer* CCTimer::timerWithTarget(Object* pTarget, SEL_SCHEDULE pfnSelector)
{
	CCTimer* pTimer = new CCTimer();

	pTimer->initWithTarget(pTarget, pfnSelector, 0.0f, kCCRepeatForever, 0.0f);
	pTimer->autorelease();

	return pTimer;
}

CCTimer* CCTimer::timerWithTarget(Object* pTarget, SEL_SCHEDULE pfnSelector, float fSeconds)
{
	CCTimer* pTimer = new CCTimer();

	pTimer->initWithTarget(pTarget, pfnSelector, fSeconds, kCCRepeatForever, 0.0f);
	pTimer->autorelease();

	return pTimer;
}

CCTimer::CCTimer()
	: m_pTarget(NULL)
	, m_fElapsed(-1)
	, m_bRunForever(false)
	, m_bUseDelay(false)
	, m_uTimesExecuted(0)
	, m_uRepeat(0)
	, m_fDelay(0.0f)
	, m_fInterval(0.0f)
	, m_pfnSelector(NULL)
{
}


bool CCTimer::initWithTarget(Object* pTarget, SEL_SCHEDULE pfnSelector)
{
	return initWithTarget(pTarget, pfnSelector, 0, kCCRepeatForever, 0.0f);
}

bool CCTimer::initWithTarget(Object* pTarget, SEL_SCHEDULE pfnSelector, float fSeconds, unsigned int nRepeat, float fDelay)
{
	m_pTarget = pTarget;
	m_pfnSelector = pfnSelector;
	m_fElapsed = -1;
	m_fInterval = fSeconds;
	m_fDelay = fDelay;
	m_bUseDelay = (fDelay > 0.0f) ? true : false;
	m_uRepeat = nRepeat;
	m_bRunForever = (nRepeat == kCCRepeatForever) ? true : false;
	return true;
}

float CCTimer::getInterval() const
{
	return m_fInterval;
}

void CCTimer::setInterval(float fInterval)
{
	m_fInterval = fInterval;
}

SEL_SCHEDULE CCTimer::getSelector() const
{
	return m_pfnSelector;
}


void CCTimer::update(float dt)
{
	if (m_fElapsed == -1)
	{
		m_fElapsed = 0;
		m_uTimesExecuted = 0;
	}
	else
	{
		if (m_bRunForever && !m_bUseDelay)
		{//standard timer usage
			m_fElapsed += dt;
			if (m_fElapsed >= m_fInterval)
			{
				if (m_pTarget && m_pfnSelector)
				{
					(m_pTarget->*m_pfnSelector)(m_fElapsed);
				}
				m_fElapsed = 0;
			}
		}
		else
		{//advanced usage
			m_fElapsed += dt;
			if (m_bUseDelay)
			{
				if (m_fElapsed >= m_fDelay)
				{
					if (m_pTarget && m_pfnSelector)
					{
						(m_pTarget->*m_pfnSelector)(m_fElapsed);
					}

					m_fElapsed = m_fElapsed - m_fDelay;
					m_uTimesExecuted += 1;
					m_bUseDelay = false;
				}
			}
			else
			{
				if (m_fElapsed >= m_fInterval)
				{
					if (m_pTarget && m_pfnSelector)
					{
						(m_pTarget->*m_pfnSelector)(m_fElapsed);
					}

					m_fElapsed = 0;
					m_uTimesExecuted += 1;

				}
			}

			if (!m_bRunForever && m_uTimesExecuted > m_uRepeat)
			{    //unschedule timer
				Director::sharedDirector()->getScheduler()->unscheduleSelector(m_pfnSelector, m_pTarget);
			}
		}
	}
}


//
//=====================================================



// A list double-linked list used for "updates with priority"
typedef struct _listEntry
{
	struct _listEntry* prev, * next;
	Object*			   target;        // not retained (retained by hashUpdateEntry)
	int                priority;
	bool               paused;
	bool               markedForDeletion; // selector will no longer be called and entry will be removed at end of the next tick
} tListEntry;

typedef struct _hashUpdateEntry
{
	tListEntry**	list;        // Which list does it belong to ?
	tListEntry*		entry;       // entry in the list
	Object*			target;      // hash key (retained)
	UT_hash_handle  hh;
} tHashUpdateEntry;

// Hash Element used for "selectors with interval"
typedef struct _hashSelectorEntry
{
	ccArray*		timers;
	Object*			target;    // hash key (retained)
	unsigned int    timerIndex;
	CCTimer*		currentTimer;
	bool            currentTimerSalvaged;
	bool            paused;
	UT_hash_handle  hh;
} tHashTimerEntry;



Scheduler::Scheduler(void)
	: m_fTimeScale(1.0f)

	, m_pUpdatesNegList(NULL)
	, m_pUpdates0List(NULL)
	, m_pUpdatesPosList(NULL)
	, m_pHashForUpdates(NULL)

	, m_pHashForTimers(NULL)
	, m_pCurrentTarget(NULL)
	, m_bUpdateHashLocked(false)
	, m_bCurrentTargetSalvaged(false)
{

}

Scheduler::~Scheduler(void)
{
	unscheduleAll();
}


void Scheduler::update(float dt)
{
	m_bUpdateHashLocked = true;

	if (m_fTimeScale != 1.0f)
	{
		dt *= m_fTimeScale;
	}

	// Iterate over all the Updates' selectors
	tListEntry* pEntry, * pTmp;

	// updates with priority < 0
	DL_FOREACH_SAFE(m_pUpdatesNegList, pEntry, pTmp)
	{
		if ((!pEntry->paused) && (!pEntry->markedForDeletion))
		{
			pEntry->target->update(dt);
		}
	}

	// updates with priority == 0
	DL_FOREACH_SAFE(m_pUpdates0List, pEntry, pTmp)
	{
		if ((!pEntry->paused) && (!pEntry->markedForDeletion))
		{
			pEntry->target->update(dt);
		}
	}

	// updates with priority > 0
	DL_FOREACH_SAFE(m_pUpdatesPosList, pEntry, pTmp)
	{
		if ((!pEntry->paused) && (!pEntry->markedForDeletion))
		{
			pEntry->target->update(dt);
		}
	}

	// Iterate over all the custom selectors
	for (tHashTimerEntry* elt = m_pHashForTimers; elt != NULL; )
	{
		m_pCurrentTarget = elt;
		m_bCurrentTargetSalvaged = false;

		if (!m_pCurrentTarget->paused)
		{
			// The 'timers' array may change while inside this loop
			for (elt->timerIndex = 0; elt->timerIndex < elt->timers->num; ++(elt->timerIndex))
			{
				elt->currentTimer = (CCTimer*)(elt->timers->arr[elt->timerIndex]);
				elt->currentTimerSalvaged = false;

				elt->currentTimer->update(dt);

				if (elt->currentTimerSalvaged)
				{
					// The currentTimer told the remove itself. To prevent the timer from
					// accidentally deallocating itself before finishing its step, we retained
					// it. Now that step is done, it's safe to release it.
					elt->currentTimer->release();
				}

				elt->currentTimer = NULL;
			}
		}

		// elt, at this moment, is still valid
		// so it is safe to ask this here (issue #490)
		elt = (tHashTimerEntry*)elt->hh.next;

		// only delete currentTarget if no actions were scheduled during the cycle (issue #481)
		if (m_bCurrentTargetSalvaged && m_pCurrentTarget->timers->num == 0)
		{
			removeHashElement(m_pCurrentTarget);
		}
	}

	// delete all updates that are marked for deletion
	// updates with priority < 0
	DL_FOREACH_SAFE(m_pUpdatesNegList, pEntry, pTmp)
	{
		if (pEntry->markedForDeletion)
		{
			this->removeUpdateFromHash(pEntry);
		}
	}

	// updates with priority == 0
	DL_FOREACH_SAFE(m_pUpdates0List, pEntry, pTmp)
	{
		if (pEntry->markedForDeletion)
		{
			this->removeUpdateFromHash(pEntry);
		}
	}

	// updates with priority > 0
	DL_FOREACH_SAFE(m_pUpdatesPosList, pEntry, pTmp)
	{
		if (pEntry->markedForDeletion)
		{
			this->removeUpdateFromHash(pEntry);
		}
	}

	m_bUpdateHashLocked = false;
	m_pCurrentTarget = NULL;
}


void Scheduler::scheduleUpdateForTarget(Object* pTarget, int nPriority, bool bPaused)
{

	tHashUpdateEntry* pHashElement = NULL;
	HASH_FIND_INT(m_pHashForUpdates, &pTarget, pHashElement);
	if (pHashElement)
	{
#if COCOS2D_DEBUG >= 1
		CCAssert(pHashElement->entry->markedForDeletion, "");
#endif
		// TODO: check if priority has changed!
		pHashElement->entry->paused = bPaused;
		pHashElement->entry->markedForDeletion = false;
		return;
	}

	// most of the updates are going to be 0, that's way there
	// is an special list for updates with priority 0
	if (nPriority == 0)
	{
		appendIn(&m_pUpdates0List, pTarget, bPaused);
	}
	else if (nPriority < 0)
	{
		priorityIn(&m_pUpdatesNegList, pTarget, nPriority, bPaused);
	}
	else
	{
		priorityIn(&m_pUpdatesPosList, pTarget, nPriority, bPaused);
	}
}

void Scheduler::scheduleSelector(SEL_SCHEDULE pfnSelector, Object* pTarget, float fInterval, bool bPaused)
{
	this->scheduleSelector(pfnSelector, pTarget, fInterval, kCCRepeatForever, 0.0f, bPaused);
}


void Scheduler::scheduleSelector(SEL_SCHEDULE pfnSelector, Object* pTarget, float fInterval, unsigned int repeat, float delay, bool bPaused)
{
	CCAssert(pfnSelector, "Argument selector must be non-NULL");
	CCAssert(pTarget, "Argument target must be non-NULL");

	tHashTimerEntry* pElement = NULL;
	HASH_FIND_INT(m_pHashForTimers, &pTarget, pElement);

	if (!pElement)
	{
		pElement = (tHashTimerEntry*)calloc(sizeof(*pElement), 1);
		pElement->target = pTarget;
		if (pTarget)
		{
			pTarget->retain();
		}
		HASH_ADD_INT(m_pHashForTimers, target, pElement);

		// Is this the 1st element ? Then set the pause level to all the selectors of this target
		pElement->paused = bPaused;
	}
	else
	{
		CCAssert(pElement->paused == bPaused, "");
	}

	if (pElement->timers == NULL)
	{
		pElement->timers = ccArrayNew(10);
	}
	else
	{
		for (unsigned int i = 0; i < pElement->timers->num; ++i)
		{
			CCTimer* timer = (CCTimer*)pElement->timers->arr[i];

			if (pfnSelector == timer->getSelector())
			{
				CCLOG("CCScheduler#scheduleSelector. Selector already scheduled. Updating interval from: %.4f to %.4f", timer->getInterval(), fInterval);
				timer->setInterval(fInterval);
				return;
			}
		}
		ccArrayEnsureExtraCapacity(pElement->timers, 1);
	}

	CCTimer* pTimer = new CCTimer();
	pTimer->initWithTarget(pTarget, pfnSelector, fInterval, repeat, delay);
	ccArrayAppendObject(pElement->timers, pTimer);
	pTimer->release();
}





void Scheduler::appendIn(_listEntry** ppList, Object* pTarget, bool bPaused)
{
	tListEntry* pListElement = (tListEntry*)malloc(sizeof(*pListElement));

	pListElement->target = pTarget;
	pListElement->paused = bPaused;
	pListElement->markedForDeletion = false;

	DL_APPEND(*ppList, pListElement);

	// update hash entry for quicker access
	tHashUpdateEntry* pHashElement = (tHashUpdateEntry*)calloc(sizeof(*pHashElement), 1);
	pHashElement->target = pTarget;
	pTarget->retain();
	pHashElement->list = ppList;
	pHashElement->entry = pListElement;
	HASH_ADD_INT(m_pHashForUpdates, target, pHashElement);
}


void Scheduler::priorityIn(tListEntry** ppList, Object* pTarget, int nPriority, bool bPaused)
{
	tListEntry* pListElement = (tListEntry*)malloc(sizeof(*pListElement));

	pListElement->target = pTarget;
	pListElement->priority = nPriority;
	pListElement->paused = bPaused;
	pListElement->next = pListElement->prev = NULL;
	pListElement->markedForDeletion = false;

	// empty list ?
	if (!*ppList)
	{
		DL_APPEND(*ppList, pListElement);
	}
	else
	{
		bool bAdded = false;

		for (tListEntry* pElement = *ppList; pElement; pElement = pElement->next)
		{
			if (nPriority < pElement->priority)
			{
				if (pElement == *ppList)
				{
					DL_PREPEND(*ppList, pListElement);
				}
				else
				{
					pListElement->next = pElement;
					pListElement->prev = pElement->prev;

					pElement->prev->next = pListElement;
					pElement->prev = pListElement;
				}

				bAdded = true;
				break;
			}
		}

		// Not added? priority has the higher value. Append it.
		if (!bAdded)
		{
			DL_APPEND(*ppList, pListElement);
		}
	}

	// update hash entry for quick access
	tHashUpdateEntry* pHashElement = (tHashUpdateEntry*)calloc(sizeof(*pHashElement), 1);
	pHashElement->target = pTarget;
	pTarget->retain();
	pHashElement->list = ppList;
	pHashElement->entry = pListElement;
	HASH_ADD_INT(m_pHashForUpdates, target, pHashElement);
}

void Scheduler::unscheduleUpdateForTarget(const Object* pTarget)
{
	if (pTarget == NULL)
	{
		return;
	}

	tHashUpdateEntry* pElement = NULL;
	HASH_FIND_INT(m_pHashForUpdates, &pTarget, pElement);
	if (pElement)
	{
		if (m_bUpdateHashLocked)
		{
			pElement->entry->markedForDeletion = true;
		}
		else
		{
			this->removeUpdateFromHash(pElement->entry);
		}
	}
}


void Scheduler::removeHashElement(_hashSelectorEntry* pElement)
{
	Object* target = pElement->target;

	ccArrayFree(pElement->timers);
	HASH_DEL(m_pHashForTimers, pElement);
	free(pElement);

	// make sure the target is released after we have removed the hash element
	// otherwise we access invalid memory when the release call deletes the target
	// and the target calls removeAllSelectors() during its destructor
	target->release();

}


void Scheduler::removeUpdateFromHash(struct _listEntry* entry)
{
	tHashUpdateEntry* element = NULL;

	HASH_FIND_INT(m_pHashForUpdates, &entry->target, element);
	if (element)
	{
		// list entry
		DL_DELETE(*element->list, element->entry);
		free(element->entry);

		// hash entry
		Object* pTarget = element->target;
		HASH_DEL(m_pHashForUpdates, element);
		free(element);

		// target#release should be the last one to prevent
		// a possible double-free. eg: If the [target dealloc] might want to remove it itself from there
		pTarget->release();
	}
}


void Scheduler::unscheduleSelector(SEL_SCHEDULE pfnSelector, Object* pTarget)
{
	// explicity handle nil arguments when removing an object
	if (pTarget == 0 || pfnSelector == 0)
	{
		return;
	}

	//CCAssert(pTarget);
	//CCAssert(pfnSelector);

	tHashTimerEntry* pElement = NULL;
	HASH_FIND_INT(m_pHashForTimers, &pTarget, pElement);

	if (pElement)
	{
		for (unsigned int i = 0; i < pElement->timers->num; ++i)
		{
			CCTimer* pTimer = (CCTimer*)(pElement->timers->arr[i]);

			if (pfnSelector == pTimer->getSelector())
			{
				if (pTimer == pElement->currentTimer && (!pElement->currentTimerSalvaged))
				{
					pElement->currentTimer->retain();
					pElement->currentTimerSalvaged = true;
				}

				ccArrayRemoveObjectAtIndex(pElement->timers, i, true);

				// update timerIndex in case we are in tick:, looping over the actions
				if (pElement->timerIndex >= i)
				{
					pElement->timerIndex--;
				}

				if (pElement->timers->num == 0)
				{
					if (m_pCurrentTarget == pElement)
					{
						m_bCurrentTargetSalvaged = true;
					}
					else
					{
						removeHashElement(pElement);
					}
				}

				return;
			}
		}
	}
}


void Scheduler::unscheduleAllWithMinPriority(int nMinPriority)
{
	// Custom Selectors
	tHashTimerEntry* pElement = NULL;
	tHashTimerEntry* pNextElement = NULL;
	for (pElement = m_pHashForTimers; pElement != NULL;)
	{
		// pElement may be removed in unscheduleAllSelectorsForTarget
		pNextElement = (tHashTimerEntry*)pElement->hh.next;
		unscheduleAllForTarget(pElement->target);

		pElement = pNextElement;
	}

	// Updates selectors
	tListEntry* pEntry, * pTmp;
	if (nMinPriority < 0)
	{
		DL_FOREACH_SAFE(m_pUpdatesNegList, pEntry, pTmp)
		{
			if (pEntry->priority >= nMinPriority)
			{
				unscheduleUpdateForTarget(pEntry->target);
			}
		}
	}

	if (nMinPriority <= 0)
	{
		DL_FOREACH_SAFE(m_pUpdates0List, pEntry, pTmp)
		{
			unscheduleUpdateForTarget(pEntry->target);
		}
	}

	DL_FOREACH_SAFE(m_pUpdatesPosList, pEntry, pTmp)
	{
		if (pEntry->priority >= nMinPriority)
		{
			unscheduleUpdateForTarget(pEntry->target);
		}
	}
}


void Scheduler::unscheduleAllForTarget(Object* pTarget)
{
	if (pTarget == NULL)
	{
		return;
	}

	tHashTimerEntry* pElement = NULL;
	HASH_FIND_INT(m_pHashForTimers, &pTarget, pElement);

	if (pElement)
	{
		if (ccArrayContainsObject(pElement->timers, pElement->currentTimer)
			&& (!pElement->currentTimerSalvaged))
		{
			pElement->currentTimer->retain();
			pElement->currentTimerSalvaged = true;
		}
		ccArrayRemoveAllObjects(pElement->timers);

		if (m_pCurrentTarget == pElement)
		{
			m_bCurrentTargetSalvaged = true;
		}
		else
		{
			removeHashElement(pElement);
		}
	}

	unscheduleUpdateForTarget(pTarget);
}

void Scheduler::unscheduleAll(void)
{
	unscheduleAllWithMinPriority(kCCPrioritySystem);
}




void Scheduler::resumeTarget(Object *pTarget)
{
    CCAssert(pTarget != NULL, "");

    // custom selectors
    tHashTimerEntry *pElement = NULL;
    HASH_FIND_INT(m_pHashForTimers, &pTarget, pElement);
    if (pElement)
    {
        pElement->paused = false;
    }

    // update selector
    tHashUpdateEntry *pElementUpdate = NULL;
    HASH_FIND_INT(m_pHashForUpdates, &pTarget, pElementUpdate);
    if (pElementUpdate)
    {
        CCAssert(pElementUpdate->entry != NULL, "");
        pElementUpdate->entry->paused = false;
    }
}

void Scheduler::pauseTarget(Object *pTarget)
{
    CCAssert(pTarget != NULL, "");

    // custom selectors
    tHashTimerEntry *pElement = NULL;
    HASH_FIND_INT(m_pHashForTimers, &pTarget, pElement);
    if (pElement)
    {
        pElement->paused = true;
    }

    // update selector
    tHashUpdateEntry *pElementUpdate = NULL;
    HASH_FIND_INT(m_pHashForUpdates, &pTarget, pElementUpdate);
    if (pElementUpdate)
    {
        CCAssert(pElementUpdate->entry != NULL, "");
        pElementUpdate->entry->paused = true;
    }
}

bool Scheduler::isTargetPaused(Object *pTarget)
{
    CCAssert( pTarget != NULL, "target must be non nil" );

    // Custom selectors
    tHashTimerEntry *pElement = NULL;
    HASH_FIND_INT(m_pHashForTimers, &pTarget, pElement);
    if( pElement )
    {
        return pElement->paused;
    }
    
    // We should check update selectors if target does not have custom selectors
	tHashUpdateEntry *elementUpdate = NULL;
	HASH_FIND_INT(m_pHashForUpdates, &pTarget, elementUpdate);
	if ( elementUpdate )
    {
		return elementUpdate->entry->paused;
    }
    
    return false;  // should never get here
}

Set* Scheduler::pauseAllTargets()
{
    return pauseAllTargetsWithMinPriority(kCCPrioritySystem);
}

Set* Scheduler::pauseAllTargetsWithMinPriority(int nMinPriority)
{
	Set* idsWithSelectors = new Set();// setWithCapacity:50];
    idsWithSelectors->autorelease();

    // Custom Selectors
    for(tHashTimerEntry *element = m_pHashForTimers; element != NULL;
        element = (tHashTimerEntry*)element->hh.next)
    {
        element->paused = true;
        idsWithSelectors->addObject(element->target);
    }

    // Updates selectors
    tListEntry *entry, *tmp;
    if(nMinPriority < 0) 
    {
        DL_FOREACH_SAFE( m_pUpdatesNegList, entry, tmp ) 
        {
            if(entry->priority >= nMinPriority) 
            {
                entry->paused = true;
                idsWithSelectors->addObject(entry->target);
            }
        }
    }

    if(nMinPriority <= 0) 
    {
        DL_FOREACH_SAFE( m_pUpdates0List, entry, tmp )
        {
            entry->paused = true;
            idsWithSelectors->addObject(entry->target);
        }
    }

    DL_FOREACH_SAFE( m_pUpdatesPosList, entry, tmp ) 
    {
        if(entry->priority >= nMinPriority) 
        {
            entry->paused = true;
            idsWithSelectors->addObject(entry->target);
        }
    }

    return idsWithSelectors;
}

void Scheduler::resumeTargets(Set* pTargetsToResume)
{
    SetIterator iter;
    for (iter = pTargetsToResume->begin(); iter != pTargetsToResume->end(); ++iter)
    {
        resumeTarget(*iter);
    }
}




NS_CC_END