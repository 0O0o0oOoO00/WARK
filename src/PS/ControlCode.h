#pragma once

#ifdef RING0
#include <ntifs.h>
#else
#include <windows.h>
#endif // RING0

#define IOCTL_SET_TARGET_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0X800, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_COLLECT_PROCESS_INFO CTL_CODE(FILE_DEVICE_UNKNOWN, 0X801, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_COLLECT_OBJECT_TYPE_INFO CTL_CODE(FILE_DEVICE_UNKNOWN, 0X802, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_COLLECT_PROCESS_DETAIL CTL_CODE(FILE_DEVICE_UNKNOWN, 0X803, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_COLLECT_OBJECT_TYPE_DETAIL CTL_CODE(FILE_DEVICE_UNKNOWN, 0X804, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_MDL_READ_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0X805, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_MDL_WRITE_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0X806, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_PHYSICAL_READ_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0X807, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_PHYSICAL_WRITE_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0X808, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_CR3_READ_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0X809, METHOD_OUT_DIRECT, FILE_ANY_ACCESS)
#define IOCTL_CR3_WRITE_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0X80A, METHOD_IN_DIRECT, FILE_ANY_ACCESS)

#define IOCTL_TERMINATE_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0X80B, METHOD_IN_DIRECT, FILE_ANY_ACCESS)