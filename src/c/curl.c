#include <ctype.h>
#include <string.h>

#include <curl/curl.h>

#include <urweb.h>
#include <curl.h>

#define BUF_MAX 10240
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

static size_t write_buffer_data(void *buffer, size_t size, size_t nmemb, void *userp) {
  uw_buffer *buf = userp;

  uw_buffer_append(buf, buffer, size * nmemb);

  return size * nmemb;
}

static uw_Basis_string doweb(uw_context ctx, CURL *c, uw_Basis_string url) {
  if (strncmp(url, "https://", 8))
    uw_error(ctx, FATAL, "URWEB_HTTP: URL is not HTTPS");

  uw_buffer *buf = uw_malloc(ctx, sizeof(uw_buffer));
  char error_buffer[CURL_ERROR_SIZE];
  CURLcode code;

  uw_buffer_init(BUF_MAX, buf, BUF_INIT);
  uw_push_cleanup(ctx, (void (*)(void *))uw_buffer_free, buf);

  curl_easy_setopt(c, CURLOPT_URL, url);
  curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, write_buffer_data);
  curl_easy_setopt(c, CURLOPT_WRITEDATA, buf);
  curl_easy_setopt(c, CURLOPT_ERRORBUFFER, error_buffer);

  code = curl_easy_perform(c);

  if (code) {
    char *ret = uw_Basis_strcat(ctx, "-1;", buf->start);
    uw_pop_cleanup(ctx);
    return ret;
  } else {
    long http_code;
    curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &http_code);

    uw_buffer_append(buf, "", 1);
    char *ret = uw_Basis_strcat(ctx, uw_Basis_strcat(ctx, uw_Basis_intToString(ctx, http_code), ";") , buf->start);
    uw_pop_cleanup(ctx);
    return ret;
  } 
}

uw_Basis_string uw_CurlFfi_post(uw_context ctx, uw_Basis_string url, uw_Basis_string body, uw_Basis_string userpwd) {
  CURL *c = curl(ctx);

  curl_easy_reset(c);
  curl_easy_setopt(c, CURLOPT_POSTFIELDS, body);

  if (userpwd) {
    curl_easy_setopt(c, CURLOPT_USERPWD, userpwd);
  }

  uw_Basis_string ret = doweb(ctx, c, url);
  return ret;
}

uw_Basis_string uw_CurlFfi_get(uw_context ctx, uw_Basis_string url, uw_Basis_string userpwd) {
  CURL *c = curl(ctx);

  curl_easy_reset(c);

  if (userpwd) {
    curl_easy_setopt(c, CURLOPT_USERPWD, userpwd);
  }

  uw_Basis_string ret = doweb(ctx, c, url);
  uw_pop_cleanup(ctx);

  return ret;
}
