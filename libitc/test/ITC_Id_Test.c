#include "ITC_Id.h"
#include "ITC_Id_package.h"
#include "ITC_Id_Test.h"

#include "ITC_Test_package.h"

/******************************************************************************
 *  Public functions
 ******************************************************************************/

/* Init test */
void setUp(void) {}

/* Fini test */
void tearDown(void) {}

/* Test destroying an ID fails with invalid param */
void ITC_Id_Test_destroyIdFailInvalidParam(void)
{
    TEST_FAILURE(ITC_Id_destroy(NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test destroying an ID suceeds */
void ITC_Id_Test_destroyIdSuccessful(void)
{
    ITC_Id_t *pt_Dummy = NULL;

    TEST_SUCCESS(ITC_Id_destroy(&pt_Dummy));

    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Dummy, NULL));
    TEST_SUCCESS(ITC_Id_destroy(&pt_Dummy));
}

/* Test creating a NULL ID fails with invalid param */
void ITC_Id_Test_createNullIdFailInvalidParam(void)
{
    TEST_FAILURE(ITC_Id_newNull(NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test creating a NULL ID succeeds */
void ITC_Id_Test_createNullIdSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* Create a new NULL ID */
    TEST_SUCCESS(ITC_Id_newNull(&pt_Id));

    /* Test this is a NULL ID */
    TEST_ASSERT_FALSE(pt_Id->pt_Parent);
    TEST_ITC_ID_IS_NULL_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test creating a seed ID fails with invalid param */
void ITC_Id_Test_createSeedIdFailInvalidParam(void)
{
    TEST_FAILURE(ITC_Id_newSeed(NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test creating a Seed ID succeeds */
void ITC_Id_Test_createSeedIdSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* Create a new seed ID */
    TEST_SUCCESS(ITC_Id_newSeed(&pt_Id));

    /* Test this is a seed ID */
    TEST_ASSERT_FALSE(pt_Id->pt_Parent);
    TEST_ITC_ID_IS_SEED_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test cloning an ID fails with invalid param */
void ITC_Id_Test_cloneIdFailInvalidParam(void)
{
  ITC_Id_t *pt_DummyId = NULL;

  TEST_FAILURE(ITC_Id_clone(NULL, &pt_DummyId), ITC_STATUS_INVALID_PARAM);
  TEST_FAILURE(ITC_Id_clone(pt_DummyId, NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test cloning an ID fails with corrupt ID */
void ITC_Id_Test_cloneIdFailWithCorruptId(void)
{
    ITC_Id_t *pt_Id;
    ITC_Id_t *pt_ClonedId;

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Id);

        /* Test for the failure */
        TEST_FAILURE(ITC_Id_clone(pt_Id, &pt_ClonedId), ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Id);
    }
}

/* Test cloning an ID succeeds */
void ITC_Id_Test_cloneIdSuccessful(void)
{
    ITC_Id_t *pt_OriginalId = NULL;
    ITC_Id_t *pt_ClonedId = NULL;

    /* Test cloning seed ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId, NULL));
    TEST_SUCCESS(ITC_Id_clone(pt_OriginalId, &pt_ClonedId));
    TEST_ASSERT_TRUE(pt_OriginalId != pt_ClonedId);
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));

    TEST_ASSERT_FALSE(pt_ClonedId->pt_Parent);
    TEST_ITC_ID_IS_SEED_ID(pt_ClonedId);
    TEST_SUCCESS(ITC_Id_destroy(&pt_ClonedId));

    /* Test cloning null ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId, NULL));
    TEST_SUCCESS(ITC_Id_clone(pt_OriginalId, &pt_ClonedId));
    TEST_ASSERT_TRUE(pt_OriginalId != pt_ClonedId);
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));

    TEST_ASSERT_FALSE(pt_ClonedId->pt_Parent);
    TEST_ITC_ID_IS_NULL_ID(pt_ClonedId);
    TEST_SUCCESS(ITC_Id_destroy(&pt_ClonedId));

    /* clang-format off */
    /* Test cloning a complex ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Left, pt_OriginalId));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId->pt_Right, pt_OriginalId));
    TEST_SUCCESS(ITC_Id_clone(pt_OriginalId, &pt_ClonedId));
    TEST_ASSERT_TRUE(pt_OriginalId != pt_ClonedId);
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_ClonedId);
    TEST_ASSERT_TRUE(pt_OriginalId->pt_Left != pt_ClonedId->pt_Left);
    TEST_ASSERT_TRUE(pt_OriginalId->pt_Right != pt_ClonedId->pt_Right);
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));
    /* clang-format on */

    TEST_ASSERT_FALSE(pt_ClonedId->pt_Parent);
    TEST_ITC_ID_IS_NULL_ID(pt_ClonedId->pt_Left);
    TEST_ASSERT_TRUE(pt_ClonedId->pt_Left->pt_Parent == pt_ClonedId);
    TEST_ITC_ID_IS_SEED_ID(pt_ClonedId->pt_Right);
    TEST_ASSERT_TRUE(pt_ClonedId->pt_Right->pt_Parent == pt_ClonedId);
    TEST_SUCCESS(ITC_Id_destroy(&pt_ClonedId));
}

/* Test cloning a subtree of an ID succeeds */
void ITC_Id_Test_cloneIdSubtreeSuccessful(void)
{
    ITC_Id_t *pt_OriginalId = NULL;
    ITC_Id_t *pt_ClonedId = NULL;

    /* clang-format off */
    /* Test cloning seed subree ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId, NULL));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId->pt_Left, pt_OriginalId));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Right, pt_OriginalId));
    TEST_SUCCESS(ITC_Id_clone(pt_OriginalId->pt_Left, &pt_ClonedId));
    TEST_ASSERT_TRUE(pt_OriginalId->pt_Left != pt_ClonedId);
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));
    /* clang-format on */

    TEST_ASSERT_FALSE(pt_ClonedId->pt_Parent);
    TEST_ITC_ID_IS_SEED_ID(pt_ClonedId);
    TEST_SUCCESS(ITC_Id_destroy(&pt_ClonedId));

    /* clang-format off */
    /* Test cloning null subree ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId, NULL));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId->pt_Left, pt_OriginalId));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Right, pt_OriginalId));
    TEST_SUCCESS(ITC_Id_clone(pt_OriginalId->pt_Right, &pt_ClonedId));
    TEST_ASSERT_TRUE(pt_OriginalId->pt_Right != pt_ClonedId);
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));
    /* clang-format on */

    TEST_ASSERT_FALSE(pt_ClonedId->pt_Parent);
    TEST_ITC_ID_IS_NULL_ID(pt_ClonedId);
    TEST_SUCCESS(ITC_Id_destroy(&pt_ClonedId));

    /* clang-format off */
    /* Test cloning a complex ID subree */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Left, pt_OriginalId));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Left->pt_Left, pt_OriginalId->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId->pt_Left->pt_Right, pt_OriginalId->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Right, pt_OriginalId));
    TEST_SUCCESS(ITC_Id_clone(pt_OriginalId->pt_Left, &pt_ClonedId));
    TEST_ASSERT_TRUE(pt_OriginalId->pt_Left != pt_ClonedId);
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_ClonedId);
    TEST_ASSERT_TRUE(pt_OriginalId->pt_Left->pt_Left != pt_ClonedId->pt_Left);
    TEST_ASSERT_TRUE(pt_OriginalId->pt_Left->pt_Right != pt_ClonedId->pt_Right);
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));
    /* clang-format on */

    TEST_ASSERT_FALSE(pt_ClonedId->pt_Parent);
    TEST_ITC_ID_IS_NULL_ID(pt_ClonedId->pt_Left);
    TEST_ASSERT_TRUE(pt_ClonedId->pt_Left->pt_Parent == pt_ClonedId);
    TEST_ITC_ID_IS_SEED_ID(pt_ClonedId->pt_Right);
    TEST_ASSERT_TRUE(pt_ClonedId->pt_Right->pt_Parent == pt_ClonedId);
    TEST_SUCCESS(ITC_Id_destroy(&pt_ClonedId));
}

/* Test spliting an ID fails with invalid param */
void ITC_Id_Test_splitIdFailInvalidParam(void)
{
  ITC_Id_t *pt_DummyId = NULL;

  TEST_FAILURE(
    ITC_Id_split(NULL, &pt_DummyId, &pt_DummyId), ITC_STATUS_INVALID_PARAM);
  TEST_FAILURE(
    ITC_Id_split(pt_DummyId, NULL, &pt_DummyId), ITC_STATUS_INVALID_PARAM);
  TEST_FAILURE(
    ITC_Id_split(pt_DummyId, &pt_DummyId, NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test splitting an ID fails with corrupt ID */
void ITC_Id_Test_splitIdFailWithCorruptId(void)
{
    ITC_Id_t *pt_Id;
    ITC_Id_t *pt_SplitId1;
    ITC_Id_t *pt_SplitId2;

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Id);

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Id_split(pt_Id, &pt_SplitId1, &pt_SplitId2),
            ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Id);
    }
}

/* Test splitting a NULL and seed IDs succeeds */
void ITC_Id_Test_splitNullAndSeedIdsSuccessful(void)
{
    ITC_Id_t *pt_OriginalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* Create a new NULL ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId, NULL));

    /* Split the NULL ID */
    TEST_SUCCESS(ITC_Id_split(pt_OriginalId, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match (0, 0) */
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId1);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId2);

    /* Test the original is still a NULL ID */
    TEST_ITC_ID_IS_NULL_ID(pt_OriginalId);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));

    /* Change the ID into a seed ID */
    pt_OriginalId->b_IsOwner = true;

    /* Split the seed ID */
    TEST_SUCCESS(ITC_Id_split(pt_OriginalId, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match ((1, 0), (0, 1)) */
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_SplitId1);
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_SplitId2);

    /* Test the original is still a seed ID */
    TEST_ITC_ID_IS_SEED_ID(pt_OriginalId);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}

/* Test splitting a NULL and seed ID subtrees succeeds */
void ITC_Id_Test_splitNullAndSeedIdSubtreesSuccessful(void)
{
    ITC_Id_t *pt_OriginalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* clang-format off */
    /* Create a new NULL ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Left, pt_OriginalId));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId->pt_Right, pt_OriginalId));
    /* clang-format on */

    /* Split the NULL ID */
    TEST_SUCCESS(ITC_Id_split(
        pt_OriginalId->pt_Left, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match (0, 0) */
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId1);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId2);

    /* Test the original is still a NULL ID */
    TEST_ITC_ID_IS_NULL_ID(pt_OriginalId->pt_Left);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));

    /* Split the seed ID */
    TEST_SUCCESS(ITC_Id_split(
        pt_OriginalId->pt_Right, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match ((1, 0), (0, 1)) */
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_SplitId1);
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_SplitId2);

    /* Test the original is still a seed ID */
    TEST_ITC_ID_IS_SEED_ID(pt_OriginalId->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}

/* Test splitting a (0, 1) and (1, 0) ID succeeds */
void ITC_Id_Test_split01And10IdsSuccessful(void)
{
    ITC_Id_t *pt_OriginalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* clang-format off */
    /* Create a new (0, 1) ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Left, pt_OriginalId));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId->pt_Right, pt_OriginalId));
    /* clang-format on */

    /* Split the (0, 1) ID */
    TEST_SUCCESS(ITC_Id_split(pt_OriginalId, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match ((0, (1, 0)), (0, (0, 1))) */
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId1);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId1->pt_Left);
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_SplitId1->pt_Right);
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId2);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId2->pt_Left);
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_SplitId2->pt_Right);

    /* Test the original is still a (0, 1) ID */
    TEST_ITC_ID_IS_NULL_ID(pt_OriginalId->pt_Left);
    TEST_ITC_ID_IS_SEED_ID(pt_OriginalId->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));

    /* Change the ID into (1, 0) ID */
    pt_OriginalId->pt_Left->b_IsOwner = true;
    pt_OriginalId->pt_Right->b_IsOwner = false;

    /* Split the (1, 0) ID */
    TEST_SUCCESS(ITC_Id_split(pt_OriginalId, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match (((1, 0), 0), ((0, 1), 0)) */
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId1);
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_SplitId1->pt_Left);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId1->pt_Right);
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId2);
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_SplitId2->pt_Left);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId2->pt_Right);

    /* Test the original is still a (1, 0) ID */
    TEST_ITC_ID_IS_SEED_ID(pt_OriginalId->pt_Left);
    TEST_ITC_ID_IS_NULL_ID(pt_OriginalId->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}

/* Test splitting a (0, 1) and (1, 0) ID subtrees succeeds */
void ITC_Id_Test_split01And10IdSubtreesSuccessful(void)
{
    ITC_Id_t *pt_OriginalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* clang-format off */
    /* Create a new (0, 1) ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Left, pt_OriginalId));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Right, pt_OriginalId));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Right->pt_Left, pt_OriginalId->pt_Right));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId->pt_Right->pt_Right, pt_OriginalId->pt_Right));
    /* clang-format on */

    /* Split the (0, 1) ID */
    TEST_SUCCESS(
        ITC_Id_split(pt_OriginalId->pt_Right, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match ((0, (1, 0)), (0, (0, 1))) */
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId1);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId1->pt_Left);
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_SplitId1->pt_Right);
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId2);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId2->pt_Left);
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_SplitId2->pt_Right);

    /* Test the original is still a (0, 1) ID */
    TEST_ITC_ID_IS_NULL_ID(pt_OriginalId->pt_Right->pt_Left);
    TEST_ITC_ID_IS_SEED_ID(pt_OriginalId->pt_Right->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));

    /* Change the ID into (1, 0) ID */
    pt_OriginalId->pt_Right->pt_Left->b_IsOwner = true;
    pt_OriginalId->pt_Right->pt_Right->b_IsOwner = false;

    /* Split the (1, 0) ID */
    TEST_SUCCESS(
        ITC_Id_split(pt_OriginalId->pt_Right, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match (((1, 0), 0), ((0, 1), 0)) */
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId1);
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_SplitId1->pt_Left);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId1->pt_Right);
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId2);
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_SplitId2->pt_Left);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId2->pt_Right);

    /* Test the original is still a (1, 0) ID */
    TEST_ITC_ID_IS_SEED_ID(pt_OriginalId->pt_Right->pt_Left);
    TEST_ITC_ID_IS_NULL_ID(pt_OriginalId->pt_Right->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}

/* Test splitting a (0, (1, 0)) ID succeeds */
void ITC_Id_Test_split010RIdSuccessful(void)
{
    ITC_Id_t *pt_OriginalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* clang-format off */
    /* Create a new (0, (1, 0)) ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId, NULL));

    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Left, pt_OriginalId));

    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Right, pt_OriginalId));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId->pt_Right->pt_Left, pt_OriginalId->pt_Right));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Right->pt_Right, pt_OriginalId->pt_Right));
    /* clang-format on */

    /* Split the (0, (1, 0)) ID */
    TEST_SUCCESS(ITC_Id_split(pt_OriginalId, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match ((0, ((1, 0), 0)), (0, ((0, 1), 0))) */
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId1);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId1->pt_Left);
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId1->pt_Right);
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_SplitId1->pt_Right->pt_Left);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId1->pt_Right->pt_Right);

    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId2);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId2->pt_Left);
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId2->pt_Right);
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_SplitId2->pt_Right->pt_Left);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId2->pt_Right->pt_Right);

    /* Test the original is still a (0, (1, 0)) ID */
    TEST_ITC_ID_IS_NULL_ID(pt_OriginalId->pt_Left);
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_OriginalId->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}

/* Test splitting a ((0, 1), 0) ID succeeds */
void ITC_Id_Test_split010LIdSuccessful(void)
{
    ITC_Id_t *pt_OriginalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* clang-format off */
    /* Create a new ((0, 1), 0) ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId, NULL));

    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Left, pt_OriginalId));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Left->pt_Left, pt_OriginalId->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId->pt_Left->pt_Right, pt_OriginalId->pt_Left));

    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Right, pt_OriginalId));
    /* clang-format on */

    /* Split the ((0, 1), 0) ID */
    TEST_SUCCESS(ITC_Id_split(pt_OriginalId, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match (((0, (1, 0)), 0), ((0, (0, 1)), 0)) */
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId1);
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId1->pt_Left);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId1->pt_Left->pt_Left);
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_SplitId1->pt_Left->pt_Right);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId1->pt_Right);

    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId2);
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId2->pt_Left);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId2->pt_Left->pt_Left);
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_SplitId2->pt_Left->pt_Right);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId2->pt_Right);

    /* Test the original is still a ((0, 1), 0) ID */
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_OriginalId->pt_Left);
    TEST_ITC_ID_IS_NULL_ID(pt_OriginalId->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}

/* Test splitting a ((1, 0), (0, 1)) ID succeeds */
void ITC_Id_Test_split1001IdSuccessful(void)
{
    ITC_Id_t *pt_OriginalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* clang-format off */
    /* Create a new ((1, 0), (0, 1)) ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId, NULL));

    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Left, pt_OriginalId));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId->pt_Left->pt_Left, pt_OriginalId->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Left->pt_Right, pt_OriginalId->pt_Left));

    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Right, pt_OriginalId));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Right->pt_Left, pt_OriginalId->pt_Right));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId->pt_Right->pt_Right, pt_OriginalId->pt_Right));
    /* clang-format on */

    /* Split the ((1, 0), (0, 1)) ID */
    TEST_SUCCESS(ITC_Id_split(pt_OriginalId, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match (((1, 0), 0), (0, (0, 1))) */
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId1);
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_SplitId1->pt_Left);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId1->pt_Right);
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId2);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId2->pt_Left);
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_SplitId2->pt_Right);

    /* Test the original is still a ((1, 0), (0, 1)) ID */
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_OriginalId->pt_Left);
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_OriginalId->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}

/* Test splitting a ((1, 0), (0, 1)) ID subtree succeeds */
void ITC_Id_Test_split1001IdSubtreeSuccessful(void)
{
    ITC_Id_t *pt_OriginalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* clang-format off */
    /* Create a new ((1, 0), (0, 1)) ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Left, pt_OriginalId));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Right, pt_OriginalId));

    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Left->pt_Left, pt_OriginalId->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId->pt_Left->pt_Left->pt_Left, pt_OriginalId->pt_Left->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Left->pt_Left->pt_Right, pt_OriginalId->pt_Left->pt_Left));

    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Left->pt_Right, pt_OriginalId->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Left->pt_Right->pt_Left, pt_OriginalId->pt_Left->pt_Right));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId->pt_Left->pt_Right->pt_Right, pt_OriginalId->pt_Left->pt_Right));
    /* clang-format on */

    /* Split the ((1, 0), (0, 1)) ID */
    TEST_SUCCESS(
        ITC_Id_split(pt_OriginalId->pt_Left, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match (((1, 0), 0), (0, (0, 1))) */
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId1);
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_SplitId1->pt_Left);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId1->pt_Right);
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId2);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId2->pt_Left);
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_SplitId2->pt_Right);

    /* Test the original is still a ((1, 0), (0, 1)) ID */
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_OriginalId->pt_Left->pt_Left);
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_OriginalId->pt_Left->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}

/* Test splitting a ((0, (1, 0)), ((0, 1), 0)) ID succeeds */
void ITC_Id_Test_split010010IdSuccessful(void)
{
    ITC_Id_t *pt_OriginalId;
    ITC_Id_t *pt_SplitId1 = NULL;
    ITC_Id_t *pt_SplitId2 = NULL;

    /* clang-format off */
    /* Create a new ((0, (1, 0)), ((0, 1), 0)) ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId, NULL));

    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Left, pt_OriginalId));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Left->pt_Left, pt_OriginalId->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Left->pt_Right, pt_OriginalId->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId->pt_Left->pt_Right->pt_Left, pt_OriginalId->pt_Left->pt_Right));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Left->pt_Right->pt_Right, pt_OriginalId->pt_Left->pt_Right));

    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Right, pt_OriginalId));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Right->pt_Left, pt_OriginalId->pt_Right));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Right->pt_Left->pt_Left, pt_OriginalId->pt_Right->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId->pt_Right->pt_Left->pt_Right, pt_OriginalId->pt_Right->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId->pt_Right->pt_Right, pt_OriginalId->pt_Right));
    /* clang-format on */

    /* Split the ((0, (1, 0)), ((0, 1), 0)) ID */
    TEST_SUCCESS(ITC_Id_split(pt_OriginalId, &pt_SplitId1, &pt_SplitId2));

    /* Test the new IDs match (((0, (1, 0)), 0), (0, ((0, 1), 0))) */
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId1);
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId1->pt_Left);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId1->pt_Left->pt_Left);
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_SplitId1->pt_Left->pt_Right);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId1->pt_Right);

    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId2);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId2->pt_Left);
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_SplitId2->pt_Right);
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_SplitId2->pt_Right->pt_Left);
    TEST_ITC_ID_IS_NULL_ID(pt_SplitId2->pt_Right->pt_Right);

    /* Test the original is still a ((0, (1, 0)), ((0, 1), 0)) ID */
    TEST_ITC_ID_IS_NULL_ID(pt_OriginalId->pt_Left->pt_Left);
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_OriginalId->pt_Left->pt_Right);
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_OriginalId->pt_Right->pt_Left);
    TEST_ITC_ID_IS_NULL_ID(pt_OriginalId->pt_Right->pt_Right);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));
}

/* Test validating an ID fails with invalid param */
void ITC_Id_Test_validateIdFailInvalidParam(void)
{
    TEST_FAILURE(ITC_Id_validate(NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test validating an ID fails with corrupt ID */
void ITC_Id_Test_validatingIdFailWithCorruptId(void)
{
    ITC_Id_t *pt_Id;

    /* Test different invalid IDs are handled properly.
     * Only test invalid IDs that are not related to normalisation */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Id);

        /* Test for the failure */
        TEST_FAILURE(ITC_Id_validate(pt_Id), ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Id);
    }
}

/* Test validating an ID succeeds */
void ITC_Id_Test_validateIdSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* Create a new ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));
    /* Validate the ID */
    TEST_SUCCESS(ITC_Id_validate(pt_Id));
    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test normalising an ID fails with invalid param */
void ITC_Id_Test_normaliseIdFailInvalidParam(void)
{
    TEST_FAILURE(ITC_Id_normalise(NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test normalising an ID fails with corrupt ID */
void ITC_Id_Test_normaliseIdFailWithCorruptId(void)
{
    ITC_Id_t *pt_Id;

    /* Test different invalid IDs are handled properly.
     * Only test invalid IDs that are not related to normalisation */
    for (uint32_t u32_I = 0;
         u32_I < FIRST_NORMALISATION_RELATED_INVALID_ID_INDEX;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Id);

        /* Test for the failure */
        TEST_FAILURE(ITC_Id_normalise(pt_Id), ITC_STATUS_CORRUPT_ID);

        /* Destroy the ID */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Id);
    }
}

/* Test normalising NULL and seed IDs succeeds */
void ITC_Id_Test_normaliseNullAndSeedIdsSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* Create a new NULL ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test this is still a NULL ID */
    TEST_ITC_ID_IS_NULL_ID(pt_Id);

    /* Change ID into a seed ID */
    pt_Id->b_IsOwner = true;

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test this is still a seed ID */
    TEST_ITC_ID_IS_SEED_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test normalising NULL and seed ID subtrees succeeds */
void ITC_Id_Test_normaliseNullAndSeedIdSubtreesSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* Create a new NULL and seed ID subtrees */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right, pt_Id));

    /* Normalise the NULL ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id->pt_Right));

    /* Test the whole ID hasn't changed */
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_Id);

    /* Normalise the seed ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id->pt_Left));

    /* Test the whole ID hasn't changed */
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test normalising a (1, 0) and (0, 1) IDs succeeds */
void ITC_Id_Test_normalise10And01IdsSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* Create a new (1, 0) ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right, pt_Id));

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test this is still a (1, 0) ID */
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_Id);

    /* Switch the (1, 0) ID into a (0, 1) ID */
    pt_Id->pt_Left->b_IsOwner = false;
    pt_Id->pt_Right->b_IsOwner = true;

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test this is still a (1, 0) ID */
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test normalising a (1, 0) and (0, 1) ID subtrees succeeds */
void ITC_Id_Test_normalise10And01IdSubtreesSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* clang-format off */
    /* Create a new (1, 0) ID subree */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Left->pt_Left, pt_Id->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Left->pt_Right, pt_Id->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right, pt_Id));
    /* clang-format on */

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id->pt_Left));

    /* Test the whole ID hasn't changed */
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_Id);
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_Id->pt_Left);
    TEST_ITC_ID_IS_NULL_ID(pt_Id->pt_Right);

    /* Switch the (1, 0) ID into a (0, 1) ID */
    pt_Id->pt_Left->pt_Left->b_IsOwner = false;
    pt_Id->pt_Left->pt_Right->b_IsOwner = true;

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id->pt_Left));

    /* Test the whole ID hasn't changed */
    TEST_ITC_ID_IS_NOT_LEAF_ID(pt_Id);
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_Id->pt_Left);
    TEST_ITC_ID_IS_NULL_ID(pt_Id->pt_Right);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test normalising a (1, 1) and (0, 0) IDs succeeds */
void ITC_Id_Test_normalise11And00IdSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* Create a new (1, 1) ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Right, pt_Id));

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a seed ID */
    TEST_ITC_ID_IS_SEED_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));

    /* Create a new (0, 0) ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right, pt_Id));

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a NULL ID */
    TEST_ITC_ID_IS_NULL_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test normalising a (1, 1) and (0, 0) ID subtrees succeeds */
void ITC_Id_Test_normalise11And00IdSubtreesSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* clang-format off */
    /* Create a new (1, 1) ID subtree (whole tree (1, (1, 1)) ) */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Right->pt_Left, pt_Id->pt_Right));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Right->pt_Right, pt_Id->pt_Right));
    /* clang-format on */

    /* Normalise the ID subtree */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id->pt_Right));

    /* Test the targeted subtree has been normalised but the rest of the tree
     * is untouched */
    TEST_ITC_ID_IS_SEED_SEED_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));

    /* clang-format off */
    /* Create a new (0, 0) ID subtree (whole tree ((0, 0), 0) ) */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Left->pt_Left, pt_Id->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Left->pt_Right, pt_Id->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right, pt_Id));
    /* clang-format on */

    /* Normalise the ID subtree */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id->pt_Left));

    /* Test the targeted subtree has been normalised but the rest of the tree
     * is untouched */
    TEST_ITC_ID_IS_NULL_NULL_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test normalising a (0, (1, 1)) and ((1, 1), 0) and IDs succeeds */
void ITC_Id_Test_normalise011And110IdSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* clang-format off */
    /* Create a new (0, (1, 1)) ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Right->pt_Left, pt_Id->pt_Right));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Right->pt_Right, pt_Id->pt_Right));
    /* clang-format on */

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a (0, 1) ID */
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));

    /* clang-format off */
    /* Create a new ((1, 1), 0) ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Left->pt_Left, pt_Id->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Left->pt_Right, pt_Id->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right, pt_Id));
    /* clang-format on */

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a (1, 0) ID */
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test normalising a (1, (1, 1)) and ((1, 1), 1) and IDs succeeds */
void ITC_Id_Test_normalise111And111IdSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* clang-format off */
    /* Create a new (1, (1, 1)) ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Right->pt_Left, pt_Id->pt_Right));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Right->pt_Right, pt_Id->pt_Right));
    /* clang-format on */

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a seed ID */
    TEST_ITC_ID_IS_SEED_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));

    /* clang-format off */
    /* Create a new ((1, 1), 1) ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Left->pt_Left, pt_Id->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Left->pt_Right, pt_Id->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Right, pt_Id));
    /* clang-format on */

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a seed ID */
    TEST_ITC_ID_IS_SEED_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test normalising a (1, (0, 0)) and ((0, 0), 1) and IDs succeeds */
void ITC_Id_Test_normalise100And001IdSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* clang-format off */
    /* Create a new (1, (0, 0)) ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right->pt_Left, pt_Id->pt_Right));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right->pt_Right, pt_Id->pt_Right));
    /* clang-format on */

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a (1, 0) ID */
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));

    /* clang-format off */
    /* Create a new ((0, 0), 1) ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Left->pt_Left, pt_Id->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Left->pt_Right, pt_Id->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Right, pt_Id));
    /* clang-format on */

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a (0, 1) ID */
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test normalising a (0, (0, 0)) and ((0, 0), 0) and IDs succeeds */
void ITC_Id_Test_normalise000And000IdSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* clang-format off */
    /* Create a new (0, (0, 0)) ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right->pt_Left, pt_Id->pt_Right));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right->pt_Right, pt_Id->pt_Right));
    /* clang-format on */

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a NULL ID */
    TEST_ITC_ID_IS_NULL_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));

    /* clang-format off */
    /* Create a new ((0, 0), 0) ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Left->pt_Left, pt_Id->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Left->pt_Right, pt_Id->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right, pt_Id));
    /* clang-format on */

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a NULL ID */
    TEST_ITC_ID_IS_NULL_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test normalising a (((1, 1), 1), (1, 1)) ID succeeds */
void ITC_Id_Test_normalise11111IdSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* clang-format off */
    /* Create a new (((1, 1), 1), (1, 1)) ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));

    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Left->pt_Left, pt_Id->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Left->pt_Left->pt_Left, pt_Id->pt_Left->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Left->pt_Left->pt_Right, pt_Id->pt_Left->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Left->pt_Right, pt_Id->pt_Left));

    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Right->pt_Left, pt_Id->pt_Right));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_Id->pt_Right->pt_Right, pt_Id->pt_Right));
    /* clang-format on */

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a seed ID */
    TEST_ITC_ID_IS_SEED_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test normalising a ((0, 0), ((0, 0), 0)) ID succeeds */
void ITC_Id_Test_normalise00000IdSuccessful(void)
{
    ITC_Id_t *pt_Id;

    /* clang-format off */
    /* Create a new ((0, 0), ((0, 0), 0)) ID */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id, NULL));

    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Left, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Left->pt_Left, pt_Id->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Left->pt_Right, pt_Id->pt_Left));

    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right, pt_Id));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right->pt_Left, pt_Id->pt_Right));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right->pt_Left->pt_Left, pt_Id->pt_Right->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right->pt_Left->pt_Right, pt_Id->pt_Right->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id->pt_Right->pt_Right, pt_Id->pt_Right));
    /* clang-format on */

    /* Normalise the ID */
    TEST_SUCCESS(ITC_Id_normalise(pt_Id));

    /* Test the ID is now a seed ID */
    TEST_ITC_ID_IS_NULL_ID(pt_Id);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_Id));
}

/* Test summing an ID fails with invalid param */
void ITC_Id_Test_sumIdFailInvalidParam(void)
{
    ITC_Id_t *pt_Dummy = NULL;

    TEST_FAILURE(
        ITC_Id_sum(NULL, pt_Dummy, &pt_Dummy), ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Id_sum(pt_Dummy, NULL, &pt_Dummy), ITC_STATUS_INVALID_PARAM);
    TEST_FAILURE(
        ITC_Id_sum(pt_Dummy, pt_Dummy, NULL), ITC_STATUS_INVALID_PARAM);
}

/* Test summing an ID fails with corrupt ID */
void ITC_Id_Test_sumIdFailWithCorruptId(void)
{
    ITC_Id_t *pt_Id1;
    ITC_Id_t *pt_Id2;
    ITC_Id_t *pt_SumId;

    /* Test different invalid IDs are handled properly */
    for (uint32_t u32_I = 0;
         u32_I < gu32_InvalidIdTablesSize;
         u32_I++)
    {
        /* Construct an invalid ID */
        gpv_InvalidIdConstructorTable[u32_I](&pt_Id1);

        /* Construct the other ID */
        TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_Id2, NULL));

        /* Test for the failure */
        TEST_FAILURE(
            ITC_Id_sum(pt_Id1, pt_Id2, &pt_SumId), ITC_STATUS_CORRUPT_ID);
        /* And the other way around */
        TEST_FAILURE(
            ITC_Id_sum(pt_Id2, pt_Id1, &pt_SumId), ITC_STATUS_CORRUPT_ID);

        /* Destroy the IDs */
        gpv_InvalidIdDestructorTable[u32_I](&pt_Id1);
        TEST_SUCCESS(ITC_Id_destroy(&pt_Id2));
    }
}

/* Test summing two NULL IDs succeeds */
void ITC_Id_Test_sumId00Succeeds(void)
{
    ITC_Id_t *pt_OriginalId1;
    ITC_Id_t *pt_OriginalId2;
    ITC_Id_t *pt_SumId = NULL;

    /* Create two NULL IDs */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId1, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId2, NULL));

    /* Sum the IDs */
    TEST_SUCCESS(ITC_Id_sum(pt_OriginalId1, pt_OriginalId2, &pt_SumId));

    /* Test the summed ID is a NULL ID */
    TEST_ITC_ID_IS_NULL_ID(pt_SumId);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId2));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SumId));
}

/* Test summing two NULL ID subtrees succeeds */
void ITC_Id_Test_sumId00SubtreesSucceeds(void)
{
    ITC_Id_t *pt_OriginalId1;
    ITC_Id_t *pt_OriginalId2;
    ITC_Id_t *pt_SumId = NULL;

    /* clang-format off */
    /* Create two NULL IDs */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId1, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId1->pt_Left, pt_OriginalId1));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId1->pt_Right, pt_OriginalId1));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId2, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId2->pt_Left, pt_OriginalId2));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId2->pt_Right, pt_OriginalId2));
    /* clang-format on */

    /* Sum the IDs */
    TEST_SUCCESS(
        ITC_Id_sum(
            pt_OriginalId1->pt_Left,
            pt_OriginalId2->pt_Right,
            &pt_SumId));

    /* Test the summed ID is a NULL ID */
    TEST_ITC_ID_IS_NULL_ID(pt_SumId);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId2));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SumId));
}

/* Test summing a NULL and a seed ID succeeds */
void ITC_Id_Test_sumId01And10Succeeds(void)
{
    ITC_Id_t *pt_OriginalId1;
    ITC_Id_t *pt_OriginalId2;
    ITC_Id_t *pt_SumId = NULL;

    /* Create the NULL and seed IDs */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId1, NULL));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId2, NULL));

    /* Sum the IDs */
    TEST_SUCCESS(ITC_Id_sum(pt_OriginalId1, pt_OriginalId2, &pt_SumId));

    /* Test the summed ID is a seed ID */
    TEST_ITC_ID_IS_SEED_ID(pt_SumId);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_SumId));

    /* Sum the IDs the other way around */
    TEST_SUCCESS(ITC_Id_sum(pt_OriginalId2, pt_OriginalId1, &pt_SumId));

    /* Test the summed ID is a seed ID */
    TEST_ITC_ID_IS_SEED_ID(pt_SumId);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId2));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SumId));
}

/* Test summing a NULL and a seed ID subtrees succeeds */
void ITC_Id_Test_sumId01And10SubtreesSucceeds(void)
{
    ITC_Id_t *pt_OriginalId1;
    ITC_Id_t *pt_OriginalId2;
    ITC_Id_t *pt_SumId = NULL;

    /* clang-format off */
    /* Create the NULL and seed IDs */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId1, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId1->pt_Left, pt_OriginalId1));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId1->pt_Right, pt_OriginalId1));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId2, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId2->pt_Left, pt_OriginalId2));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId2->pt_Right, pt_OriginalId2));
    /* clang-format on */

    /* Sum the IDs */
    TEST_SUCCESS(
        ITC_Id_sum(
            pt_OriginalId1->pt_Left,
            pt_OriginalId2->pt_Right,
            &pt_SumId));

    /* Test the summed ID is a seed ID */
    TEST_ITC_ID_IS_SEED_ID(pt_SumId);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_SumId));

    /* Sum the IDs the other way around */
    TEST_SUCCESS(
        ITC_Id_sum(
            pt_OriginalId2->pt_Right,
            pt_OriginalId1->pt_Left,
            &pt_SumId));

    /* Test the summed ID is a seed ID */
    TEST_ITC_ID_IS_SEED_ID(pt_SumId);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId2));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SumId));
}

/* Test summing a NULL and a (0, 1) ID succeeds */
void ITC_Id_Test_sumId001And010Succeeds(void)
{
    ITC_Id_t *pt_OriginalId1;
    ITC_Id_t *pt_OriginalId2;
    ITC_Id_t *pt_SumId = NULL;

    /* clang-format off */
    /* Create the NULL and (0, 1) IDs */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId1, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId2, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId2->pt_Left, pt_OriginalId2));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId2->pt_Right, pt_OriginalId2));
    /* clang-format on */

    /* Sum the IDs */
    TEST_SUCCESS(ITC_Id_sum(pt_OriginalId1, pt_OriginalId2, &pt_SumId));

    /* Test the summed ID is a (0, 1) ID */
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_SumId);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_SumId));

    /* Sum the IDs the other way around */
    TEST_SUCCESS(ITC_Id_sum(pt_OriginalId2, pt_OriginalId1, &pt_SumId));

    /* Test the summed ID is a (0, 1) ID */
    TEST_ITC_ID_IS_NULL_SEED_ID(pt_SumId);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId2));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SumId));
}

/* Test summing a NULL and a (1, 0) ID succeeds */
void ITC_Id_Test_sumId010And100Succeeds(void)
{
    ITC_Id_t *pt_OriginalId1;
    ITC_Id_t *pt_OriginalId2;
    ITC_Id_t *pt_SumId = NULL;

    /* clang-format off */
    /* Create the NULL and (1, 0) IDs */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId1, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId2, NULL));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId2->pt_Left, pt_OriginalId2));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId2->pt_Right, pt_OriginalId2));
    /* clang-format on */

    /* Sum the IDs */
    TEST_SUCCESS(ITC_Id_sum(pt_OriginalId1, pt_OriginalId2, &pt_SumId));

    /* Test the summed ID is a (1, 0) ID */
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_SumId);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_SumId));

    /* Sum the IDs the other way around */
    TEST_SUCCESS(ITC_Id_sum(pt_OriginalId2, pt_OriginalId1, &pt_SumId));

    /* Test the summed ID is a (1, 0) ID */
    TEST_ITC_ID_IS_SEED_NULL_ID(pt_SumId);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId2));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SumId));
}

/* Test summing a (1, 0) and a (0, 1) ID succeeds */
void ITC_Id_Test_sumId1001And0110Succeeds(void)
{
    ITC_Id_t *pt_OriginalId1;
    ITC_Id_t *pt_OriginalId2;
    ITC_Id_t *pt_SumId = NULL;

    /* clang-format off */
    /* Create the (1, 0) and (0, 1) IDs */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId1, NULL));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId1->pt_Left, pt_OriginalId1));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId1->pt_Right, pt_OriginalId1));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId2, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId2->pt_Left, pt_OriginalId2));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId2->pt_Right, pt_OriginalId2));
    /* clang-format on */

    /* Sum the IDs */
    TEST_SUCCESS(ITC_Id_sum(pt_OriginalId1, pt_OriginalId2, &pt_SumId));

    /* Test the summed ID is a seed ID */
    TEST_ITC_ID_IS_SEED_ID(pt_SumId);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_SumId));

    /* Sum the IDs the other way around */
    TEST_SUCCESS(ITC_Id_sum(pt_OriginalId2, pt_OriginalId1, &pt_SumId));

    /* Test the summed ID is a seed ID */
    TEST_ITC_ID_IS_SEED_ID(pt_SumId);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId2));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SumId));
}

/* Test summing a ((1, 0), 1) and a ((0, 1), 0) ID succeeds */
void ITC_Id_Test_sumId110001And001110Succeeds(void)
{
    ITC_Id_t *pt_OriginalId1;
    ITC_Id_t *pt_OriginalId2;
    ITC_Id_t *pt_SumId = NULL;

    /* clang-format off */
    /* Create the (1, 0), 1) and a ((0, 1), 0) IDs */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId1, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId1->pt_Left, pt_OriginalId1));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId1->pt_Left->pt_Left, pt_OriginalId1->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId1->pt_Left->pt_Right, pt_OriginalId1->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId1->pt_Right, pt_OriginalId1));

    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId2, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId2->pt_Left, pt_OriginalId2));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId2->pt_Left->pt_Left, pt_OriginalId2->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId2->pt_Left->pt_Right, pt_OriginalId2->pt_Left));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId2->pt_Right, pt_OriginalId2));
    /* clang-format on */

    /* Sum the IDs */
    TEST_SUCCESS(ITC_Id_sum(pt_OriginalId1, pt_OriginalId2, &pt_SumId));

    /* Test the summed ID is a seed ID */
    TEST_ITC_ID_IS_SEED_ID(pt_SumId);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_SumId));

    /* Sum the IDs the other way around */
    TEST_SUCCESS(ITC_Id_sum(pt_OriginalId2, pt_OriginalId1, &pt_SumId));

    /* Test the summed ID is a seed ID */
    TEST_ITC_ID_IS_SEED_ID(pt_SumId);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId2));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SumId));
}

/* Test summing a (1, (1, 0)) and a (0, (0, 1)) ID succeeds */
void ITC_Id_Test_sumId001110And110001Succeeds(void)
{
    ITC_Id_t *pt_OriginalId1;
    ITC_Id_t *pt_OriginalId2;
    ITC_Id_t *pt_SumId = NULL;

    /* clang-format off */
    /* Create the (1, (1, 0)) and a (0, (0, 1)) IDs */
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId1, NULL));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId1->pt_Left, pt_OriginalId1));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId1->pt_Right, pt_OriginalId1));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId1->pt_Right->pt_Left, pt_OriginalId1->pt_Right));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId1->pt_Right->pt_Right, pt_OriginalId1->pt_Right));

    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId2, NULL));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId2->pt_Left, pt_OriginalId2));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId2->pt_Right, pt_OriginalId2));
    TEST_SUCCESS(ITC_TestUtil_newNullId(&pt_OriginalId2->pt_Right->pt_Left, pt_OriginalId2->pt_Right));
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId2->pt_Right->pt_Right, pt_OriginalId2->pt_Right));
    /* clang-format on */

    /* Sum the IDs */
    TEST_SUCCESS(ITC_Id_sum(pt_OriginalId1, pt_OriginalId2, &pt_SumId));

    /* Test the summed ID is a seed ID */
    TEST_ITC_ID_IS_SEED_ID(pt_SumId);

    /* Destroy the ID */
    TEST_SUCCESS(ITC_Id_destroy(&pt_SumId));

    /* Sum the IDs the other way around */
    TEST_SUCCESS(ITC_Id_sum(pt_OriginalId2, pt_OriginalId1, &pt_SumId));

    /* Test the summed ID is a seed ID */
    TEST_ITC_ID_IS_SEED_ID(pt_SumId);

    /* Destroy the IDs */
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId2));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SumId));
}

/* Test splitting a seed ID several times and summing it back succeeds */
void ITC_Id_Test_sumIdSplitSeedAndSumItBackToSeedSucceeds(void)
{
    ITC_Id_t *pt_OriginalId;
    ITC_Id_t *pt_SplitId1;
    ITC_Id_t *pt_SplitId11;
    ITC_Id_t *pt_SplitId12;
    ITC_Id_t *pt_SplitId111;
    ITC_Id_t *pt_SplitId121;
    ITC_Id_t *pt_SplitId112;
    ITC_Id_t *pt_SplitId122;
    ITC_Id_t *pt_SplitId2;
    ITC_Id_t *pt_SplitId21;
    ITC_Id_t *pt_SplitId22;
    ITC_Id_t *pt_SplitId211;
    ITC_Id_t *pt_SplitId221;
    ITC_Id_t *pt_SplitId212;
    ITC_Id_t *pt_SplitId222;

    ITC_Id_t *pt_SummedId = NULL;
    ITC_Id_t *pt_TmpId = NULL;

    /* Create the seed ID */
    TEST_SUCCESS(ITC_TestUtil_newSeedId(&pt_OriginalId, NULL));

    /* Split into (1, 0) and (0, 1) */
    TEST_SUCCESS(ITC_Id_split(pt_OriginalId, &pt_SplitId1, &pt_SplitId2));
    TEST_SUCCESS(ITC_Id_destroy(&pt_OriginalId));

    /* Split into:
     * - ((1, 0), 0)
     * - (0, (1, 0))
     * - ((0, 1), 0)
     * - (0, (0, 1))
     */
    TEST_SUCCESS(ITC_Id_split(pt_SplitId1, &pt_SplitId11, &pt_SplitId21));
    TEST_SUCCESS(ITC_Id_split(pt_SplitId2, &pt_SplitId12, &pt_SplitId22));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId1));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId2));

    /* Split into:
     * - (((1, 0), 0), 0)
     * - (0, ((1, 0), 0))
     * - ((0, (1, 0)), 0)
     * - (0, (0, (1, 0)))
     * - (((0, 1), 0), 0)
     * - (0, ((0, 1), 0))
     * - ((0, (0, 1)), 0)
     * - (0, (0, (0, 1)))
     */
    TEST_SUCCESS(ITC_Id_split(pt_SplitId11, &pt_SplitId111, &pt_SplitId211));
    TEST_SUCCESS(ITC_Id_split(pt_SplitId12, &pt_SplitId112, &pt_SplitId212));
    TEST_SUCCESS(ITC_Id_split(pt_SplitId21, &pt_SplitId121, &pt_SplitId221));
    TEST_SUCCESS(ITC_Id_split(pt_SplitId22, &pt_SplitId122, &pt_SplitId222));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId11));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId12));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId21));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId22));

    /* Sum them back into to a seed in arbitrary order */

    TEST_SUCCESS(ITC_Id_sum(pt_SplitId222, pt_SplitId121, &pt_SummedId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId222));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId121));

    TEST_SUCCESS(ITC_Id_clone(pt_SummedId, &pt_TmpId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SummedId));

    TEST_SUCCESS(ITC_Id_sum(pt_SplitId211, pt_TmpId, &pt_SummedId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId211));
    TEST_SUCCESS(ITC_Id_destroy(&pt_TmpId));

    TEST_SUCCESS(ITC_Id_clone(pt_SummedId, &pt_TmpId));

    TEST_SUCCESS(ITC_Id_destroy(&pt_SummedId));

    TEST_SUCCESS(ITC_Id_sum(pt_SplitId122, pt_TmpId, &pt_SummedId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId122));
    TEST_SUCCESS(ITC_Id_destroy(&pt_TmpId));

    TEST_SUCCESS(ITC_Id_clone(pt_SummedId, &pt_TmpId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SummedId));

    TEST_SUCCESS(ITC_Id_sum(pt_SplitId111, pt_TmpId, &pt_SummedId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId111));
    TEST_SUCCESS(ITC_Id_destroy(&pt_TmpId));

    TEST_SUCCESS(ITC_Id_clone(pt_SummedId, &pt_TmpId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SummedId));

    TEST_SUCCESS(ITC_Id_sum(pt_SplitId221, pt_TmpId, &pt_SummedId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId221));
    TEST_SUCCESS(ITC_Id_destroy(&pt_TmpId));

    TEST_SUCCESS(ITC_Id_clone(pt_SummedId, &pt_TmpId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SummedId));

    TEST_SUCCESS(ITC_Id_sum(pt_SplitId212, pt_TmpId, &pt_SummedId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId212));
    TEST_SUCCESS(ITC_Id_destroy(&pt_TmpId));

    TEST_SUCCESS(ITC_Id_clone(pt_SummedId, &pt_TmpId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SummedId));

    TEST_SUCCESS(ITC_Id_sum(pt_SplitId112, pt_TmpId, &pt_SummedId));
    TEST_SUCCESS(ITC_Id_destroy(&pt_SplitId112));
    TEST_SUCCESS(ITC_Id_destroy(&pt_TmpId));

    TEST_SUCCESS(ITC_Id_destroy(&pt_SummedId));
}
