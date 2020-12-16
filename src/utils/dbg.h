/*
 * dbg.h
 *
 *  Created on: 10 Feb 2012
 *      Author: exuvo
 */

#ifndef DBG_H_
#define DBG_H_

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <log4cxx/logger.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) LOG4CXX_DEBUG(log, M)
#endif

#define clean_errno() (errno == 0 ? "" : strerror(errno))
#define clean_errno2() (errno == 0 ? "" : "; errno: ")

#define log_err(M) LOG4CXX_ERROR(log, M << clean_errno2() << clean_errno())

#define check(A, M) if(!(A)) { log_err(M); errno=0; goto error; }
#define sentinel(M)  { log_err(M); errno=0; goto error; }
#define check_mem(A) check((A), "Out of memory.")
#define check_debug(A, M) if(!(A)) { debug(M); errno=0; goto error; }
	
#ifdef NDEBUG
#define printf_debug(M, ...)
#else
#define printf_debug(M, ...) printf("DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define printf_log_err(M, ...) printf("[ERROR] (%s:%d%s%s) " M "\n", __FILE__, __LINE__, clean_errno2(), clean_errno(), ##__VA_ARGS__)
//#define printf_log_warn(M, ...) printf("[WARN] (%s:%d%s%s) " M "\n", __FILE__, __LINE__, clean_errno2(), clean_errno(), ##__VA_ARGS__)
//#define printf_log_info(M, ...) printf("[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define printf_check(A, M, ...) if(!(A)) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }
#define printf_sentinel(M, ...)  { log_err(M, ##__VA_ARGS__); errno=0; goto error; }
#define printf_check_mem(A) check((A), "Out of memory.")
#define printf_check_debug(A, M, ...) if(!(A)) { debug(M, ##__VA_ARGS__); errno=0; goto error; }

#ifdef __cplusplus
}
#endif
#endif /* DBG_H_ */
