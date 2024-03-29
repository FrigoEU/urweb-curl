#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include <curl/curl.h>

#include <urweb.h>
#include <curl.h>

#define BUF_MAX (10 * 1024 * 1024)
#define BUF_INIT 1024

static CURL *curl(uw_context ctx) {
  CURL *r;

  if (!(r = uw_get_global(ctx, "curl"))) {
    r = curl_easy_init();
    if (r)
      uw_set_global(ctx, "curl", r, curl_easy_cleanup);
  }

  return r;
}

typedef struct {
  uw_context ctx;
  uw_buffer *buf;
} ctx_buffer;

static size_t write_buffer_data(void *buffer, size_t size, size_t nmemb, void *userp) {
  ctx_buffer *cb = userp;

  if (uw_buffer_append(cb->buf, buffer, size * nmemb))
    uw_error(cb->ctx, FATAL, "Exceeded maximum size (%d bytes) for payload returned by remote Web server", BUF_MAX);

  return size * nmemb;
}

static const char curl_failure[] = "error=fetch_url&error_description=";
/* static const char server_failure[] = "error=fetch_url&error_description="; */

static long doweb(uw_context ctx, uw_buffer *buf, CURL *c, uw_Basis_string url, int encode_errors) {
  if (strncmp(url, "https://", 8))
    uw_error(ctx, FATAL, "Curl: URL is not HTTPS");

  ctx_buffer cb = {ctx, buf};
  char error_buffer[CURL_ERROR_SIZE];
  CURLcode code;

  uw_buffer_init(BUF_MAX, buf, BUF_INIT);
  uw_push_cleanup(ctx, (void (*)(void *))uw_buffer_free, buf);

  curl_easy_setopt(c, CURLOPT_URL, url);
  curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, write_buffer_data);
  curl_easy_setopt(c, CURLOPT_WRITEDATA, &cb);
  curl_easy_setopt(c, CURLOPT_ERRORBUFFER, error_buffer);

  code = curl_easy_perform(c);

  if (code) {
    if (encode_errors) {
      uw_buffer_reset(buf);
      uw_buffer_append(buf, curl_failure, sizeof curl_failure - 1);
      char *message = curl_easy_escape(c, error_buffer, 0);
      uw_buffer_append(buf, message, strlen(message));
      curl_free(message);
      return -1;
    } else
      uw_error(ctx, FATAL, "Error fetching URL: %s", error_buffer);
  } else {
    long http_code;
    curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &http_code);

    uw_buffer_append(buf, "", 1);
    return http_code;

    /* if (http_code == 200 || http_code == 204) */
    /*   uw_buffer_append(buf, "", 1); */
    /* else if (encode_errors) { */
    /*   uw_buffer_reset(buf); */
    /*   uw_buffer_append(buf, server_failure, sizeof server_failure - 1); */
    /*   char *message = curl_easy_escape(c, error_buffer, 0); */
    /*   uw_buffer_append(buf, message, strlen(message)); */
    /*   curl_free(message); */
    /* } else { */
    /*   uw_buffer_append(buf, "", 1); */
    /*   uw_error(ctx, FATAL, "Error response #%d from remote server: %s", http_code, buf->start); */
    /* } */


    /* curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &http_code); */

  }
}

/* typedef struct uw_CurlFfi_curl { */
/*   CURL* c; */
/*   struct curl_slist* headers; */
/* } uw_CurlFfi_curl; */

struct uw_CurlFfi_curl uw_CurlFfi_mkCurl(uw_context ctx, uw_Basis_string verb) {
  CURL *c = curl(ctx);
  struct curl_slist *slist = NULL;
  slist = curl_slist_append(slist, "User-Agent: Ur/Web Curl library");
  curl_easy_reset(c);

  if (verb)
    curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, verb);
  
  return ((struct uw_CurlFfi_curl){c, slist});
}
struct uw_CurlFfi_curl uw_CurlFfi_setBodyString(uw_context ctx, struct uw_CurlFfi_curl curlstruct, uw_Basis_string body) {
  if (body)
    curl_easy_setopt(curlstruct.c, CURLOPT_POSTFIELDS, body);
  return curlstruct;
}
struct uw_CurlFfi_curl uw_CurlFfi_setBodyBlob(uw_context ctx, struct uw_CurlFfi_curl curlstruct, uw_Basis_blob body) {
  curl_easy_setopt(curlstruct.c, CURLOPT_POSTFIELDSIZE, body.size);
  curl_easy_setopt(curlstruct.c, CURLOPT_POSTFIELDS, body.data);

  return curlstruct;
}

struct uw_CurlFfi_curl uw_CurlFfi_addHeader(uw_context ctx, struct uw_CurlFfi_curl curlstruct, uw_Basis_string headerName, uw_Basis_string content) {
  uw_Basis_string header = uw_Basis_strcat(ctx, uw_Basis_strcat(ctx, headerName, ": "), content);
  struct curl_slist *slist = curl_slist_append(curlstruct.headers, header);
  return ((struct uw_CurlFfi_curl){curlstruct.c, slist});
}
uw_Basis_unit uw_CurlFfi_setUserPwd(uw_context ctx, struct uw_CurlFfi_curl curlstruct, uw_Basis_string userpwd){
  curl_easy_setopt(curlstruct.c, CURLOPT_USERPWD, userpwd);
  return 0;
}

/* typedef struct uw_CurlFfi_result { */
/*   uw_Basis_int http_code; */
/*   uw_Basis_string result; */
/* } uw_CurlFfi_result; */
struct uw_CurlFfi_result uw_CurlFfi_run(uw_context ctx, struct uw_CurlFfi_curl curlstruct, uw_Basis_string url){
  curl_easy_setopt(curlstruct.c, CURLOPT_HTTPHEADER, curlstruct.headers);
  uw_push_cleanup(ctx, (void (*)(void *))curl_slist_free_all, curlstruct.headers);
 
  uw_buffer buf;
  long http_code = doweb(ctx, &buf, curlstruct.c, url, 0);
  uw_Basis_string ret = uw_strdup(ctx, buf.start);
  uw_pop_cleanup(ctx);
  uw_pop_cleanup(ctx);

  return ((struct uw_CurlFfi_result){ http_code, ret });
}

uw_Basis_int uw_CurlFfi_getHttpCode(uw_context ctx, uw_CurlFfi_result res){
  return res.http_code;
}
uw_Basis_string uw_CurlFfi_getResult(uw_context ctx, uw_CurlFfi_result res){
  return res.result;
}

static uw_Basis_string nonget(const char *verb, uw_context ctx, uw_Basis_string url, uw_Basis_string auth, uw_Basis_string userpwd, uw_Basis_string bodyContentType, uw_Basis_string body) {
  uw_buffer buf;
  
  // This was added By Adam Chlipala at some point, but this is really extremely dangerous
  // I suppose this is to allow for some rollback use cases, but it just makes a lot of things worse

  /* uw_Basis_string lastUrl = uw_get_global(ctx, "curl.lastUrl"); */
  /* if (lastUrl && !strcmp(lastUrl, url)) { */
  /*   uw_Basis_string lastVerb = uw_get_global(ctx, "curl.lastVerb"); */
  /*   if (lastVerb && (verb ? !strcmp(lastVerb, verb) : !lastVerb[0])) { */
  /*     uw_Basis_string lastBody = uw_get_global(ctx, "curl.lastBody"); */
  /*     if (lastBody && (body ? !strcmp(lastBody, body) : !lastBody[0])) { */
  /*       uw_Basis_string lastAuth = uw_get_global(ctx, "curl.lastAuth"); */
  /*       if (lastAuth && (auth ? !strcmp(lastAuth, auth) : !lastAuth[0])) { */
  /*         uw_Basis_string lastUserPwd = uw_get_global(ctx, "curl.lastUserPwd"); */
  /*         if (lastUserPwd && (userpwd ? !strcmp(lastUserPwd, userpwd) : !lastUserPwd[0])) { */
  /*           uw_Basis_string lastResponse = uw_get_global(ctx, "curl.lastResponse"); */
  /*           uw_Basis_string lastHttpCodeStr = uw_get_global(ctx, "curl.lastHttpCodeStr"); */
  /*           if (!lastResponse || !lastHttpCodeStr) */
  /*             uw_error(ctx, FATAL, "Missing response in Curl cache"); */
  /*           return uw_Basis_strcat(ctx, uw_Basis_strcat(ctx, lastHttpCodeStr, ";") , lastResponse); */
  /*         } */
  /*       } */
  /*     } */
  /*   } */
  /* } */

  CURL *c = curl(ctx);

  curl_easy_reset(c);
  if (body)
    curl_easy_setopt(c, CURLOPT_POSTFIELDS, body);
  if (verb)
    curl_easy_setopt(c, CURLOPT_CUSTOMREQUEST, verb);

  struct curl_slist *slist = NULL;
  slist = curl_slist_append(slist, "User-Agent: Ur/Web Curl library");
  slist = curl_slist_append(slist, "Accept: application/json");

  if (userpwd) {
    curl_easy_setopt(c, CURLOPT_USERPWD, userpwd);
  }

  if (auth) {
    uw_Basis_string header = uw_Basis_strcat(ctx, "Authorization: ", auth);
    slist = curl_slist_append(slist, header);
  }

  if (bodyContentType) {
    uw_Basis_string header = uw_Basis_strcat(ctx, "Content-Type: ", bodyContentType);
    slist = curl_slist_append(slist, header);
  }

  if (slist == NULL)
    uw_error(ctx, FATAL, "Can't append to libcurl slist");

  curl_easy_setopt(c, CURLOPT_HTTPHEADER, slist);
  uw_push_cleanup(ctx, (void (*)(void *))curl_slist_free_all, slist);

  long http_code = doweb(ctx, &buf, c, url, 0);
  char *httpcodestr = uw_Basis_intToString(ctx, http_code);
  uw_set_global(ctx, "curl.lastUrl", strdup(url), free);
  uw_set_global(ctx, "curl.lastVerb", strdup(verb ? verb : ""), free);
  uw_set_global(ctx, "curl.lastBody", strdup(body ? body : ""), free);
  uw_set_global(ctx, "curl.lastAuth", strdup(auth ? auth : ""), free);
  uw_set_global(ctx, "curl.lastUserPwd", strdup(userpwd ? userpwd : ""), free);
  char *ret = strdup(buf.start);
  uw_set_global(ctx, "curl.lastHttpCodeStr", strdup(httpcodestr), free);
  uw_set_global(ctx, "curl.lastResponse", ret, free);
  uw_pop_cleanup(ctx);
  uw_pop_cleanup(ctx);
  return uw_Basis_strcat(ctx, uw_Basis_strcat(ctx, httpcodestr, ";") , ret);
}

uw_Basis_string uw_CurlFfi_post(uw_context ctx, uw_Basis_string url, uw_Basis_string auth, uw_Basis_string userpwd, uw_Basis_string bodyContentType, uw_Basis_string body) {
  return nonget(NULL, ctx, url, auth, userpwd, bodyContentType, body);
}

uw_Basis_string uw_CurlFfi_put(uw_context ctx, uw_Basis_string url, uw_Basis_string auth, uw_Basis_string userpwd, uw_Basis_string bodyContentType, uw_Basis_string body) {
  return nonget("PUT", ctx, url, auth, userpwd, bodyContentType, body);
}

uw_Basis_string uw_CurlFfi_delete(uw_context ctx, uw_Basis_string url, uw_Basis_string auth, uw_Basis_string userpwd) {
  return nonget("DELETE", ctx, url, auth, userpwd, NULL, NULL);
}

uw_Basis_string uw_CurlFfi_get(uw_context ctx, uw_Basis_string url, uw_Basis_string auth, uw_Basis_string userpwd ) {
  uw_buffer buf;
  CURL *c = curl(ctx);

  curl_easy_reset(c);

  struct curl_slist *slist = NULL;
  slist = curl_slist_append(slist, "User-Agent: Ur/Web Curl library");
  slist = curl_slist_append(slist, "Accept: application/json");

  if (userpwd) {
    curl_easy_setopt(c, CURLOPT_USERPWD, userpwd);
  }

  if (auth) {
    uw_Basis_string header = uw_Basis_strcat(ctx, "Authorization: ", auth);
    slist = curl_slist_append(slist, header);
  }

  if (slist == NULL)
    uw_error(ctx, FATAL, "Can't append to libcurl slist");

  curl_easy_setopt(c, CURLOPT_HTTPHEADER, slist);
  uw_push_cleanup(ctx, (void (*)(void *))curl_slist_free_all, slist);
 
  long http_code = doweb(ctx, &buf, c, url, 0);
  uw_Basis_string ret = uw_strdup(ctx, buf.start);
  uw_pop_cleanup(ctx);
  uw_pop_cleanup(ctx);

  return uw_Basis_strcat(ctx, uw_Basis_strcat(ctx, uw_Basis_intToString(ctx, http_code), ";") , ret);
}
