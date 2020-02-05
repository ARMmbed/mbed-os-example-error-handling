/* mbed Microcontroller Library
 * Copyright (c) 2018 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "mbed.h"
#include "HeapBlockDevice.h"
#include "LittleFileSystem.h"

mbed_error_ctx error_ctx = {0};

#if MBED_CONF_PLATFORM_ERROR_HIST_ENABLED

#define MBED_FS_BLOCK_COUNT 8
#define MBED_FS_BLOCK_SIZE 512

HeapBlockDevice fd(MBED_FS_BLOCK_COUNT *MBED_FS_BLOCK_SIZE, 1, 1, MBED_FS_BLOCK_SIZE);
LittleFileSystem fs("fs");
mbed_error_status_t save_error_history()
{
    mbed_error_status_t status = MBED_SUCCESS;
    //Log some errors
    MBED_WARNING1(MBED_ERROR_TIME_OUT, "Timeout error", 1);
    MBED_WARNING1(MBED_ERROR_ALREADY_IN_USE, "Already in use error", 2);
    MBED_WARNING1(MBED_ERROR_UNSUPPORTED, "Not supported error", 3);
    MBED_WARNING1(MBED_ERROR_ACCESS_DENIED, "Access denied error", 4);
    MBED_WARNING1(MBED_ERROR_ITEM_NOT_FOUND, "Not found error", 5);
    int error = 0;

    error = LittleFileSystem::format(&fd);
    if (error < 0) {
        printf("save_error_history: Failed formatting\n");
        return MBED_ERROR_FAILED_OPERATION;
    } else {
        error = fs.mount(&fd);
        if (error < 0) {
            printf("save_error_history: Failed mounting fs\n");
            return MBED_ERROR_FAILED_OPERATION;
        } else {
            if (MBED_SUCCESS != mbed_save_error_hist("/fs/errors.txt")) {
                printf("save_error_history: Failed saving error history\n");
                return MBED_ERROR_FAILED_OPERATION;
            } else {
                FILE *error_file = fopen("/fs/errors.txt", "r");
                if (error_file == NULL) {
                    printf("save_error_history: Unable to find error log in fsfopen\n");
                    return MBED_ERROR_FAILED_OPERATION;
                } else {
                    char buff[64] = {0};
                    while (!feof(error_file)) {
                        int size = fread(&buff[0], 1, 15, error_file);
                        fwrite(&buff[0], 1, size, stdout);
                    }
                    fclose(error_file);
                }
            }

            error = fs.unmount();
            if (error < 0) {
                printf("save_error_history: Failed unmounting fs\n");
                return MBED_ERROR_FAILED_OPERATION;
            }
        }
    }
    return MBED_SUCCESS;
}

int log_warning_error_hist()
{
    // Add 4 error entries
    MBED_WARNING1(MBED_ERROR_TIME_OUT, "Timeout error", 100);
    MBED_WARNING1(MBED_ERROR_FAILED_OPERATION, "Already in use error", 101);
    MBED_WARNING1(MBED_ERROR_UNSUPPORTED, "Not supported", 102);
    MBED_WARNING1(MBED_ERROR_ACCESS_DENIED, "Access denied", 103);

    // Retrieve the logged errors from the error history
    mbed_get_error_hist_info(0, &error_ctx);
    if (error_ctx.error_status != MBED_ERROR_TIME_OUT) {
        printf("ERROR: Invalid Error Status Detected, expected MBED_ERROR_TIME_OUT\n");
        return -1;
    }

    mbed_get_error_hist_info(1, &error_ctx);
    if (error_ctx.error_status != MBED_ERROR_FAILED_OPERATION) {
        printf("ERROR: Invalid Error Status Detected, expected MBED_ERROR_FAILED_OPERATION\n");
        return -1;
    }

    mbed_get_error_hist_info(2, &error_ctx);
    if (error_ctx.error_status != MBED_ERROR_UNSUPPORTED) {
        printf("ERROR: Invalid Error Status Detected, expected MBED_ERROR_UNSUPPORTED\n");
        return -1;
    }

    mbed_get_error_hist_info(3, &error_ctx);
    if (error_ctx.error_status != MBED_ERROR_ACCESS_DENIED) {
        printf("ERROR: Invalid Error Status Detected, expected MBED_ERROR_ACCESS_DENIED\n");
        return -1;
    }

    printf("Error history capture successful\n");

    return MBED_SUCCESS;

}
#endif // #if MBED_CONF_PLATFORM_ERROR_HIST_ENABLED

static Semaphore callback_sem;
void my_error_hook(const mbed_error_ctx *error_ctx)
{
    // Fire the semaphore
    callback_sem.release();
}

int error_hook()
{
    // Use error hook functionality
    if (MBED_SUCCESS != mbed_set_error_hook(my_error_hook)) {
        printf("ERROR: Registering error hook failed\n");
        return -1;
    }

    MBED_WARNING1(MBED_ERROR_INVALID_ARGUMENT, "Test for error hook", 1234);
    bool sem_status = callback_sem.try_acquire_until(5000);

    if (!sem_status) {
        printf("ERROR: semaphore timedout\n");
        return -1;
    }
    printf("Error hook successful\n");
    return MBED_SUCCESS;
}

int log_warning_error()
{
    // Log an warning error which is not catastrophic/fatal and this would not halt the system.
    MBED_WARNING1(
        MBED_MAKE_ERROR(
            MBED_MODULE_APPLICATION, MBED_ERROR_CODE_INVALID_ARGUMENT
        ),
        "System type error", 0x1234
    );

    MBED_WARNING1(
        MBED_MAKE_ERROR(
            MBED_MODULE_PLATFORM, MBED_ERROR_CODE_ALREADY_IN_USE
        ),
        "Already in use error", 0xABCD);

    // Retrieve the first error info logged
    if (MBED_SUCCESS == mbed_get_first_error_info(&error_ctx)) {
        if (error_ctx.error_value != 0x1234) {
            printf("ERROR: Invalid Error Value Detected\n");
            return -1;
        }
        if (MBED_GET_ERROR_MODULE(error_ctx.error_status) != MBED_MODULE_APPLICATION) {
            printf("ERROR: Invalid Error Module Detected\n");
            return -1;
        }
        if (MBED_GET_ERROR_CODE(error_ctx.error_status) != MBED_ERROR_CODE_INVALID_ARGUMENT) {
            printf("ERROR: Invalid Error Code Detected\n");
            return -1;
        }
    } else {
        printf("Retrieve the first error failed\n");
        return -1;
    }
    // Retrieve the last error info logged
    if (MBED_SUCCESS == mbed_get_last_error_info(&error_ctx)) {
        if (error_ctx.error_value != 0xABCD) {
            printf("ERROR: Invalid Error Value Detected\n");
            return -1;
        }
        if (MBED_GET_ERROR_MODULE(error_ctx.error_status) != MBED_MODULE_PLATFORM) {
            printf("ERROR: Invalid Error Module Detected\n");
            return -1;
        }
        if (MBED_GET_ERROR_CODE(error_ctx.error_status) != MBED_ERROR_CODE_ALREADY_IN_USE) {
            printf("ERROR: Invalid Error Code Detected\n");
            return -1;
        }
    } else {
        printf("Retrieve the last error failed\n");
        return -1;
    }
    return MBED_SUCCESS;
}


void error_halt()
{
    MBED_ERROR1(
        MBED_MAKE_ERROR(MBED_MODULE_DRIVER_I2C, MBED_ERROR_OPERATION_PROHIBITED
                       ),
        "I2C driver error", 0xDEADDEAD
    );
}
int main()
{
    printf("This is the error handling Mbed OS example\n");
    // Clear the currently logged warnings/errors and error count
    mbed_clear_all_errors();

    if (MBED_SUCCESS != log_warning_error()) {
        printf("ERROR: logging and retrieving the warning failed\n");
    }

#if MBED_CONF_PLATFORM_ERROR_HIST_ENABLED
    if (MBED_SUCCESS != log_warning_error_hist()) {
        printf("ERROR: logging and retrieving error hist failed\n");
    }

    if (MBED_SUCCESS != save_error_history()) {
        printf("ERROR: Saving Error history failed\n");
    }
#endif

    if (MBED_SUCCESS != error_hook()) {
        printf("ERROR: Error hook failed\n");
    }

    ThisThread::sleep_for(1000);

    error_halt();
}
