fun buildQueryParams
  (params: (list (string * string))) : string =
  case params of
    [] => ""
  | first :: rest =>
    List.foldl
      (fn tup acc => acc ^ "&" ^ tup.1 ^ "=" ^ Urls.urlencode tup.2)
      (first.1 ^ "=" ^ Urls.urlencode first.2)
      rest

fun handleResult str =
    let val splitted = Option.unsafeGet (String.split str #";")
    in { StatusCode = Option.unsafeGet (read splitted.1)
       , Response = if String.trim splitted.2 = "" then None else Some splitted.2
       }
    end

fun postWithFormdata
      (url: url)
      (queryParams: list (string * string))
      (authheader: option string)
      (userpwd: option string)
    : transaction {StatusCode: int, Response: option string} =
    Monad.mp handleResult
             (CurlFfi.post url (buildQueryParams queryParams) authheader userpwd)

fun getWithUrlEncoding
      (url: url)
      (queryParams: list (string * string))
      (authheader: option string)
      (userpwd: option string)
    : transaction {StatusCode: int, Response: option string} =
    Monad.mp
      handleResult
      (CurlFfi.get
         (bless ((show url) ^ "?" ^ buildQueryParams queryParams))
         authheader
         userpwd)
