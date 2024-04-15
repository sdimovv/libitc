/**
 * @file ITC_Event.c
 * @author Stan
 * @brief Implementation of the Interval Tree Clock's Event mechanism
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ITC_Event.h"
#include "ITC_Event_private.h"
#include "ITC_Port.h"
#include "ITC_package.h"

#include "ITC_Id_private.h"

/******************************************************************************
 * Private functions
 ******************************************************************************/

/**
 * @brief Validate an existing ITC Event
 *
 * Should be used to validate all incoming Events before any processing is done.
 *
 * @param pt_Event The Event to validate
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t validateEvent(
    const ITC_Event_t *const pt_Event
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    const ITC_Event_t *pt_CurrentEvent = pt_Event;
    const ITC_Event_t *pt_ParentCurrentEvent = NULL;
    const ITC_Event_t *pt_ParentRootEvent = NULL;

    if(!pt_CurrentEvent)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        /* Remember the root parent Event as this might be a subtree */
        pt_ParentRootEvent = pt_CurrentEvent->pt_Parent;

        pt_ParentCurrentEvent = pt_ParentRootEvent;
    }

    /* Perform a pre-order traversal */
    while (t_Status == ITC_STATUS_SUCCESS && pt_CurrentEvent)
    {
        /* Checks:
         *  - The parent pointer must match pt_ParentCurrentEvent.
         *  - If not a leaf node:
         *    - `pt_CurrentEvent->pt_Left != pt_CurrentEvent->pt_Right != NULL`
         */
        if (pt_ParentCurrentEvent != pt_CurrentEvent->pt_Parent ||
            (!ITC_EVENT_IS_LEAF_EVENT(pt_CurrentEvent) &&
             !ITC_EVENT_IS_VALID_PARENT(pt_CurrentEvent)))
        {
            t_Status = ITC_STATUS_CORRUPT_EVENT;
        }
        else
        {
            /* Descend into left tree */
            if (pt_CurrentEvent->pt_Left)
            {
                /* Remember the parent address */
                pt_ParentCurrentEvent = pt_CurrentEvent;

                pt_CurrentEvent = pt_CurrentEvent->pt_Left;
            }
            /* Descend into right tree */
            else if (pt_CurrentEvent->pt_Right)
            {
                /* Remember the parent address */
                pt_ParentCurrentEvent = pt_CurrentEvent;

                pt_CurrentEvent = pt_CurrentEvent->pt_Right;
            }
            else
            {
                /* Trust the parent pointers.
                 * They were validated on the way down */
                pt_ParentCurrentEvent = pt_CurrentEvent->pt_Parent;

                /* Loop until the current element is no longer reachable
                 * through he parent's right child */
                while (pt_ParentCurrentEvent != pt_ParentRootEvent &&
                    pt_ParentCurrentEvent->pt_Right == pt_CurrentEvent)
                {
                    pt_CurrentEvent = pt_CurrentEvent->pt_Parent;
                    pt_ParentCurrentEvent = pt_ParentCurrentEvent->pt_Parent;
                }

                /* There is a right subtree that has not been explored yet */
                if (pt_ParentCurrentEvent != pt_ParentRootEvent)
                {
                    pt_CurrentEvent = pt_ParentCurrentEvent->pt_Right;
                }
                else
                {
                    pt_CurrentEvent = NULL;
                }
            }
        }
    }

    return t_Status;
}

/**
 * @brief Allocate a new ITC Event
 *
 * @param ppt_Event (out) The pointer to the new Event
 * @param ppt_Parent The pointer to the parent Event in the tree.
 * Otherwise NULL.
 * @param t_Count The number of events witnessed by the Event
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t newEvent(
    ITC_Event_t **ppt_Event,
    ITC_Event_t *const pt_Parent,
    const ITC_Event_Counter_t t_Count
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Event_t *pt_Alloc = NULL;

    if (!ppt_Event)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        t_Status = ITC_Port_malloc((void **)&pt_Alloc, sizeof(ITC_Event_t));
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Initialise members */
        pt_Alloc->t_Count = t_Count;
        pt_Alloc->pt_Parent = pt_Parent;
        pt_Alloc->pt_Left = NULL;
        pt_Alloc->pt_Right = NULL;

        /* Return the pointer to the allocated memory */
        *ppt_Event = pt_Alloc;
    }

    return t_Status;
}

/**
 * @brief Clone an existing ITC Event
 *
 * @note Memory for the new ITC Event will be dynamically allocated.
 * On error, the cloned Event is automatically deallocated.
 * @param pt_Event The existing Event
 * @param ppt_ClonedEvent The pointer to the cloned Event
 * @param pt_ParentEvent The pointer to parent Event. Otherwise NULL
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t cloneEvent(
    const ITC_Event_t *const pt_Event,
    ITC_Event_t **ppt_ClonedEvent,
    ITC_Event_t *const pt_ParentEvent
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    const ITC_Event_t *pt_CurrentEvent = pt_Event; /* The current root */
    const ITC_Event_t *pt_CurrentEventParent; /* pt_CurrentEvent's parent */
    ITC_Event_t *pt_ClonedEventClone; /* The current cloned root */

    if (pt_CurrentEvent == NULL)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        /* Remember the parent of the root as this might be a subree */
        pt_CurrentEventParent = pt_CurrentEvent->pt_Parent;

        /* Allocate the root */
        t_Status = newEvent(
            ppt_ClonedEvent, pt_ParentEvent, pt_CurrentEvent->t_Count);

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            /* Initialise the cloned root pointer */
            pt_ClonedEventClone = *ppt_ClonedEvent;
        }

        while(t_Status == ITC_STATUS_SUCCESS &&
              pt_CurrentEvent != pt_CurrentEventParent)
        {
            if (pt_CurrentEvent->pt_Left && !pt_ClonedEventClone->pt_Left)
            {
                /* Allocate left subtree */
                t_Status = newEvent(
                    &pt_ClonedEventClone->pt_Left,
                    pt_ClonedEventClone,
                    pt_CurrentEvent->pt_Left->t_Count);

                if (t_Status == ITC_STATUS_SUCCESS)
                {
                    /* Descend into the left child */
                    pt_CurrentEvent = pt_CurrentEvent->pt_Left;
                    pt_ClonedEventClone = pt_ClonedEventClone->pt_Left;
                }
            }
            else if (
                pt_CurrentEvent->pt_Right && !pt_ClonedEventClone->pt_Right)
            {
                /* Allocate right subtree */
                t_Status = newEvent(
                    &pt_ClonedEventClone->pt_Right,
                    pt_ClonedEventClone,
                    pt_CurrentEvent->pt_Right->t_Count);

                if (t_Status == ITC_STATUS_SUCCESS)
                {
                    /* Descend into the right child */
                    pt_CurrentEvent = pt_CurrentEvent->pt_Right;
                    pt_ClonedEventClone = pt_ClonedEventClone->pt_Right;
                }
            }
            else
            {
                /* Go up the tree */
                pt_CurrentEvent = pt_CurrentEvent->pt_Parent;
                pt_ClonedEventClone = pt_ClonedEventClone->pt_Parent;
            }
        }
    }

    /* If something goes wrong during the cloning - the Event is invalid and
     * must not be used. */
    if (t_Status != ITC_STATUS_SUCCESS && t_Status != ITC_STATUS_INVALID_PARAM)
    {
        /* There is nothing else to do if the cloning fails. Also it is more
         * important to convey the cloning failed, rather than the destroy */
        (void)ITC_Event_destroy(ppt_ClonedEvent);
    }

    return t_Status;
}

/**
 * @brief Increment an `ITC_EventCounter_t` and detect overflows
 *
 * Essentially this performs a lift operation on a given Event,
 * fulfilling `lift(e)`.
 *
 * The rules for a `lift` operation are:
 *  - lift(n, m) = (n + m)
 *  - lift((n, e1, e2), m) = (n + m, e1, e2)
 *
 * @param pt_Counter The counter to increment
 * @param t_IncCount The amount to increment with
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t incEventCounter(
    ITC_Event_Counter_t *pt_Counter,
    ITC_Event_Counter_t t_IncCount
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    /* Detect overflow */
    if (*pt_Counter + t_IncCount < *pt_Counter)
    {
        t_Status = ITC_STATUS_EVENT_COUNTER_OVERFLOW;
    }
    else
    {
        *pt_Counter += t_IncCount;
    }

    return t_Status;
}

/**
 * @brief Decrement an `ITC_EventCounter_t` and detect underflows
 *
 * Essentially this performs a sink operation on a given Event,
 * fulfilling `sink(e)`.
 *
 * The rules for a `sink` operation are:
 *  - sink(n, m) = (n - m)
 *  - sink((n, e1, e2), m) = (n - m, e1, e2)
 *
 * @param pt_Counter The counter to decrement
 * @param t_DecCount The amount to decrement with
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t decEventCounter(
    ITC_Event_Counter_t *pt_Counter,
    ITC_Event_Counter_t t_DecCount
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    /* Detect underflow */
    if (*pt_Counter - t_DecCount > *pt_Counter)
    {
        t_Status = ITC_STATUS_EVENT_COUNTER_UNDERFLOW;
    }
    else
    {
        *pt_Counter -= t_DecCount;
    }

    return t_Status;
}

/**
 * @brief Given an event `(n, e1, e2)` performs:
 *     `(n, e1, e2) = (lift(n, m), sink(e1, m), sink(e2, m))`,
 *     where `m = min(e1, e2)`.
 *
 * Performs the operation and tries to do damange control (revert to original
 * state) if any of the steps fail.
 *
 * @note It is assumed `e1` and `e2` are normalised Events, such that
 * `min((n, e1, e2)) == n`, i.e one of the subtrees has an event counter
 * equal to 0
 *
 * @param pt_Event The Event on which to perform the operation
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t liftSinkSinkEvent(
    ITC_Event_t *pt_Event
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS;
    ITC_Status_t t_OpStatus = ITC_STATUS_SUCCESS;
    ITC_Event_Counter_t t_Count = 0;

    /* Remember the count */
    t_Count = MIN(pt_Event->pt_Left->t_Count, pt_Event->pt_Right->t_Count);

    /* Sink the children */
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = decEventCounter(&pt_Event->pt_Left->t_Count, t_Count);

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            t_Status = decEventCounter(&pt_Event->pt_Right->t_Count, t_Count);

            if (t_Status != ITC_STATUS_SUCCESS)
            {
                /* Restore the other child */
                t_OpStatus = incEventCounter(
                    &pt_Event->pt_Left->t_Count, t_Count);

                if (t_OpStatus != ITC_STATUS_SUCCESS)
                {
                    /* Return last error */
                    t_Status = t_OpStatus;
                }
            }
        }
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Lift the event counter of the root node */
        t_Status = incEventCounter(&pt_Event->t_Count, t_Count);
    }

    return t_Status;
}

/**
 * @brief Given an event `(n, e1, e2)` performs:
 *     `(n, e1, e2) = lift(n, m)`,
 *     where `m = max(e1, e2)`.
 *
 * Performs the operation and tries to do damange control (revert to original
 * state) if any of the steps fail.
 *
 * @note It is assumed `e1` and `e2` are leaf Events, such that
 * `max((n, e1, e2)) == n + max(e1, e2)`
 *
 * @param pt_Event The Event on which to perform the operation
 * children
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t liftDestroyDestroyEvent(
    ITC_Event_t *pt_Event
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS;
    ITC_Status_t t_OpStatus = ITC_STATUS_SUCCESS;
    ITC_Event_Counter_t t_LeftCount = 0;
    ITC_Event_Counter_t t_MaxCount = 0;

    /* Remember the counts */
    t_LeftCount = pt_Event->pt_Left->t_Count;
    t_MaxCount = MAX(t_LeftCount, pt_Event->pt_Right->t_Count);

    /* Destroy the children */
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = ITC_Event_destroy(&pt_Event->pt_Left);

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            t_Status = ITC_Event_destroy(&pt_Event->pt_Right);

            if (t_Status != ITC_STATUS_SUCCESS)
            {
                /* Restore the other child */
                t_OpStatus = newEvent(
                    &pt_Event->pt_Left, pt_Event, t_LeftCount);

                if (t_OpStatus != ITC_STATUS_SUCCESS)
                {
                    /* Return last error */
                    t_Status = t_OpStatus;
                }
            }
        }
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Lift the event counter of the root node */
        t_Status = incEventCounter(&pt_Event->t_Count, t_MaxCount);
    }

    return t_Status;
}

/**
 * @brief Normalise an Event fulfilling `norm(e)`
 * Rules:
 *  - norm(n) = n
 *  - norm(n, m, m) = lift(n, m)
 *  - norm((n, e1, e2)) = (lift(n, m), sink(e1, m), sink(e2, m)), where:
 *    - m = min(min(e1), min(e2))
 *    - For normalised event trees (one subtree having an event counter == 0):
 *      - min(n) = n
 *      - min((n, e1, e2)) = n
 *
 * @param pt_Event The Event to normalise
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t normEventE(
    ITC_Event_t *pt_Event
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Event_t *pt_CurrentEvent = pt_Event;
    /* Remember the parent as this might be a subtree */
    ITC_Event_t *pt_ParentRootEvent = pt_Event->pt_Parent;

    while (t_Status == ITC_STATUS_SUCCESS &&
           pt_CurrentEvent != pt_ParentRootEvent)
    {
        /* norm((n, e1, e2)) */
        if (!ITC_EVENT_IS_LEAF_EVENT(pt_CurrentEvent))
        {
            /* Normalise e1 */
            if (!ITC_EVENT_IS_NORMALISED_EVENT(pt_CurrentEvent->pt_Left))
            {
                pt_CurrentEvent = pt_CurrentEvent->pt_Left;
            }
            /* Normalise e2 */
            else if (!ITC_EVENT_IS_NORMALISED_EVENT(pt_CurrentEvent->pt_Right))
            {
                pt_CurrentEvent = pt_CurrentEvent->pt_Right;
            }
            /* norm((n, m, m)) = lift(n, m) */
            else if (ITC_EVENT_IS_LEAF_EVENT(pt_CurrentEvent->pt_Left) &&
                     ITC_EVENT_IS_LEAF_EVENT(pt_CurrentEvent->pt_Right) &&
                     (pt_CurrentEvent->pt_Left->t_Count ==
                          pt_CurrentEvent->pt_Right->t_Count))
            {
                /* Lift the root, destroy the children */
                t_Status = liftDestroyDestroyEvent(pt_CurrentEvent);

                if (t_Status == ITC_STATUS_SUCCESS)
                {
                    pt_CurrentEvent = pt_CurrentEvent->pt_Parent;
                }
            }
            /*
             * norm((n, e1, e2)) = (lift(n, m), sink(e1, m), sink(e2, m)),
             *
             * Where:
             *    - e1 and e2 are normalised event trees or leafs
             *    - min(n) = n
             *    - min((n, e1, e2)) = n
             */
            else if (!ITC_EVENT_IS_NORMALISED_EVENT(pt_CurrentEvent))
            {
                /* Lift the root, sink the children */
                t_Status = liftSinkSinkEvent(pt_CurrentEvent);

                if (t_Status == ITC_STATUS_SUCCESS)
                {
                    pt_CurrentEvent = pt_CurrentEvent->pt_Parent;
                }
            }
            /* pt_CurrentEvent event is normalised. Nothing to do */
            else
            {
                pt_CurrentEvent = pt_CurrentEvent->pt_Parent;
            }
        }
        /* norm(n) = n */
        else
        {
            pt_CurrentEvent = pt_CurrentEvent->pt_Parent;
        }
    }

    return t_Status;
}

/**
 * @brief Join two Events into a new Event fulfilling `join(e1, e2)`
 * Rules:
 *  - join(n1, n2) = max(n1, n2)
 *  - join(n1, (n2, l2, r2)) = join((n1, 0, 0), (n2, l2, r2))
 *  - join((n1, l1, r1), n2) = join((n1, l1, r1), (n2, 0, 0))
 *  - join((n1, l1, r1), (n2, l2, r2)):
 *    - If n1 > n2:
 *         join((n2, l2, r2), (n1, l1, r1))
 *    - If n1 <= n2:
 *         norm((n1, join(l1, lift(l2, n2 - n1)), join(r1, lift(r2, n2 - n1))))
 *
 * @param pt_Event1 The first Event
 * @param pt_Event2 The second Event
 * @param ppt_Event The new Event
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t joinEventE(
    const ITC_Event_t *const pt_Event1,
    const ITC_Event_t *const pt_Event2,
    ITC_Event_t **ppt_Event
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    ITC_Event_t *pt_CurrentEvent1 = NULL;
    ITC_Event_t *pt_RootCurrentEvent1 = NULL;
    ITC_Event_t *pt_CurrentEvent2 = NULL;
    ITC_Event_t *pt_RootCurrentEvent2 = NULL;

    ITC_Event_t *pt_SwapEvent = NULL;

    ITC_Event_t **ppt_CurrentEvent = ppt_Event;
    ITC_Event_t *pt_ParentCurrentEvent = NULL;

    if (!pt_Event1 || !pt_Event2  || !ppt_Event)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        /* Init Event */
        *ppt_CurrentEvent = NULL;

        /* Clone the input events, as they will get modified during the
         * joining process */
        t_Status = cloneEvent(pt_Event1, &pt_CurrentEvent1, NULL);

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            /* Save the root so it can be easily deallocated */
            pt_RootCurrentEvent1 = pt_CurrentEvent1;

            t_Status = cloneEvent(pt_Event2, &pt_CurrentEvent2, NULL);

            if (t_Status == ITC_STATUS_SUCCESS)
            {
                pt_RootCurrentEvent2 = pt_CurrentEvent2;
            }
        }
    }

    while (t_Status == ITC_STATUS_SUCCESS &&
           pt_CurrentEvent1 != pt_RootCurrentEvent1->pt_Parent &&
           pt_CurrentEvent2 != pt_RootCurrentEvent2->pt_Parent)
    {
        /* join(n1, n2) = max(n1, n2) */
        if (ITC_EVENT_IS_LEAF_EVENT(pt_CurrentEvent1) &&
            ITC_EVENT_IS_LEAF_EVENT(pt_CurrentEvent2))
        {
            t_Status = newEvent(
                ppt_CurrentEvent,
                pt_ParentCurrentEvent,
                MAX(pt_CurrentEvent1->t_Count, pt_CurrentEvent2->t_Count));

            if (t_Status == ITC_STATUS_SUCCESS)
            {
                /* Climb back to the parent node
                * Use the parent pointer saved on the stack instead of
                * `(*ppt_CurrentEvent)->pt_Parent` as that will be the child
                * element on the next iteration and may get destroyed by
                * `normEventI`
                */
                ppt_CurrentEvent = &pt_ParentCurrentEvent;
                pt_CurrentEvent1 = pt_CurrentEvent1->pt_Parent;
                pt_CurrentEvent2 = pt_CurrentEvent2->pt_Parent;
            }
        }
        /* join((n1, l1, r1), (n2, l2, r2)):
         * - If n1 > n2:
         *      join((n2, l2, r2), (n1, l1, r1))
         * - If n1 <= n2:
         *      norm((n1, join(l1, lift(l2, n2 - n1)), join(r1, lift(r2, n2 - n1))))
         */
        else if(!ITC_EVENT_IS_LEAF_EVENT(pt_CurrentEvent1) &&
                !ITC_EVENT_IS_LEAF_EVENT(pt_CurrentEvent2))
        {
            /* Create the parent node.
             * This might exist from a previous iteration. This is fine. */
            if (!*ppt_CurrentEvent)
            {
                t_Status = newEvent(ppt_CurrentEvent, pt_ParentCurrentEvent, 0);
            }

            if (t_Status == ITC_STATUS_SUCCESS)
            {
                if (!(*ppt_CurrentEvent)->pt_Left ||
                    !(*ppt_CurrentEvent)->pt_Right)
                {
                    /* Save the parent pointer on the stack */
                    pt_ParentCurrentEvent = *ppt_CurrentEvent;

                    /* If n1 > n2: flip them around */
                    if (pt_CurrentEvent1->t_Count > pt_CurrentEvent2->t_Count)
                    {
                        pt_SwapEvent = pt_CurrentEvent1;
                        pt_CurrentEvent1 = pt_CurrentEvent2;
                        pt_CurrentEvent2 = pt_SwapEvent;
                    }
                }

                /* Descend into left child */
                if (!(*ppt_CurrentEvent)->pt_Left)
                {
                    ppt_CurrentEvent = &(*ppt_CurrentEvent)->pt_Left;
                    pt_CurrentEvent1 = pt_CurrentEvent1->pt_Left;
                    pt_CurrentEvent2 = pt_CurrentEvent2->pt_Left;

                    t_Status = incEventCounter(
                        &pt_CurrentEvent2->t_Count,
                        pt_CurrentEvent2->pt_Parent->t_Count -
                            pt_CurrentEvent1->pt_Parent->t_Count);
                }
                /* Descend into right child */
                else if (!(*ppt_CurrentEvent)->pt_Right)
                {
                    ppt_CurrentEvent = &(*ppt_CurrentEvent)->pt_Right;
                    pt_CurrentEvent1 = pt_CurrentEvent1->pt_Right;
                    pt_CurrentEvent2 = pt_CurrentEvent2->pt_Right;

                    t_Status = incEventCounter(
                        &pt_CurrentEvent2->t_Count,
                        pt_CurrentEvent2->pt_Parent->t_Count -
                            pt_CurrentEvent1->pt_Parent->t_Count);
                }
                else
                {
                    /* Copy n1. This is always the smaller of the two */
                    (*ppt_CurrentEvent)->t_Count = pt_CurrentEvent1->t_Count;

                    /* If the events were swapped during the descend (i.e
                     * n1 was bigger than n2), swap them them back before
                     * continuing */
                    if (pt_CurrentEvent1->t_Count < pt_CurrentEvent2->t_Count)
                    {
                        pt_SwapEvent = pt_CurrentEvent1;
                        pt_CurrentEvent1 = pt_CurrentEvent2;
                        pt_CurrentEvent2 = pt_SwapEvent;
                    }

                    /* Normalise Event.
                     * This may destroy all child nodes stored under
                     * *ppt_CurrentEvent
                     */
                    t_Status = normEventE(*ppt_CurrentEvent);

                    if (t_Status == ITC_STATUS_SUCCESS)
                    {
                        /* Save the parent pointer on the stack */
                        pt_ParentCurrentEvent = (*ppt_CurrentEvent)->pt_Parent;

                        /* Climb back to the parent node */
                        ppt_CurrentEvent = &pt_ParentCurrentEvent;
                        pt_CurrentEvent2 = pt_CurrentEvent2->pt_Parent;
                        pt_CurrentEvent1 = pt_CurrentEvent1->pt_Parent;
                    }
                }
            }
        }
        /* join(n1, (n2, l2, r2)) = join((n1, 0, 0), (n2, l2, r2)) */
        else if (ITC_EVENT_IS_LEAF_EVENT(pt_CurrentEvent1))
        {
            t_Status = newEvent(
                &pt_CurrentEvent1->pt_Left, pt_CurrentEvent1, 0);

            if (t_Status == ITC_STATUS_SUCCESS)
            {
                t_Status = newEvent(
                    &pt_CurrentEvent1->pt_Right, pt_CurrentEvent1, 0);
            }
        }
        /* join((n1, l1, r1), n2) = join((n1, l1, r1), (n2, 0, 0)) */
        else if (ITC_EVENT_IS_LEAF_EVENT(pt_CurrentEvent2))
        {
            t_Status = newEvent(
                &pt_CurrentEvent2->pt_Left, pt_CurrentEvent2, 0);

            if (t_Status == ITC_STATUS_SUCCESS)
            {
                t_Status = newEvent(
                    &pt_CurrentEvent2->pt_Right, pt_CurrentEvent2, 0);
            }
        }
        else
        {
            t_Status = ITC_STATUS_CORRUPT_EVENT;
        }
    }

    /* Destroy the copied input events */
    if (pt_RootCurrentEvent1)
    {
        /* There is nothing else to do if the destroy fails. */
        (void)ITC_Event_destroy(&pt_RootCurrentEvent1);
    }

    if (pt_RootCurrentEvent2)
    {
        /* There is nothing else to do if the destroy fails. */
        (void)ITC_Event_destroy(&pt_RootCurrentEvent2);
    }

    /* If something goes wrong during the joining process - the Event is invalid
     * and must not be used. */
    if (t_Status != ITC_STATUS_SUCCESS && t_Status != ITC_STATUS_INVALID_PARAM)
    {
        /* There is nothing else to do if the destroy fails. Also it is more
         * important to convey the join failed, rather than the destroy */
        (void)ITC_Event_destroy(ppt_CurrentEvent);
    }

    return t_Status;
}

/**
 * @brief Check if one Event is `<=` to another, fulfilling `leq(e1, e2)`
 * Rules:
 *  - leq(n1, n2) = n1 <= n2
 *  - leq(n1, (n2, l2, r2)) = n1 <= n2
 *  - leq((n1, l1, r1), n2):
 *       n1 <= n2 && leq(lift(l1, n1), n2) && leq(lift(r1, n1), n2)
 *  - leq((n1, l1, r1), (n2, l2, r2)):
 *       n1 <= n2 && leq(lift(l1, n1), lift(l2, n2)) && leq(lift(r1, n1), lift(r2, n2))
 *
 * @param pt_Event1 The first Event
 * @param pt_Event2 The second Event
 * @param pb_IsLeq (out) `true` if `*pt_Event1 <= *pt_Event2`. Otherwise `false`
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t leqEventE(
    const ITC_Event_t *pt_Event1,
    const ITC_Event_t *pt_Event2,
    bool *pb_IsLeq
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    const ITC_Event_t *pt_ParentCurrentEvent1 = NULL;
    const ITC_Event_t *pt_ParentRootEvent1 = NULL;

    /* Holds the event count from the root to the current parent node */
    ITC_Event_Counter_t t_ParentsCountEvent1 = 0;
    ITC_Event_Counter_t t_ParentsCountEvent2 = 0;

    /* Holds the total current event count
     * (pt_EventX->t_Count + t_ParentsCountEventX) */
    ITC_Event_Counter_t t_CurrentCountEvent1 = 0;
    ITC_Event_Counter_t t_CurrentCountEvent2 = 0;

    /* Keeps track of how many descends have been skipped by pt_Event2 due
     * to its tree branch being shallower than the one in pt_Event1 */
    uint32_t u32_CurrentEvent2DescendSkips = 0;

    if(!pt_Event1 || !pt_Event2)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        /* Init flag */
        *pb_IsLeq = true;

        /* Remember the root parent Event as this might be a subtree */
        pt_ParentRootEvent1 = pt_Event1->pt_Parent;

        pt_ParentCurrentEvent1 = pt_ParentRootEvent1;
    }

    /* Perform a pre-order traversal.
     *
     * For `pt_Event1 <= pt_Event2` all `<=` checks must pass.
     * If a check fails - exit early */
    while (t_Status == ITC_STATUS_SUCCESS && *pb_IsLeq && pt_Event1)
    {
        /* Calculate the total current event count for both Event trees
         *
         * Essentially this is a `lift([lr]X, nX)` operation but
         * doesn't modify the original Event trees */
        t_CurrentCountEvent1 = pt_Event1->t_Count;
        t_Status = incEventCounter(&t_CurrentCountEvent1, t_ParentsCountEvent1);

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            t_CurrentCountEvent2 = pt_Event2->t_Count;
            t_Status = incEventCounter(
                &t_CurrentCountEvent2, t_ParentsCountEvent2);
        }

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            /* n1 <= n2 */
            *pb_IsLeq = t_CurrentCountEvent1 <= t_CurrentCountEvent2;

            if (*pb_IsLeq)
            {
                /* Descend into left tree */
                if (pt_Event1->pt_Left)
                {
                    /* Increment the parent height */
                    t_Status = incEventCounter(
                        &t_ParentsCountEvent1, pt_Event1->t_Count);

                    if (t_Status == ITC_STATUS_SUCCESS)
                    {
                        pt_Event1 = pt_Event1->pt_Left;

                        /* If pt_Event2 has a left node - descend down */
                        if (pt_Event2->pt_Left)
                        {
                            /* Increment the parent height */
                            t_Status = incEventCounter(
                                &t_ParentsCountEvent2,
                                pt_Event2->t_Count);

                            if (t_Status == ITC_STATUS_SUCCESS)
                            {
                                pt_Event2 = pt_Event2->pt_Left;
                            }
                        }
                        /* Otherwise, keep track of how many times the descend
                        * was skipped due to a shallow tree */
                        else
                        {
                            u32_CurrentEvent2DescendSkips++;
                        }
                    }
                }
                /* Valid parent ITC Event trees always have both left and right
                * nodes. Thus, there is no need to check if the current node
                * doesn't have a left child but has a right one.
                *
                * Instead directly start backtracking up the tree */
                else
                {
                    /* Get the current parent node */
                    pt_ParentCurrentEvent1 = pt_Event1->pt_Parent;

                    /* Loop until the current node is no longer its
                     * parent's right child node */
                    while (t_Status == ITC_STATUS_SUCCESS &&
                           pt_ParentCurrentEvent1 != pt_ParentRootEvent1 &&
                           pt_ParentCurrentEvent1->pt_Right == pt_Event1)
                    {
                        pt_Event1 = pt_Event1->pt_Parent;
                        pt_ParentCurrentEvent1 =
                            pt_ParentCurrentEvent1->pt_Parent;

                        /* Decrement the parent height */
                        t_Status = decEventCounter(
                            &t_ParentsCountEvent1, pt_Event1->t_Count);

                        if (t_Status == ITC_STATUS_SUCCESS)
                        {
                            /* Decrement the amount of skipped descends */
                            if (u32_CurrentEvent2DescendSkips)
                            {
                                u32_CurrentEvent2DescendSkips--;
                            }
                            /* If no descends were skipped - start backtracking
                            * up the pt_Event2 tree.
                            * There is no need to check if the current pt_Event2
                            * is it's parent's right child, because its descend
                            * is tied to the descends of pt_Event1 */
                            else
                            {
                                pt_Event2 = pt_Event2->pt_Parent;

                                /* Decrement the parent height */
                                t_Status = decEventCounter(
                                    &t_ParentsCountEvent2,
                                    pt_Event2->t_Count);
                            }
                        }
                    }

                    /* There is a right subtree that has not been explored
                    * yet */
                    if (pt_ParentCurrentEvent1 != pt_ParentRootEvent1)
                    {
                        /* Jump from the left node of the current parent to
                         * right one */
                        pt_Event1 = pt_ParentCurrentEvent1->pt_Right;

                        /* Do the same for pt_Event2 if it hasn't skipped
                         * any descends due to its tree being shallow */
                        if(!u32_CurrentEvent2DescendSkips)
                        {
                            pt_Event2 = pt_Event2->pt_Parent->pt_Right;
                        }
                    }
                    /* The tree has been fully explored. Exit loop */
                    else
                    {
                        pt_Event1 = NULL;
                    }
                }
            }
        }
    }

    return t_Status;
}

/**
 * @brief Maximise an Event fulfilling `max(e)`
 * Rules:
 *  - max(n) = n
 *  - max(n, e1, e2) = n + max(max(e1), max(e2))
 *
 * The resulting Event is always a leaf Event
 *
 * @param pt_Event The Event to maximise
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t maxEventE(
    ITC_Event_t *pt_Event
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS;
    ITC_Event_t *pt_ParentRootEvent;

    if (!pt_Event)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        /* Remember the parent as this might be a subtree */
        pt_ParentRootEvent = pt_Event->pt_Parent;
    }

    while (t_Status == ITC_STATUS_SUCCESS && pt_Event != pt_ParentRootEvent)
    {
        if (ITC_EVENT_IS_LEAF_EVENT(pt_Event))
        {
            /* The Event is maximised. Nothing to do. */
            pt_Event = pt_Event->pt_Parent;
        }
        else if (!ITC_EVENT_IS_LEAF_EVENT(pt_Event->pt_Left))
        {
            /* Explore left subtree */
            pt_Event = pt_Event->pt_Left;
        }
        else if (!ITC_EVENT_IS_LEAF_EVENT(pt_Event->pt_Right))
        {
            /* Explore right subtree */
            pt_Event = pt_Event->pt_Right;
        }
        /* Both Event subtrees are leafs */
        else
        {
            /* Maximise the Event count */
            t_Status = liftDestroyDestroyEvent(pt_Event);
        }
    }

    return t_Status;
}

/**
 * @brief Fill an Event, fulfilling `fill(i, e)`
 * Rules:
 *  - fill(0, e) = e
 *  - fill(1, e) = max(e)
 *  - fill(i, n) = n
 *  - fill((1, ir), (n, el, er)):
 *        norm((n, max(max(el), min(er')), er')), where er' = fill(ir, er)
 *  - fill((il, 1), (n, el, er)):
 *        norm((n, el', max(max(er), min(el')))), where el' = fill(il, el)
 *  - fill((il, ir), (n, el, er)):
 *        norm((n, fill(il, el), fill(ir, er)))
 *
 * @param pt_Event The Event to fill
 * @param pt_Id The ID showing the ownership information for the interval
 * @param pb_WasFilled (out) Whether the event was filled or not. In some cases
 * filling an Event (simplifying + inflating) is not possible
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t fillEventE(
    ITC_Event_t *pt_Event,
    const ITC_Id_t *pt_Id,
    bool *pb_WasFilled
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Event_t *pt_ParentRootEvent = pt_Event->pt_Parent;
    ITC_Id_t *pt_ParentRootId = pt_Id->pt_Parent;

    /* The previously iterated ID subtree.
     * Used to keep track of which nodes have been explored */
    const ITC_Id_t *pt_PrevId = NULL;

    /* Init the flag */
    *pb_WasFilled = false;

    while (t_Status == ITC_STATUS_SUCCESS &&
           pt_Event != pt_ParentRootEvent &&
           pt_Id != pt_ParentRootId)
    {
        /* fill(0, e) = e || fill(i, n) = n */
        if(ITC_ID_IS_NULL_ID(pt_Id) ||
           ITC_EVENT_IS_LEAF_EVENT(pt_Event))
        {
            pt_PrevId = pt_Id;

            /* Nothing to inflate. Go up the tree */
            pt_Id = pt_Id->pt_Parent;
            pt_Event = pt_Event->pt_Parent;
        }
        /* fill(1, e) = max(e) */
        else if (ITC_ID_IS_SEED_ID(pt_Id))
        {
            pt_PrevId = pt_Id;

            /* Maximise Event (turn it into a leaf) */
            t_Status = maxEventE(pt_Event);

            *pb_WasFilled = true;

            pt_Id = pt_Id->pt_Parent;
            pt_Event = pt_Event->pt_Parent;
        }
        /* fill((1, ir), (n, el, er)):
         *     norm((n, max(max(el), min(er')), er')), where er' = fill(ir, er)
         */
        else if (ITC_ID_IS_SEED_ID(pt_Id->pt_Left))
        {
            /* er' = fill(ir, er) */
            if (pt_PrevId != pt_Id->pt_Right)
            {
                pt_PrevId = pt_Id;

                pt_Id = pt_Id->pt_Right;
                pt_Event = pt_Event->pt_Right;
            }
            /* norm((n, max(max(el), min(er')), er')) */
            else
            {
                /* el = max(el) */
                if (!ITC_EVENT_IS_LEAF_EVENT(pt_Event->pt_Left))
                {
                    /* Turn el into a leaf Event */
                    t_Status = maxEventE(pt_Event->pt_Left);

                    if (t_Status == ITC_STATUS_SUCCESS)
                    {
                        *pb_WasFilled = true;
                    }
                }

                /* el = max(el, min(er')) */
                if (t_Status == ITC_STATUS_SUCCESS &&
                    (pt_Event->pt_Left->t_Count <
                     pt_Event->pt_Right->t_Count))
                {
                    /* For a normalised Event: min((n, el, er)) = n */
                    pt_Event->pt_Left->t_Count = pt_Event->pt_Right->t_Count;

                    *pb_WasFilled = true;
                }

                /* norm((n, el, er')) */
                if (t_Status == ITC_STATUS_SUCCESS)
                {
                    t_Status = normEventE(pt_Event);

                    if (t_Status == ITC_STATUS_SUCCESS)
                    {
                        pt_PrevId = pt_Id;

                        pt_Id = pt_Id->pt_Parent;
                        pt_Event = pt_Event->pt_Parent;
                    }
                }
            }
        }
        /* fill((il, 1), (n, el, er)):
         *     norm((n, el', max(max(er), min(el')))), where el' = fill(il, el)
         */
        else if (ITC_ID_IS_SEED_ID(pt_Id->pt_Right))
        {
            /* el' = fill(il, el) */
            if (pt_PrevId != pt_Id->pt_Left)
            {
                pt_PrevId = pt_Id;

                pt_Id = pt_Id->pt_Left;
                pt_Event = pt_Event->pt_Left;
            }
            /* norm((n, el', max(max(er), min(el')))) */
            else
            {
                /* er = max(er) */
                if (!ITC_EVENT_IS_LEAF_EVENT(pt_Event->pt_Right))
                {
                    /* Turn er into a leaf Event */
                    t_Status = maxEventE(pt_Event->pt_Right);

                    if (t_Status == ITC_STATUS_SUCCESS)
                    {
                        *pb_WasFilled = true;
                    }
                }

                /* er = max(er, min(el')) */
                if (t_Status == ITC_STATUS_SUCCESS &&
                    (pt_Event->pt_Right->t_Count <
                     pt_Event->pt_Left->t_Count))
                {
                    /* For a normalised Event: min((n, el, er)) = n */
                    pt_Event->pt_Right->t_Count = pt_Event->pt_Left->t_Count;

                    *pb_WasFilled = true;
                }

                /* norm((n, el', er)) */
                if (t_Status == ITC_STATUS_SUCCESS)
                {
                    t_Status = normEventE(pt_Event);

                    if (t_Status == ITC_STATUS_SUCCESS)
                    {
                        pt_PrevId = pt_Id;

                        pt_Id = pt_Id->pt_Parent;
                        pt_Event = pt_Event->pt_Parent;
                    }
                }
            }
        }
        /* fill((il, ir), (n, el, er)):
         *     norm((n, fill(il, el), fill(ir, er)))
         */
        else
        {
            /* fill(il, el) */
            if (pt_PrevId != pt_Id->pt_Left && pt_PrevId != pt_Id->pt_Right)
            {
                pt_PrevId = pt_Id;

                pt_Id = pt_Id->pt_Left;
                pt_Event = pt_Event->pt_Left;
            }
            /* fill(ir, er) */
            else if (pt_PrevId != pt_Id->pt_Right)
            {
                pt_PrevId = pt_Id;

                pt_Id = pt_Id->pt_Right;
                pt_Event = pt_Event->pt_Right;
            }
            /* norm((n, el, er) */
            else
            {
                t_Status = normEventE(pt_Event);

                if (t_Status == ITC_STATUS_SUCCESS)
                {
                    pt_PrevId = pt_Id;

                    pt_Id = pt_Id->pt_Parent;
                    pt_Event = pt_Event->pt_Parent;
                }
            }
        }
    }

    return t_Status;
}

/******************************************************************************
 * Public functions
 ******************************************************************************/

/******************************************************************************
 * Allocate a new ITC Event and initialise it
 ******************************************************************************/

ITC_Status_t ITC_Event_new(
    ITC_Event_t **ppt_Event
)
{
    return newEvent(ppt_Event, NULL, 0);
}

/******************************************************************************
 * Free an allocated ITC Event
 ******************************************************************************/

ITC_Status_t ITC_Event_destroy(
    ITC_Event_t **ppt_Event
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Status_t t_FreeStatus = ITC_STATUS_SUCCESS; /* The last free status */
    ITC_Event_t *pt_CurrentEvent = NULL; /* The current element */
    ITC_Event_t *pt_ParentCurrentEvent = NULL;
    ITC_Event_t *pt_RootParent = NULL;

    if (!ppt_Event || !(*ppt_Event))
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        pt_CurrentEvent = *ppt_Event;
        pt_RootParent = pt_CurrentEvent->pt_Parent;

        /* Keep trying to free elements even if some frees fail */
        while(pt_CurrentEvent && pt_CurrentEvent != pt_RootParent)
        {
            /* Advance into left subtree */
            if(pt_CurrentEvent->pt_Left)
            {
                pt_CurrentEvent = pt_CurrentEvent->pt_Left;
            }
            /* Advance into right subtree */
            else if(pt_CurrentEvent->pt_Right)
            {
                pt_CurrentEvent = pt_CurrentEvent->pt_Right;
            }
            else
            {
                /* Remember the parent element */
                pt_ParentCurrentEvent = pt_CurrentEvent->pt_Parent;

                if(pt_ParentCurrentEvent)
                {
                    /* Remove the current element address from the parent */
                    if(pt_ParentCurrentEvent->pt_Left == pt_CurrentEvent)
                    {
                        pt_ParentCurrentEvent->pt_Left = NULL;
                    }
                    else
                    {
                        pt_ParentCurrentEvent->pt_Right = NULL;
                    }
                }

                /* Free the current element */
                t_FreeStatus = ITC_Port_free(pt_CurrentEvent);

                if (t_Status == ITC_STATUS_SUCCESS)
                {
                    t_Status = t_FreeStatus;
                }

                /* Go up the tree */
                pt_CurrentEvent = pt_ParentCurrentEvent;
            }
        }
    }

    /* Sanitize the freed pointer */
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        *ppt_Event = NULL;
    }

    return t_Status;
}

/******************************************************************************
 * Clone an existing ITC Event
 ******************************************************************************/

ITC_Status_t ITC_Event_clone(
    const ITC_Event_t *const pt_Event,
    ITC_Event_t **ppt_ClonedEvent
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    t_Status = validateEvent(pt_Event);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = cloneEvent(pt_Event, ppt_ClonedEvent, NULL);
    }

    return t_Status;
}

/******************************************************************************
 * Normalise an Event
 ******************************************************************************/

ITC_Status_t ITC_Event_normalise(
    ITC_Event_t *pt_Event
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    t_Status = validateEvent(pt_Event);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = normEventE(pt_Event);
    }

    return t_Status;
}

/******************************************************************************
 * Maximise an Event
 ******************************************************************************/

ITC_Status_t ITC_Event_maximise(
    ITC_Event_t *pt_Event
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    t_Status = validateEvent(pt_Event);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = maxEventE(pt_Event);
    }

    return t_Status;
}

/******************************************************************************
 * Join two existing Events into a single Event
 ******************************************************************************/

ITC_Status_t ITC_Event_join(
    const ITC_Event_t *const pt_Event1,
    const ITC_Event_t *const pt_Event2,
    ITC_Event_t **ppt_Event
)
{
    ITC_Status_t t_Status; /* The current status */

    t_Status = validateEvent(pt_Event1);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateEvent(pt_Event2);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = joinEventE(pt_Event1, pt_Event2, ppt_Event);
    }

    return t_Status;
}

/******************************************************************************
 * Check if an Event is `less than or equal` (`<=`) to another Event
 ******************************************************************************/

ITC_Status_t ITC_Event_leq(
    const ITC_Event_t *const pt_Event1,
    const ITC_Event_t *const pt_Event2,
    bool *pb_IsLeq

)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    if (!pb_IsLeq)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateEvent(pt_Event1);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateEvent(pt_Event2);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Check if `pt_Event1 <= pt_Event2` */
        t_Status = leqEventE(pt_Event1, pt_Event2, pb_IsLeq);
    }

    return t_Status;
}

/******************************************************************************
 * Fill an Event
 ******************************************************************************/

ITC_Status_t ITC_Event_fill(
    ITC_Event_t *pt_Event,
    const ITC_Id_t *const pt_Id,
    bool *pb_WasFilled
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    if (!pb_WasFilled)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateEvent(pt_Event);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* TODO: Validate the ID */
        if (!pt_Id)
        {
            t_Status = ITC_STATUS_INVALID_PARAM;
        }
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = fillEventE(pt_Event, pt_Id, pb_WasFilled);
    }

    return t_Status;
}
