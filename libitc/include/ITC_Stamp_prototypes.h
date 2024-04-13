/**
 * @file ITC_Stamp_prototypes.h
 * @author Stan
 * @brief Prototypes for the Interval Tree Clock's Stamp mechanism
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef ITC_STAMP_PROTOTYPES_H_
#define ITC_STAMP_PROTOTYPES_H_

#include "ITC_Stamp.h"
#include "ITC_Status.h"

/******************************************************************************
 * Functions
 ******************************************************************************/

/**
 * @brief Allocate a new ITC seed Stamp and initialise it
 *
 * @param ppt_Stamp (out) The pointer to the Stamp
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Stamp_newSeed(
    ITC_Stamp_t **ppt_Stamp
);

/**
 * @brief Allocate a new ITC peek Stamp based on an existing Stamp
 *
 * @param pt_Stamp The existing Stamp
 * @param ppt_PeekStamp The pointer to the peek Stamp
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Stamp_newPeek(
    const ITC_Stamp_t *const pt_Stamp,
    ITC_Stamp_t **ppt_PeekStamp
);

/**
 * @brief Free an allocated ITC Stamp
 *
 * @param ppt_Stamp (in) The pointer to the Stamp to deallocate. (out) NULL
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Stamp_destroy(
    ITC_Stamp_t **ppt_Stamp
);

/**
 * @brief Clone an existing ITC Stamp
 *
 * @note Memory for the new ITC Stamp will be dynamically allocated.
 * On error, the cloned Stamp is automatically deallocated.
 * @param pt_Stamp The existing Stamp
 * @param ppt_ClonedStamp The pointer to the cloned Stamp
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Stamp_clone(
    const ITC_Stamp_t *const pt_Stamp,
    ITC_Stamp_t **ppt_ClonedStamp
);

/**
 * @brief Compare two existing Stamps
 *
 * - If `*pt_Stamp1 < *pt_Stamp2`:
 *      `*pt_Result == ITC_EVENT_COMPARISON_LESS_THAN`
 * - If `*pt_Stamp1 > *pt_Stamp2`:
 *      `*pt_Result == ITC_EVENT_COMPARISON_GREATER_THAN`
 * - If `*pt_Stamp1 == *pt_Stamp2`:
 *      `*pt_Result == ITC_EVENT_COMPARISON_EQUAL`
 * - If `*pt_Stamp1 <> *pt_Stamp2`:
 *      `*pt_Result == ITC_EVENT_COMPARISON_CONCURRENT`
 *
 * @param pt_Stamp1 The first Stamp
 * @param pt_Stamp2 The second Stamp
 * @param pt_Result The result of the comparison
 * @return ITC_Status_t The status of the operation
 * @retval ITC_STATUS_SUCCESS on success
 */
ITC_Status_t ITC_Stamp_compare(
    const ITC_Stamp_t *const pt_Stamp1,
    const ITC_Stamp_t *const pt_Stamp2,
    ITC_Event_Comparison_t *pt_Result
);

#endif /* ITC_STAMP_PROTOTYPES_H_ */
