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

#ifdef MBED_TEST_SIM_BLOCKDEVICE

// test configuration
#ifndef MBED_TEST_FILESYSTEM
#define MBED_TEST_FILESYSTEM LittleFileSystem
#endif

#ifndef MBED_TEST_FILESYSTEM_DECL
#define MBED_TEST_FILESYSTEM_DECL MBED_TEST_FILESYSTEM fs("fs")
#endif

#ifndef MBED_TEST_BLOCK_COUNT
#define MBED_TEST_BLOCK_COUNT 64
#endif

#ifndef MBED_TEST_SIM_BLOCKDEVICE_DECL
#define MBED_TEST_SIM_BLOCKDEVICE_DECL MBED_TEST_SIM_BLOCKDEVICE fd(MBED_TEST_BLOCK_COUNT*512, 1, 1, 512)
#endif

// declarations
#define STRINGIZE(x) STRINGIZE2(x)
#define STRINGIZE2(x) #x
#define INCLUDE(x) STRINGIZE(x.h)

#include INCLUDE(MBED_TEST_FILESYSTEM)
#include INCLUDE(MBED_TEST_SIM_BLOCKDEVICE)

MBED_TEST_FILESYSTEM_DECL;
MBED_TEST_SIM_BLOCKDEVICE_DECL;

/** Test save error log
 */
mbed_error_status_t save_error_history()
{
    mbed_error_status_t status = MBED_SUCCESS;
    
    //Log some errors
    MBED_WARNING1(MBED_ERROR_TIME_OUT, "Timeout error", 1 );
    MBED_WARNING1(MBED_ERROR_ALREADY_IN_USE, "Already in use error", 2 );
    MBED_WARNING1(MBED_ERROR_UNSUPPORTED, "Not supported error", 3 );
    MBED_WARNING1(MBED_ERROR_ACCESS_DENIED, "Access denied error", 4 );
    MBED_WARNING1(MBED_ERROR_ITEM_NOT_FOUND, "Not found error", 5 );
    
    int error = 0;
    
    error = MBED_TEST_FILESYSTEM::format(&fd);
    if(error < 0) {
        printf("save_error_history: Failed formatting");
        status = MBED_ERROR_FAILED_OPERATION;
    } else {
        error = fs.mount(&fd);
        if(error < 0) {
            printf("save_error_history: Failed mounting fs");
            status = MBED_ERROR_FAILED_OPERATION;
        } else {
            if(MBED_SUCCESS != mbed_save_error_hist("/fs/errors.txt")) {
                printf("save_error_history: Failed saving error history");
                status = MBED_ERROR_FAILED_OPERATION;
            } else {
                FILE *error_file = fopen("/fs/errors.txt", "r");
                if(error_file == NULL) {
                    printf("save_error_history: Unable to find error log in fs");
                } else {
                    char buff[64] = {0};
                    while (!feof(error_file)){
                      int size = fread(&buff[0], 1, 15, error_file);
                      fwrite(&buff[0], 1, size, stdout);
                    }
                    fclose(error_file);
                }
            }
            
            error = fs.unmount();
            if(error < 0) {
                printf("save_error_history: Failed unmounting fs");
                status = MBED_ERROR_FAILED_OPERATION;
            }
        }
    }
    
    return status;
}

#endif

static Semaphore    callback_sem;
void my_error_hook(const mbed_error_ctx *error_ctx)
{
    printf("\nError Hook invoked");
    callback_sem.release();
}

int main()
{
    mbed_error_ctx error_ctx = {0};
    
    printf("\nError Handling and Error Coding Demo...");    
    
    //Use mbed_clear_all_errors to clear the currently logged warnings/errors and reset error count to 0
    mbed_clear_all_errors();
    
    //Use MBED_WARNING macros to log an error which is not catastrophic/fatal. This would log an error but would not halt the system. 
    //The erroring module is set to MBED_MODULE_APPLICATION and error code is set to MBED_ERROR_CODE_INVALID_ARGUMENT
    MBED_WARNING1(MBED_MAKE_ERROR(MBED_MODULE_APPLICATION, MBED_ERROR_CODE_INVALID_ARGUMENT), "System type error", 0x1234 );
    
    //Here the erroring module is set to MBED_MODULE_PLATFORM and error code is set to MBED_ERROR_CODE_ALREADY_IN_USE
    MBED_WARNING1(MBED_MAKE_ERROR(MBED_MODULE_PLATFORM, MBED_ERROR_CODE_ALREADY_IN_USE), "Already in use error", 0xABCD );
    
    //Now that we logged the error retrieve them and ensure they are logged correctly
    //Use mbed_get_last_error_info API to retrieve the very first error we logged. In this case
    //it would be MBED_ERROR_INVALID_ARGUMENT since we cleared all the previous in the start of this app
    mbed_error_status_t status = mbed_get_first_error_info( &error_ctx );
    if(error_ctx.error_value != 0x1234) {
        printf("\nERROR: Invalid Error Value Detected");
        return -1;
    }
    if(MBED_GET_ERROR_MODULE(error_ctx.error_status) != MBED_MODULE_APPLICATION) {
        printf("\nERROR: Invalid Error Module Detected");
        return -1;
    }
    if(MBED_GET_ERROR_CODE(error_ctx.error_status) != MBED_ERROR_CODE_INVALID_ARGUMENT) {
        printf("\nERROR: Invalid Error Code Detected");
        return -1;
    }
    
    //Use mbed_get_last_error_info API to retrieve the very first error we logged. In this case
    //it would be MBED_ERROR_ALREADY_IN_USE since we cleared all the previous in the start of this app
    status = mbed_get_last_error_info( &error_ctx );
    if(status !=MBED_SUCCESS || error_ctx.error_value != 0xABCD) {
        printf("\nERROR: Invalid Error Value Detected");
        return -1;
    }
    if(MBED_GET_ERROR_MODULE(error_ctx.error_status) != MBED_MODULE_PLATFORM) {
        printf("\nERROR: Invalid Error Module Detected");
        return -1;
    }
    if(MBED_GET_ERROR_CODE(error_ctx.error_status) != MBED_ERROR_CODE_ALREADY_IN_USE) {
        printf("\nERROR: Invalid Error Code Detected");
        return -1;
    }
    
    printf("\nError Status and Context capture successful");
    
    //Add 4 error entries
    MBED_WARNING1(MBED_ERROR_TIME_OUT, "Timeout error", 100 );
    MBED_WARNING1(MBED_ERROR_FAILED_OPERATION, "Already in use error", 101 );
    MBED_WARNING1(MBED_ERROR_UNSUPPORTED, "Not supported", 102 );
    MBED_WARNING1(MBED_ERROR_ACCESS_DENIED, "Access denied", 103 );
        
    //Retrieve the logged errors from error history
    status = mbed_get_error_hist_info( 0, &error_ctx );
    if(status !=MBED_SUCCESS || error_ctx.error_status != MBED_ERROR_TIME_OUT) {
        printf("\nERROR: Invalid Error Status Detected for mbed_get_error_hist_info, expected MBED_ERROR_TIME_OUT");
        return -1;
    }
    
    status = mbed_get_error_hist_info( 1, &error_ctx );
    if(status !=MBED_SUCCESS || error_ctx.error_status != MBED_ERROR_FAILED_OPERATION) {
        printf("\nERROR: Invalid Error Status Detected for mbed_get_error_hist_info, expected MBED_ERROR_FAILED_OPERATION");
        return -1;
    }
    
    status = mbed_get_error_hist_info( 2, &error_ctx );
    if(status !=MBED_SUCCESS || error_ctx.error_status != MBED_ERROR_UNSUPPORTED) {
        printf("\nERROR: Invalid Error Status Detected for mbed_get_error_hist_info, expected MBED_ERROR_UNSUPPORTED");
        return -1;
    }
    
    status = mbed_get_error_hist_info( 3, &error_ctx );
    if(status !=MBED_SUCCESS || status !=MBED_SUCCESS || error_ctx.error_status != MBED_ERROR_ACCESS_DENIED) {
        printf("\nERROR: Invalid Error Status Detected for mbed_get_error_hist_info, expected MBED_ERROR_ACCESS_DENIED");
        return -1;
    }
    
    printf("\nError history capture successful");
    
#ifdef MBED_TEST_SIM_BLOCKDEVICE    
    if( MBED_SUCCESS != save_error_history()) {
        printf("\nERROR: Saving Error history failed");
        return -1;
    }
    
    printf("\nSaving error history to filesystem successful");
#endif

    //Use error hook functionality
    if(MBED_SUCCESS != mbed_set_error_hook(my_error_hook)) {
        printf("\nERROR: Registering error hook failed");
        return -1;
    }
    
    MBED_WARNING1(MBED_ERROR_INVALID_ARGUMENT, "Test for error hook", 1234);
    int32_t sem_status = callback_sem.wait(5000);
    
    if(sem_status <= 0) {
        printf("\nERROR: Error hook failed");
        return -1;
    }
    
    printf("\nError hook successful");
    
    printf("\nDone...\n");    
}
