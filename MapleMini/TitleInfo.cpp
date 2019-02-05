#include "stdafx.h"
#include "TitleInfo.h"


TitleInfo::TitleInfo()
{
}


TitleInfo::~TitleInfo()
{
}

TitleInfo::TitleInfo(char * str, size_t len)
{
	struct json_token t;

	for (int i = 0; json_scanf_array_elem(str, (int) len, "", i, &t) > 0; i++) {
		json_scanf(t.ptr, t.len, "{uid: %Q}", &uid);
		json_scanf(t.ptr, t.len, "{ID: %Q}", &id);
		json_scanf(t.ptr, t.len, "{Key: %Q}", &key);
		json_scanf(t.ptr, t.len, "{Name: %Q}", &name);
		json_scanf(t.ptr, t.len, "{Region: %Q}", &region);
		json_scanf(t.ptr, t.len, "{Versions: %Q}", &versions);
		json_scanf(t.ptr, t.len, "{HasDLC: %Q}", &hasdlc);
		json_scanf(t.ptr, t.len, "{HasPatch: %Q}", &haspatch);
		json_scanf(t.ptr, t.len, "{ContentType: %Q}", &contenttype);
		json_scanf(t.ptr, t.len, "{AvailableOnCDN: %Q}", &cdn);
		json_scanf(t.ptr, t.len, "{ProductCode: %Q}", &pcode);
		json_scanf(t.ptr, t.len, "{CompanyCode: %Q}", &ccode);
		json_scanf(t.ptr, t.len, "{ImageLocation: %Q}", &iloc);
		json_scanf(t.ptr, t.len, "{Notes: %Q}", &notes);
	}

	return;
}
