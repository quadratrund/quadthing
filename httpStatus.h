#include "httpResponse.h"

HttpResponse http_redirect(IPAddress ip, const char* url);
HttpResponse http_notFound();
HttpResponse http_okHead();
HttpResponse http_askForLogin();
HttpResponse http_ok();
