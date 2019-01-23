#pragma once

#if defined(USE_LPC6432)
#define LPC_CLIENT_ID CLIENT_ID64
#define LPC_SIZE_T ULONGLONG
#define LPC_PVOID ULONGLONG
#define LPC_HANDLE ULONGLONG
#else
#define LPC_CLIENT_ID CLIENT_ID
#define LPC_SIZE_T SIZE_T
#define LPC_PVOID PVOID
#define LPC_HANDLE HANDLE
#endif

#define MAX_LPC_DATA		0x200    // 通过LPC可以拷贝的最大字节数
#define LARGE_MESSAGE_SIZE	0x9000

typedef struct _PORT_MESSAGE {
	union {
		struct {
			CSHORT DataLength;
			CSHORT TotalLength;
		} s1;
		ULONG Length;
	} u1;
	union {
		struct {
			CSHORT Type;
			CSHORT DataInfoOffset;
		} s2;
		ULONG ZeroInit;
	} u2;
	union {
		LPC_CLIENT_ID ClientId;
		double DoNotUseThisField;       // Force quadword alignment
	};
	ULONG MessageId;
	union {
		LPC_SIZE_T ClientViewSize;          // Only valid on LPC_CONNECTION_REQUEST message
		ULONG CallbackId;                   // Only valid on LPC_REQUEST message
	};
	//UCHAR Data[1];
} PORT_MESSAGE, *PPORT_MESSAGE;

typedef struct _PORT_VIEW {
	ULONG Length;
	LPC_HANDLE SectionHandle;
	ULONG SectionOffset;
	LPC_SIZE_T ViewSize;
	LPC_PVOID ViewBase;
	LPC_PVOID ViewRemoteBase;
} PORT_VIEW, *PPORT_VIEW;


typedef struct _CLIENT_MESSAGE_INFO_
{
	NTSTATUS	st_result;
	int			nMsgType;
	int			nCPId;
	int			nCTId;
	int			nMsgSize;
	int			nRetSize;
	int			nCmd;
}CLIENT_MSG, *PCLIENT_MSG;


typedef struct _REMOTE_PORT_VIEW {
	ULONG Length;
	LPC_SIZE_T ViewSize;
	LPC_PVOID ViewBase;
} REMOTE_PORT_VIEW, *PREMOTE_PORT_VIEW;


#define GET_MESSAGE_INFO_POINT(p) ( PCLIENT_MSG )(p)
#define GET_MESSAGE_DATA_POINT(p) ( (LPBYTE)p + sizeof(CLIENT_MSG))
#define GET_MESSAGE_DATA_SIZE(p)  ( (( PCLIENT_MSG )p)->nMsgSize )
#define GET_MESSAGE_RECEIVE_DATA_SIZE(p)  ( (( PCLIENT_MSG )p)->nRetSize )

#define SET_MESSAGE_INFO(info, point) memcpy(point, info, sizeof(CLIENT_MSG));
#define SET_MESSAGE_DATA(data,size, point) ((PCLIENT_MSG)(point))->nMsgSize = size; \
						 memcpy((LPBYTE)point + sizeof(CLIENT_MSG), data, size);


NTSTATUS NTAPI NtCreatePort(
	OUT PHANDLE PortHandle,
	IN POBJECT_ATTRIBUTES ObjectAttributes,
	IN ULONG MaxConnectionInfoLength,
	IN ULONG MaxMessageLength,
	IN ULONG MaxPoolUsage
);


extern "C"
{
	NTSTATUS
		NTAPI
		ZwConnectPort(
			__out PHANDLE PortHandle,
			__in PUNICODE_STRING PortName,
			__in PSECURITY_QUALITY_OF_SERVICE SecurityQos,
			__inout_opt PPORT_VIEW ClientView,
			__inout_opt PREMOTE_PORT_VIEW ServerView,
			__out_opt PULONG MaxMessageLength,
			__inout_opt PVOID ConnectionInformation,
			__inout_opt PULONG ConnectionInformationLength
		);

	NTSTATUS
		NTAPI
		ZwCreatePort(
			__out PHANDLE PortHandle,
			__in POBJECT_ATTRIBUTES ObjectAttributes,
			__in ULONG MaxConnectionInfoLength,
			__in ULONG MaxMessageLength,
			__in_opt ULONG MaxPoolUsage
		);

	NTSTATUS
		NTAPI
		ZwReplyPort(
			__in HANDLE PortHandle,
			__in PPORT_MESSAGE ReplyMessage
		);
}
