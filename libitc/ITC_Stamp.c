/**
 * @file ITC_Stamp.c
 * @author Stan
 * @brief Implementation of the Interval Tree Clock's Stamp mechanism
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "ITC_Stamp.h"
#include "ITC_Port.h"

#include <stdbool.h>

/******************************************************************************
 * Private functions
 ******************************************************************************/

/**
 * @brief Validate an existing ITC Stamp
 *
 * Should be used to validate all incoming Stamps before any processing is done.
 *
 * @param pt_Stamp The Stamp to validate
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t validateStamp(
    const ITC_Stamp_t *const pt_Stamp
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    if (!pt_Stamp)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else if (!pt_Stamp->pt_Id || !pt_Stamp->pt_Event)
    {
        t_Status = ITC_STATUS_CORRUPT_STAMP;
    }

    /* Validation of the ID and Event trees is done by their respective
     * validation functions */

    return t_Status;
}

/**
 * @brief Allocate a new ITC Stamp
 *
 * @param ppt_Stamp (out) The pointer to the new Stamp
 * @param pt_Id The pointer to an existing ID tree to be cloned. Otherwise NULL.
 * @param pt_Event The pointer to an existing Event tree to be cloned.
 * Otherwise NULL.
 * @param b_CreateNullID Allocate a NULL ID instead of a Seed ID.
 * Ignored if pt_Id != NULL
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
static ITC_Status_t newStamp(
    ITC_Stamp_t **ppt_Stamp,
    ITC_Id_t *pt_Id,
    ITC_Event_t *pt_Event,
    bool b_CreateNullID
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Stamp_t *pt_Alloc = NULL;

    if (!ppt_Stamp)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        t_Status = ITC_Port_malloc((void **)&pt_Alloc, sizeof(ITC_Stamp_t));
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        if (pt_Id)
        {
            t_Status = ITC_Id_clone(pt_Id, &pt_Alloc->pt_Id);
        }
        else if (b_CreateNullID)
        {
            t_Status = ITC_Id_newNull(&pt_Alloc->pt_Id);
        }
        else
        {
            t_Status = ITC_Id_newSeed(&pt_Alloc->pt_Id);
        }
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        if (pt_Event)
        {
            t_Status = ITC_Event_clone(pt_Event, &pt_Alloc->pt_Event);
        }
        else
        {
            t_Status = ITC_Event_new(&pt_Alloc->pt_Event);
        }
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Return the pointer to the allocated memory */
        *ppt_Stamp = pt_Alloc;
    }
    else if(pt_Alloc)
    {
        /* Destroy anything that might have been allocated.
         * Ignore return status, there is nothing else to do if destroy fails */
        (void)ITC_Stamp_destroy(&pt_Alloc);
        *ppt_Stamp = NULL;
    }

    return t_Status;
}

/******************************************************************************
 * Public functions
 ******************************************************************************/

/******************************************************************************
 * Allocate a new ITC seed Stamp and initialise it
 ******************************************************************************/

ITC_Status_t ITC_Stamp_newSeed(
    ITC_Stamp_t **ppt_Stamp
)
{
    return newStamp(ppt_Stamp, NULL, NULL, false);
}

/******************************************************************************
 * Allocate a new ITC peek Stamp based on an existing Stamp
 ******************************************************************************/

ITC_Status_t ITC_Stamp_newPeek(
    const ITC_Stamp_t *const pt_Stamp,
    ITC_Stamp_t **ppt_PeekStamp
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    t_Status = validateStamp(pt_Stamp);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = newStamp(
            ppt_PeekStamp, NULL, pt_Stamp->pt_Event, true);
    }

    return t_Status;
}

/******************************************************************************
 * Free an allocated ITC Stamp
 ******************************************************************************/

ITC_Status_t ITC_Stamp_destroy(
    ITC_Stamp_t **ppt_Stamp
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */
    ITC_Status_t t_FreeStatus = ITC_STATUS_SUCCESS; /* The last free status */

    if (!ppt_Stamp || !(*ppt_Stamp))
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }
    else
    {
        if ((*ppt_Stamp)->pt_Event)
        {
            /* Destroy the Event tree */
            t_FreeStatus = ITC_Event_destroy(&(*ppt_Stamp)->pt_Event);

            if (t_FreeStatus != ITC_STATUS_SUCCESS)
            {
                t_Status = t_FreeStatus;
            }
        }

        if ((*ppt_Stamp)->pt_Id)
        {
            /* Destroy the ID tree */
            t_FreeStatus = ITC_Id_destroy(&(*ppt_Stamp)->pt_Id);

            if (t_FreeStatus != ITC_STATUS_SUCCESS)
            {
                t_Status = t_FreeStatus;
            }
        }

        /* Only free the Stamp container if all members have been freed */
        if (t_Status == ITC_STATUS_SUCCESS)
        {
            t_FreeStatus = ITC_Port_free(*ppt_Stamp);

            if (t_FreeStatus != ITC_STATUS_SUCCESS)
            {
                t_Status = t_FreeStatus;
            }
        }

    }

    /* Sanitize the freed pointer */
    if (t_Status == ITC_STATUS_SUCCESS)
    {
        *ppt_Stamp = NULL;
    }

    return t_Status;
}

/******************************************************************************
 * Clone an existing ITC Stamp
 ******************************************************************************/

ITC_Status_t ITC_Stamp_clone(
    const ITC_Stamp_t *const pt_Stamp,
    ITC_Stamp_t **ppt_ClonedStamp
)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    t_Status = validateStamp(pt_Stamp);

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = newStamp(
            ppt_ClonedStamp, pt_Stamp->pt_Id, pt_Stamp->pt_Event, false);
    }

    return t_Status;
}

/******************************************************************************
 * Compare two existing Stamps
 ******************************************************************************/

ITC_Status_t ITC_Stamp_compare(
    const ITC_Stamp_t *const pt_Stamp1,
    const ITC_Stamp_t *const pt_Stamp2,
    ITC_Event_Comparison_t *pt_Result

)
{
    ITC_Status_t t_Status = ITC_STATUS_SUCCESS; /* The current status */

    if (!pt_Result)
    {
        t_Status = ITC_STATUS_INVALID_PARAM;
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateStamp(pt_Stamp1);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        t_Status = validateStamp(pt_Stamp2);
    }

    if (t_Status == ITC_STATUS_SUCCESS)
    {
        /* Compare *pt_Stamp1 to *pt_Stamp2 */
        t_Status = ITC_Event_compare(
            pt_Stamp1->pt_Event, pt_Stamp2->pt_Event, pt_Result);
    }

    return t_Status;
}
