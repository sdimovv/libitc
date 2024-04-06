/**
 * @file ITC_Event_Test_private.h
 * @author Stan
 * @brief Private testing definitions for Interval Tree Clock's Event mechanism
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ITC_EVENT_TEST_PRIVATE_H_
#define ITC_EVENT_TEST_PRIVATE_H_

#include "unity.h"
#include "ITC_Status.h"

/******************************************************************************
 * Defines
 ******************************************************************************/

/** Checks whether the given `ITC_Event_t` is a leaf Event */
#define ITC_EVENT_IS_LEAF_EVENT(pt_Event)                                     \
    ((pt_Event) && !(pt_Event)->pt_Left && !(pt_Event)->pt_Right)

/** Checks whether the given `ITC_Event_t` is a valid parent node
 * The ID must:
 *  - Have 2 child node addresses != NULL
 *  - Have 2 unique child node addresses
 */
#define ITC_EVENT_IS_VALID_PARENT(pt_Event)                                   \
    ((pt_Event->pt_Left && pt_Event->pt_Right) &&                             \
     (pt_Event->pt_Left != pt_Event->pt_Right))                               \

/** Checks whether the given `ITC_Event_t` is a (0) Event */
#define ITC_EVENT_IS_LEAF_0_EVENT(pt_Event)                                   \
    (ITC_EVENT_IS_LEAF_EVENT(pt_Event) && !(pt_Event)->u32_Count)

/** Test an Event is a (0) Event */
#define TEST_EVENT_IS_LEAF_0_EVENT(pt_Event)                                  \
    TEST_ASSERT_TRUE(ITC_EVENT_IS_LEAF_0_EVENT(pt_Event))

/** Test an Event is not a leaf Event */
#define TEST_EVENT_IS_NOT_LEAF_EVENT(pt_Event)                                \
    TEST_ASSERT_TRUE(ITC_EVENT_IS_VALID_PARENT(pt_Event))

/** Test the Event is a leaf node and has a specific event count */
#define TEST_EVENT_IS_N_EVENT(pt_Event, u32_Count_)                           \
    TEST_ASSERT_TRUE(                                                         \
        (ITC_EVENT_IS_LEAF_EVENT(pt_Event) &&                                 \
        (pt_Event)->u32_Count == (u32_Count_)))

/** Test the Event is a parent node and has a specific event count */
#define TEST_EVENT_IS_PARENT_N_EVENT(pt_Event, u32_Count_)                    \
    TEST_ASSERT_TRUE(                                                         \
        (ITC_EVENT_IS_VALID_PARENT(pt_Event) &&                               \
        (pt_Event)->u32_Count == (u32_Count_)))

#endif /* ITC_EVENT_TEST_PRIVATE_H_ */