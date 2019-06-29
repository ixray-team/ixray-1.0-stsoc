/*
 * This is a part of the BugTrap package.
 * Copyright (c) 2005-2006 IntelleSoft.
 * All rights reserved.
 *
 * Description: Definitions of external functions.
 * Author: Maksim Pyatkovskiy.
 *
 * This source code is only intended as a supplement to the
 * BugTrap package reference and related electronic documentation
 * provided with the product. See these sources for detailed
 * information regarding the BugTrap package.
 */

#pragma once
#ifndef _BUGTRAP_H_
#define _BUGTRAP_H_

/*
 * The following ifdef block is the standard way of creating macros which make exporting
 * from a DLL simpler. All files within this DLL are compiled with the BUGTRAP_EXPORTS
 * symbol defined on the command line. this symbol should not be defined on any project
 * that uses this DLL. This way any other project whose source files include this file see
 * BUGTRAP_API functions as being imported from a DLL, wheras this DLL sees symbols
 * defined with this macro as being exported.
 */
#ifdef BUGTRAP_EXPORTS
 #define BUGTRAP_API __declspec(dllexport)
#else
 #define BUGTRAP_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup TypeDef Common type definitions
 * @{
 */

/**
 * @brief Type of action which is performed in response to the error.
 */
typedef enum BUGTRAP_ACTIVITY_tag
{
	/**
	 * @brief Display BugTrap dialog to allow user selecting desirable
	 * option. This is the default option.
	 */
	BTA_SHOWUI       = 1,
	/**
	 * @brief Automatically save error report to file.
	 * Use BT_SetReportFilePath() to specify report path.
	 */
	BTA_SAVEREPORT   = 2,
	/**
	 * @brief Automatically send error report by e-mail.
	 */
	BTA_MAILREPORT   = 3,
	/**
	 * @brief Automatically send bug report to support server.
	 */
	BTA_SENDREPORT = 4
}
BUGTRAP_ACTIVITY;

/**
 * @brief Different BugTrap options. You can use any combinations
 * of these flags.
 */
typedef enum BUGTRAP_FLAGS_tag
{
	/**
	 * @brief Equivalent of no options.
	 */
	BTF_NONE           = 0x00,
	/**
	 * @brief In detailed mode BugTrap generates mini-dump and
	 * packs custom log files within the report.
	 */
	BTF_DETAILEDMODE   = 0x01,
	/**
	 * @brief BugTrap may open its own editor for e-mail messages
	 * instead of the editor used by the system. Use this
	 * option if you aren't aware of the type of e-mail
	 * client installed on user computers.
	 */
	BTF_EDIETMAIL      = 0x02,
	/**
	 * @brief Specify this option to attach bug report to e-mail
	 * messages. Be careful with this option. It's potentially
	 * dangerous for the detailed mode because of even zipped
	 * mini-dump may require huge size. It may exceed the
	 * maximum size of e-mail message supported by Internet
	 * provider.
	 */
	BTF_ATTACHREPORT   = 0x04,
	/**
	 * @brief Set this flag to add list of all processes and loaded
	 * modules to the report. Disable this option to speedup report
	 * generation.
	 */
	BTF_LISTPROCESSES  = 0x08,
	/**
	 * By default BugTrap displays simplified dialog on the screen
	 * allowing user to perform only common actions. Enable this flag
	 * to immediately display dialog with advanced error information.
	 */
	BTF_SHOWADVANCEDUI = 0x10,
	/**
	 * Bug report in detailed error mode may also include a screen
	 * shot automatically captured by BugTrap. By default this option
	 * is disabled to minimize report size, but it may be useful if
	 * you want to know which dialogs were shown on the screen.
	 */
	BTF_SCREENCAPTURE  = 0x20
}
BUGTRAP_FLAGS;

/**
 * @brief Set of available log levels.
 */
typedef enum BUGTRAP_LOGLEVEL_tag
{
	/**
	 * @brief All levels of messages are disabled.
	 */
	BTLL_NONE    = 0,
	/**
	 * @brief Error message.
	 */
	BTLL_ERROR   = 1,
	/**
	 * @brief Waning message.
	 */
	BTLL_WARNING = 2,
	/**
	 * @brief Information message.
	 */
	BTLL_INFO    = 3,
	/**
	 * @brief All levels of messages are accepted.
	 */
	BTLL_ALL     = BTLL_INFO
}
BUGTRAP_LOGLEVEL;

/**
 * @brief Type of log echo mode.
 */
typedef enum BUGTRAP_LOGECHOTYPE_tag
{
	/**
	 * @brief Do not duplicate log output to file.
	 */
	BTLE_NONE   = 0x00,
	/**
	 * @brief Send a copy of log output to STDOUT (mutually exclusive with STDERR mode).
	 */
	BTLE_STDOUT = 0x01,
	/**
	 * @brief Send a copy of log output to STDERR (mutually exclusive with STDOUT mode).
	 */
	BTLE_STDERR = 0x02,
	/**
	 * @brief Send a copy of log output to the debugger (can accompany STDOUT or STDERR mode).
	 */
	BTLE_DBGOUT = 0x04
}
BUGTRAP_LOGECHOTYPE;

/**
 * @brief Set of log file options.
 */
typedef enum BUGTRAP_LOGFLAGS_tag
{
	/**
	 * @brief Do not show any additional entires in the log.
	 */
	BTLF_NONE          = 0x00,
	/**
	 * @brief Use this option if you want to store message levels in a file.
	 */
	BTLF_SHOWLOGLEVEL  = 0x01,
	/**
	 * @brief Use this option if you want to store message timestamps in a file.
	 * Timestamps are stored in universal (locale independent) format: YYYY/MM/DD HH:MM:SS
	 */
	BTLF_SHOWTIMESTAMP = 0x02
}
BUGTRAP_LOGFLAGS;

/**
 * @brief Format of error report.
 */
typedef enum BUGTRAP_REPORTFORMAT_tag
{
	/**
	 * @brief Report stored in structured XML format.
	 */
	BTRF_XML  = 1,
	/**
	 * @brief Report stored in plain text format.
	 */
	BTRF_TEXT = 2
}
BUGTRAP_REPORTFORMAT;

/**
 * @brief Format of log file.
 */
typedef enum BUGTRAP_LOGFORMAT_tag
{
	/**
	 * @brief Log stored in structured XML format.
	 * @note At this moment only text format is supported.
	 */
	BTLF_XML  = 1,
	/**
	 * @brief Log stored in plain text format.
	 */
	BTLF_TEXT = 2
}
BUGTRAP_LOGFORMAT;

/**
 * @brief Type definition of user-defined error handler that's called before and after main BugTrap dialog.
 */
typedef void (CALLBACK * BT_ErrHandler)(INT_PTR nErrHandlerParam);

/** @} */

/**
 * @defgroup AppTitle Application title management
 * @{
 */

/**
 * @brief Get application name.
 */
BUGTRAP_API LPCTSTR APIENTRY BT_GetAppName(void);
/**
 * @brief Set application name of the project where BugTrap is used.
 */
BUGTRAP_API void APIENTRY BT_SetAppName(LPCTSTR pszAppName);

/**
 * @brief Get application version number.
 */
BUGTRAP_API LPCTSTR APIENTRY BT_GetAppVersion(void);
/**
 * @brief Set application version number.
 */
BUGTRAP_API void APIENTRY BT_SetAppVersion(LPCTSTR pszAppVersion);
/**
 * @brief Read application name and version number from version info block.
 * @note @a hModule can be set to NULL for the main executable.
 */
BUGTRAP_API BOOL APIENTRY BT_ReadVersionInfo(HMODULE hModule);

/** @} */

/**
 * @defgroup SupportURL Support URL management
 * @{
 */

/**
 * @brief Get HTTP address of product support site.
 */
BUGTRAP_API LPCTSTR APIENTRY BT_GetSupportURL(void);
/**
 * @brief Set HTTP address of product support site.
 */
BUGTRAP_API void APIENTRY BT_SetSupportURL(LPCTSTR pszSupportURL);
/**
 * @brief Get product support e-mail address.
 */
BUGTRAP_API LPCTSTR APIENTRY BT_GetSupportEMail(void);
/**
 * @brief Set product support e-mail address.
 */
BUGTRAP_API void APIENTRY BT_SetSupportEMail(LPCTSTR pszSupportEMail);

/** @} */

/**
 * @defgroup SrvConfig Server configuration
 * @{
 */

/// Use this port for HTTP protocol.
#define BUGTRAP_HTTP_PORT             80

/**
 * @brief Get host name of BugTrap Server. This server can automatically
 * gather bug reports for the application.
 */
BUGTRAP_API LPCTSTR APIENTRY BT_GetSupportHost(void);
/**
 * @brief Get port number of BugTrap Server. This server can automatically
 * gather bug reports for the application.
 */
BUGTRAP_API SHORT APIENTRY BT_GetSupportPort(void);
/**
 * @brief Set host name (address) and port number of BugTrap Server. This server
 * can automatically gather bug reports for the application.
 */
BUGTRAP_API void APIENTRY BT_SetSupportServer(LPCTSTR pszSupportHost, SHORT nSupportPort);
/**
 * @brief Get error notification e-mail address.
 * BugTrap Server may automatically notify product support by e-mail about new bug reports.
 */
BUGTRAP_API LPCTSTR APIENTRY BT_GetNotificationEMail(void);
/**
 * @brief Set error notification e-mail address.
 * BugTrap Server may automatically notify product support by e-mail about new bug reports.
 */
BUGTRAP_API void APIENTRY BT_SetNotificationEMail(LPCTSTR pszNotificationEMail);

/** @} */

/**
 * @defgroup CustomReport Report customization
 * @{
 */

/// Disables mini-dump generation in detailed mode.
#define MiniDumpNoDump                MAXDWORD

/**
 * @brief Get current BugTrap options.
 */
BUGTRAP_API DWORD APIENTRY BT_GetFlags(void);
/**
 * @brief Set various BugTrap options.
 */
BUGTRAP_API void APIENTRY BT_SetFlags(DWORD dwFlags);
/**
 * @brief Get the type of produced mini-dump in detailed mode.
 */
BUGTRAP_API DWORD APIENTRY BT_GetDumpType(void);
/**
 * @brief Set the type of produced mini-dump in detailed mode.
 */
BUGTRAP_API void APIENTRY BT_SetDumpType(DWORD dwDumpType);
/**
 * @brief Get format of error report.
 */
BUGTRAP_API BUGTRAP_REPORTFORMAT APIENTRY BT_GetReportFormat(void);
/**
 * @brief Set format of error report.
 */
BUGTRAP_API void APIENTRY BT_SetReportFormat(BUGTRAP_REPORTFORMAT eReportFormat);
/**
 * @brief Get user defined message printed in log file.
 */
BUGTRAP_API LPCTSTR APIENTRY BT_GetUserMessage(void);
/**
 * @brief Set user defined message printed in log file.
 */
BUGTRAP_API void APIENTRY BT_SetUserMessage(LPCTSTR pszUserMessage);
/**
 * @brief Set user defined message printed in log file.
 */
BUGTRAP_API void APIENTRY BT_SetUserMessageFromCode(DWORD dwErrorCode);

/** @} */

/**
 * @defgroup SilentFunc Silent mode configuration
 * @{
 */

/**
 * @brief Get the type of action which is performed in response to the error.
 */
BUGTRAP_API BUGTRAP_ACTIVITY APIENTRY BT_GetActivityType(void);
/**
 * @brief Set the type of action which is performed in response to the error.
 */
BUGTRAP_API void APIENTRY BT_SetActivityType(BUGTRAP_ACTIVITY eActivityType);
/**
 * @brief Get the path of error report.
 */
BUGTRAP_API LPCTSTR APIENTRY BT_GetReportFilePath(void);
/**
 * @brief Set the path of error report.
 * This function has effect only for @a BTA_SAVEREPORT activity.
 */
BUGTRAP_API void APIENTRY BT_SetReportFilePath(LPCTSTR pszReportFilePath);
/**
 * @brief Get the name of MAPI profile.
 */
BUGTRAP_API LPCTSTR APIENTRY BT_GetMailProfile(void);
/**
 * @brief Set the name and password of MAPI profile.
 */
BUGTRAP_API void APIENTRY BT_SetMailProfile(LPCTSTR pszMailProfile, LPCTSTR pszMailPassword);

/** @} */

/**
 * @defgroup LogFiles Custom log files management
 * @{
 */

/**
 * @brief Get the name of current custom log file.
 */
BUGTRAP_API LPCTSTR* APIENTRY BT_GetLogFiles(void);
/**
 * @brief Set the list of custom log files automatically attached to bug report.
 */
BUGTRAP_API void APIENTRY BT_SetLogFiles(LPCTSTR* arrLogFiles);
/**
 * @brief Clear the list of custom log files automatically attached to bug report.
 */
BUGTRAP_API void APIENTRY BT_ClearLogFiles(void);
/**
 * @brief Add one custom log entry to the list of custom log files automatically
 * attached to bug report.
 */
BUGTRAP_API void APIENTRY BT_AddLogFile(LPCTSTR pszLogFile);

/** @} */

/**
 * @defgroup CustomErrHandlers Custom error handlers
 * @{
 */

/**
 * @brief Get address of error handler called before BugTrap dialog.
 */
BUGTRAP_API BT_ErrHandler APIENTRY BT_GetPreErrHandler(void);
/**
 * @brief Set address of error handler called before BugTrap dialog.
 */
BUGTRAP_API void APIENTRY BT_SetPreErrHandler(BT_ErrHandler pfnPreErrHandler, INT_PTR nPreErrHandlerParam);
/**
 * @brief Get address of error handler called after BugTrap dialog.
 */
BUGTRAP_API BT_ErrHandler APIENTRY BT_GetPostErrHandler(void);
/**
 * @brief Set address of error handler called after BugTrap dialog.
 */
BUGTRAP_API void APIENTRY BT_SetPostErrHandler(BT_ErrHandler pfnPostErrHandler, INT_PTR nPostErrHandlerParam);

/** @} */

/**
 * @defgroup TracingFunc Tracing functions
 * @{
 */

/**
 * @brief Open custom log file. This function is thread safe.
 */
BUGTRAP_API INT_PTR APIENTRY BT_OpenLogFile(LPCTSTR pszLogFileName, BUGTRAP_LOGFORMAT eLogFormat);
/**
 * @brief Close custom log file. This function is thread safe.
 */
BUGTRAP_API void APIENTRY BT_CloseLogFile(INT_PTR iHandle);
/**
 * @brief Flush contents of the log file.
 * @note This function is optional and not required in normal conditions.
 */
BUGTRAP_API void APIENTRY BT_FlushLogFile(INT_PTR iHandle);
/**
 * @brief Get custom log file name. This function is thread safe.
 */
BUGTRAP_API LPCTSTR APIENTRY BT_GetLogFileName(INT_PTR iHandle);
/**
 * @brief Get custom log file size in records. This function is thread safe.
 */
BUGTRAP_API DWORD APIENTRY BT_GetLogSize(INT_PTR iHandle);
/**
 * @brief Set custom log file size in records. This function is thread safe.
 */
BUGTRAP_API void APIENTRY BT_SetLogSize(INT_PTR iHandle, DWORD dwLogSize);
/**
 * @brief Return true if time stamp is added to every log entry.
 */
BUGTRAP_API DWORD APIENTRY BT_GetLogFlags(INT_PTR iHandle);
/**
 * @brief Set true if time stamp is added to every log entry.
 */
BUGTRAP_API void APIENTRY BT_SetLogFlags(INT_PTR iHandle, DWORD dwLogFlags);
/**
 * @brief Return minimal log level accepted by tracing functions.
 */
BUGTRAP_API BUGTRAP_LOGLEVEL APIENTRY BT_GetLogLevel(INT_PTR iHandle);
/**
 * @brief Set minimal log level accepted by tracing functions.
 */
BUGTRAP_API void APIENTRY BT_SetLogLevel(INT_PTR iHandle, BUGTRAP_LOGLEVEL eLogLevel);
/**
 * @brief Get echo mode.
 */
BUGTRAP_API DWORD APIENTRY BT_GetLogEchoMode(INT_PTR iHandle);
/**
 * @brief Set echo mode.
 */
BUGTRAP_API void APIENTRY BT_SetLogEchoMode(INT_PTR iHandle, DWORD dwLogEchoMode);
/**
 * @brief Clear log file. This function is thread safe.
 */
BUGTRAP_API void APIENTRY BT_ClearLog(INT_PTR iHandle);
/**
 * @brief Insert entry into the beginning of custom log file. This function is thread safe.
 */
BUGTRAP_API void CDECL BT_InsLogEntryF(INT_PTR iHandle, BUGTRAP_LOGLEVEL eLogLevel, LPCTSTR pszFormat, ...);
/**
 * @brief Append entry to the end of custom log file. This function is thread safe.
 */
BUGTRAP_API void CDECL BT_AppLogEntryF(INT_PTR iHandle, BUGTRAP_LOGLEVEL eLogLevel, LPCTSTR pszFormat, ...);
/**
 * @brief Insert entry into the beginning of custom log file. This function is thread safe.
 */
BUGTRAP_API void APIENTRY BT_InsLogEntryV(INT_PTR iHandle, BUGTRAP_LOGLEVEL eLogLevel, LPCTSTR pszFormat, va_list argList);
/**
 * @brief Append entry to the end of custom log file. This function is thread safe.
 */
BUGTRAP_API void APIENTRY BT_AppLogEntryV(INT_PTR iHandle, BUGTRAP_LOGLEVEL eLogLevel, LPCTSTR pszFormat, va_list argList);
/**
 * @brief Insert entry into the beginning of custom log file. This function is thread safe.
 */
BUGTRAP_API void APIENTRY BT_InsLogEntry(INT_PTR iHandle, BUGTRAP_LOGLEVEL eLogLevel, LPCTSTR pszEntry);
/**
 * @brief Append entry to the end of custom log file. This function is thread safe.
 */
BUGTRAP_API void APIENTRY BT_AppLogEntry(INT_PTR iHandle, BUGTRAP_LOGLEVEL eLogLevel, LPCTSTR pszEntry);

/** @} */

/**
 * @defgroup InternalFunc Internal functions
 * @{
 */

/**
 * @brief Executes structured exception filter.
 * @note Don't call this function directly in your code.
 */
BUGTRAP_API LONG CALLBACK BT_SehFilter(PEXCEPTION_POINTERS pExceptionPointers);
/**
 * @brief Executes C++ exception filter.
 * @note Don't call this function directly in your code.
 */
BUGTRAP_API LONG CALLBACK BT_CppFilter(PEXCEPTION_POINTERS pExceptionPointers);
/**
 * @brief Simulates access violation in C++ application. Used by set_terminate().
 * @note Don't call this function directly in your code.
 */
BUGTRAP_API void CDECL BT_CallCppFilter(void);

/**
 * Installs BugTrap handler to be called by the runtime in response to
 * un-handled C++ exception.
 */
#if defined __cplusplus && defined _INC_EH
 #define BT_SetTerminate() set_terminate(BT_CallCppFilter)
#else
 #define BT_SetTerminate()
#endif

/** @} */

#ifdef __cplusplus
}

 #include "BTTrace.h"

 #if defined __ATLWIN_H__ && defined __AFX_H__
  #define BT_DO_NOT_USE_DEFAULT_NAMESPACES
 #endif

 #ifdef __ATLWIN_H__
  #include "BTAtlWindow.h"
 #endif

 #ifdef __AFX_H__
  #include "BTMfcWindow.h"
 #endif

#endif

#endif
