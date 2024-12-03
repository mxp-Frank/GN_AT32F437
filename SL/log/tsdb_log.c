/*
 * Copyright (c) 2020, Armink, <armink.ztl@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief TSDB samples.
 *
 * Time series log (like TSDB) feature samples source file.
 *
 * TSL is time series log, the TSDB saved many TSLs.
 */

#include <flashdb.h>
#include <string.h>

#ifdef FDB_USING_TSDB

#define FDB_LOG_TAG "[sample][tsdb]"


struct env_status {
    int temp;
    int humi;
};
static struct fdb_tsdb log_tsdb;
static void tsdb_sample(fdb_tsdb_t tsdb);
static bool query_cb(fdb_tsl_t tsl, void *arg);
static bool query_by_time_cb(fdb_tsl_t tsl, void *arg);
static bool set_status_cb(fdb_tsl_t tsl, void *arg);

static int counts=0;
static void lock(fdb_db_t db)
{
    __disable_irq();
}

static void unlock(fdb_db_t db)
{
    __enable_irq();
}

static fdb_time_t get_time(void)
{
    /* Using the counts instead of timestamp.
     * Please change this function to return RTC time.
     */
    return ++counts;
}

void fdb_fdb_tsdb_Init(void)
{
	fdb_err_t result;
	#ifdef FDB_USING_TSDB
    { /* TSDB Sample */
        /* set the lock and unlock function if you want */
        fdb_tsdb_control(&log_tsdb, FDB_TSDB_CTRL_SET_LOCK, lock);
        fdb_tsdb_control(&log_tsdb, FDB_TSDB_CTRL_SET_UNLOCK, unlock);
        /* Time series database initialization
         *
         *       &tsdb: database object
         *       "log": database name
         * "fdb_tsdb1": The flash partition name base on FAL. Please make sure it's in FAL partition table.
         *              Please change to YOUR partition name.
         *    get_time: The get current timestamp function.
         *         128: maximum length of each log
         *        NULL: The user data if you need, now is empty.
         */
        result = fdb_tsdb_init(&log_tsdb, "log", "fdb_tsdb1", get_time, 128, NULL);
        /* read last saved time for simulated timestamp */
        fdb_tsdb_control(&log_tsdb, FDB_TSDB_CTRL_GET_LAST_TIME, &counts);

        if (result == FDB_NO_ERR) 
        {  
			/* run TSDB sample */
			tsdb_sample(&log_tsdb);
		}
    }
#endif /* FDB_USING_TSDB */
}	

static void tsdb_sample(fdb_tsdb_t tsdb)
{
    struct fdb_blob blob;

    FDB_INFO("==================== tsdb_sample ====================\n");

    { /* APPEND new TSL (time series log) */
        struct env_status status;

        /* append new log to TSDB */
        status.temp = 36;
        status.humi = 85;
        fdb_tsl_append(tsdb, fdb_blob_make(&blob, &status, sizeof(status)));
        FDB_INFO("append the new status.temp (%d) and status.humi (%d)\n", status.temp, status.humi);

        status.temp = 38;
        status.humi = 90;
        fdb_tsl_append(tsdb, fdb_blob_make(&blob, &status, sizeof(status)));
        FDB_INFO("append the new status.temp (%d) and status.humi (%d)\n", status.temp, status.humi);
    }

    { /* QUERY the TSDB */
        /* query all TSL in TSDB by iterator */
        fdb_tsl_iter(tsdb, query_cb, tsdb);
    }

    { /* QUERY the TSDB by time */
        /* prepare query time (from 1970-01-01 00:00:00 to 2020-05-05 00:00:00) */
        struct tm tm_from = { .tm_year = 1970 - 1900, .tm_mon = 0, .tm_mday = 1, .tm_hour = 0, .tm_min = 0, .tm_sec = 0 };
        struct tm tm_to = { .tm_year = 2020 - 1900, .tm_mon = 4, .tm_mday = 5, .tm_hour = 0, .tm_min = 0, .tm_sec = 0 };
        time_t from_time = mktime(&tm_from), to_time = mktime(&tm_to);
        size_t count;
        /* query all TSL in TSDB by time */
        fdb_tsl_iter_by_time(tsdb, from_time, to_time, query_by_time_cb, tsdb);
        /* query all FDB_TSL_WRITE status TSL's count in TSDB by time */
        count = fdb_tsl_query_count(tsdb, from_time, to_time, FDB_TSL_WRITE);
        FDB_INFO("query count is: %zu\n", count);
    }

    { /* SET the TSL status */
        /* Change the TSL status by iterator or time iterator
         * set_status_cb: the change operation will in this callback
         *
         * NOTE: The actions to modify the state must be in order.
         *       like: FDB_TSL_WRITE -> FDB_TSL_USER_STATUS1 -> FDB_TSL_DELETED -> FDB_TSL_USER_STATUS2
         *       The intermediate states can also be ignored.
         *       such as: FDB_TSL_WRITE -> FDB_TSL_DELETED
         */
        fdb_tsl_iter(tsdb, set_status_cb, tsdb);
    }

    FDB_INFO("===========================================================\n");
}

static bool query_cb(fdb_tsl_t tsl, void *arg)
{
    struct fdb_blob blob;
    struct env_status status;
    fdb_tsdb_t db = arg;

    fdb_blob_read((fdb_db_t) db, fdb_tsl_to_blob(tsl, fdb_blob_make(&blob, &status, sizeof(status))));
    FDB_INFO("[query_cb] queried a TSL: time: %ld, temp: %d, humi: %d\n", tsl->time, status.temp, status.humi);

    return false;
}

static bool query_by_time_cb(fdb_tsl_t tsl, void *arg)
{
    struct fdb_blob blob;
    struct env_status status;
    fdb_tsdb_t db = arg;

    fdb_blob_read((fdb_db_t) db, fdb_tsl_to_blob(tsl, fdb_blob_make(&blob, &status, sizeof(status))));
    FDB_INFO("[query_by_time_cb] queried a TSL: time: %ld, temp: %d, humi: %d\n", tsl->time, status.temp, status.humi);

    return false;
}

static bool set_status_cb(fdb_tsl_t tsl, void *arg)
{
    fdb_tsdb_t db = arg;

    FDB_INFO("set the TSL (time %ld) status from %d to %d\n", tsl->time, tsl->status, FDB_TSL_USER_STATUS1);
    fdb_tsl_set_status(db, tsl, FDB_TSL_USER_STATUS1);

    return false;
}

#endif /* FDB_USING_TSDB */

//*****************************************************************************
//* END
//*****************************************************************************
