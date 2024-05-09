/**
 * @file ITC_SerDes_Test.h
 * @brief Unit tests for the Interval Tree Clock's serialisation and
 * deserialisation mechanism
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#include "ITC_SerDes.h"
#include "ITC_SerDes_package.h"
#include "ITC_SerDes_Test.h"

#include "ITC_SerDes_Test_package.h"
#include "ITC_Test_package.h"
#include "ITC_TestUtil.h"

#include "ITC_Id_package.h"
#include "ITC_Event_package.h"

#include "ITC_Stamp.h"

#include <stdint.h>

/******************************************************************************
 *  Private functions
 ******************************************************************************/

/******************************************************************************
 *  Global variables
 ******************************************************************************/

/******************************************************************************
 *  Public functions
 ******************************************************************************/

/* Init test */
void setUp(void) {}

/* Fini test */
void tearDown(void) {}

/* Test serialising a Id fails with invalid param */
void ITC_SerDes_Test_serialiseIdFailInvalidParam(void)
{
    ITC_Id_t *pt_Dummy = NULL;
    uint8_t ru8_Buffer[10] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    TEST_FAILURE(
        ITC_SerDes_serialiseId(
            pt_Dummy,
            &ru8_Buffer[0],
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_serialiseId(
            NULL,
            &ru8_Buffer[0],
            &u32_BufferSize),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_serialiseId(
            pt_Dummy,
            NULL,
            &u32_BufferSize),
        ITC_STATUS_INVALID_PARAM);
}

/* Test serialising an ID fails with corrupt ID */
void ITC_SerDes_Test_serialiseIdFailWithCorruptId(void)
{
    ITC_Id_t *pt_Id;
    uint8_t ru8_Buffer[10] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_SerDes_serialiseId(
                pt_Id,
                &ru8_Buffer[0],
                &u32_BufferSize),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Id);
    }
}

/* Test serialising a leaf ID succeeds */
void ITC_SerDes_Test_serialiseIdLeafSuccessful(void)
{
    ITC_Id_t *pt_Id = NULL;
    uint8_t ru8_Buffer[10] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    uint8_t ru8_ExpectedSeedIdSerialisedData[] = {
        ITC_SERDES_SEED_ID_HEADER
    };
    uint8_t ru8_ExpectedNullIdSerialisedData[] = {
        ITC_SERDES_NULL_ID_HEADER
    };

    /* Create a new seed ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id, NULL));

    /* Serialise the ID */
    TEST_SUCCESS(
        ITC_SerDes_serialiseId(pt_Id, &ru8_Buffer[0], &u32_BufferSize));

    /* Test the serialised data is what is expected */
    TEST_ASSERT_EQUAL(sizeof(ru8_ExpectedSeedIdSerialisedData), u32_BufferSize);
    TEST_ASSERT_EQUAL_MEMORY(
        &ru8_ExpectedSeedIdSerialisedData[0],
        &ru8_Buffer[0],
        sizeof(ru8_ExpectedSeedIdSerialisedData));

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));

    /* Create a new null ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));

    /* Reset the buffer size */
    u32_BufferSize = sizeof(ru8_Buffer);

    /* Serialise the ID */
    TEST_SUCCESS(
        ITC_SerDes_serialiseId(pt_Id, &ru8_Buffer[0], &u32_BufferSize));

    /* Test the serialised data is what is expected */
    TEST_ASSERT_EQUAL(sizeof(ru8_ExpectedNullIdSerialisedData), u32_BufferSize);
    TEST_ASSERT_EQUAL_MEMORY(
        &ru8_ExpectedNullIdSerialisedData[0],
        &ru8_Buffer[0],
        sizeof(ru8_ExpectedNullIdSerialisedData));

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test serialising a leaf ID fails with insufficent resources */
void ITC_SerDes_Test_serialiseIdFailWithInsufficentResources(void)
{
    ITC_Id_t *pt_Id = NULL;
    uint8_t ru8_Buffer[2] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Create a new ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right, pt_Id));

    /* Serialise the ID */
    TEST_FAILURE(
        ITC_SerDes_serialiseId(
            pt_Id,
            &ru8_Buffer[0],
            &u32_BufferSize),
        ITC_STATUS_INSUFFICIENT_RESOURCES);

    u32_BufferSize = 0;

    /* Serialise the ID */
    TEST_FAILURE(
        ITC_SerDes_serialiseId(
            pt_Id->pt_Left,
            &ru8_Buffer[0],
            &u32_BufferSize),
        ITC_STATUS_INVALID_PARAM);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test serialising a leaf ID subtree succeeds */
void ITC_SerDes_Test_serialiseIdLeafSubtreeSuccessful(void)
{
    ITC_Id_t *pt_Id = NULL;
    uint8_t ru8_Buffer[10] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    uint8_t ru8_ExpectedSeedIdSerialisedData[] = {
        ITC_SERDES_SEED_ID_HEADER
    };
    uint8_t ru8_ExpectedNullIdSerialisedData[] = {
        ITC_SERDES_NULL_ID_HEADER
    };

    /* Create a new ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right, pt_Id));

    /* Serialise the ID */
    TEST_SUCCESS(
        ITC_SerDes_serialiseId(
            pt_Id->pt_Left,
            &ru8_Buffer[0],
            &u32_BufferSize));

    /* Test the serialised data is what is expected */
    TEST_ASSERT_EQUAL(sizeof(ru8_ExpectedSeedIdSerialisedData), u32_BufferSize);
    TEST_ASSERT_EQUAL_MEMORY(
        &ru8_ExpectedSeedIdSerialisedData[0],
        &ru8_Buffer[0],
        sizeof(ru8_ExpectedSeedIdSerialisedData));

    /* Reset the buffer size */
    u32_BufferSize = sizeof(ru8_Buffer);

    /* Serialise the ID */
    TEST_SUCCESS(
        ITC_SerDes_serialiseId(
            pt_Id->pt_Right,
            &ru8_Buffer[0],
            &u32_BufferSize));

    /* Test the serialised data is what is expected */
    TEST_ASSERT_EQUAL(sizeof(ru8_ExpectedNullIdSerialisedData), u32_BufferSize);
    TEST_ASSERT_EQUAL_MEMORY(
        &ru8_ExpectedNullIdSerialisedData[0],
        &ru8_Buffer[0],
        sizeof(ru8_ExpectedNullIdSerialisedData));

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test serialising a parent ID succeeds */
void ITC_SerDes_Test_serialiseIdParentSuccessful(void)
{
    ITC_Id_t *pt_Id = NULL;
    uint8_t ru8_Buffer[10] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Serialised (0, ((1, 0), 1)) ID */
    uint8_t ru8_ExpectedIdSerialisedData[] = {
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_NULL_ID_HEADER,
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
        ITC_SERDES_NULL_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
    };

    /* clang-format off */
    /* Create a new (0, ((1, 0), 1)) ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right->pt_Left, pt_Id->pt_Right));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Right->pt_Left->pt_Left, pt_Id->pt_Right->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right->pt_Left->pt_Right, pt_Id->pt_Right->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Right->pt_Right, pt_Id->pt_Right));
    /* clang-format on */

    /* Serialise the ID */
    TEST_SUCCESS(
        ITC_SerDes_serialiseId(
            pt_Id,
            &ru8_Buffer[0],
            &u32_BufferSize));

    /* Test the serialised data is what is expected */
    TEST_ASSERT_EQUAL(sizeof(ru8_ExpectedIdSerialisedData), u32_BufferSize);
    TEST_ASSERT_EQUAL_MEMORY(
        &ru8_ExpectedIdSerialisedData[0],
        &ru8_Buffer[0],
        sizeof(ru8_ExpectedIdSerialisedData));

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test deserialising a ID fails with invalid param */
void ITC_SerDes_Test_deserialiseIdFailInvalidParam(void)
{
    ITC_Id_t *pt_Dummy = NULL;
    uint8_t ru8_Buffer[10] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    TEST_FAILURE(
        ITC_SerDes_deserialiseId(
            &ru8_Buffer[0],
            0,
            &pt_Dummy),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_deserialiseId(
            NULL,
            u32_BufferSize,
            &pt_Dummy),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_deserialiseId(
            &ru8_Buffer[0],
            u32_BufferSize,
            NULL),
        ITC_STATUS_INVALID_PARAM);
}

/* Test deserialising an ID fails with corrupt ID */
void ITC_SerDes_Test_deserialiseIdFailWithCorruptId(void)
{
    ITC_Id_t *pt_Id;
    const uint8_t *pu8_Buffer = NULL;
    uint32_t u32_BufferSize = 0;

    /* Test different invalid serialised IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidSerialisedIdTableSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidSerialisedIdConstructorTable[u32_I](
            &pu8_Buffer, &u32_BufferSize);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_SerDes_deserialiseId(
                pu8_Buffer,
                u32_BufferSize,
                &pt_Id),
            ITC_STATUS_CORRUPT_ID);
    }
}

/* Test deserialising a leaf ID suceeds */
void ITC_SerDes_Test_deserialiseLeafIdSuccessful(void)
{
    ITC_Id_t *pt_Id;
    uint8_t ru8_Buffer[] = { ITC_SERDES_SEED_ID_HEADER };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Test deserialising a seed ID */
    TEST_SUCCESS(
        ITC_SerDes_deserialiseId(&ru8_Buffer[0], u32_BufferSize, &pt_Id));

    /* Test this is a seed ID */
    TEST_ITC_ID_IS_SEED_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));

    /* Test with a null ID */
    ru8_Buffer[0] = ITC_SERDES_NULL_ID_HEADER;

    /* Test deserialising a seed ID */
    TEST_SUCCESS(
        ITC_SerDes_deserialiseId(&ru8_Buffer[0], u32_BufferSize, &pt_Id));

    /* Test this is a seed ID */
    TEST_ITC_ID_IS_NULL_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test deserialising a parent ID suceeds */
void ITC_SerDes_Test_deserialiseParentIdSuccessful(void)
{
    ITC_Id_t *pt_Id;
    /* Serialised (0, ((1, 0), 1)) ID */
    uint8_t ru8_Buffer[] = {
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_NULL_ID_HEADER,
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
        ITC_SERDES_NULL_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
    };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Test deserialising the ID */
    TEST_SUCCESS(
        ITC_SerDes_deserialiseId(&ru8_Buffer[0], u32_BufferSize, &pt_Id));

    /* Test this is a (0, ((1, 0), 1)) ID */
    TEST_ITC_ID_IS_NULL_ID(pt_Id->pt_Left);
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_Id->pt_Right->pt_Left);
    TEST_ITC_ID_IS_SEED_ID(pt_Id->pt_Right->pt_Right);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test serialising an Event fails with invalid param */
void ITC_SerDes_Test_serialiseEventFailInvalidParam(void)
{
    ITC_Event_t *pt_Dummy = NULL;
    uint8_t ru8_Buffer[10] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    TEST_FAILURE(
        ITC_SerDes_serialiseEvent(
            pt_Dummy,
            &ru8_Buffer[0],
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_serialiseEvent(
            NULL,
            &ru8_Buffer[0],
            &u32_BufferSize),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_serialiseEvent(
            pt_Dummy,
            NULL,
            &u32_BufferSize),
        ITC_STATUS_INVALID_PARAM);
}

/* Test serialising an Event fails with corrupt Event */
void ITC_SerDes_Test_serialiseEventFailWithCorruptEvent(void)
{
    ITC_Event_t *pt_Event;
    uint8_t ru8_Buffer[10] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Test different invalid Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidEventTablesSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidEventConstructorTable[u32_I](&pt_Event);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_SerDes_serialiseEvent(
                pt_Event,
                &ru8_Buffer[0],
                &u32_BufferSize),
            ITC_STATUS_CORRUPT_EVENT);

        /* Destroy the Event */
        gpv_InvalidEventDestructorTable[u32_I](&pt_Event);
    }
}

/* Test serialising a leaf Event succeeds */
void ITC_SerDes_Test_serialiseEventLeafSuccessful(void)
{
    ITC_Event_t *pt_Event = NULL;
    uint8_t ru8_Buffer[10] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    uint8_t ru8_ExpectedEventSerialisedData[] = {
        ITC_SERDES_CREATE_EVENT_HEADER(false, 1),
        123
    };

    uint8_t ru8_Expected0EventSerialisedData[] = {
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0)
    };

    /* Create a new Event */
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event, NULL, 123));

    /* Serialise the Event */
    TEST_SUCCESS(
        ITC_SerDes_serialiseEvent(pt_Event, &ru8_Buffer[0], &u32_BufferSize));

    /* Test the serialised data is what is expected */
    TEST_ASSERT_EQUAL(sizeof(ru8_ExpectedEventSerialisedData), u32_BufferSize);
    TEST_ASSERT_EQUAL_MEMORY(
        &ru8_ExpectedEventSerialisedData[0],
        &ru8_Buffer[0],
        sizeof(ru8_ExpectedEventSerialisedData));

    /* Destroy the Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));

    /* Create a new Event */
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event, NULL, 0));

    /* Serialise the Event */
    TEST_SUCCESS(
        ITC_SerDes_serialiseEvent(pt_Event, &ru8_Buffer[0], &u32_BufferSize));

    /* Test the serialised data is what is expected */
    TEST_ASSERT_EQUAL(sizeof(ru8_Expected0EventSerialisedData), u32_BufferSize);
    TEST_ASSERT_EQUAL_MEMORY(
        &ru8_Expected0EventSerialisedData[0],
        &ru8_Buffer[0],
        sizeof(ru8_Expected0EventSerialisedData));

    /* Destroy the Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
}

/* Test serialising a leaf Event fails with insufficent resources */
void ITC_SerDes_Test_serialiseEventFailWithInsufficentResources(void)
{
    ITC_Event_t *pt_Event = NULL;
    uint8_t ru8_Buffer[3] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Create a new Event */
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event, NULL, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event->pt_Left, pt_Event, 1));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event->pt_Right, pt_Event, 0));

    /* Serialise the Event */
    TEST_FAILURE(
        ITC_SerDes_serialiseEvent(
            pt_Event,
            &ru8_Buffer[0],
            &u32_BufferSize),
        ITC_STATUS_INSUFFICIENT_RESOURCES);

    u32_BufferSize = 0;

    /* Serialise the Event */
    TEST_FAILURE(
        ITC_SerDes_serialiseEvent(
            pt_Event->pt_Left,
            &ru8_Buffer[0],
            &u32_BufferSize),
        ITC_STATUS_INVALID_PARAM);

    /* Destroy the Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
}

/* Test serialising a leaf Event subtree succeeds */
void ITC_SerDes_Test_serialiseEventLeafSubtreeSuccessful(void)
{
    ITC_Event_t *pt_Event = NULL;
    uint8_t ru8_Buffer[10] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    uint8_t ru8_ExpectedEventSerialisedData[] = {
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0)
    };

    /* Create a new Event */
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event, NULL, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event->pt_Left, pt_Event, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event->pt_Right, pt_Event, 1));

    /* Serialise the Event */
    TEST_SUCCESS(
        ITC_SerDes_serialiseEvent(
            pt_Event->pt_Left,
            &ru8_Buffer[0],
            &u32_BufferSize));

    /* Test the serialised data is what is expected */
    TEST_ASSERT_EQUAL(sizeof(ru8_ExpectedEventSerialisedData), u32_BufferSize);
    TEST_ASSERT_EQUAL_MEMORY(
        &ru8_ExpectedEventSerialisedData[0],
        &ru8_Buffer[0],
        sizeof(ru8_ExpectedEventSerialisedData));

    /* Destroy the Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
}

/* Test serialising a parent Event succeeds */
void ITC_SerDes_Test_serialiseEventParentSuccessful(void)
{
    ITC_Event_t *pt_Event = NULL;
    uint8_t ru8_Buffer[14] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Serialised (0, 1, (0, (4242, 0, 123123123), 0)) Event */
    uint8_t ru8_ExpectedEventSerialisedData[] = {
        ITC_SERDES_CREATE_EVENT_HEADER(true, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 1),
        1,
        ITC_SERDES_CREATE_EVENT_HEADER(true, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(true, 2),
        (4242U >> 8U) & 0xFFU,
        4242U & 0xFFU,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 4),
        (123123123U >> 24U) & 0xFFU,
        (123123123U >> 16U) & 0xFFU,
        (123123123U >> 8U) & 0xFFU,
        123123123U & 0xFFU,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
    };

    /* clang-format off */
    /* Create a new (0, 1, (0, (4242, 0, 123123123), 0)) Event */
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event, NULL, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event->pt_Left, pt_Event, 1));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event->pt_Right, pt_Event, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event->pt_Right->pt_Left, pt_Event->pt_Right, 4242));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event->pt_Right->pt_Left->pt_Left, pt_Event->pt_Right->pt_Left, 0));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event->pt_Right->pt_Left->pt_Right, pt_Event->pt_Right->pt_Left, 123123123));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Event->pt_Right->pt_Right, pt_Event->pt_Right, 0));
    /* clang-format on */

    /* Serialise the Event */
    TEST_SUCCESS(
        ITC_SerDes_serialiseEvent(
            pt_Event,
            &ru8_Buffer[0],
            &u32_BufferSize));

    /* Test the serialised data is what is expected */
    TEST_ASSERT_EQUAL(sizeof(ru8_ExpectedEventSerialisedData), u32_BufferSize);
    TEST_ASSERT_EQUAL_MEMORY(
        &ru8_ExpectedEventSerialisedData[0],
        &ru8_Buffer[0],
        sizeof(ru8_ExpectedEventSerialisedData));

    /* Destroy the Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
}

/* Test deserialising an Event fails with invalid param */
void ITC_SerDes_Test_deserialiseEventFailInvalidParam(void)
{
    ITC_Event_t *pt_Dummy = NULL;
    uint8_t ru8_Buffer[10] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    TEST_FAILURE(
        ITC_SerDes_deserialiseEvent(
            &ru8_Buffer[0],
            0,
            &pt_Dummy),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_deserialiseEvent(
            NULL,
            u32_BufferSize,
            &pt_Dummy),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_deserialiseEvent(
            &ru8_Buffer[0],
            u32_BufferSize,
            NULL),
        ITC_STATUS_INVALID_PARAM);
}

/* Test deserialising an Event fails with corrupt Event */
void ITC_SerDes_Test_deserialiseEventFailWithCorruptEvent(void)
{
    ITC_Event_t *pt_Event;
    const uint8_t *pu8_Buffer = NULL;
    uint32_t u32_BufferSize = 0;

    /* Test different invalid serialised Events are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidSerialisedEventTableSize;
         u32_I++)
    {
        /* Construct an invalid Event */
        gpv_InvalidSerialisedEventConstructorTable[u32_I](
            &pu8_Buffer, &u32_BufferSize);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_SerDes_deserialiseEvent(
                pu8_Buffer,
                u32_BufferSize,
                &pt_Event),
            ITC_STATUS_CORRUPT_EVENT);
    }
}

/* Test deserialising an Event fails with unsupported counter size */
void ITC_SerDes_Test_deserialiseEventFailWithUnsupportedCounterSize(void)
{
    ITC_Event_t *pt_Event;
    uint8_t ru8_Buffer[] = {
        ITC_SERDES_CREATE_EVENT_HEADER(
            false,
            (sizeof(ITC_Event_Counter_t) + 1)),
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
    };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Test for the failure */
    TEST_FAILURE(
        ITC_SerDes_deserialiseEvent(
            &ru8_Buffer[0],
            u32_BufferSize,
            &pt_Event),
        ITC_STATUS_EVENT_UNSUPPORTED_COUNTER_SIZE);
}

/* Test deserialising a leaf Event suceeds */
void ITC_SerDes_Test_deserialiseLeafEventSuccessful(void)
{
    ITC_Event_t *pt_Event;
    uint8_t ru8_Buffer[] = {
        ITC_SERDES_CREATE_EVENT_HEADER(false, 1),
        123,
    };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);
    uint8_t ru8_0EventBuffer[] = {
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0)
    };
    uint32_t u32_0EventBufferSize = sizeof(ru8_0EventBuffer);

    /* Test deserialising a leaf Event */
    TEST_SUCCESS(
        ITC_SerDes_deserialiseEvent(&ru8_Buffer[0], u32_BufferSize, &pt_Event));

    /* Test this is a leaf Event with the correct event count */
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Event, 123);

    /* Destroy the Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));

    /* Test deserialising a leaf Event */
    TEST_SUCCESS(
        ITC_SerDes_deserialiseEvent(
            &ru8_0EventBuffer[0],
            u32_0EventBufferSize,
            &pt_Event));

    /* Test this is a leaf Event with the correct event count */
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Event, 0);

    /* Destroy the Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
}

/* Test deserialising a parent Event suceeds */
void ITC_SerDes_Test_deserialiseParentEventSuccessful(void)
{
    ITC_Event_t *pt_Event;
    /* Serialised (0, 1, (0, (4242, 0, 123123123), 0)) Event */
    uint8_t ru8_Buffer[] = {
        ITC_SERDES_CREATE_EVENT_HEADER(true, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 1),
        1,
        ITC_SERDES_CREATE_EVENT_HEADER(true, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(true, 2),
        (4242U >> 8U) & 0xFFU,
        4242U & 0xFFU,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 4),
        (123123123U >> 24U) & 0xFFU,
        (123123123U >> 16U) & 0xFFU,
        (123123123U >> 8U) & 0xFFU,
        123123123U & 0xFFU,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
    };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Test deserialising the Event */
    TEST_SUCCESS(
        ITC_SerDes_deserialiseEvent(&ru8_Buffer[0], u32_BufferSize, &pt_Event));

    /* clang-format off */
    /* Test this is a (0, 1, (0, (4242, 0, 123123123), 0)) Event */
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_Event, 0);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Left, 1);
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_Event->pt_Right, 0);
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_Event->pt_Right->pt_Left, 4242);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Right->pt_Left->pt_Left, 0);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Right->pt_Left->pt_Right, 123123123);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Event->pt_Right->pt_Right, 0);
    /* clang-format on */

    /* Destroy the Event */
    TEST_SUCCESS(ITC_Event_destroy(&pt_Event));
}

/* Test serialising a Stamp fails with invalid param */
void ITC_SerDes_Test_serialiseStampFailInvalidParam(void)
{
    ITC_Stamp_t *pt_Dummy = NULL;
    uint8_t ru8_Buffer[10];
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    TEST_FAILURE(
        ITC_SerDes_serialiseStamp(
            pt_Dummy,
            &ru8_Buffer[0],
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_serialiseStamp(
            NULL,
            &ru8_Buffer[0],
            &u32_BufferSize),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_serialiseStamp(
            pt_Dummy,
            NULL,
            &u32_BufferSize),
        ITC_STATUS_INVALID_PARAM);
}

/* Test serialising a Stamp fails with corrupt stamp */
void ITC_SerDes_Test_serialiseStampFailWithCorruptStamp(void)
{
    ITC_Stamp_t *pt_Stamp;
    uint8_t ru8_Buffer[10] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Test different invalid Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidStampTablesSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidStampConstructorTable[u32_I](&pt_Stamp);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_SerDes_serialiseStamp(
                pt_Stamp,
                &ru8_Buffer[0],
                &u32_BufferSize),
            ITC_STATUS_CORRUPT_STAMP);

        /* Destroy the Stamp */
        gpv_InvalidStampDestructorTable[u32_I](&pt_Stamp);
    }
}

/* Test serialising a Stamp with leaf ID and Event nodes succeeds */
void ITC_SerDes_Test_serialiseStampWithLeafComponentsSuccessful(void)
{
    ITC_Stamp_t *pt_Stamp = NULL;
    uint8_t ru8_Buffer[10] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    uint8_t ru8_ExpectedStampSerialisedData[] = {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_STAMP_HEADER(1, 1),
        1,
        ITC_SERDES_SEED_ID_HEADER,
        1,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
    };

    /* Create a new Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Serialise the Stamp */
    TEST_SUCCESS(
        ITC_SerDes_serialiseStamp(pt_Stamp, &ru8_Buffer[0], &u32_BufferSize));

    /* Test the serialised data is what is expected */
    TEST_ASSERT_EQUAL(sizeof(ru8_ExpectedStampSerialisedData), u32_BufferSize);
    TEST_ASSERT_EQUAL_MEMORY(
        &ru8_ExpectedStampSerialisedData[0],
        &ru8_Buffer[0],
        sizeof(ru8_ExpectedStampSerialisedData));

    /* Destroy the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
}

/* Test serialising a leaf Stamp fails with insufficent resources */
void ITC_SerDes_Test_serialiseStampFailWithInsufficentResources(void)
{
    ITC_Stamp_t *pt_Stamp = NULL;
    uint8_t ru8_Buffer[7] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Create a new Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* Make the event a parent */
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Stamp->pt_Event->pt_Left, pt_Stamp->pt_Event, 1));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Stamp->pt_Event->pt_Right, pt_Stamp->pt_Event, 0));

    /* Test the Event does not fit */
    TEST_FAILURE(
        ITC_SerDes_serialiseStamp(
            pt_Stamp,
            &ru8_Buffer[0],
            &u32_BufferSize),
        ITC_STATUS_INSUFFICIENT_RESOURCES);

    /* Test the ID and Event do not fit */
    u32_BufferSize = 4;
    TEST_FAILURE(
        ITC_SerDes_serialiseStamp(
            pt_Stamp,
            &ru8_Buffer[0],
            &u32_BufferSize),
        ITC_STATUS_INSUFFICIENT_RESOURCES);

    u32_BufferSize = 0;

    /* Serialise the Stamp */
    TEST_FAILURE(
        ITC_SerDes_serialiseStamp(
            pt_Stamp,
            &ru8_Buffer[0],
            &u32_BufferSize),
        ITC_STATUS_INVALID_PARAM);

    /* Destroy the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
}

/* Test serialising a Stamp with parent ID and Event nodes succeeds */
void ITC_SerDes_Test_serialiseStampWithParentComponentsSuccessful(void)
{
    ITC_Stamp_t *pt_Stamp = NULL;
    uint8_t ru8_Buffer[12] = { 0 };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    uint8_t ru8_ExpectedStampSerialisedData[] = {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_STAMP_HEADER(1, 1),
        3,
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
        ITC_SERDES_NULL_ID_HEADER,
        5,
        ITC_SERDES_CREATE_EVENT_HEADER(true, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 2),
        1,
        0,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
    };

    /* Create a new Stamp */
    TEST_SUCCESS(ITC_Stamp_newSeed(&pt_Stamp));

    /* clang-format off */
    /* Add nodes to the ID component */
    pt_Stamp->pt_Id->b_IsOwner = false;
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Stamp->pt_Id->pt_Left, pt_Stamp->pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Stamp->pt_Id->pt_Right, pt_Stamp->pt_Id));

    /* Add nodes to the Event component */
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Stamp->pt_Event->pt_Left, pt_Stamp->pt_Event, 256));
    TEST_SUCCESS(ITC_TestUtil_newEvent(&pt_Stamp->pt_Event->pt_Right, pt_Stamp->pt_Event, 0));
    /* clang-format on */

    /* Serialise the Stamp */
    TEST_SUCCESS(
        ITC_SerDes_serialiseStamp(pt_Stamp, &ru8_Buffer[0], &u32_BufferSize));

    /* Test the serialised data is what is expected */
    TEST_ASSERT_EQUAL(sizeof(ru8_ExpectedStampSerialisedData), u32_BufferSize);
    TEST_ASSERT_EQUAL_MEMORY(
        &ru8_ExpectedStampSerialisedData[0],
        &ru8_Buffer[0],
        sizeof(ru8_ExpectedStampSerialisedData));

    /* Destroy the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
}

/* Test deserialising a Stamp fails with invalid param */
void ITC_SerDes_Test_deserialiseStampFailInvalidParam(void)
{
    ITC_Stamp_t *pt_Dummy = NULL;
    uint8_t ru8_Buffer[10] = { 0 };

    TEST_FAILURE(
        ITC_SerDes_deserialiseStamp(
            &ru8_Buffer[0],
            sizeof(ru8_Buffer),
            NULL),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_deserialiseStamp(
            NULL,
            sizeof(ru8_Buffer),
            &pt_Dummy),
        ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_SerDes_deserialiseStamp(
            &ru8_Buffer[0],
            0,
            &pt_Dummy),
        ITC_STATUS_INVALID_PARAM);
}

/* Test deserialising an Stamp fails with corrupt Stamp */
void ITC_SerDes_Test_deserialiseStampFailWithCorruptStamp(void)
{
    ITC_Stamp_t *pt_Stamp;
    const uint8_t *pu8_Buffer = NULL;
    uint32_t u32_BufferSize = 0;

    /* Test different invalid serialised Stamps are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidSerialisedStampTableSize;
         u32_I++)
    {
        /* Construct an invalid Stamp */
        gpv_InvalidSerialisedStampConstructorTable[u32_I](
            &pu8_Buffer, &u32_BufferSize);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_SerDes_deserialiseStamp(
                pu8_Buffer,
                u32_BufferSize,
                &pt_Stamp),
            ITC_STATUS_CORRUPT_STAMP);
    }
}

/* Test deserialising a Stamp fails with unsupported ID component length length
 * size */
void ITC_SerDes_Test_deserialiseStampFailWithUnsupportedIdLengthLengthSize(void)
{
    ITC_Stamp_t *pt_Stamp;
    uint8_t ru8_Buffer[] = {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_STAMP_HEADER(sizeof(uint32_t) + 1, 1),
        1,
        1,
        1,
        1,
        1,
        ITC_SERDES_SEED_ID_HEADER,
        1,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0)
    };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Test for the failure */
    TEST_FAILURE(
        ITC_SerDes_deserialiseStamp(
            &ru8_Buffer[0],
            u32_BufferSize,
            &pt_Stamp),
        /* There is no special exception as a length length size of more than
         * the size of sizeof(uint32_t) is most likely user error */
        ITC_STATUS_INVALID_PARAM);
}

/* Test deserialising a Stamp fails with unsupported Event component length length
 * size */
void ITC_SerDes_Test_deserialiseStampFailWithUnsupportedEventLengthLengthSize(void)
{
    ITC_Stamp_t *pt_Stamp;
    uint8_t ru8_Buffer[] = {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_STAMP_HEADER(1, sizeof(uint32_t) + 1),
        1,
        ITC_SERDES_SEED_ID_HEADER,
        1,
        1,
        1,
        1,
        1,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0)
    };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Test for the failure */
    TEST_FAILURE(
        ITC_SerDes_deserialiseStamp(
            &ru8_Buffer[0],
            u32_BufferSize,
            &pt_Stamp),
        /* There is no special exception as a length length size of more than
         * the size of sizeof(uint32_t) is most likely user error */
        ITC_STATUS_INVALID_PARAM);
}

/* Test deserialising a Stamp with incompatible lib version */
void ITC_SerDes_Test_deserialiseStampFailWithIncompatibleLibVersion(void)
{
    ITC_Stamp_t *pt_Stamp;
    uint8_t ru8_Buffer[] = {
        ITC_VERSION_MAJOR + 1, /* Provided by build system c args */
        ITC_SERDES_CREATE_STAMP_HEADER(1, 1),
        1,
        ITC_SERDES_SEED_ID_HEADER,
        1,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0)
    };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Test for the failure */
    TEST_FAILURE(
        ITC_SerDes_deserialiseStamp(
            &ru8_Buffer[0],
            u32_BufferSize,
            &pt_Stamp),
        ITC_STATUS_SERDES_INCOMPATIBLE_LIB_VERSION);
}

/* Test deserialising a stamp with leaf ID and Event components suceeds */
void ITC_SerDes_Test_deserialiseLeafComponentsStampSuccessful(void)
{
    ITC_Stamp_t *pt_Stamp;
    uint8_t ru8_Buffer[] = {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_STAMP_HEADER(1, 1),
        1,
        ITC_SERDES_SEED_ID_HEADER,
        1,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
    };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Test deserialising a leaf Stamp */
    TEST_SUCCESS(
        ITC_SerDes_deserialiseStamp(&ru8_Buffer[0], u32_BufferSize, &pt_Stamp));

    /* Test this is a Stamp with a leaf seed ID and leaf 0 Event counter */
    TEST_ITC_ID_IS_SEED_ID(pt_Stamp->pt_Id);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp->pt_Event, 0);

    /* Destroy the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
}

/* Test deserialising a parent Stamp suceeds */
void ITC_SerDes_Test_deserialiseParentStampSuccessful(void)
{
    ITC_Stamp_t *pt_Stamp;
    /* Serialised stamp with:
     * - (0, ((1, 0), 1)) ID
     * - (0, 1, (0, (4242, 0, 123123123), 0)) Event */
    uint8_t ru8_Buffer[] = {
        ITC_VERSION_MAJOR, /* Provided by build system c args */
        ITC_SERDES_CREATE_STAMP_HEADER(1, 1),
        7,
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_NULL_ID_HEADER,
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_PARENT_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
        ITC_SERDES_NULL_ID_HEADER,
        ITC_SERDES_SEED_ID_HEADER,
        14,
        ITC_SERDES_CREATE_EVENT_HEADER(true, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 1),
        1,
        ITC_SERDES_CREATE_EVENT_HEADER(true, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(true, 2),
        (4242U >> 8U) & 0xFFU,
        4242U & 0xFFU,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
        ITC_SERDES_CREATE_EVENT_HEADER(false, 4),
        (123123123U >> 24U) & 0xFFU,
        (123123123U >> 16U) & 0xFFU,
        (123123123U >> 8U) & 0xFFU,
        123123123U & 0xFFU,
        ITC_SERDES_CREATE_EVENT_HEADER(false, 0),
    };
    uint32_t u32_BufferSize = sizeof(ru8_Buffer);

    /* Test deserialising the Stamp */
    TEST_SUCCESS(
        ITC_SerDes_deserialiseStamp(&ru8_Buffer[0], u32_BufferSize, &pt_Stamp));

    /* clang-format off */
    /* Test this is a (0, ((1, 0), 1)) ID */
    TEST_ITC_ID_IS_NULL_ID(pt_Stamp->pt_Id->pt_Left);
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_Stamp->pt_Id->pt_Right->pt_Left);
    TEST_ITC_ID_IS_SEED_ID(pt_Stamp->pt_Id->pt_Right->pt_Right);
    /* Test this is a (0, 1, (0, (4242, 0, 123123123), 0)) Event */
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_Stamp->pt_Event, 0);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp->pt_Event->pt_Left, 1);
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_Stamp->pt_Event->pt_Right, 0);
    TEST_ITC_EVENT_IS_PARENT_N_EVENT(pt_Stamp->pt_Event->pt_Right->pt_Left, 4242);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp->pt_Event->pt_Right->pt_Left->pt_Left, 0);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp->pt_Event->pt_Right->pt_Left->pt_Right, 123123123);
    TEST_ITC_EVENT_IS_LEAF_N_EVENT(pt_Stamp->pt_Event->pt_Right->pt_Right, 0);
    /* clang-format on */

    /* Destroy the Stamp */
    TEST_SUCCESS(ITC_Stamp_destroy(&pt_Stamp));
}