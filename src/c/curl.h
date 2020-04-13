#include <urweb.h>
/* #include <curl/curl.h> */

uw_Basis_string uw_CurlFfi_get(uw_context ctx, uw_Basis_string url, uw_Basis_string auth, uw_Basis_string userpwd);
uw_Basis_string uw_CurlFfi_post(uw_context ctx, uw_Basis_string url, uw_Basis_string auth, uw_Basis_string userpwd, uw_Basis_string bodyContentType, uw_Basis_string body);
uw_Basis_string uw_CurlFfi_put(uw_context ctx, uw_Basis_string url, uw_Basis_string auth, uw_Basis_string userpwd, uw_Basis_string bodyContentType, uw_Basis_string body);
uw_Basis_string uw_CurlFfi_delete(uw_context ctx, uw_Basis_string url, uw_Basis_string auth, uw_Basis_string userpwd);

typedef struct uw_CurlFfi_curl {
  CURL *c;
  struct curl_slist *headers;
} uw_CurlFfi_curl;
uw_CurlFfi_curl uw_CurlFfi_mkCurl(uw_context ctx, uw_Basis_string verb, uw_Basis_string body);
uw_CurlFfi_curl uw_CurlFfi_addHeader(uw_context ctx, uw_CurlFfi_curl curlstruct, uw_Basis_string headerName, uw_Basis_string content);
void uw_CurlFfi_setUserPwd(uw_context ctx, uw_CurlFfi_curl curlstruct, uw_Basis_string userpwd);

typedef struct uw_CurlFfi_result {
  uw_Basis_int http_code;
  uw_Basis_string result;
} uw_CurlFfi_result;
uw_CurlFfi_result uw_CurlFfi_run(uw_context ctx, uw_CurlFfi_curl curlstruct, uw_Basis_string url);
uw_Basis_int uw_CurlFfi_getHttpCode(uw_context ctx, uw_CurlFfi_result res);
uw_Basis_string uw_CurlFfi_getResult(uw_context ctx, uw_CurlFfi_result res);
