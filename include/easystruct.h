typedef struct _UNICODE_STRING {
		USHORT Length;
		USHORT MaximumLength;
#ifdef MIDL_PASS
		[size_is(MaximumLength / 2), length_is((Length) / 2) ] USHORT * Buffer;
#else // MIDL_PASS
		__field_bcount_part(MaximumLength, Length) PWCH   Buffer;
#endif // MIDL_PASS
	} UNICODE_STRING;
	typedef UNICODE_STRING *PUNICODE_STRING;
	typedef const UNICODE_STRING *PCUNICODE_STRING;

#define UNICODE_NULL ((WCHAR)0) // winnt
typedef __success(return >= 0) LONG NTSTATUS;
/*lint -save -e624 */  // Don't complain about different typedefs.
typedef NTSTATUS *PNTSTATUS;