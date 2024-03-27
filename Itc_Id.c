/**
 * @file Itc_Id.c
 * @author Stan
 * @brief Implementation of the Interval Tree Clock's ID mechanism
 *
 * @copypt_Right Copypt_Right (c) 2024
 *
 */
#include "Itc_Id.h"
#include "Itc_Id_package.h"

#include <stdlib.h>

/******************************************************************************
 * Public functions
 ******************************************************************************/

/**
 * @brief Clone an existing ITC ID
 *
 * @note Memory for the new ITC ID will be dynamically allocated.
 * On error, the cloned ID is automatically deallocated.
 * @param pt_Id The existing ID
 * @param ppt_ClonedId The pointer to the cloned ID
 * @param pt_ParentId The pointer to parent ID. Otherwise NULL
 * @return Itc_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static Itc_Status_t cloneId(
    const ITC_Id_t *const pt_Id,
    ITC_Id_t **ppt_ClonedId,
    ITC_Id_t *pt_ParentId
)
{
    Itc_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    const ITC_Id_t *pt_Root = pt_Id; /* The current root */
    ITC_Id_t *pt_ClonedRoot; /* The current cloned root */

    if (pt_Root == NULL)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        /* Allocate the root */
        t_Status = ITC_Id_new(ppt_ClonedId, pt_ParentId, pt_Root->b_IsOwner);

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            /* Initialise the cloned root pointer */
            pt_ClonedRoot = *ppt_ClonedId;
        }

        while(t_Status == ITC_STATUS_SUCCESS && pt_Root != NULL)
        {
            if (pt_Root->pt_Left && !pt_ClonedRoot->pt_Left)
            {
                /* Allocate left subtree */
                t_Status = ITC_Id_new(
                    &pt_ClonedRoot->pt_Left,
                    pt_ClonedRoot,
                    pt_Root->pt_Left->b_IsOwner);

                if (t_Status == ITC_STATUS_SUCCESS)
                {
                    /* Advance into the tree */
                    pt_Root = pt_Root->pt_Left;
                    pt_ClonedRoot = pt_ClonedRoot->pt_Left;
                }
            }
            else if (pt_Root->pt_Right && !pt_ClonedRoot->pt_Right)
            {
                /* Allocate right subtree */
                t_Status = ITC_Id_new(
                    &pt_ClonedRoot->pt_Right,
                    pt_ClonedRoot,
                    pt_Root->pt_Right->b_IsOwner);

                if (t_Status == ITC_STATUS_SUCCESS)
                {
                    /* Advance into the tree */
                    pt_Root = pt_Root->pt_Right;
                    pt_ClonedRoot = pt_ClonedRoot->pt_Right;
                }
            }
            else
            {
                /* Go up the tree */
                pt_Root = pt_Root->pt_Parent;
                pt_ClonedRoot = pt_ClonedRoot->pt_Parent;
            }
        }
    }

    /* Something went wrong */
    if (t_Status != ITC_STATUS_SUCCESS)
    {
        /* Deallocate clone */
        ITC_Id_destroy(ppt_ClonedId);
    }

    return t_Status;
}

/**
 * @brief Allocates 2 new IDs fulfilling `split(1) = ((1, 0), (0, 1))`
 *
 * @param ppt_Id1 (out) The first ID
 * @param pt_ParentId1 The parent of ID1. Otherwise NULL
 * @param ppt_Id2 (out) The second ID
 * @param pt_ParentId2 The parent of ID2. Otherwise NULL
 * @return Itc_Status_t
 */
static Itc_Status_t newSplit1(
    ITC_Id_t **ppt_Id1,
    ITC_Id_t *pt_ParentId1,
    ITC_Id_t **ppt_Id2,
    ITC_Id_t *pt_ParentId2
)
{
    Itc_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    /* Allocate the first root */
    t_Status = ITC_Id_new(ppt_Id1, pt_ParentId1, 0);

    /* Allocate the children for the first root: (1, 0) */
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = ITC_Id_new(&(*ppt_Id1)->pt_Left, *ppt_Id1, 1);
    }
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = ITC_Id_new(&(*ppt_Id1)->pt_Right, *ppt_Id1, 0);
    }

    /* Allocate the second root */
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = ITC_Id_new(ppt_Id2, pt_ParentId2, 0);
    }

    /* Allocate the children for the second root: (0, 1) */
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = ITC_Id_new(&(*ppt_Id2)->pt_Left, *ppt_Id2, 0);
    }
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = ITC_Id_new(&(*ppt_Id2)->pt_Right, *ppt_Id2, 1);
    }

    return t_Status;
}

/**
 * @brief Allocates 2 new IDs fulfilling `split(0) = (0, 0)`
 *
 * @param ppt_Id1 (out) The first ID
 * @param pt_ParentId1 The parent of ID1. Otherwise NULL
 * @param ppt_Id2 (out) The second ID
 * @param pt_ParentId2 The parent of ID2. Otherwise NULL
 * @return Itc_Status_t
 */
static Itc_Status_t newSplit0(
    ITC_Id_t **ppt_Id1,
    ITC_Id_t *pt_ParentId1,
    ITC_Id_t **ppt_Id2,
    ITC_Id_t *pt_ParentId2
)
{
    Itc_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    t_Status = ITC_Id_new(ppt_Id1, pt_ParentId1, 0);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = ITC_Id_new(ppt_Id2, pt_ParentId2, 0);
    }

    return t_Status;
}

/**
 * @brief Allocates 2 new IDs fulfilling `split(i)`
 *
 * Rules:
 *  - split(0) = (0, 0)
 *  - split(1) = ((1, 0), (0, 1))
 *  - split((0, i)) = ((0, i1), (0, i2)), where (i1, i2) = split(i)
 *  - split((i, 0)) = ((i1, 0), (i2, 0)), where (i1, i2) = split(i)
 *  - split((i1, i2)) = ((i1, 0), (0, i2))
 * @param pt_Id The existing ID
 * @param ppt_Id1 The first ID
 * @param ppt_Id2 The second ID
 * @return Itc_Status_t
 */
static Itc_Status_t newSplitI(
    const ITC_Id_t *const pt_Id,
    ITC_Id_t **ppt_Id1,
    ITC_Id_t **ppt_Id2
)
{
    Itc_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    Itc_Status_t t_OpStatus = ITC_STATUS_SUCCESS; /* The current status */

    const ITC_Id_t *pt_CurrentId = pt_Id;
    ITC_Id_t **ppt_CurrentId1 = ppt_Id1;
    ITC_Id_t *pt_ParentCurrentId1 = NULL;
    ITC_Id_t **ppt_CurrentId2 = ppt_Id2;
    ITC_Id_t *pt_ParentCurrentId2 = NULL;

    /* TODO: cleanup & simplify*/

    if ((*ppt_CurrentId1) || (*ppt_CurrentId2))
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }

    while (t_Status == ITC_STATUS_SUCCESS && pt_CurrentId)
    {
        /* split(0) = (0, 0) */
        if (ITC_ID_IS_NULL_ID(pt_CurrentId) && !(*ppt_CurrentId1) && !(*ppt_CurrentId2))
        {
            t_Status = newSplit0(
                ppt_CurrentId1,
                pt_ParentCurrentId1,
                ppt_CurrentId2,
                pt_ParentCurrentId2);

            if (pt_CurrentId)
            {
                pt_CurrentId = pt_CurrentId->pt_Parent;
                if (pt_CurrentId)
                {
                    ppt_CurrentId1 = &(*ppt_CurrentId1)->pt_Parent;
                    ppt_CurrentId2 = &(*ppt_CurrentId2)->pt_Parent;
                    pt_ParentCurrentId1 = (*ppt_CurrentId1) ? (*ppt_CurrentId1)->pt_Parent : NULL;
                    pt_ParentCurrentId2 = (*ppt_CurrentId2) ? (*ppt_CurrentId2)->pt_Parent : NULL;
                }
            }
        }
        /* split(1) = ((1, 0), (0, 1)) */
        else if (ITC_ID_IS_SEED_ID(pt_CurrentId) && !(*ppt_CurrentId1) && !(*ppt_CurrentId2))
        {
            t_Status = newSplit1(
                ppt_CurrentId1,
                pt_ParentCurrentId1,
                ppt_CurrentId2,
                pt_ParentCurrentId2);

            if (pt_CurrentId)
            {
                pt_CurrentId = pt_CurrentId->pt_Parent;
                if (pt_CurrentId)
                {
                    ppt_CurrentId1 = &(*ppt_CurrentId1)->pt_Parent;
                    ppt_CurrentId2 = &(*ppt_CurrentId2)->pt_Parent;
                    pt_ParentCurrentId1 = (*ppt_CurrentId1) ? (*ppt_CurrentId1)->pt_Parent : NULL;
                    pt_ParentCurrentId2 = (*ppt_CurrentId2) ? (*ppt_CurrentId2)->pt_Parent : NULL;
                }
            }
        }
        else if(!ITC_ID_IS_LEAF_NODE(pt_CurrentId))
        {
            if(!(*ppt_CurrentId1))
            {
                t_Status = ITC_Id_new(
                    ppt_CurrentId1, pt_ParentCurrentId1, 0);
            }

            if(t_Status == ITC_STATUS_SUCCESS && !(*ppt_CurrentId2))
            {
                t_Status = ITC_Id_new(
                    ppt_CurrentId2, pt_ParentCurrentId2, 0);
            }

            if (t_Status == ITC_STATUS_SUCCESS)
            {
                /* split((0, i)) = ((0, i1), (0, i2)), where (i1, i2) = split(i) */
                if (ITC_ID_IS_NULL_ID(pt_CurrentId->pt_Left))
                {
                    if (ITC_ID_IS_LEAF_NODE(*ppt_CurrentId1) &&
                        ITC_ID_IS_LEAF_NODE(*ppt_CurrentId2))
                    {
                        t_Status =
                            ITC_Id_new(&(*ppt_CurrentId1)->pt_Left, *ppt_CurrentId1, 0);

                        if(t_Status == ITC_STATUS_SUCCESS)
                        {
                            t_Status = ITC_Id_new(
                                &(*ppt_CurrentId2)->pt_Left,
                                *ppt_CurrentId2,
                                0);
                        }

                        if (t_Status == ITC_STATUS_SUCCESS)
                        {
                            pt_CurrentId = pt_CurrentId->pt_Right;
                            pt_ParentCurrentId1 = *ppt_CurrentId1;
                            ppt_CurrentId1 = &(*ppt_CurrentId1)->pt_Right;
                            pt_ParentCurrentId2 = *ppt_CurrentId2;
                            ppt_CurrentId2 = &(*ppt_CurrentId2)->pt_Right;
                        }
                    }
                    else if (pt_CurrentId)
                    {
                        pt_CurrentId = pt_CurrentId->pt_Parent;
                        if (pt_CurrentId)
                        {
                            ppt_CurrentId1 = &(*ppt_CurrentId1)->pt_Parent;
                            ppt_CurrentId2 = &(*ppt_CurrentId2)->pt_Parent;
                            pt_ParentCurrentId1 = (*ppt_CurrentId1) ? (*ppt_CurrentId1)->pt_Parent : NULL;
                            pt_ParentCurrentId2 = (*ppt_CurrentId2) ? (*ppt_CurrentId2)->pt_Parent : NULL;
                        }
                    }
                }
                /* split((i, 0)) = ((i1, 0), (i2, 0)), where (i1, i2) = split(i) */
                else if (ITC_ID_IS_NULL_ID(pt_CurrentId->pt_Right))
                {
                    if (ITC_ID_IS_LEAF_NODE(*ppt_CurrentId1) &&
                        ITC_ID_IS_LEAF_NODE(*ppt_CurrentId2))
                    {
                        t_Status =
                            ITC_Id_new(&(*ppt_CurrentId1)->pt_Right, *ppt_CurrentId1, 0);

                        if(t_Status == ITC_STATUS_SUCCESS)
                        {
                            t_Status = ITC_Id_new(
                                &(*ppt_CurrentId2)->pt_Right,
                                *ppt_CurrentId2,
                                0);
                        }

                        if (t_Status == ITC_STATUS_SUCCESS)
                        {
                            pt_CurrentId = pt_CurrentId->pt_Left;
                            pt_ParentCurrentId1 = *ppt_CurrentId1;
                            ppt_CurrentId1 = &(*ppt_CurrentId1)->pt_Left;
                            pt_ParentCurrentId2 = *ppt_CurrentId2;
                            ppt_CurrentId2 = &(*ppt_CurrentId2)->pt_Left;
                        }
                    }
                    else if (pt_CurrentId)
                    {
                        pt_CurrentId = pt_CurrentId->pt_Parent;
                        if (pt_CurrentId)
                        {
                            ppt_CurrentId1 = &(*ppt_CurrentId1)->pt_Parent;
                            ppt_CurrentId2 = &(*ppt_CurrentId2)->pt_Parent;
                            pt_ParentCurrentId1 = (*ppt_CurrentId1) ? (*ppt_CurrentId1)->pt_Parent : NULL;
                            pt_ParentCurrentId2 = (*ppt_CurrentId2) ? (*ppt_CurrentId2)->pt_Parent : NULL;
                        }
                    }
                }
                /* split((i1, i2)) = ((i1, 0), (0, i2)) */
                else if (ITC_ID_IS_LEAF_NODE(*ppt_CurrentId1) && ITC_ID_IS_LEAF_NODE(*ppt_CurrentId2))
                {
                    t_Status = ITC_Id_new(
                        &(*ppt_CurrentId1)->pt_Right,
                        *ppt_CurrentId1,
                        0);

                    if (t_Status == ITC_STATUS_SUCCESS)
                    {
                        t_Status = cloneId(
                            (const ITC_Id_t *const)pt_CurrentId->pt_Left,
                            &(*ppt_CurrentId1)->pt_Left,
                            *ppt_CurrentId1);
                    }

                    if (t_Status == ITC_STATUS_SUCCESS)
                    {
                        t_Status = ITC_Id_new(
                            &(*ppt_CurrentId2)->pt_Left,
                            *ppt_CurrentId2,
                            0);
                    }

                    if (t_Status == ITC_STATUS_SUCCESS)
                    {
                        t_Status = cloneId(
                            (const ITC_Id_t *const)pt_CurrentId->pt_Right,
                            &(*ppt_CurrentId2)->pt_Right,
                            *ppt_CurrentId2);
                    }

                    if (t_Status == ITC_STATUS_SUCCESS && pt_CurrentId)
                    {
                        pt_CurrentId = pt_CurrentId->pt_Parent;
                        if (pt_CurrentId)
                        {
                            ppt_CurrentId1 = &(*ppt_CurrentId1)->pt_Parent;
                            ppt_CurrentId2 = &(*ppt_CurrentId2)->pt_Parent;
                            pt_ParentCurrentId1 = (*ppt_CurrentId1) ? (*ppt_CurrentId1)->pt_Parent : NULL;
                            pt_ParentCurrentId2 = (*ppt_CurrentId2) ? (*ppt_CurrentId2)->pt_Parent : NULL;
                        }
                    }
                }
                else
                {
                    t_Status = ITC_STATUS_CORRUPT_ID;
                }
            }
        }
        else
        {
            t_Status = ITC_STATUS_CORRUPT_ID;
        }
    }

    if (t_Status != ITC_STATUS_SUCCESS && t_Status != ITC_STATUS_INVALID_PARAM)
    {
        t_Status = ITC_Id_destroy(ppt_Id1);
        t_OpStatus = ITC_Id_destroy(ppt_Id2);

        if (t_Status == ITC_STATUS_SUCCESS)
        {
            t_Status = t_OpStatus;
        }
    }

    return t_Status;
}

/******************************************************************************
 * Allocate a new ITC ID
 ******************************************************************************/

Itc_Status_t ITC_Id_new(
    ITC_Id_t **ppt_Id,
    ITC_Id_t *pt_Parent,
    bool b_IsOwner
)
{
    Itc_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Id_t *pt_Alloc = malloc(sizeof(ITC_Id_t));

    /* Allocation failed */
    if (!pt_Alloc)
    {
        t_Status = ITC_STATUS_INSUFFICIENT_RESOURCES;
    }

    /* Initialise members */
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        pt_Alloc->b_IsOwner = b_IsOwner;
        pt_Alloc->pt_Parent = pt_Parent;
        pt_Alloc->pt_Left = NULL;
        pt_Alloc->pt_Right = NULL;
    }

    /* Return the pointer to the allocated memory */
    *ppt_Id = pt_Alloc;

    return t_Status;
}

/******************************************************************************
 * Free an allocated ITC ID
 ******************************************************************************/

Itc_Status_t ITC_Id_destroy(
    ITC_Id_t **ppt_Id
)
{
    Itc_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Id_t *pt_Current = *ppt_Id; /* The current element */
    ITC_Id_t *pt_Tmp = NULL;

    if (pt_Current == NULL)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        while(t_Status == ITC_STATUS_SUCCESS && pt_Current != NULL)
        {
            if(pt_Current->pt_Left)
            {
                /* Advance into left subtree */
                pt_Current = pt_Current->pt_Left;
            }
            else if(pt_Current->pt_Right)
            {
                /* Advance into right subtree */
                pt_Current = pt_Current->pt_Right;
            }
            else
            {
                /* Remember the parent element */
                pt_Tmp = pt_Current->pt_Parent;

                if(pt_Tmp)
                {
                    /* Remove the current element address from the parent */
                    if(pt_Tmp->pt_Left == pt_Current)
                    {
                        pt_Tmp->pt_Left = NULL;
                    }
                    else
                    {
                        pt_Tmp->pt_Right = NULL;
                    }
                }

                /* Free the current element */
                free(pt_Current);

                /* Go up the tree */
                pt_Current = pt_Tmp;
            }
        }
    }

    /* Sanitize the freed pointer */
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        *ppt_Id = NULL;
    }

    return t_Status;
}

/******************************************************************************
 * Clone an existing ITC ID
 ******************************************************************************/

Itc_Status_t ITC_Id_clone(
    const ITC_Id_t *const pt_Id,
    ITC_Id_t **ppt_ClonedId
)
{
    return cloneId(pt_Id, ppt_ClonedId, NULL);
}

/******************************************************************************
 * Allocate a new ITC ID and initialise it as a seed ID (1)
 ******************************************************************************/

Itc_Status_t ITC_Id_newSeedId(
    ITC_Id_t **ppt_Id
)
{
    return ITC_Id_new(ppt_Id, NULL, 1);
}

/******************************************************************************
 * Allocate a new ITC ID and initialise it as a null ID (0)
 ******************************************************************************/

Itc_Status_t ITC_Id_newNullId(
    ITC_Id_t **ppt_Id
)
{
    return ITC_Id_new(ppt_Id, NULL, 0);
}

/******************************************************************************
 * Split an existing ITC ID into two distinct (non-overlaping) ITC IDs
 ******************************************************************************/

Itc_Status_t ITC_Id_split(
    const ITC_Id_t *const pt_Id,
    ITC_Id_t **ppt_Id1,
    ITC_Id_t **ppt_Id2
)
{
    return newSplitI(pt_Id, ppt_Id1, ppt_Id2);
}