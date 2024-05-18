/**
 * @file ITC_Stamp_Test.h
 * @brief Unit tests for the Interval Tree Clock's Stamp mechanism
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#include "ITC_Stamp.h"
#include "ITC_Stamp_Test.h"

#include "ITC_Event_package.h"
#include "ITC_Id_package.h"
#include "ITC_Port.h"

#include "ITC_Test_package.h"
#include "ITC_TestUtil.h"
#include "ITC_config.h"

/******************************************************************************
 *  Public functions
 ******************************************************************************/

/* Init test */
void setUp(void) {}

/* Fini test */
void tearDown(void) {}

/* Test destroying a Stamp fails with invalid param */
void ITC_Stamp_Test_destroyStampFailInvalidParam(void)
{
    ITC_Stamp_t *pt_Dummy = NULL;

    TEST_FAILURE(ITC_Stamp_destroy(NULL), ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(ITC_Stamp_destroy(&pt_Dummy), ITC_STATUS_INVALID_PARAM);
}

/* Test creating a Stamp fails with invalid param */
void ITC_Stamp_Test_createStampFailInvalidParam(void)
{
    TEST_FAILURE(ITC_Stamp_newSeed(NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test creating a Stamp succeeds */
void ITC_Stamp_Test_createStampSuccessful(void)
{
    ITC_Stamp_t *pt_Stamp;

    /* Create a new Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Test this is a Stamp with Seed ID node with leaf Event with 0 events */
    TEST_ITC_ID_IS_SEED_ID(pt_Stamp->pt_Id);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp->pt_Event, 0);

    /* Destroy the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
}

/* Test creating a peek Stamp fails with invalid param */
void ITC_Stamp_Test_createPeekStampFailInvalidParam(void)
{
    ITC_Stamp_t *pt_DummyStamp = NULL;

    TEST_FAILURE(
        ITC_Stamp_newPeek(NULL, &pt_DummyStamp), ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_newPeek(pt_DummyStamp, NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test creating a peek Stamp fails with corrupt stamp */
void ITC_Stamp_Test_createPeekStampFailWithCorruptStamp(void)
{
    ITC_Stamp_t *pt_Stamp;
    ITC_Stamp_t *pt_PeekStamp;

    /* Test different invalid Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidStampTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_newPeek(pt_Stamp, &pt_PeekStamp),
            ITC_STATUS_CORRUPT_STAMP);

        /* Destroy the Stamp */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp);
    }
}

/* Test creating a peek Stamp fails with corrupt Id or Event */
void ITC_Stamp_Test_createPeekStampFailWithCorruptIdOrEvent(void)
{
    ITC_Stamp_t *pt_Stamp;
    ITC_Stamp_t *pt_PeekStamp;

    /* Create a new stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Deallocate the valid ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Stamp->pt_Id));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Stamp->pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_newPeek(pt_Stamp, &pt_PeekStamp),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Stamp->pt_Id);
    }

    /* Allocate a valid ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Stamp->pt_Id, NULL));

    /* Deallocate the valid Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Stamp->pt_Event));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Stamp->pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_newPeek(pt_Stamp, &pt_PeekStamp),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Stamp->pt_Event);
    }

    /* Deallocate the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
}

/* Test creating a peek Stamp succeeds */
void ITC_Stamp_Test_createPeekStampSuccessful(void)
{
    ITC_Stamp_t *pt_OriginalStamp;
    ITC_Stamp_t *pt_PeekStamp;

    /* Create a new Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_OriginalStamp));

    /* Create the peek Stamp */
    TEST_SUCCESS(ITC_Stamp_newPeek(pt_OriginalStamp, &pt_PeekStamp));

    /* Destroy the original stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_OriginalStamp));

    /* Test this is a Stamp with NULL ID node with leaf Event with 0 events */
    TEST_ITC_ID_IS_NULL_ID(pt_PeekStamp->pt_Id);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_PeekStamp->pt_Event, 0);

    /* Destroy the peek Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_PeekStamp));
}

/* Test cloning an Stamp fails with invalid param */
void ITC_Stamp_Test_cloneStampFailInvalidParam(void)
{
  ITC_Stamp_t *pt_DummyStamp = NULL;

  TEST_FAILURE(ITC_Stamp_clone(NULL, &pt_DummyStamp), ITC_STATUS_INVALID_PARAM);
  TEST_FAILURE(ITC_Stamp_clone(pt_DummyStamp, NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test cloning an Stamp fails with corrupt stamp */
void ITC_Stamp_Test_cloneStampFailWithCorruptStamp(void)
{
    ITC_Stamp_t *pt_Stamp;
    ITC_Stamp_t *pt_ClonedStamp;

    /* Test different invalid Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidStampTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_clone(pt_Stamp, &pt_ClonedStamp),
            ITC_STATUS_CORRUPT_STAMP);

        /* Destroy the Stamp */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp);
    }
}

/* Test cloning a Stamp fails with corrupt Id or Event */
void ITC_Stamp_Test_cloneStampFailWithCorruptIdOrEvent(void)
{
    ITC_Stamp_t *pt_Stamp;
    ITC_Stamp_t *pt_ClonedStamp;

    /* Create a new stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Deallocate the valid ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Stamp->pt_Id));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Stamp->pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_clone(pt_Stamp, &pt_ClonedStamp),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Stamp->pt_Id);
    }

    /* Allocate a valid ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Stamp->pt_Id, NULL));

    /* Deallocate the valid Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Stamp->pt_Event));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Stamp->pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_clone(pt_Stamp, &pt_ClonedStamp),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Stamp->pt_Event);
    }

    /* Deallocate the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
}

/* Test cloning a Stamp succeeds */
void ITC_Stamp_Test_cloneStampSuccessful(void)
{
    ITC_Stamp_t *pt_OriginalStamp = NULL;
    ITC_Stamp_t *pt_ClonedStamp = NULL;

    /* Test cloning an Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_OriginalStamp));
    TEST_SUCCESS(ITC_Stamp_clone(pt_OriginalStamp, &pt_ClonedStamp));
    TEST_ASSERT_TRUE(pt_OriginalStamp != pt_ClonedStamp);
    TEST_ASSERT_TRUE(pt_OriginalStamp->pt_Id != pt_ClonedStamp->pt_Id);
    TEST_ASSERT_TRUE(pt_OriginalStamp->pt_Event != pt_ClonedStamp->pt_Event);
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_OriginalStamp));

    /* Test the cloned Stamp has a Seed ID node with leaf Event with 0 events */
    TEST_ITC_ID_IS_SEED_ID(pt_ClonedStamp->pt_Id);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_ClonedStamp->pt_Event, 0);
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_ClonedStamp));
}

/* Test forking a Stamp fails with invalid param */
void ITC_Stamp_Test_forkStampFailInvalidParam(void)
{
    ITC_Stamp_t *pt_DummyStamp = NULL;

    TEST_FAILURE(
        ITC_Stamp_fork(pt_DummyStamp, NULL, NULL), ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_fork(NULL, &pt_DummyStamp, NULL), ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_fork(NULL, NULL, &pt_DummyStamp), ITC_STATUS_INVALID_PARAM);
}

/* Test forking a Stamp fails with corrupt stamp */
void ITC_Stamp_Test_forkStampFailWithCorruptStamp(void)
{
    ITC_Stamp_t *pt_Stamp;
    ITC_Stamp_t *pt_ForkedStamp1;
    ITC_Stamp_t *pt_ForkedStamp2;

    /* Test different invalid Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidStampTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_fork(pt_Stamp, &pt_ForkedStamp1, &pt_ForkedStamp2),
            ITC_STATUS_CORRUPT_STAMP);

        /* Destroy the Stamp */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp);
    }
}

/* Test forking a Stamp fails with corrupt Id or Event */
void ITC_Stamp_Test_forkStampFailWithCorruptIdOrEvent(void)
{
    ITC_Stamp_t *pt_Stamp;
    ITC_Stamp_t *pt_ForkedStamp1;
    ITC_Stamp_t *pt_ForkedStamp2;

    /* Create a new stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Deallocate the valid ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Stamp->pt_Id));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Stamp->pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_fork(pt_Stamp, &pt_ForkedStamp1, &pt_ForkedStamp2),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Stamp->pt_Id);
    }

    /* Allocate a valid ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Stamp->pt_Id, NULL));

    /* Deallocate the valid Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Stamp->pt_Event));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Stamp->pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_fork(pt_Stamp, &pt_ForkedStamp1, &pt_ForkedStamp2),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Stamp->pt_Event);
    }

    /* Deallocate the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
}

/* Test forking a Stamp succeeds */
void ITC_Stamp_Test_forkStampSuccessful(void)
{
    ITC_Stamp_t *pt_OriginalStamp;
    ITC_Stamp_t *pt_ForkedStamp1;
    ITC_Stamp_t *pt_ForkedStamp2;

    /* Create a new Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_OriginalStamp));

    /* Fork the Stamp */
    TEST_SUCCESS(
        ITC_Stamp_fork(pt_OriginalStamp, &pt_ForkedStamp1, &pt_ForkedStamp2));

    /* Destroy the original Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_OriginalStamp));

    /* Test the ID was cloned and split and the Event history was cloned */
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_ForkedStamp1->pt_Id);
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_ForkedStamp2->pt_Id);
    TEST_ASSERT_TRUE(pt_ForkedStamp1->pt_Event != pt_ForkedStamp2->pt_Event);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_ForkedStamp1->pt_Event, 0);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_ForkedStamp2->pt_Event, 0);

    /* Destroy the forked Stamps */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_ForkedStamp1));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_ForkedStamp2));
}

/* Test joining two Stamps fails with invalid param */
void ITC_Stamp_Test_joinStampsFailInvalidParam(void)
{
    ITC_Stamp_t *pt_DummyStamp = NULL;

    TEST_FAILURE(
        ITC_Stamp_join(pt_DummyStamp, NULL, NULL), ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_join(NULL, pt_DummyStamp, NULL), ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_join(NULL, NULL, &pt_DummyStamp), ITC_STATUS_INVALID_PARAM);
}

/* Test joining two Stamps fails with corrupt stamp */
void ITC_Stamp_Test_joinStampsFailWithCorruptStamp(void)
{
    ITC_Stamp_t *pt_Stamp1;
    ITC_Stamp_t *pt_Stamp2;
    ITC_Stamp_t *pt_JoinedStamp;

    /* Construct the other Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp2));

    /* Test different invalid Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidStampTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp1);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_join(pt_Stamp1, pt_Stamp2, &pt_JoinedStamp),
            ITC_STATUS_CORRUPT_STAMP);
        /* And the other way around */
        TEST_FAILURE(
            ITC_Stamp_join(pt_Stamp2, pt_Stamp1, &pt_JoinedStamp),
            ITC_STATUS_CORRUPT_STAMP);

        /* Destroy the Stamp */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp1);
    }

    /* Destroy the other Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp2));
}

/* Test joining a Stamp fails with corrupt Id or Event */
void ITC_Stamp_Test_joinStampFailWithCorruptIdOrEvent(void)
{
    ITC_Stamp_t *pt_Stamp1;
    ITC_Stamp_t *pt_Stamp2;
    ITC_Stamp_t *pt_JoinedStamp;

    /* Create new Stamps */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp1));
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp2));

    /* Deallocate the valid ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Stamp1->pt_Id));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Stamp1->pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_join(pt_Stamp1, pt_Stamp2, &pt_JoinedStamp),
            ITC_STATUS_CORRUPT_ID);
        /* Test the other way around */
        TEST_FAILURE(
            ITC_Stamp_join(pt_Stamp2, pt_Stamp1, &pt_JoinedStamp),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Stamp1->pt_Id);
    }

    /* Allocate a valid ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Stamp1->pt_Id, NULL));

    /* Deallocate the valid Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Stamp1->pt_Event));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Stamp1->pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_join(pt_Stamp1, pt_Stamp2, &pt_JoinedStamp),
            ITC_STATUS_CORRUPT_EVENT);
        /* Test the other way around */
        TEST_FAILURE(
            ITC_Stamp_join(pt_Stamp2, pt_Stamp1, &pt_JoinedStamp),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Stamp1->pt_Event);
    }

    /* Deallocate the Stamps */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp1));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp2));
}

/* Test joining two Stamps succeeds */
void ITC_Stamp_Test_joinStampsSuccessful(void)
{
    ITC_Stamp_t *pt_Stamp;
    ITC_Stamp_t *pt_ForkedStamp1;
    ITC_Stamp_t *pt_ForkedStamp2;

    /* Create a new Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Fork the Stamp */
    TEST_SUCCESS(
        ITC_Stamp_fork(pt_Stamp, &pt_ForkedStamp1, &pt_ForkedStamp2));

    /* Destroy the original Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));

    /* Join the Stamps */
    TEST_SUCCESS(
        ITC_Stamp_join(pt_ForkedStamp1, pt_ForkedStamp2, &pt_Stamp));

    /* Destroy the forked Stamps */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_ForkedStamp1));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_ForkedStamp2));

    /* Test the ID is a seed ID and the Event history is a leaf with 0 events */
    TEST_ITC_ID_IS_SEED_ID(pt_Stamp->pt_Id);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp->pt_Event, 0);

    /* Destroy the original Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
}

/* Test inflating the Event of as Stamp fails with invalid param */
void ITC_Stamp_Test_eventStampFailInvalidParam(void)
{
    TEST_FAILURE(ITC_Stamp_event(NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test inflating the Event of a Stamp fails with corrupt stamp */
void ITC_Stamp_Test_eventStampFailWithCorruptStamp(void)
{
    ITC_Stamp_t *pt_Stamp;

    /* Test different invalid Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidStampTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp);

        /* Test for the failure */
        TEST_FAILURE(ITC_Stamp_event(pt_Stamp), ITC_STATUS_CORRUPT_STAMP);

        /* Destroy the Stamp */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp);
    }
}

/* Test inflating the Event of a Stamp fails with corrupt Id or Event */
void ITC_Stamp_Test_eventStampFailWithCorruptIdOrEvent(void)
{
    ITC_Stamp_t *pt_Stamp;

    /* Create a new stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Deallocate the valid ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Stamp->pt_Id));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Stamp->pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_event(pt_Stamp),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Stamp->pt_Id);
    }

    /* Allocate a valid ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Stamp->pt_Id, NULL));

    /* Deallocate the valid Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Stamp->pt_Event));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Stamp->pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_event(pt_Stamp),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Stamp->pt_Event);
    }

    /* Deallocate the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
}

/* Test inflating the Event of a Stamp succeeds */
void ITC_Stamp_Test_eventStampSuccessful(void)
{
    ITC_Stamp_t *pt_Stamp;
    ITC_Stamp_t *pt_PeekStamp;
    ITC_Stamp_t *pt_OriginalStamp;
    ITC_Stamp_Comparison_t t_Result;

    /* Create a new Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Retain a copy for comparison */
    TEST_SUCCESS(ITC_Stamp_clone(pt_Stamp, &pt_OriginalStamp));

    /* Inflate the Stamp Event tree by growing it */
    TEST_SUCCESS(ITC_Stamp_event(pt_Stamp));

    /* Test the Event counter has grown */
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp->pt_Event, 1);
    TEST_SUCCESS(ITC_Stamp_compare(pt_Stamp, pt_OriginalStamp, &t_Result));
    TEST_ASSERT_TRUE(t_Result == ITC_STAMP_COMPARISON_GREATER_THAN);

    /* Create a new peek Stamp */
    TEST_SUCCESS(ITC_Stamp_newPeek(pt_Stamp, &pt_PeekStamp));

    /* Attempt to inflate the peek Stamp */
    TEST_SUCCESS(ITC_Stamp_event(pt_PeekStamp));

    /* Test the Event counter has not changed */
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_PeekStamp->pt_Event, 1);
    TEST_SUCCESS(ITC_Stamp_compare(pt_PeekStamp, pt_Stamp, &t_Result));
    TEST_ASSERT_TRUE(t_Result == ITC_STAMP_COMPARISON_EQUAL);

    /* Destroy the Stamps */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_PeekStamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_OriginalStamp));

    /* Create a new Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Retain a copy for comparison */
    TEST_SUCCESS(ITC_Stamp_clone(pt_Stamp, &pt_OriginalStamp));

    /* Add children to the Event tree */
    TEST_SUCCESS(
        ITC_TestUtil_newEvent(
            &pt_Stamp->pt_Event->pt_Left, pt_Stamp->pt_Event, 0));
    TEST_SUCCESS(
        ITC_TestUtil_newEvent(
            &pt_Stamp->pt_Event->pt_Right, pt_Stamp->pt_Event, 3));

    /* Inflate the Stamp Event tree this time by filling it */
    TEST_SUCCESS(ITC_Stamp_event(pt_Stamp));

    /* Test the Event counter has been filled */
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp->pt_Event, 3);
    TEST_SUCCESS(ITC_Stamp_compare(pt_Stamp, pt_OriginalStamp, &t_Result));
    TEST_ASSERT_TRUE(t_Result == ITC_STAMP_COMPARISON_GREATER_THAN);

    /* Destroy the Stamps */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_OriginalStamp));
}

/* Test comparing Stamps fails with invalid param */
void ITC_Stamp_Test_compareStampsFailInvalidParam(void)
{
    ITC_Stamp_t *pt_DummyStamp = NULL;
    ITC_Stamp_Comparison_t t_DummyResult;

    TEST_FAILURE(
        ITC_Stamp_compare(pt_DummyStamp, NULL, NULL), ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_compare(NULL, pt_DummyStamp, NULL), ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_compare(
            NULL,
            NULL,
            &t_DummyResult),
        ITC_STATUS_INVALID_PARAM);
}

/* Test comparing an Stamp fails with corrupt Stamp */
void ITC_Stamp_Test_compareStampFailWithCorruptStamp(void)
{
    ITC_Stamp_t *pt_Stamp1;
    ITC_Stamp_t *pt_Stamp2;
    ITC_Stamp_Comparison_t t_Result;

    /* Test different invalid Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidStampTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp1);

        /* Construct the other Stamp */
        TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp2));

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_compare(pt_Stamp1, pt_Stamp2, &t_Result),
            ITC_STATUS_CORRUPT_STAMP);
        /* And the other way around */
        TEST_FAILURE(
            ITC_Stamp_compare(pt_Stamp2, pt_Stamp1, &t_Result),
            ITC_STATUS_CORRUPT_STAMP);

        /* Destroy the Stamps */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp1);
        TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp2));
    }
}

/* Test comparing a Stamp fails with corrupt Id or Event */
void ITC_Stamp_Test_compareStampFailWithCorruptIdOrEvent(void)
{
    ITC_Stamp_t *pt_Stamp1;
    ITC_Stamp_t *pt_Stamp2;
    ITC_Stamp_Comparison_t t_Result;

    /* Create a new stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp1));
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp2));

    /* Deallocate the valid ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Stamp1->pt_Id));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Stamp1->pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_compare(pt_Stamp1, pt_Stamp2, &t_Result),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Stamp1->pt_Id);
    }

    /* Allocate a valid ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Stamp1->pt_Id, NULL));

    /* Deallocate the valid Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Stamp1->pt_Event));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Stamp1->pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_compare(pt_Stamp1, pt_Stamp2, &t_Result),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Stamp1->pt_Event);
    }

    /* Deallocate the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp1));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp2));
}

/* Test comparing two Stamps succeeds */
void ITC_Stamp_Test_compareStampsSucceeds(void)
{
    ITC_Stamp_t *pt_Stamp1;
    ITC_Stamp_t *pt_Stamp2;
    ITC_Stamp_Comparison_t t_Result;

    /* Create the Stamps */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp1));
    TEST_SUCCESS(ITC_Stamp_newPeek(pt_Stamp1, &pt_Stamp2));

    TEST_SUCCESS(
        ITC_TestUtil_newEvent(
            &pt_Stamp1->pt_Event->pt_Left, pt_Stamp1->pt_Event, 0));
    TEST_SUCCESS(
        ITC_TestUtil_newEvent(
            &pt_Stamp1->pt_Event->pt_Right, pt_Stamp1->pt_Event, 3));

    pt_Stamp2->pt_Event->t_Count = 1;
    TEST_SUCCESS(
        ITC_TestUtil_newEvent(
            &pt_Stamp2->pt_Event->pt_Left, pt_Stamp2->pt_Event, 0));
    TEST_SUCCESS(
        ITC_TestUtil_newEvent(
            &pt_Stamp2->pt_Event->pt_Right, pt_Stamp2->pt_Event, 2));

    /* Compare Stamps */
    TEST_SUCCESS(ITC_Stamp_compare(pt_Stamp1, pt_Stamp2, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_LESS_THAN, t_Result);
    /* Compare the other way around */
    TEST_SUCCESS(ITC_Stamp_compare(pt_Stamp2, pt_Stamp1, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);

    /* Make the 2 Stamps concurrent */
    pt_Stamp2->pt_Event->pt_Right->t_Count -= 1;

    /* Compare Stamps */
    TEST_SUCCESS(ITC_Stamp_compare(pt_Stamp1, pt_Stamp2, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_CONCURRENT, t_Result);
    /* Compare the other way around */
    TEST_SUCCESS(ITC_Stamp_compare(pt_Stamp2, pt_Stamp1, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_CONCURRENT, t_Result);

    /* Check stamps are equal to themselves */
    TEST_SUCCESS(ITC_Stamp_compare(pt_Stamp1, pt_Stamp1, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_EQUAL, t_Result);
    TEST_SUCCESS(ITC_Stamp_compare(pt_Stamp2, pt_Stamp2, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_EQUAL, t_Result);

    /* Destroy the Stamps */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp1));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp2));
}

/* Test full Stamp lifecycle */
void ITC_Stamp_Test_fullStampLifecycle(void)
{
    ITC_Stamp_t *pt_OriginalStamp;
    ITC_Stamp_t *pt_SplitStamp1;
    ITC_Stamp_t *pt_SplitStamp11;
    ITC_Stamp_t *pt_SplitStamp12;
    ITC_Stamp_t *pt_SplitStamp111;
    ITC_Stamp_t *pt_SplitStamp121;
    ITC_Stamp_t *pt_SplitStamp112;
    ITC_Stamp_t *pt_SplitStamp122;
    ITC_Stamp_t *pt_SplitStamp2;
    ITC_Stamp_t *pt_SplitStamp21;
    ITC_Stamp_t *pt_SplitStamp22;
    ITC_Stamp_t *pt_SplitStamp211;
    ITC_Stamp_t *pt_SplitStamp221;
    ITC_Stamp_t *pt_SplitStamp212;
    ITC_Stamp_t *pt_SplitStamp222;

    ITC_Stamp_t *pt_SummedStamp = NULL;
    ITC_Stamp_t *pt_TmpStamp = NULL;

    ITC_Stamp_Comparison_t t_Result;

    /* Create the initial stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_OriginalStamp));

    /* Split into Stamps with (1, 0) and (0, 1) IDs */
    TEST_SUCCESS(
        ITC_Stamp_fork(pt_OriginalStamp, &pt_SplitStamp1, &pt_SplitStamp2));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_OriginalStamp));

    /* Add some events */
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp1));
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp1));
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp2));

    /* Test stamp ordering */
    TEST_SUCCESS(ITC_Stamp_compare(pt_SplitStamp1, pt_SplitStamp2, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_CONCURRENT, t_Result);

    /* Split into Stamps with IDs:
     * - ((1, 0), 0)
     * - (0, (1, 0))
     * - ((0, 1), 0)
     * - (0, (0, 1))
     */
    TEST_SUCCESS(
        ITC_Stamp_fork(pt_SplitStamp1, &pt_SplitStamp11, &pt_SplitStamp21));
    TEST_SUCCESS(
        ITC_Stamp_fork(pt_SplitStamp2, &pt_SplitStamp12, &pt_SplitStamp22));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp1));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp2));

    /* Add some events */
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp11));
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp22));

    /* Test stamp ordering */
    TEST_SUCCESS(ITC_Stamp_compare(pt_SplitStamp11, pt_SplitStamp22, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_CONCURRENT, t_Result);
    TEST_SUCCESS(ITC_Stamp_compare(pt_SplitStamp11, pt_SplitStamp21, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);
    TEST_SUCCESS(ITC_Stamp_compare(pt_SplitStamp12, pt_SplitStamp22, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_LESS_THAN, t_Result);
    TEST_SUCCESS(ITC_Stamp_compare(pt_SplitStamp12, pt_SplitStamp21, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_CONCURRENT, t_Result);

    /* Split into Stamps with IDs:
     * - (((1, 0), 0), 0)
     * - (0, ((1, 0), 0))
     * - ((0, (1, 0)), 0)
     * - (0, (0, (1, 0)))
     * - (((0, 1), 0), 0)
     * - (0, ((0, 1), 0))
     * - ((0, (0, 1)), 0)
     * - (0, (0, (0, 1)))
     */
    TEST_SUCCESS(ITC_Stamp_fork(
        pt_SplitStamp11, &pt_SplitStamp111, &pt_SplitStamp211));
    TEST_SUCCESS(ITC_Stamp_fork(
        pt_SplitStamp12, &pt_SplitStamp112, &pt_SplitStamp212));
    TEST_SUCCESS(ITC_Stamp_fork(
        pt_SplitStamp21, &pt_SplitStamp121, &pt_SplitStamp221));
    TEST_SUCCESS(ITC_Stamp_fork(
        pt_SplitStamp22, &pt_SplitStamp122, &pt_SplitStamp222));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp11));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp12));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp21));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp22));

    /* Add some events */
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp211));
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp212));
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp222));
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp222));
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp122));
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp111));

    /* Too lasy to test ordering here... It's probably fine (TM) */

    /* Sum them back into to a seed Stamp while adding events in
     * arbitrary order */

    TEST_SUCCESS(ITC_Stamp_join(pt_SplitStamp222, pt_SplitStamp121, &pt_SummedStamp));

    /* Test the joined Stamp is greater than both of the source stamps */
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_SplitStamp222, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_SplitStamp121, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);

    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp222));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp121));

    /* Add some events */
    TEST_SUCCESS(ITC_Stamp_event(pt_SummedStamp));
    TEST_SUCCESS(ITC_Stamp_event(pt_SummedStamp));

    TEST_SUCCESS(ITC_Stamp_clone(pt_SummedStamp, &pt_TmpStamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SummedStamp));

    TEST_SUCCESS(ITC_Stamp_join(pt_SplitStamp211, pt_TmpStamp, &pt_SummedStamp));

    /* Test the joined Stamp is greater than both of the source stamps */
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_SplitStamp211, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_TmpStamp, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);

    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp211));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_TmpStamp));

    /* Add some events */
    TEST_SUCCESS(ITC_Stamp_event(pt_SummedStamp));

    TEST_SUCCESS(ITC_Stamp_clone(pt_SummedStamp, &pt_TmpStamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SummedStamp));

    TEST_SUCCESS(ITC_Stamp_join(pt_SplitStamp122, pt_TmpStamp, &pt_SummedStamp));

    /* Test the joined Stamp is greater than both of the source stamps */
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_SplitStamp122, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_TmpStamp, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);

    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp122));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_TmpStamp));

    /* Add some events */
    TEST_SUCCESS(ITC_Stamp_event(pt_SummedStamp));

    TEST_SUCCESS(ITC_Stamp_clone(pt_SummedStamp, &pt_TmpStamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SummedStamp));

    TEST_SUCCESS(ITC_Stamp_join(pt_SplitStamp111, pt_TmpStamp, &pt_SummedStamp));

    /* Test the joined Stamp is greater than both of the source stamps */
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_SplitStamp111, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_TmpStamp, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);

    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp111));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_TmpStamp));

    TEST_SUCCESS(ITC_Stamp_clone(pt_SummedStamp, &pt_TmpStamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SummedStamp));

    TEST_SUCCESS(ITC_Stamp_join(pt_SplitStamp221, pt_TmpStamp, &pt_SummedStamp));

    /* Test the joined Stamp is greater than or equal to both of the
     * source stamps */
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_SplitStamp221, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_TmpStamp, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_EQUAL, t_Result);

    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp221));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_TmpStamp));

    /* Add some events */
    TEST_SUCCESS(ITC_Stamp_event(pt_SummedStamp));
    TEST_SUCCESS(ITC_Stamp_event(pt_SummedStamp));
    TEST_SUCCESS(ITC_Stamp_event(pt_SummedStamp));

    TEST_SUCCESS(ITC_Stamp_clone(pt_SummedStamp, &pt_TmpStamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SummedStamp));

    TEST_SUCCESS(ITC_Stamp_join(pt_SplitStamp212, pt_TmpStamp, &pt_SummedStamp));

    /* Test the joined Stamp is greater than both of the source stamps */
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_SplitStamp212, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_TmpStamp, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);

    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp212));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_TmpStamp));

    TEST_SUCCESS(ITC_Stamp_clone(pt_SummedStamp, &pt_TmpStamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SummedStamp));

    TEST_SUCCESS(ITC_Stamp_join(pt_SplitStamp112, pt_TmpStamp, &pt_SummedStamp));

    /* Test the joined Stamp is greater than or equal to both of the
     * source stamps */
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_SplitStamp112, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_GREATER_THAN, t_Result);
    TEST_SUCCESS(
        ITC_Stamp_compare(pt_SummedStamp, pt_TmpStamp, &t_Result));
    TEST_ASSERT_EQUAL(ITC_STAMP_COMPARISON_EQUAL, t_Result);

    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp112));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_TmpStamp));

    /* clang-format off */
    /* Test the summed up Stamp has a seed ID with a
     * (1, 3, (0, (0, 0, 1), 8)) Event tree */
    TEST_ITC_ID_IS_SEED_ID(pt_SummedStamp->pt_Id);
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_SummedStamp->pt_Event, 1);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_SummedStamp->pt_Event->pt_Left, 3);
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_SummedStamp->pt_Event->pt_Right, 0);
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_SummedStamp->pt_Event->pt_Right->pt_Left, 0);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_SummedStamp->pt_Event->pt_Right->pt_Left->pt_Left, 0);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_SummedStamp->pt_Event->pt_Right->pt_Left->pt_Right, 1);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_SummedStamp->pt_Event->pt_Right->pt_Right, 8);
    /* clang-format on */

    /* Add an event */
    TEST_SUCCESS(ITC_Stamp_event(pt_SummedStamp));

    /* Test the summed up Stamp has a seed ID with a
     * (9) Event tree */
    TEST_ITC_ID_IS_SEED_ID(pt_SummedStamp->pt_Id);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_SummedStamp->pt_Event, 9);

    /* Split into Stamps with (1, 0) and (0, 1) IDs again */
    TEST_SUCCESS(ITC_Stamp_fork(
        pt_SummedStamp, &pt_SplitStamp1, &pt_SplitStamp2));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SummedStamp));

    /* Add Event */
    TEST_SUCCESS(ITC_Stamp_event(pt_SplitStamp1));

    /* Share the Event history */
    TEST_SUCCESS(ITC_Stamp_newPeek(pt_SplitStamp1, &pt_SummedStamp));
    TEST_SUCCESS(ITC_Stamp_join(pt_SummedStamp, pt_SplitStamp2, &pt_TmpStamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SummedStamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp2));

    /* Test the Stamps have the different IDs but the same Event history */
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_SplitStamp1->pt_Id);
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_SplitStamp1->pt_Event, 9);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_SplitStamp1->pt_Event->pt_Left, 1);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_SplitStamp1->pt_Event->pt_Right, 0);
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_TmpStamp->pt_Id);
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_TmpStamp->pt_Event, 9);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_TmpStamp->pt_Event->pt_Left, 1);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_TmpStamp->pt_Event->pt_Right, 0);

    TEST_SUCCESS(ITC_Stamp_join(pt_SplitStamp1, pt_TmpStamp, &pt_SummedStamp));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SplitStamp1));
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_TmpStamp));

    /* Test the Stamps has a seed ID but the same Event history */
    TEST_ITC_ID_IS_SEED_ID(pt_SummedStamp->pt_Id);
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_SummedStamp->pt_Event, 9);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_SummedStamp->pt_Event->pt_Left, 1);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_SummedStamp->pt_Event->pt_Right, 0);

    TEST_SUCCESS(ITC_Stamp_destroy(&pt_SummedStamp));
}

/* Test creating a Stamp from an ID fails with invalid param */
void ITC_Stamp_Test_createStampFromIdFailInvalidParam(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_DummyStamp = NULL;
    ITC_Id_t *pt_DummyId = NULL;

    TEST_FAILURE(
        ITC_Stamp_newFromId(
            pt_DummyId,
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_newFromId(
            NULL,
            &pt_DummyStamp),
        ITC_STATUS_INVALID_PARAM);
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test creating a Stamp from an invalid ID fails with corrupt ID */
void ITC_Stamp_Test_createStampFromIdFailWithCorruptId(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Id_t *pt_Id;

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_newFromId(pt_Id, &pt_Stamp),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the Stamp */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Id);
    }
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test creating a Stamp from an ID succeeds */
void ITC_Stamp_Test_createStampFromIdSuccessful(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp = NULL;
    ITC_Id_t *pt_Id = NULL;

    /* Create a new ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id, NULL));

    /* Create the Stamp */
    TEST_SUCCESS(ITC_Stamp_newFromId(pt_Id, &pt_Stamp));

    /* Test the ID has been copied and an Event tree has been allocated */
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp->pt_Event, 0);
    TEST_ASSERT_TRUE(pt_Id != pt_Stamp->pt_Id);
    TEST_ITC_ID_IS_SEED_ID(pt_Stamp->pt_Id);

    /* Deallocate everything */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test creating a Stamp from an ID and Event fails with invalid param */
void ITC_Stamp_Test_createStampFromIdAndEventFailInvalidParam(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_DummyStamp = NULL;
    ITC_Id_t *pt_DummyId = NULL;
    ITC_Event_t *pt_DummyEvent = NULL;

    TEST_FAILURE(
        ITC_Stamp_newFromIdAndEvent(
            pt_DummyId,
            pt_DummyEvent,
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_newFromIdAndEvent(
            NULL,
            pt_DummyEvent,
            &pt_DummyStamp),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_newFromIdAndEvent(
            pt_DummyId,
            NULL,
            &pt_DummyStamp),
        ITC_STATUS_INVALID_PARAM);
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test creating a Stamp from an invalid ID or Event fails with corrupt ID/Event */
void ITC_Stamp_Test_createStampFromIdFailWithCorruptEventAndId(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Event_t *pt_Event;
    ITC_Id_t *pt_Id;

    /* Create a valid ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id, NULL));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Event);

        /* Test for the failure */
        TEST_FAILURE(ITC_Stamp_newFromIdAndEvent(
            pt_Id,
            pt_Event,
            &pt_Stamp),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Event);
    }

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));

    /* Create a valid Event */
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event, NULL, 0));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid Id */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_newFromIdAndEvent(
                pt_Id,
                pt_Event,
                &pt_Stamp),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the Id */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Id);
    }

    /* Destroy the Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test creating a Stamp from an ID and Event succeeds */
void ITC_Stamp_Test_createStampFromIdAndEventSuccessful(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp = NULL;
    ITC_Id_t *pt_Id = NULL;
    ITC_Event_t *pt_Event = NULL;

    /* Create a new ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id, NULL));

    /* Create a new Event */
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event, NULL, 42));

    /* Create the Stamp */
    TEST_SUCCESS(ITC_Stamp_newFromIdAndEvent(pt_Id, pt_Event, &pt_Stamp));

    /* Test the ID and Event trees have been copied */
    TEST_ASSERT_TRUE(pt_Id != pt_Stamp->pt_Id);
    TEST_ASSERT_TRUE(pt_Event != pt_Stamp->pt_Event);
    TEST_ITC_ID_IS_SEED_ID(pt_Stamp->pt_Id);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp->pt_Event, 42);

    /* Deallocate everything */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test creating a peek Stamp from an Event fails with invalid param */
void ITC_Stamp_Test_createPeekStampFromEventFailInvalidParam(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_DummyStamp = NULL;
    ITC_Event_t *pt_DummyEvent = NULL;

    TEST_FAILURE(
        ITC_Stamp_newPeekFromEvent(
            pt_DummyEvent,
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_newPeekFromEvent(
            NULL,
            &pt_DummyStamp),
        ITC_STATUS_INVALID_PARAM);
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test creating a Stamp from an invalid Event fails with corrupt Event */
void ITC_Stamp_Test_createPeekStampFromEventFailWithCorruptEvent(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Event_t *pt_Event;

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_newPeekFromEvent(pt_Event, &pt_Stamp),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Stamp */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Event);
    }
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test creating a peek Stamp from an Event succeeds */
void ITC_Stamp_Test_createPeekStampFromEventSuccessful(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp = NULL;
    ITC_Event_t *pt_Event = NULL;

    /* Create a new ID */
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event, NULL, 100));

    /* Create the Stamp */
    TEST_SUCCESS(ITC_Stamp_newPeekFromEvent(pt_Event, &pt_Stamp));

    /* Test the Event has been copied and an ID tree has been allocated */
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp->pt_Event, 100);
    TEST_ASSERT_TRUE(pt_Event != pt_Stamp->pt_Event);
    TEST_ITC_ID_IS_NULL_ID(pt_Stamp->pt_Id);

    /* Deallocate everything */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test getting the ID component of a Stamp fails with invalid param */
void ITC_Stamp_Test_getIdFromStampFailInvalidParam(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_DummyStamp = NULL;
    ITC_Id_t *pt_DummyId;

    TEST_FAILURE(
        ITC_Stamp_getId(
            pt_DummyStamp,
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_getId(
            NULL,
            &pt_DummyId),
        ITC_STATUS_INVALID_PARAM);
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test getting the ID component of a Stamp fails with corrupt stamp */
void ITC_Stamp_Test_getIdFromStampFailWithCorruptStamp(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Id_t *pt_DummyId;

    /* Test different invalid Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidStampTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_getId(pt_Stamp, &pt_DummyId),
            ITC_STATUS_CORRUPT_STAMP);

        /* Destroy the Stamp */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp);
    }
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test getting the ID component of a Stamp fails with corrupt Id or Event */
void ITC_Stamp_Test_getIdFromStampFailWithCorruptIdOrEvent(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Id_t *pt_DummyId;

    /* Create a new stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Deallocate the valid ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Stamp->pt_Id));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Stamp->pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_getId(pt_Stamp, &pt_DummyId),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Stamp->pt_Id);
    }

    /* Allocate a valid ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Stamp->pt_Id, NULL));

    /* Deallocate the valid Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Stamp->pt_Event));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Stamp->pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_getId(pt_Stamp, &pt_DummyId),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Stamp->pt_Event);
    }

    /* Deallocate the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test getting the ID component of a Stamp succeeds */
void ITC_Stamp_Test_getIdFromStampSuccessful(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp = NULL;
    ITC_Id_t *pt_Id = NULL;

    /* Create the Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Get the ID */
    TEST_SUCCESS(ITC_Stamp_getId(pt_Stamp, &pt_Id));

    /* Test the ID has been copied */
    TEST_ASSERT_TRUE(pt_Id != pt_Stamp->pt_Id);
    TEST_ITC_ID_IS_SEED_ID(pt_Id);

    /* Deallocate everything */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test setting the ID component of a Stamp fails with invalid param */
void ITC_Stamp_Test_setIdOfStampFailInvalidParam(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_DummyStamp = NULL;
    ITC_Id_t *pt_DummyId = NULL;

    TEST_FAILURE(
        ITC_Stamp_setId(
            pt_DummyStamp,
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_setId(
            NULL,
            pt_DummyId),
        ITC_STATUS_INVALID_PARAM);
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test setting the Id component of a Stamp fails with corrupt stamp */
void ITC_Stamp_Test_setIdFromStampFailWithCorruptStamp(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Id_t *pt_DummyId = NULL;

    /* Test different invalid Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidStampTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_setId(pt_Stamp, pt_DummyId),
            ITC_STATUS_CORRUPT_STAMP);

        /* Destroy the Stamp */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp);
    }
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test setting the ID component of a Stamp with invalid components fails with
 * corrupt Id or Event */
void ITC_Stamp_Test_setIdOfStampWithInvalidComponentFailWithCorruptIdOrEvent(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Id_t *pt_DummyId = NULL;

    /* Create a new stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Deallocate the valid ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Stamp->pt_Id));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Stamp->pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_setId(pt_Stamp, pt_DummyId),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Stamp->pt_Id);
    }

    /* Allocate a valid ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Stamp->pt_Id, NULL));

    /* Deallocate the valid Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Stamp->pt_Event));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Stamp->pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_setId(pt_Stamp, pt_DummyId),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Stamp->pt_Event);
    }

    /* Deallocate the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test setting the ID component of a Stamp fails with corrupt ID */
void ITC_Stamp_Test_setIdOfStampFailWithCorruptId(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Id_t *pt_Id;

    /* Create a Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_setId(pt_Stamp, pt_Id),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the Stamp */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Id);
    }

    /* Destroy the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
#else
  TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test setting the ID component of a Stamp succeeds */
void ITC_Stamp_Test_setIdOfStampSuccessful(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp = NULL;
    ITC_Id_t *pt_Id = NULL;

    /* Create the Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Create the ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));

    /* Set the ID */
    TEST_SUCCESS(ITC_Stamp_setId(pt_Stamp, pt_Id));

    /* Test the ID has been copied */
    TEST_ASSERT_TRUE(pt_Id != pt_Stamp->pt_Id);
    /* XXX: Deliberately not testing if addresses of the old and new
     * `pt_Stamp->pt_Id`s differ, as depending on the `malloc` implementation
     * they might end up being the same address (since the old ID is deallocated
     * first, followed immediately by the new ID allocation) */
    TEST_ITC_ID_IS_NULL_ID(pt_Stamp->pt_Id);

    /* Deallocate everything */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test getting the Event component of a Stamp fails with invalid param */
void ITC_Stamp_Test_getEventFromStampFailInvalidParam(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_DummyStamp = NULL;
    ITC_Event_t *pt_DummyEvent;

    TEST_FAILURE(
        ITC_Stamp_getEvent(
            pt_DummyStamp,
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_getEvent(
            NULL,
            &pt_DummyEvent),
        ITC_STATUS_INVALID_PARAM);
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test getting the Event component of a Stamp fails with corrupt stamp */
void ITC_Stamp_Test_getEventFromStampFailWithCorruptStamp(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Event_t *pt_DummyEvent;

    /* Test different invalid Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidStampTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_getEvent(pt_Stamp, &pt_DummyEvent),
            ITC_STATUS_CORRUPT_STAMP);

        /* Destroy the Stamp */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp);
    }
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test getting the Event component of a Stamp fails with corrupt Id or Event */
void ITC_Stamp_Test_getEventFromStampFailWithCorruptIdOrEvent(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Event_t *pt_DummyEvent;

    /* Create a new stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Deallocate the valid ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Stamp->pt_Id));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Stamp->pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_getEvent(pt_Stamp, &pt_DummyEvent),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Stamp->pt_Id);
    }

    /* Allocate a valid ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Stamp->pt_Id, NULL));

    /* Deallocate the valid Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Stamp->pt_Event));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Stamp->pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_getEvent(pt_Stamp, &pt_DummyEvent),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Stamp->pt_Event);
    }

    /* Deallocate the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test getting the Event component of a Stamp succeeds */
void ITC_Stamp_Test_getEventFromStampSuccessful(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp = NULL;
    ITC_Event_t *pt_Event = NULL;

    /* Create the Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));
    pt_Stamp->pt_Event->t_Count = 42;

    /* Get the Event */
    TEST_SUCCESS(ITC_Stamp_getEvent(pt_Stamp, &pt_Event));

    /* Test the Event has been copied */
    TEST_ASSERT_TRUE(pt_Event != pt_Stamp->pt_Event);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Event, 42);

    /* Deallocate everything */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test setting the Event component of a Stamp fails with invalid param */
void ITC_Stamp_Test_setEventOfStampFailInvalidParam(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_DummyStamp = NULL;
    ITC_Event_t *pt_DummyEvent = NULL;

    TEST_FAILURE(
        ITC_Stamp_setEvent(
            pt_DummyStamp,
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Stamp_setEvent(
            NULL,
            pt_DummyEvent),
        ITC_STATUS_INVALID_PARAM);
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test setting the Event component of a Stamp fails with corrupt stamp */
void ITC_Stamp_Test_setEventFromStampFailWithCorruptStamp(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Event_t *pt_DummyEvent = NULL;

    /* Test different invalid Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidStampTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_setEvent(pt_Stamp, pt_DummyEvent),
            ITC_STATUS_CORRUPT_STAMP);

        /* Destroy the Stamp */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp);
    }
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test setting the Event component of a Stamp with invalid components fails with
 * corrupt Event or Event */
void ITC_Stamp_Test_setEventOfStampWithInvalidComponentFailWithCorruptIdOrEvent(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Event_t *pt_DummyEvent = NULL;

    /* Create a new stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Deallocate the valid ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Stamp->pt_Id));

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Stamp->pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_setEvent(pt_Stamp, pt_DummyEvent),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Stamp->pt_Id);
    }

    /* Allocate a valid ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Stamp->pt_Id, NULL));

    /* Deallocate the valid Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Stamp->pt_Event));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Stamp->pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_setEvent(pt_Stamp, pt_DummyEvent),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Stamp->pt_Event);
    }

    /* Deallocate the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test setting the Event component of a Stamp fails with corrupt Event */
void ITC_Stamp_Test_setEventOfStampFailWithCorruptEvent(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp;
    ITC_Event_t *pt_Event;

    /* Create a Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Stamp_setEvent(pt_Stamp, pt_Event),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Stamp */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Event);
    }

    /* Destroy the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
#else
  TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}

/* Test setting the Event component of a Stamp succeeds */
void ITC_Stamp_Test_setEventOfStampSuccessful(void)
{
#if ITC_CONFIG_ENABLE_EXTENDED_API
    ITC_Stamp_t *pt_Stamp = NULL;
    ITC_Event_t *pt_Event = NULL;

    /* Create the Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Create the Event */
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event, NULL, 100));

    /* Set the Event */
    TEST_SUCCESS(ITC_Stamp_setEvent(pt_Stamp, pt_Event));

    /* Test the Event has been copied */
    TEST_ASSERT_TRUE(pt_Event != pt_Stamp->pt_Event);
    /* XXX: Deliberately not testing if addresses of the old and new
     * `pt_Stamp->pt_Event`s differ, as depending on the `malloc` implementation
     * they might end up being the same address (since the old Event is
     * deallocated first, followed immediately by the new ID allocation) */
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp->pt_Event, 100);

    /* Deallocate everything */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
#else
    TEST_IGNORE_MESSAGE("Extended API support is disabled");
#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */
}
