/**
 * @file ITC_Stamp_prototypes.h
 * @brief Prototypes for the Interval Tree Clock's Stamp mechanism
 *
 * @copyright Copyright (c) 2024 libitc project. Released under AGPL-3.0
 * license. Refer to the LICENSE file for details or visit:
 * https://www.gnu.org/licenses/agpl-3.0.en.html
 *
 */
#ifndef ITC_STAMP_PROTOTYPES_H_
#define ITC_STAMP_PROTOTYPES_H_

#include "ITC_Stamp.h"
#include "ITC_Status.h"
#include "ITC_config.h"

/******************************************************************************
 * Functions
 ******************************************************************************/

/**
 * @brief Allocate a new ITC seed Stamp and initialise it
 *
 * @param ppt_Stamp (out) The pointer to the Stamp
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Stamp_newSeed(
    ITC_Stamp_t **ppt_Stamp
);

/**
 * @brief Allocate a new ITC peek Stamp based on an existing Stamp
 *
 * @param pt_Stamp The existing Stamp
 * @param ppt_PeekStamp (out) The pointer to the peek Stamp
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Stamp_newPeek(
    const ITC_Stamp_t *const pt_Stamp,
    ITC_Stamp_t **ppt_PeekStamp
);

/**
 * @brief Free an allocated ITC Stamp
 *
 * @warning Even if the function call fails, the Stamp might be partically freed
 * and not safe to use. For this reason, the returned `*ppt_Stamp` will always
 * be set to `NULL`.
 *
 * @param ppt_Stamp (in) The pointer to the Stamp to deallocate. (out) NULL
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Stamp_destroy(
    ITC_Stamp_t **ppt_Stamp
);

/**
 * @brief Clone an existing ITC Stamp
 *
 * @param pt_Stamp The existing Stamp
 * @param ppt_ClonedStamp (out) The pointer to the cloned Stamp
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Stamp_clone(
    const ITC_Stamp_t *const pt_Stamp,
    ITC_Stamp_t **ppt_ClonedStamp
);

/**
 * @brief Fork an existing Stamp
 * Creates 2 stamps with distinct (non-overlaping) IDs and the same
 * event history.
 *
 * @param pt_Stamp The existing Stamp
 * @param ppt_Stamp1 (out) The first Stamp
 * @param ppt_Stamp2 (out) The second Stamp
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Stamp_fork(
    const ITC_Stamp_t *const pt_Stamp,
    ITC_Stamp_t **ppt_Stamp1,
    ITC_Stamp_t **ppt_Stamp2
);

/**
 * @brief Add a new Event to the Stamp
 *
 * @warning If the function fails, the Stamp might be left in a partially
 * updated state and should not be used further.
 *
 * @param pt_Stamp The existing Stamp
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Stamp_event(
    const ITC_Stamp_t *const pt_Stamp
);

/**
 * @brief Join two existing Stamps
 * Joins 2 stamps into a single Stamp, combining their IDs and event histories.
 *
 * @param pt_Stamp1 The first Stamp
 * @param pt_Stamp2 The second Stamp
 * @param ppt_Stamp (out) The pointer to the joined Stamp
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Stamp_join(
    const ITC_Stamp_t *const pt_Stamp1,
    const ITC_Stamp_t *const pt_Stamp2,
    ITC_Stamp_t **ppt_Stamp
);

/**
 * @brief Compare two existing Stamps
 *
 * - If `*pt_Stamp1 < *pt_Stamp2`:
 *      `*pt_Result == ITC_STAMP_COMPARISON_LESS_THAN`
 * - If `*pt_Stamp1 > *pt_Stamp2`:
 *      `*pt_Result == ITC_STAMP_COMPARISON_GREATER_THAN`
 * - If `*pt_Stamp1 == *pt_Stamp2`:
 *      `*pt_Result == ITC_STAMP_COMPARISON_EQUAL`
 * - If `*pt_Stamp1 <> *pt_Stamp2`:
 *      `*pt_Result == ITC_STAMP_COMPARISON_CONCURRENT`
 *
 * @param pt_Stamp1 The first Stamp
 * @param pt_Stamp2 The second Stamp
 * @param pt_Result (out) The result of the comparison
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Stamp_compare(
    const ITC_Stamp_t *const pt_Stamp1,
    const ITC_Stamp_t *const pt_Stamp2,
    ITC_Stamp_Comparison_t *pt_Result
);

#if ITC_CONFIG_ENABLE_EXTENDED_API

/**
 * @brief Allocate a new ITC Stamp, initialising it with a copy of the passed ID
 *
 * @param pt_Id The ID to copy
 * @param ppt_Stamp (out) The pointer to the Stamp
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Stamp_newFromId(
    ITC_Id_t *pt_Id,
    ITC_Stamp_t **ppt_Stamp
);

/**
 * @brief Allocate a new ITC Stamp, initialising it with a copy of the passed ID
 * and Event
 *
 * @param pt_Id The ID to copy
 * @param pt_Event The Event to copy
 * @param ppt_Stamp (out) The pointer to the Stamp
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Stamp_newFromIdAndEvent(
    ITC_Id_t *pt_Id,
    ITC_Event_t *pt_Event,
    ITC_Stamp_t **ppt_Stamp
);

/**
 * @brief Allocate a new ITC peek Stamp, initialising it with a copy of the passed
 * Event
 *
 * @param pt_Event The Event to copy
 * @param ppt_Stamp (out) The pointer to the Stamp
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Stamp_newPeekFromEvent(
    ITC_Event_t *pt_Event,
    ITC_Stamp_t **ppt_Stamp
);

/**
 * @brief Get a copy of the ID component of a Stamp
 *
 * @param pt_Stamp The existing Stamp
 * @param ppt_Id (out) The pointer to the ID copy
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Stamp_getId(
    const ITC_Stamp_t *const pt_Stamp,
    ITC_Id_t **ppt_Id
);

/**
 * @brief Get a copy of the Event component of a Stamp
 *
 * @param pt_Stamp The existing Stamp
 * @param ppt_Event (out) The pointer to the Event copy
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Stamp_getEvent(
    const ITC_Stamp_t *const pt_Stamp,
    ITC_Event_t **ppt_Event
);

/**
 * @brief Set the ID component of an existing Stamp.
 *
 * The passed ID will be copied.
 *
 * @param pt_Stamp The existing Stamp
 * @param pt_Id The ID to copy
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Stamp_setId(
    ITC_Stamp_t *const pt_Stamp,
    const ITC_Id_t *const pt_Id
);

/**
 * @brief Set the Event component of an existing Stamp
 *
 * The passed Event will be copied.
 *
 * @param pt_Stamp The existing Stamp
 * @param pt_Event The Event to copy
 * @return `ITC_Status_t` The status of the operation
 * @retval `ITC_STATUS_SUCCESS` on success
 */
ITC_Status_t ITC_Stamp_setEvent(
    ITC_Stamp_t *const pt_Stamp,
    const ITC_Event_t *const pt_Event
);

#endif /* ITC_CONFIG_ENABLE_EXTENDED_API */

#endif /* ITC_STAMP_PROTOTYPES_H_ */
