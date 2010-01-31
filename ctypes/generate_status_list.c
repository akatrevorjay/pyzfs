#include <ctype.h>
#include <libzfs.h>
#include <string.h>
#define fn(x) {.name = #x, .st = x}

int main(int argc, char *argv[])
{
	char realname[ZPOOL_MAXPROPLEN];
	typedef struct { zpool_status_t st; char *name; } foo;
	foo thelist[] = {fn(ZPOOL_STATUS_CORRUPT_CACHE),
	fn(ZPOOL_STATUS_MISSING_DEV_R),
	fn(ZPOOL_STATUS_MISSING_DEV_NR),
	fn(ZPOOL_STATUS_CORRUPT_LABEL_R),
	fn(ZPOOL_STATUS_CORRUPT_LABEL_NR),
	fn(ZPOOL_STATUS_BAD_GUID_SUM),
	fn(ZPOOL_STATUS_CORRUPT_POOL),
	fn(ZPOOL_STATUS_CORRUPT_DATA),
	fn(ZPOOL_STATUS_FAILING_DEV),
	fn(ZPOOL_STATUS_VERSION_NEWER),
	fn(ZPOOL_STATUS_HOSTID_MISMATCH),
	fn(ZPOOL_STATUS_IO_FAILURE_WAIT),
	fn(ZPOOL_STATUS_IO_FAILURE_CONTINUE),
	fn(ZPOOL_STATUS_FAULTED_DEV_R),
	fn(ZPOOL_STATUS_FAULTED_DEV_NR),
	fn(ZPOOL_STATUS_VERSION_OLDER),
	fn(ZPOOL_STATUS_RESILVERING),
	fn(ZPOOL_STATUS_OFFLINE_DEV),
	fn(ZPOOL_STATUS_OK)
	};
	int i, j;
	printf("\t\t{ ");
	for (i = 0; i < 19; i++)
	{
		for (j=0; j < strlen(thelist[i].name); j++)
			realname[j] = tolower(thelist[i].name[j]);
		realname[strlen(thelist[i].name)] = '\0';
		printf("\t\t%s = %d\n", &realname[strlen("ZPOOL_STATUS_")], thelist[i].st);
	}
	printf("\t\t}\n");
	return 0;
}
